#!/usr/bin/env bash
#
# test_replay.sh - validate the spoof by running the SHIPPED MIPS binary under
# qemu-mipsel and driving it with the EXACT requests the device's wsa emits.
#
# Why replay instead of running wsa itself: the real wsa binary (uClibc,
# hard-float MIPS32r2, statically linked) does not load under qemu-user 10.2
# (the loader rejects it and exits before main - see docs/on-device-spoof.md).
# So we emulate the SERVER (sb_mips) and reproduce wsa's wire protocol with an
# OpenSSL TLS 1.0 client - the same TLS family and byte-for-byte the same
# requests wsa's buildRegistrationRequest()/buildChannelRequest() produce - then
# assert the reply passes every check in wsa's response parser (wsa-overview
# section 5.5).
#
# No root or namespaces needed: the server listens on an unprivileged test port.
#
set -uo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
QEMU="$(command -v qemu-mipsel)"
SB="$HERE/sb_mips"
PORT="${PORT:-8443}"
HOST=127.0.0.1

# Device values (from root/etc/config/sbi + meldTechDeviceCapability.xml).
RD=US; FCC=OKVMT300; SER=371; LAT=38.8132280; LON=-77.0443610; TYPE=3; ANT=30
SNI=tvws.spectrumbridge.com
TLS1="-tls1 -cipher AES128-SHA:AES256-SHA:DES-CBC3-SHA:@SECLEVEL=0"

[ -x "$SB" ] || { echo "missing $SB - run build.sh first"; exit 1; }

echo "[*] starting MIPS server under qemu on https://$HOST:$PORT"
"$QEMU" "$SB" "https://$HOST:$PORT" >/tmp/sb_replay.log 2>&1 &
SRV=$!
trap 'kill $SRV 2>/dev/null' EXIT
sleep 2

send() {  # $1 = request bytes; echoes the raw response
  # -ign_eof: keep reading after stdin EOF until the server closes (it sends
  # Connection: close). The trailing sleep keeps the socket open long enough
  # for the reply to arrive before the client tears down.
  { printf '%b' "$1"; sleep 3; } | timeout 8 openssl s_client -quiet -ign_eof $TLS1 \
     -connect "$HOST:$PORT" 2>/dev/null
}

pass=0; fail=0
check() { if eval "$2"; then echo "  PASS: $1"; pass=$((pass+1)); else echo "  FAIL: $1"; fail=$((fail+1)); fi; }

# --- 1. Registration PUT (exact format from buildRegistrationRequest) --------
BODY="<RegistrationRequest xmlns=\"http://schemas.datacontract.org/2004/07/SpectrumBridge.WhiteSpaces.Services.v3\"><AntennaHeight>$ANT</AntennaHeight><ContactCity/><ContactCountry>US</ContactCountry><ContactEmail/><ContactName/><ContactPhone/><ContactState/><ContactStreet/><ContactZip/><DeviceOwner/><DeviceType>$TYPE</DeviceType><Latitude>$LAT</Latitude><Longitude>$LON</Longitude></RegistrationRequest>"
REG="PUT https://$SNI/v3/devices/$RD/$FCC/$SER HTTP/1.1\r\nHost: $SNI\r\nContent-Type: Application/xml \r\nContent-Length: ${#BODY}\r\n\n${BODY}\r\n\r\n"
echo "[*] registration PUT"
R="$(send "$REG")"
check "HTTP 200"              "head -1 <<<\"\$R\" | grep -q ' 200 '"
check "SBI-Status: 0 header"  "grep -qi 'SBI-Status: 0' <<<\"\$R\""

# --- 2. Channel GET (exact format from buildChannelRequest) ------------------
CH="GET https://$SNI/v3/channels/$RD/$LAT/$LON/?fccid=$FCC&serial=$SER&type=$TYPE&ant=$ANT HTTP/1.0\r\nHost: $SNI\r\nUser-Agent: HTMLGET 1.0\r\n\r\n"
echo "[*] channel GET"
R="$(send "$CH")"
echo "---- response ----"; printf '%s\n' "$R" | sed 's/\r$//' | grep -vE '^$' | head -12; echo "------------------"

check "HTTP 200"             "head -1 <<<\"\$R\" | grep -q ' 200 '"
check "SBI-Status: 0 header" "grep -qi 'SBI-Status: 0' <<<\"\$R\""
check "has <ChannelResponse>" "grep -q '<ChannelResponse>' <<<\"\$R\""

CNT=$(grep -o '<ChannelCount>[0-9]*' <<<"$R" | grep -o '[0-9]*' | head -1)
LIST=$(grep -o '<ChannelList>[^<]*' <<<"$R" | sed 's/<ChannelList>//' | head -1)
DISTINCT=$(tr ',' '\n' <<<"$LIST" | sort -u | grep -c '[0-9]')
echo "[*] ChannelCount=$CNT  ChannelList=$LIST  distinct=$DISTINCT"

check "ChannelCount == distinct channels" "[ \"\$CNT\" = \"\$DISTINCT\" ]"
check "all channels in wsa's 21-51 filter" \
  "! tr ',' '\n' <<<\"\$LIST\" | awk 'NF && (\$1<21 || \$1>51){bad=1} END{exit !bad}'"
BYTES=$(grep -o '<ChannelResponse>.*</ChannelResponse>' <<<"$R" | wc -c)
check "response body < 1 KB (wsa receiveBuffer)" "[ \"\$BYTES\" -lt 1024 ]"

echo
echo "[*] server saw:"; grep -E 'request on TLS|replied 200' /tmp/sb_replay.log || true
echo
echo "================  $pass passed, $fail failed  ================"
[ "$fail" = 0 ]
