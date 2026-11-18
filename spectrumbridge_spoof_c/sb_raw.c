#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"

#define LISTEN_PORT 443
#define HTTP_RESPONSE \
    "HTTP/1.1 200 OK\r\n" \
    "Content-Type: text/xml; charset=utf-8\r\n" \
    "Connection: close\r\n\r\n" \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
    "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n" \
    "  <soap:Body>\n" \
    "    <RegistrationResponse xmlns=\"http://spectrumbridge.com/WS/Registration\">\n" \
    "      <Status>Success</Status>\n" \
    "    </RegistrationResponse>\n" \
    "  </soap:Body>\n" \
    "</soap:Envelope>"

static const char *s_ssl_cert = "-----BEGIN CERTIFICATE-----\n"
"MIIDozCCAougAwIBAgIUSc6KghUcrnPPfaUK8yvtRmgXHlUwDQYJKoZIhvcNAQEF\n"
"BQAwYTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAlZBMRAwDgYDVQQHDAdGYWlyZmF4\n"
"MREwDwYDVQQKDAhNZWxkVGVjaDEgMB4GA1UEAwwXdHZ3cy5zcGVjdHJ1bWJyaWRn\n"
"ZS5jb20wHhcNMjYwMzE2MTQyNDMzWhcNMzYwMzEzMTQyNDMzWjBhMQswCQYDVQQG\n"
"EwJVUzELMAkGA1UECAwCVkExEDAOBgNVBAcMB0ZhaXJmYXgxETAPBgNVBAoMCE1l\n"
"bGRUZWNoMSAwHgYDVQQDDBd0dndzLnNwZWN0cnVtYnJpZGdlLmNvbTCCASIwDQYJ\n"
"KoZIhvcNAQEBBQADggEPADCCAQoCggEBAKd2WKz5DoLJIVQJZMMTkv0OXZ18PFT4\n"
"pDc5ww/Gav4yvTK51DoW/jW23oNBoeopAVnPxW41Fo34RxvUNeHQoye/ds32dlXo\n"
"MqvJad7XXDFSNCPx/JJf1p+r4/n6u9iYQDDh4tacbgLHe2LsbH9OanKGoyAqyeNo\n"
"DaA8Wl6e9mALrEanXS1FcVY8GCkm3ror6EV2Hjo1c/insI9FjnEXZaEprLniO/jp\n"
"nBFCtGapZXU/e/tx4ewLjcZVJj5oS0ZF9qVAWsIvZsdk8tXU+WjVCqLhiMZ2QZHx\n"
"IHiYZLKT8zAyCPBesV4Dkj/r+ccytdAOoMd/ENF/7Nsl3Dth2ime9o8CAwEAAaNT\n"
"MFEwHQYDVR0OBBYEFJJGk3Hf0vFRf7U8PxxyXG8q+L6HMB8GA1UdIwQYMBaAFJJG\n"
"k3Hf0vFRf7U8PxxyXG8q+L6HMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEF\n"
"BQADggEBAEpsWsuoFKNX1DaioZMVrNNbpBB/5A71YqD3Ol8SpoIZZhCqSSlPqRUf\n"
"8agyhIUf41vghnfckaGalhHxzf7hgOYA8N/rybjYzbilfYjfbWx9p/XRaT1SdiFs\n"
"vANvVNBvJfzKAcybzVKKLj+aucmQm/jTiv6znmhdEtY+fUWCGCiTS/fSVXudxbm1\n"
"RZq39PvQgSEtUphmIt2rra6mZLLsLwxrsw9eCrRQWgZG7beL7ItnKA5eSdr1OEPf\n"
"cfweb6iGz4N0VU8ptnOlRgFWGhq25mTpO6e+ZjhQNRvrkCpRWY51NzwZzze3xAmj\n"
"knMSRatQob9DT9AA3Wbl1jiCHPWW3tQ=\n"
"-----END CERTIFICATE-----";

