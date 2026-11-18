#!/usr/bin/env bash
#
# build.sh - cross-compile the SpectrumBridge white-space spoof (sb.c) into a
# static MIPS32 little-endian binary for the Meld MT300, using a self-contained
# musl toolchain and mbedTLS. No root required.
#
# Output: ./sb_mips  (static, stripped, runnable on the device or under qemu-mipsel)
#
# Env overrides:
#   WORK   build/work directory (default: ./build, gitignored)
#   JOBS   parallel make jobs (default: nproc)
#
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
WORK="${WORK:-$HERE/build}"
JOBS="${JOBS:-$(nproc)}"

# Pinned versions (match spectrumbridge_spoof_c/notes.txt).
TC_TGZ="mipsel-linux-muslsf-cross.tgz"
TC_URL="https://musl.cc/${TC_TGZ}"
TC_DIR="$WORK/mipsel-linux-muslsf-cross"
CC="$TC_DIR/bin/mipsel-linux-muslsf-gcc"
AR="$TC_DIR/bin/mipsel-linux-muslsf-ar"
STRIP="$TC_DIR/bin/mipsel-linux-muslsf-strip"

MBED_VER="2.28.2"
MBED_TGZ="v${MBED_VER}.tar.gz"
MBED_URL="https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/${MBED_TGZ}"
MBED_DIR="$WORK/mbedtls-${MBED_VER}"

# -no-pie: emit a classic ET_EXEC (not static-pie), matching the device wsa so
# the MT300's Linux 2.6.36 kernel loads it reliably.
CFLAGS_COMMON="-O2 -static -no-pie -msoft-float"

mkdir -p "$WORK"

# --- 1. musl MIPS toolchain -------------------------------------------------
if [ ! -x "$CC" ]; then
  echo "[*] fetching musl MIPS toolchain"
  [ -f "$WORK/$TC_TGZ" ] || wget -q -O "$WORK/$TC_TGZ" "$TC_URL"
  tar -xzf "$WORK/$TC_TGZ" -C "$WORK"
fi
echo "[*] CC = $CC"
"$CC" -v 2>&1 | tail -1

# --- 2. mbedTLS (with TLS 1.0 re-enabled) -----------------------------------
if [ ! -f "$MBED_DIR/library/libmbedtls.a" ]; then
  echo "[*] fetching + building mbedTLS ${MBED_VER}"
  [ -f "$WORK/$MBED_TGZ" ] || wget -q -O "$WORK/$MBED_TGZ" "$MBED_URL"
  tar -xzf "$WORK/$MBED_TGZ" -C "$WORK"
  # Enable TLS1.0/1.1 by pointing mbedTLS at our user-config overlay.
  make -C "$MBED_DIR" clean
  make -C "$MBED_DIR" -j"$JOBS" lib \
     CC="$CC" AR="$AR" \
     CFLAGS="$CFLAGS_COMMON -I$HERE -DMBEDTLS_USER_CONFIG_FILE='\"mbedtls_user_config.h\"'" \
     LDFLAGS="-static"
fi

# --- 3. patch a throwaway copy of mongoose to allow a TLS1.0 floor ----------
# Mongoose 7.20 calls mbedtls_ssl_config_defaults(PRESET_DEFAULT) and never
# lowers the min version, so even with TLS1.0 compiled in the server would
# still refuse it. Inject a min-version call right after config_defaults.
# The committed mongoose.c is left untouched.
MG_SRC="$WORK/mongoose_tls10.c"
cp "$HERE/mongoose.c" "$MG_SRC"
python3 - "$MG_SRC" <<'PY'
import sys, re
p = sys.argv[1]
s = open(p).read()
needle = "MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {\n"
anchor = s.find(needle)
if anchor == -1:
    sys.exit("could not find config_defaults call to patch")
# end of the whole `if (... config_defaults ...) { mg_error; goto fail; }` block
close = s.index("  }\n", anchor) + len("  }\n")
inject = (
    "#if defined(MBEDTLS_SSL_MINOR_VERSION_1)\n"
    "  /* Meld spoof: allow TLS 1.0 for the OpenSSL-1.0.0 wsa client. */\n"
    "  mbedtls_ssl_conf_min_version(&tls->conf, MBEDTLS_SSL_MAJOR_VERSION_3,\n"
    "                               MBEDTLS_SSL_MINOR_VERSION_1);\n"
    "#endif\n"
)
s = s[:close] + inject + s[close:]
open(p, "w").write(s)
print("patched mongoose min TLS version -> 1.0")
PY

# --- 4. compile + link ------------------------------------------------------
echo "[*] compiling"
INCS="-I$HERE -I$MBED_DIR/include -DMG_ENABLE_MBEDTLS=1 -DMG_ENABLE_TCPIP=0 \
      -DMBEDTLS_USER_CONFIG_FILE='\"mbedtls_user_config.h\"'"

eval "$CC" $CFLAGS_COMMON $INCS -c "$MG_SRC"        -o "$WORK/mongoose.o"
eval "$CC" $CFLAGS_COMMON $INCS -c "$HERE/sb.c"     -o "$WORK/sb.o"

echo "[*] linking"
"$CC" $CFLAGS_COMMON "$WORK/sb.o" "$WORK/mongoose.o" \
    "$MBED_DIR/library/libmbedtls.a" \
    "$MBED_DIR/library/libmbedx509.a" \
    "$MBED_DIR/library/libmbedcrypto.a" \
    -lpthread -o "$HERE/sb_mips"

"$STRIP" "$HERE/sb_mips"
echo "[*] done:"
file "$HERE/sb_mips"
ls -la "$HERE/sb_mips"
