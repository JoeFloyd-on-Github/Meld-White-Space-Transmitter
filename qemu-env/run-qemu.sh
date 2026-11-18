#!/usr/bin/env bash
#
# run-qemu.sh - boot the Meld MT300 userspace on a generic qemu `malta` board,
# with a stand-in radio on guest ttyS1 so wsa's serial thread unblocks and the
# database cycle runs.
#
# The device's own 2.6.36 ViXS kernel (extracted from the NAND dump, load addr
# 0x8C000000) cannot run on qemu's malta machine, so we use a stock mipsel malta
# kernel and the device's userspace on an ext3 disk image (see build-rootfs.sh).
#
#   ttyS0 -> console (stdio)
#   ttyS1 -> TCP :5556, where radio-emu.py answers the NXP radio protocol
#   http://localhost:8080  -> guest thttpd :80
#   https://localhost:8443 -> guest sb_mips :443
#
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
KERNEL="${KERNEL:-$HERE/vmlinux-2.6.32-5-4kc-malta}"
DISK="${DISK:-$HERE/build/rootfs.ext3}"
SECS="${SECS:-0}"
RADIO_PORT="${RADIO_PORT:-5556}"

# Start the radio emulator in the background; it retries until qemu listens.
python3 "$HERE/radio-emu.py" > /tmp/radio.log 2>&1 &
RADIO_PID=$!
trap 'kill $RADIO_PID 2>/dev/null' EXIT

CMD=(qemu-system-mipsel -M malta -cpu 24Kf -m 256
     -kernel "$KERNEL"
     -append "console=ttyS0 root=/dev/sda rw rootfstype=ext3 init=/init nokaslr"
     -drive file="$DISK",format=raw,if=ide
     -netdev user,id=n0,hostfwd=tcp::8080-:80,hostfwd=tcp::8443-:443
     -device pcnet,netdev=n0
     -serial mon:stdio
     -serial "tcp:127.0.0.1:${RADIO_PORT},server,nowait"
     -display none -no-reboot)

if [ "$SECS" -gt 0 ]; then
  timeout "$SECS" "${CMD[@]}" < /dev/null
else
  "${CMD[@]}"
fi