static const char *s_ssl_key = "-----BEGIN PRIVATE KEY-----\n"
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCndlis+Q6CySFU\n"
"CWTDE5L9Dl2dfDxU+KQ3OcMPxmr+Mr0yudQ6Fv41tt6DQaHqKQFZz8VuNRaN+Ecb\n"
"1DXh0KMnv3bN9nZV6DKryWne11wxUjQj8fySX9afq+P5+rvYmEAw4eLWnG4Cx3ti\n"
"7Gx/TmpyhqMgKsnjaA2gPFpenvZgC6xGp10tRXFWPBgpJt66K+hFdh46NXP4p7CP\n"
"RY5xF2WhKay54jv46ZwRQrRmqWV1P3v7ceHsC43GVSY+aEtGRfalQFrCL2bHZPLV\n"
"1Plo1Qqi4YjGdkGR8SB4mGSyk/MwMgjwXrFeA5I/6/nHMrXQDqDHfxDRf+zbJdw7\n"
"YdopnvaPAgMBAAECggEAT5XU6s+qb8l6h1+3fWZOEunC39dtLMzF7mELLPIVYx1m\n"
"ATeDft/yjECOWz7T3l8Cf1ZY89lTEMGPudAOk3T+NSOvKI8bChTCEBY2tGsRK3dv\n"
"xC165W2aqGOegu0AXR1uVEzl6yyIs9Vt9EN8BLyVpjwKRn1PWT3YS/Ci2hkd6eKd\n"
"Xt/eKSqRDOxtsm4vCAQVOIMJMtnlxyOKtyqARVI4tY2Jqg8HvKZUQe4UZ8V8S7qr\n"
"kWesokG9G7Y1sKFVzWC8saqsipOPeePb+CH3QXi/GseOpjAy64cj8VBcq4BRIMNz\n"
"j0Ijpx5mV7nv8AgdRVvF6XTnEU++kC8B7ap9P1pLgQKBgQDWbJWMBjS8YXqD7Ad2\n"
"WWVp6BlzHa58WF2jmzgnPT74WX89vCzqiYWonXEgKbbPY2ox7uSmtbcaYkxSxhbs\n"
"DH9fUXXS84jFNVTNW7b4aDIji4QDNIFB9DGwEiWNqBgSUJ+MC7W4ucFLYcw8lq9S\n"
"1rmrQffhfLD12JYwN6zKvZD/oQKBgQDH7rTY/EC9LfvMw7Dg1thvPHdNLnxXvBe7\n"
"oA6XYsYdKNdVh+LngbgTq48/AS1nQjiE4e6t/OPVmaFZbBAGSVKETe1+cX3tr7Dr\n"
"CZhDfDj3iKwBPZphFWNi+4Mlt71hqWbjxvll+0j0kxACi29vjcw2Ic3d9Tolcy+z\n"
"q1y6QSMILwKBgQCUAoQAEdCZMFhXc7d69B5Syom2sohdAMoyQtSyEU7EM1LnlFsU\n"
"0jTWvUb3QpQT97W4HgTrJofryGqSwHXWHpJVg29nW//O0Q9maaaVNadWbdmENNv8\n"
"YMljlkMh0yQ6kDhxfSvFBUPt+uX19LW04s9q7bBjj6S/LvAM/IFjuRK7wQKBgGtm\n"
"+lPOPuDhwPCg93XxNFedPeGCZV2kN4OdWflNIGwtsm/KDcSb1M0MTmlkMoXT9xS7\n"
"YkKpJaNrYlftSzZ1URSt4DUatrFmvj2k69zk8J1ZGD2aYEJjAP79adhrOhsqq5EV\n"
"JIj6UpjAH8ueKADfRaKWykbAI7pWhHuaRLfxoeYVAoGAFygcEnL5ta0SE+CYI2ra\n"
"DQ4+gKtEQPYHFD1t6WfyfMV/DEIGI1p+2nLlrXavxt793S/Dp2GYefJ6toKR/O56\n"
"BDG3FNH20x2YLfJR/g+PvZsYEBjJfJYchHr5AZt8VMowpw/8pmc44u6mz5q9R9yp\n"
"2Rihx0+084JGjQzf07zWYq8=\n"
"-----END PRIVATE KEY-----";

int main() {
    int listen_fd, client_fd;
    struct sockaddr_in serv_addr;
    char buf[2048];
    int ret;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt srvcert;
    mbedtls_pk_context pkey;

    printf("\n--- Starting Raw MIPS SSL Server (Port %d) ---\n", LISTEN_PORT);

    // 1. Initialize MbedTLS
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_pk_init(&pkey);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)"ws", 2);
    ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *)s_ssl_cert, strlen(s_ssl_cert) + 1);
    ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *)s_ssl_key, strlen(s_ssl_key) + 1, NULL, 0);

    mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
    mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey);
    mbedtls_ssl_setup(&ssl, &conf);

    // 2. Create Raw Socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(LISTEN_PORT);

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(listen_fd, 5);
    printf("Listening on port %d... (Kernel-level Bind Successful)\n", LISTEN_PORT);

    while (1) {
        client_fd = accept(listen_fd, NULL, NULL);
        printf("\n[Client Connected]\n");

        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_context client_net;
        client_net.fd = client_fd;
        mbedtls_ssl_set_bio(&ssl, &client_net, mbedtls_net_send, mbedtls_net_recv, NULL);

        printf("Performing SSL Handshake...\n");
        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                printf("Handshake failed! Error: -0x%x\n", -ret);
                goto close_client;
            }
        }

        printf("Handshake Success!\n");

        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, sizeof(buf) - 1);
        if (ret > 0) {
            printf("Received Request:\n%s\n", buf);
            mbedtls_ssl_write(&ssl, (const unsigned char *)HTTP_RESPONSE, strlen(HTTP_RESPONSE));
            printf("Success Response Sent.\n");
        }

close_client:
        mbedtls_ssl_close_notify(&ssl);
        close(client_fd);
    }

    return 0;
}
