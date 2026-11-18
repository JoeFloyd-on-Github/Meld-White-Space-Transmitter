#!/usr/bin/env bash
#
# build-rootfs.sh - assemble an initramfs (cpio.gz) that runs the Meld MT300
# userspace on a generic qemu `malta` kernel. Uses the device root filesystem
# snapshot in ../root, overlays our custom /init and the sb_mips stand-in.
#
# Output: qemu-env/build/initramfs.cpio.gz
#
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
STAGE="$HERE/build/rootfs"
OUT="$HERE/build/initramfs.cpio.gz"

rm -rf "$STAGE"; mkdir -p "$STAGE"

echo "[*] copying device rootfs snapshot (excluding nested firmware copies)"
# Copy root/ but skip the big nested upgrade payloads under disk/ to keep it small.
tar -C "$REPO/root" \
    --exclude='./disk/Meld' --exclude='./disk/meldtech' \
    --exclude='./lost+found' -cf - . | tar -C "$STAGE" -xf -

echo "[*] overlaying sb_mips + custom init"
install -D "$REPO/spectrumbridge_spoof_c/sb_mips" "$STAGE/disk/usr/bin/sb_mips"
install -m 0755 "$HERE/init" "$STAGE/init"

echo "[*] fixing exec bits on device binaries"
chmod +x "$STAGE/init"
find "$STAGE" -type f -path '*bin/*' -exec chmod +x {} + 2>/dev/null || true
find "$STAGE" -type f -name '*.sh'   -exec chmod +x {} + 2>/dev/null || true

echo "[*] creating busybox applet symlinks (snapshot lacks /bin/sh etc.)"
# The device rootfs ships /bin/busybox (static) but not the applet symlinks the
# kernel/scripts need. Create the ones init + wsagent_start rely on.
APPLETS="sh ash mount umount mkdir rmdir ifconfig route udhcpc ip grep sed awk \
         cat echo ls sleep ps netstat wget stty ln chmod chown cp mv rm kill \
         dmesg hostname env cut tr head tail find xargs test true false nc \
         telnetd pidof touch date df free vi more printf sort uniq wc id uname"
for a in $APPLETS; do
  [ -e "$STAGE/bin/$a" ] || ln -sf busybox "$STAGE/bin/$a"
done
mkdir -p "$STAGE/usr/bin"
[ -e "$STAGE/usr/bin/awk" ] || ln -sf /bin/busybox "$STAGE/usr/bin/awk"

echo "[*] recreating glibc soname symlinks (snapshot stripped them)"
# Dynamic binaries (thttpd, the web UI) need the loader + glibc sonames, which
# the snapshot did not preserve. The versioned libs live in /disk/lib.
if [ -d "$STAGE/disk/lib" ]; then
  ( cd "$STAGE/disk/lib"
    for v in *-2.9.so; do :; done
    ln -sf ld-2.9.so        ld.so.1        2>/dev/null
    ln -sf libc-2.9.so      libc.so.6      2>/dev/null
    ln -sf libcrypt-2.9.so  libcrypt.so.1  2>/dev/null
    ln -sf libm-2.9.so      libm.so.6      2>/dev/null
    ln -sf libdl-2.9.so     libdl.so.2     2>/dev/null
    ln -sf libpthread-2.9.so libpthread.so.0 2>/dev/null
    ln -sf libnsl-2.9.so    libnsl.so.1    2>/dev/null
    # NSS modules (glibc getpwnam etc. dlopen these by soname at runtime).
    ln -sf libnss_files-2.9.so  libnss_files.so.2  2>/dev/null
    ln -sf libnss_dns-2.9.so    libnss_dns.so.2    2>/dev/null
    ln -sf libnss_compat-2.9.so libnss_compat.so.2 2>/dev/null
  )
  # getpwnam/gethostbyname need an nsswitch policy; the snapshot had none.
  cat > "$STAGE/etc/nsswitch.conf" <<'NSS'
passwd:     files
group:      files
shadow:     files
hosts:      files dns
NSS
  # The ELF interpreter path is hard-coded to /lib/ld.so.1.
  mkdir -p "$STAGE/lib"
  ln -sf /disk/lib/ld-2.9.so "$STAGE/lib/ld.so.1"
  # The loader is exec'd as the interpreter, so it needs the execute bit.
  find "$STAGE/disk/lib" -type f -exec chmod +rx {} + 2>/dev/null || true
fi

# Mount points the init needs.
mkdir -p "$STAGE/proc" "$STAGE/sys" "$STAGE/dev" "$STAGE/tmp"

echo "[*] packing cpio.gz (reference / alternate initramfs boot)"
( cd "$STAGE" && find . | cpio -o -H newc --quiet | gzip -9 ) > "$OUT"
ls -la "$OUT"

echo "[*] building ext3 root disk image (the malta kernel has ext3 built in)"
IMG="$HERE/build/rootfs.ext3"
rm -f "$IMG"
# mke2fs -d populates from a directory with no root privileges needed.
mke2fs -q -F -t ext3 -b 1024 -L meldroot -d "$STAGE" "$IMG" 163840
ls -la "$IMG"
echo "[*] done: $OUT and $IMG"
