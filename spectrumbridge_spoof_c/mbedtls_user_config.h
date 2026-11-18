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
