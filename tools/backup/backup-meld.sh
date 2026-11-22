#!/usr/bin/env bash
#
# backup-meld.sh HOST DEST_PARENT
#
# Full, verified backup of a running Meld device over the network, with nothing
# needed on the device beyond its stock BusyBox tools. Produces, under
# DEST_PARENT, a directory:
#
#   backup-<model>-sn<serial>-<appver>-<YYYY-MM-DD>/
#     README.txt
#     mtd/           mtdblockN-<Name>.img + MD5SUMS.txt      (raw flash)
#     rootfs-live/   rootfs-*.cpio.gz, disk-*.cpio.gz + MD5SUMS.txt (live fs)
#     meta/          device-state.txt, modifications/sb_mips-changes.txt
#
# It is the procedure in docs/device-backup.md wired together. READ THAT FIRST.
# The capture is read-only on the device; it changes nothing.
#
# Requirements on this host: bash, nc, md5sum, gzip, python3 (for meld_telnet.py,
# which must sit next to this script). The device must be reachable on telnet 23
# (passwordless root, as Meld firmware ships) and must allow an inbound nc on
# PORT below.
#
set -u
HOST="${1:?usage: backup-meld.sh HOST DEST_PARENT}"
DESTPARENT="${2:?usage: backup-meld.sh HOST DEST_PARENT}"
PORT="${PORT:-9000}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TN="python3 $HERE/meld_telnet.py -H $HOST"

# --- helper: run command(s) on the device, print raw output -----------------
dev() { $TN "$@"; }
# extract the first 32-hex md5 hash from device output (avoids the command echo)
hash_of() { awk '$1 ~ /^[0-9a-f]+$/ && length($1)==32 {print $1; exit}'; }

echo "[*] probing $HOST ..."
MODEL=$(dev 'grep -i fccid /etc/config/fccidSerialInfo 2>/dev/null; echo' | grep -i okv >/dev/null && echo mt300 || echo meld)
SERIAL=$(dev 'cat /etc/config/fccidSerialInfo 2>/dev/null' | awk -F: '/^Serial Number/{gsub(/ /,"",$2);print $2;exit}')
APPVER=$(dev 'grep -i "option .Version." /etc/config/sbi 2>/dev/null' | grep -oE "[0-9]+\.[0-9]+" | head -1)
SERIAL="${SERIAL:-unknown}"; APPVER="${APPVER:-vX}"
DATE=$(date +%Y-%m-%d)
BK="$DESTPARENT/backup-${MODEL}-sn${SERIAL}-v${APPVER}-${DATE}"
echo "[*] destination: $BK"
mkdir -p "$BK/mtd" "$BK/rootfs-live" "$BK/meta/modifications" || exit 1

# --- 1. metadata ------------------------------------------------------------
echo "[*] metadata ..."
dev 'echo "### uname"; uname -a
     echo "### cmdline"; cat /proc/cmdline
     echo "### mtd"; cat /proc/mtd
     echo "### mounts"; mount
     echo "### df"; df -h
     echo "### actimage"; /bin/actimage; echo "rc=$?"
     echo "### ps"; ps
     echo "### RELEASE_NOTES"; cat /disk/Meld/meldtech/RELEASE_NOTES 2>/dev/null | head -40
     echo "### fccidSerialInfo"; cat /etc/config/fccidSerialInfo 2>/dev/null
     echo "### queryInfo"; cat /etc/config/queryInfo 2>/dev/null
     echo "### sbi version"; grep -iE "version|date" /etc/config/sbi 2>/dev/null' \
     > "$BK/meta/device-state.txt" 2>&1

# record sb_mips modification evidence if present
dev 'if [ -f /usr/bin/sb_mips ]; then
       echo "### sb_mips present - DEVICE MODIFIED"
       ls -l /usr/bin/sb_mips /usr/bin/sb_start 2>/dev/null
       md5sum /usr/bin/sb_mips /usr/bin/sb_start 2>/dev/null
       echo "### /etc/hosts"; cat /etc/hosts
       echo "### rcS hook"; grep -n -A2 -B1 sb_start /etc/init.d/rcS
       echo "### originals"; ls -l /etc/init.d/rcS.orig /disk/hosts.bak 2>&1
     else
       echo "### no sb_mips - device appears unmodified"
     fi' > "$BK/meta/modifications/sb_mips-changes.txt" 2>&1

