/*
 * mbedtls_user_config.h - appended to mbedTLS's default config via
 * -DMBEDTLS_USER_CONFIG_FILE when building the Meld white-space spoof.
 *
 * The device's whitespaceagent is linked against OpenSSL 1.0.0, which can
 * negotiate at most TLS 1.0. mbedTLS 2.28 LTS compiles TLS 1.0/1.1 OUT by
 * default, so without this the server only speaks TLS 1.2 and the handshake
 * with wsa fails. Re-enable the legacy protocol versions here; the matching
 * RSA / AES-CBC / SHA-1 ciphersuites and the TLS1.0 PRF (MD5+SHA1) are already
 * in the default config. The server's min version is additionally lowered to
 * TLS 1.0 by a patch to mongoose's mg_tls_init() (see build.sh).
 */
#define MBEDTLS_SSL_PROTO_TLS1
#define MBEDTLS_SSL_PROTO_TLS1_1

/* wsa uses OpenSSL 1.0.0's SSLv23_client_method, which announces itself with a
 * legacy SSLv2-format ClientHello (that then negotiates TLS 1.0). mbedTLS will
 * not parse that hello unless this is enabled - without it the server rejects
 * the ClientHello (-0x7900). Also enable SSLv3 in case the client steps down. */
#define MBEDTLS_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO
#define MBEDTLS_SSL_PROTO_SSL3
