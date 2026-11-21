/*
 * sb.c - On-device SpectrumBridge white-space database stand-in for the Meld MT300.
 *
 * Replaces tvws.spectrumbridge.com (dead since the service shut down) so the
 * device's `whitespaceagent` ("wsa") gets a valid registration + channel
 * response and turns the transmitter on. Designed to run ON the device as a
 * static MIPS binary, or on a host under qemu-mipsel for testing.
 *
 * It answers EVERY request with one HTTP/1.1 200 response that satisfies both
 * code paths in wsa (see docs/wsa-overview.md section 5.5):
 *   - registration PUT  -> only needs "HTTP .. 200" + "SBI-Status: 0"
 *   - channel GET        -> also needs <ChannelCount>/<ChannelList>/<RefreshIn>,
 *                           ChannelCount == count of DISTINCT channels in the
 *                           list, channels kept inside wsa's 21-51 filter,
 *                           and the whole reply < ~1 KB (wsa's receiveBuffer).
 * wsa reads until the peer closes, so we drain (close) the connection after
 * replying.
 *
 * TLS: wsa is OpenSSL 1.0.0 (TLS 1.0 max, RSA-AES-CBC-SHA ciphers) and its
 * verify_callback accepts a self-signed leaf (X509_V_ERR 18), so the embedded
 * self-signed cert below is trusted with no CA work and no hostname/CN check.
 * The Mongoose mbedTLS build must have TLS 1.0 compiled in and its min version
 * lowered to TLS 1.0 - see the build script and mbedtls_user_config.h.
 *
 * Listen address: argv[1] if given, else https://0.0.0.0:443 (wsa's port is
 * hard-coded to 443). A high port can be passed for unprivileged host testing.
 */

#define MG_ENABLE_MBEDTLS 1
#define MG_ENABLE_TCPIP   0
#define MG_ENABLE_PACKET_LOG 0

#include "mongoose.h"

#include <stdio.h>
#include <time.h>

static const char *s_default_addr = "https://0.0.0.0:443";

/* Channels advertised to the device. All inside wsa's 21-51 capability filter
 * so every one survives reAdjustChannelMapToMeldCapabilities(). ChannelCount
 * MUST equal the number of distinct entries here. Keep the whole reply < 1 KB. */
static const char *s_channel_list = "23,42";
static const int   s_channel_count = 2;

/* --- EMBEDDED KEY MATERIAL (self-signed, CN=tvws.spectrumbridge.com) ---
 * The CN is cosmetic: wsa does no hostname check. Trust comes from wsa
 * whitelisting the self-signed-leaf verify error, not from this name. */
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
"-----END CERTIFICATE-----\n";

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
"-----END PRIVATE KEY-----\n";

static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_ACCEPT) {
    /* Attach TLS to each accepted connection. */
    struct mg_tls_opts opts;
    memset(&opts, 0, sizeof(opts));
    opts.cert = mg_str(s_ssl_cert);
    opts.key  = mg_str(s_ssl_key);
    mg_tls_init(c, &opts);
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    char date[40];
    char headers[128];
    char body[256];
    time_t now = time(NULL);
    struct tm *g = gmtime(&now);

    printf("\n--- request on TLS: %.*s %.*s ---\n",
           (int) hm->method.len, hm->method.buf,
           (int) hm->uri.len, hm->uri.buf);
    if (hm->body.len > 0)
      printf("body: %.*s\n", (int) hm->body.len, hm->body.buf);

    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", g);

    mg_snprintf(body, sizeof(body),
        "<ChannelResponse>"
        "<ChannelCount>%d</ChannelCount>"
        "<ChannelList>%s</ChannelList>"
        "<RefreshIn>24</RefreshIn>"
        "</ChannelResponse>",
        s_channel_count, s_channel_list);

    /* wsa needs: status 200, a "SBI-Status: 0" header (space after colon),
     * then the XML. We add Date (stored by wsa) and close the connection so
     * wsa's read loop terminates. mg_http_reply emits status, Content-Length
     * and these verbatim headers; the body is formatted separately. */
    mg_snprintf(headers, sizeof(headers),
        "Date: %s\r\n"
        "SBI-Status: 0\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Connection: close\r\n",
        date);

    mg_http_reply(c, 200, headers, "%s", body);

    c->is_draining = 1;  /* flush the reply, then close -> wsa stops reading */
    printf("--- replied 200, SBI-Status 0, %d channels ---\n", s_channel_count);
  } else if (ev == MG_EV_ERROR) {
    printf("mongoose error: %s\n", (char *) ev_data);
  }
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  const char *addr = (argc > 1) ? argv[1] : s_default_addr;

  setvbuf(stdout, NULL, _IONBF, 0);  /* unbuffered: logs survive if killed */
  mg_mgr_init(&mgr);
  printf("Meld white-space DB stand-in (SpectrumBridge spoof)\n");
  printf("listening on %s\n", addr);

  if (mg_http_listen(&mgr, addr, fn, NULL) == NULL) {
    printf("error: failed to listen on %s (port < 1024 needs privilege)\n", addr);
    return 1;
  }

  for (;;) mg_mgr_poll(&mgr, 1000);

  mg_mgr_free(&mgr);
  return 0;
}