# --- 2. MTD partitions ------------------------------------------------------
# read the partition list from the device: "mtdN: SIZE ERASE "NAME""
mapfile -t MTDS < <(dev 'cat /proc/mtd' | awk -F'[: "]+' '/^mtd[0-9]/{print $1" "$4}')
: > "$BK/mtd/MD5SUMS.txt"
for row in "${MTDS[@]}"; do
  devname="${row%% *}"; name="${row##* }"; n="${devname#mtd}"
  out="$BK/mtd/mtdblock${n}-${name}.img"
  echo "[*] mtd$n ($name) ..."
  devmd5=$(dev "md5sum /dev/mtdblock$n" "(nc -l -p $PORT < /dev/mtdblock$n &); sleep 1; echo up" | hash_of)
  nc -w 25 "$HOST" "$PORT" > "$out"
  sz=$(stat -c %s "$out"); pcmd5=$(md5sum "$out" | awk '{print $1}')
  if [ "$devmd5" = "$pcmd5" ]; then v=OK; else v="SNAPSHOT/MISMATCH (live fs? verify)"; fi
  printf '  size=%-10s dev=%s pc=%s %s\n' "$sz" "$devmd5" "$pcmd5" "$v"
  echo "$pcmd5  mtdblock${n}-${name}.img  (device:$devmd5) $v" >> "$BK/mtd/MD5SUMS.txt"
done

# --- 3. live filesystems ----------------------------------------------------
# active rootfs = root=/dev/mtdblockN ; /disk mount = its mtdblock
ROOTBLK=$(dev 'cat /proc/cmdline' | grep -oE 'mtdblock[0-9]+' | head -1); ROOTBLK=${ROOTBLK#mtdblock}
DISKBLK=$(dev 'mount' | awk '/\/disk /{print $1}' | grep -oE '[0-9]+' | head -1)
pull_fs() {  # $1=device dir  $2=output basename
  local dir="$1" out="$BK/rootfs-live/$2"
  local cnt; cnt=$(dev "cd $dir && find . -xdev | wc -l" | grep -oE '[0-9]+' | tail -1)
  dev "(cd $dir && find . -xdev | cpio -o -H newc 2>/dev/null | nc -l -p $PORT &); sleep 1; echo up" >/dev/null
  nc -w 25 "$HOST" "$PORT" | gzip > "$out"
  local got; got=$(zcat "$out" | cpio -t 2>/dev/null | wc -l)
  gzip -t "$out" && echo "  $2: gzip OK, device_files=$cnt archive_entries=$got"
}
echo "[*] live rootfs (mtdblock${ROOTBLK}) ..."
pull_fs /     "rootfs-mtdblock${ROOTBLK}-live.cpio.gz"
echo "[*] live /disk (mtdblock${DISKBLK}) ..."
pull_fs /disk "disk-mtdblock${DISKBLK}-live.cpio.gz"
(cd "$BK/rootfs-live" && md5sum *.cpio.gz > MD5SUMS.txt)

# --- 4. README --------------------------------------------------------------
cat > "$BK/README.txt" <<EOF
Meld device backup -- ${MODEL} serial ${SERIAL}
Captured ${DATE} over the network from the live, running device.

Software version: Meld app v${APPVER} (see meta/device-state.txt for kernel,
radio firmware, AV app and the exact RELEASE_NOTES).
Active boot image: root=mtdblock${ROOTBLK}, /disk=mtdblock${DISKBLK}
(the A/B spare slots are the other RootFS/AppFS images).

MODIFICATION STATUS: see meta/modifications/sb_mips-changes.txt. If sb_mips is
present, this device was modified to run the local database stand-in and is NOT
pristine firmware; the inactive rootfs slot image in mtd/ is unmodified.

Captured with docs/device-backup.md (nc streaming; BLOCK-level mtdblock images,
no OOB/ECC because the device has no nanddump; live filesystems as newc cpio).
Every MTD image md5 was compared device-vs-stored (see mtd/MD5SUMS.txt); the
ACTIVE rootfs image is a live snapshot whose hash is not reproducible -- use
rootfs-live/*.cpio.gz for a consistent file view.

SENSITIVITY: nothing is redacted. A Meld unit's /etc/config/sbi carries the
registered owner's name, contact, address and GPS. Treat this directory as
sensitive; do not publish it.
EOF

echo
echo "[*] done. Tree:"
find "$BK" -maxdepth 2 -type f | sort
echo
echo "[*] If DEST is CIFS/SMB, verify the stored copies now:"
echo "      (cd '$BK/mtd' && md5sum -c MD5SUMS.txt 2>/dev/null | grep -v OK || echo all-OK)"
