#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <unistd.h>


// mipsel-linux-muslsf-gcc -static -O2 -msoft-float sniff.c -o sniff


// Filter Globals
uint32_t filter_ip = 0;
uint16_t filter_port = 0;

// --- Function to Process and Filter Packets ---
void ProcessPacket(unsigned char* buffer, int size) {
    // 1. Ethernet Header (14 bytes)
    struct ethhdr *eth = (struct ethhdr *)buffer;
    
    // Only process IPv4 (0x0800)
    if (ntohs(eth->h_proto) != 0x0800) return;

    // 2. IP Header
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    unsigned short iphdrlen = iph->ihl * 4;

    // Apply IP Filter (source or destination)
    if (filter_ip != 0) {
        if (iph->saddr != filter_ip && iph->daddr != filter_ip) return;
    }

    uint16_t src_port = 0, dest_port = 0;
    const char *proto_name = "UNK";

    // 3. Transport Layer - Using Musl/BSD compatible naming
    if (iph->protocol == 6) { // TCP
        struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
        src_port = ntohs(tcph->th_sport);
        dest_port = ntohs(tcph->th_dport);
        proto_name = "TCP";
    } else if (iph->protocol == 17) { // UDP
        struct udphdr *udph = (struct udphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
        src_port = ntohs(udph->uh_sport);
        dest_port = ntohs(udph->uh_dport);
        proto_name = "UDP";
    }

    // Apply Port Filter
    if (filter_port != 0) {
        if (src_port != filter_port && dest_port != filter_port) return;
    }

    // 4. Reporting
    struct in_addr src, dest;
    src.s_addr = iph->saddr;
    dest.s_addr = iph->daddr;

    printf("[%s] %15s:%-5d -> %15s:%-5d (Len: %d)\n", 
           proto_name, inet_ntoa(src), src_port, inet_ntoa(dest), dest_port, size);
}

void print_usage(char *name) {
    printf("MIPS Raw Sniffer\n");
    printf("Usage: %s [-i ip_address] [-p port]\n", name);
    printf("  -i, --ip      Filter by IP address\n");
    printf("  -p, --port    Filter by Port\n");
    printf("  -h, --help    Display this help\n");
}

int main(int argc, char *argv[]) {
    int sock_raw;
    int data_size;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    int c;

    static struct option long_options[] = {
        {"ip",   required_argument, 0, 'i'},
        {"port", required_argument, 0, 'p'},
        {"help", no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "i:p:h", long_options, NULL)) != -1) {
        switch (c) {
            case 'i':
                filter_ip = inet_addr(optarg);
                break;
            case 'p':
                filter_port = (uint16_t)atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                return 1;
        }
    }

    printf("Starting MIPS Sniffer...\n");
    if (filter_ip) printf("Filter IP: %s\n", inet_ntoa(*(struct in_addr*)&filter_ip));
    if (filter_port) printf("Filter Port: %u\n", filter_port);
    printf("--------------------------------------------------\n");

    // Capture everything at the link layer
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket Error");
        return 1;
    }

    while (1) {
        data_size = recvfrom(sock_raw, buffer, 65536, 0, NULL, NULL);
        if (data_size > 0) {
            ProcessPacket(buffer, data_size);
        }
    }

    free(buffer);
    close(sock_raw);
    return 0;
}
