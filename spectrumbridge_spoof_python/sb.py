import socket, ssl, sys, datetime, time

LISTEN_ADDRESS = '0.0.0.0'
LISTEN_PORT = 443
CERT_FILE = 'cert.pem'
KEY_FILE = 'key.pem'

def start_spoof_service():
    bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    bind_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    bind_socket.bind((LISTEN_ADDRESS, LISTEN_PORT))
    bind_socket.listen(5)
    print "Service Active. Waiting for MIPS device..."

    while True:
        try:
            newsocket, fromaddr = bind_socket.accept()
            connstream = ssl.wrap_socket(newsocket, server_side=True, certfile=CERT_FILE, keyfile=KEY_FILE, ssl_version=ssl.PROTOCOL_SSLv23)
            request = connstream.recv(8192)
            if request:
                # The sscanf in getWhiteSpaceStatus expects 7 tokens in the Date header:
                # 1:Date: 2:DayName, 3:Day 4:Month 5:Year 6:Time 7:GMT
                # Example: Date: Sun, 15 Mar 2026 01:30:00 GMT
                now = datetime.datetime.utcnow()
                http_date = now.strftime('%a, %d %b %Y %H:%M:%S GMT')
                
                # ChannelCount MUST match the comma-separated list length exactly
                response_body = (
                    '<?xml version="1.0" encoding="utf-8"?>'
                    '<RegistrationResponse>'
                    '<ChannelCount>26</ChannelCount>'
                    '<ChannelList>3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28</ChannelList>'
                    '<RefreshIn>24</RefreshIn>'
                    '</RegistrationResponse>'
                )
                
                # SBI-Status MUST have a space after the colon for %s %d to work
                # We use \r\n explicitly to match standard HTTP/MIPS expectations
                response = (
                    "HTTP/1.1 200 OK\r\n"
                    "Date: {}\r\n"
                    "SBI-Status: 0\r\n"
                    "Content-Type: text/xml\r\n"
                    "Content-Length: {}\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "{}"
                ).format(http_date, len(response_body), response_body)

                connstream.sendall(response)
                print "Success payload sent."
                time.sleep(0.5)
            connstream.shutdown(socket.SHUT_RDWR)
            connstream.close()
        except Exception as e:
            print "Error: {}".format(e)

if __name__ == "__main__":
    start_spoof_service()
