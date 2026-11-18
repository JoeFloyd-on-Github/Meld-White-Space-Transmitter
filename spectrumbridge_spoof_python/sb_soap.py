import socket
import ssl
import sys
import datetime
import time

# Configuration
LISTEN_ADDRESS = '0.0.0.0'
LISTEN_PORT = 443
CERT_FILE = 'cert.pem'
KEY_FILE = 'key.pem'

def log(message):
    timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print "[{}] {}".format(timestamp, message)

def start_spoof_service():
    bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    bind_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        bind_socket.bind((LISTEN_ADDRESS, LISTEN_PORT))
        log("Bound to 0.0.0.0:443")
    except socket.error as e:
        log("FATAL: {}".format(e))
        sys.exit(1)

    bind_socket.listen(5)
    log("Waiting for SOAP Registration...")

    while True:
        try:
            newsocket, fromaddr = bind_socket.accept()
            connstream = ssl.wrap_socket(newsocket,
                                         server_side=True,
                                         certfile=CERT_FILE,
                                         keyfile=KEY_FILE,
                                         ssl_version=ssl.PROTOCOL_SSLv23)
            
            request = connstream.recv(8192)
            if request:
                log("--- REQUEST RECEIVED ---")
                
                expiry = (datetime.datetime.utcnow() + datetime.timedelta(days=365)).strftime('%Y-%m-%dT%H:%M:%SZ')
                
                # FULL SOAP ENVELOPE
                # This structure is what a .NET/WCF service actually outputs.
                # It wraps the RegistrationResponse in a Body and Envelope.
                response_body = (
                    '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">'
                    '<s:Body>'
                    '<RegistrationResponse xmlns="http://schemas.datacontract.org/2004/07/SpectrumBridge.WhiteSpaces.Services.v3" '
                    'xmlns:i="http://www.w3.org/2001/XMLSchema-instance">'
                    '<DatabaseExpiration>{}</DatabaseExpiration>'
                    '<Error i:nil="true"/>'
                    '<RegistrationId>SPOOF-371-OK</RegistrationId>'
                    '<RulesetIds xmlns:a="http://schemas.microsoft.com/2003/10/Serialization/Arrays">'
                    '<a:string>fcc-v2</a:string>'
                    '</RulesetIds>'
                    '</RegistrationResponse>'
                    '</s:Body>'
                    '</s:Envelope>'
                ).format(expiry)
                
                full_payload = '<?xml version="1.0" encoding="utf-8"?>' + response_body

                response_headers = (
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/xml; charset=utf-8\r\n"
                    "Content-Length: {}\r\n"
                    "Server: Microsoft-IIS/7.5\r\n"
                    "Date: {}\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                ).format(len(full_payload), datetime.datetime.utcnow().strftime('%a, %d %b %Y %H:%M:%S GMT'))

                connstream.sendall(response_headers + full_payload)
                log("SOAP Envelope sent.")
                
                # Give MIPS agent time to parse before closing
                time.sleep(0.5)
            
            connstream.shutdown(socket.SHUT_RDWR)
            connstream.close()
            
        except Exception as e:
            log("Error: {}".format(e))
        log("==========================================")

if __name__ == "__main__":
    start_spoof_service()
