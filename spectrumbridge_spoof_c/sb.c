#define MG_ENABLE_MBEDTLS 1
#define MG_ENABLE_TCPIP 1
#define MG_ENABLE_PACKET_LOG 1

#include "mongoose.h"

#include <stdio.h>


// Use port 443 for HTTPS

static const char *s_lsn_addr = "https://0.0.0.0:8443";


// Standard SOAP Success Response

static const char *s_response =

    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

    "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"

    "  <soap:Body>\n"

    "    <RegistrationResponse xmlns=\"http://spectrumbridge.com/WS/Registration\">\n"

    "      <Status>Success</Status>\n"

    "    </RegistrationResponse>\n"

    "  </soap:Body>\n"

    "</soap:Envelope>";


// --- EMBEDDED KEY MATERIAL ---

static const char *s_ssl_cert =

"-----BEGIN CERTIFICATE-----\n"

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


static const char *s_ssl_key =

"-----BEGIN PRIVATE KEY-----\n"

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


// Handler matching Mongoose 7.x signature

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {

  if (ev == MG_EV_HTTP_MSG) {

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;


    printf("\n--- Secure HTTP POST Received ---\n");

    printf("Method: %.*s\n", (int) hm->method.len, hm->method.buf);

    printf("URI:    %.*s\n", (int) hm->uri.len, hm->uri.buf);

    

    if (hm->body.len > 0) {

        printf("XML Body Content:\n%.*s\n", (int) hm->body.len, hm->body.buf);

    }


    // Send SOAP Response

    mg_http_reply(c, 200, "Content-Type: text/xml; charset=utf-8\r\n", "%s", s_response);

    printf("--- Response Sent (SOAP Success) ---\n");

  } else if (ev == MG_EV_ERROR) {

    printf("Mongoose Error: %s\n", (char *) ev_data);

  }

}


int main(void) {

  struct mg_mgr mgr;

  struct mg_connection *c;


  mg_mgr_init(&mgr);                                      

 

  printf("Starting Static MIPS White Space SSL Server\n");

 

  // Change this line in main:
  if ((c = mg_http_listen(&mgr, s_lsn_addr, (mg_event_handler_t) fn, NULL)) == NULL) {


    printf("Error: Failed to start listener on %s\n", s_lsn_addr);

    return 1;

  }


  // Initialize TLS using embedded cert and key

  struct mg_tls_opts tls_opts = {

    .cert = (char *) s_ssl_cert,

    .key = (char *) s_ssl_key

  };

  mg_tls_init(c, &tls_opts);


  printf("Listening for encrypted connections on port 443...\n");


  for (;;) {

    mg_mgr_poll(&mgr, 1000);

  }


  mg_mgr_free(&mgr);

  return 0;

} 
