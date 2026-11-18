# 9. Tips, tricks and pitfalls — a field guide

[← the database cycle & web](08-wsa-sb-and-web.md) · [Guide index](README.md)

A consolidated reference of the traps that cost the most time and the one-liners
that got out of them. Grouped by theme; each links back to where it came up.

## QEMU & emulation strategy

- **qemu-user first, qemu-system when forced.** User mode is minutes of setup and
  often enough. Switch to system mode the moment a binary won't *load*, needs an
  emulated *device*, needs a specific *kernel*, or needs *sibling processes*.
  ([ch.2](02-qemu-user.md), [ch.3](03-user-vs-system.md))
- **A vendor SoC kernel usually won't boot on a generic machine.** Extract it to
  learn from (version, drivers, fs); boot a stock same-ISA kernel to host the
  userspace. ([ch.4](04-kernel-from-nand.md))
- **Match endianness before anything.** `mipsel` userspace needs a `mipsel`
  kernel; an OpenWrt `be` image is useless for an `le` device. ([ch.5](05-malta-kernel.md))
- **`people.debian.org/~aurel32/qemu/` is the reliable QEMU-MIPS kernel source**
  when OpenWrt and Debian mirrors have dropped your variant. ([ch.5](05-malta-kernel.md))
- **Use both modes.** Replay-test the server in qemu-user (fast, no boot); run the
  real agent in qemu-system (faithful). ([ch.3](03-user-vs-system.md), [ch.8](08-wsa-sb-and-web.md))

## Diagnosing "it didn't run / did nothing"

- **The pipe exit-code trap.** `cmd | head; echo $?` reports *head's* status, not
  `cmd`'s. Capture the real exit: `cmd >o 2>e; echo $?`. This hid a total load
  failure for a while. ([ch.2](02-qemu-user.md))
- **stdio buffering hides output under emulation.** A program's stdout is fully
  buffered to a file, so logs look empty. Point it at a **tty** (`/dev/console`)
  for line buffering, or `setvbuf(stdout, NULL, _IONBF, 0)` in code you control.
  ([ch.6](06-rootfs-and-init.md), [ch.7](07-radio-handshake.md))
- **When qemu-user's `-strace` is empty, strace qemu itself:**
  `strace -f -qq qemu-mipsel ./bin` shows the translated guest calls and the
  loader's `exit_group`. ([ch.2](02-qemu-user.md))
- **"Runs but does nothing" → read `wchan` + fds per thread.**
  `cat /proc/<pid>/wchan` and `ls -l /proc/<pid>/fd`. Blocked in `poll` on a tty
  with no socket open = waiting on a peripheral, not the network. ([ch.7](07-radio-handshake.md))
- **uClibc LinuxThreads = one PID per thread.** `/proc/<pid>/task` shows only one;
  iterate the separate PIDs from `ps`. ([ch.6](06-rootfs-and-init.md))
- **Remove your own probes before reading the target's logs.** A stray
  `nc 127.0.0.1 443` made the TLS server log a fake "bad ClientHello" (`0x47`
  = `'G'`) that looked like the target misbehaving. ([ch.7](07-radio-handshake.md))
- **`debugfs -R "cat /path" image.ext3`** reads files/logs out of a disk image
  after the VM dies — the way to catch a daemon that crashes mid-boot. Have the
  daemon log to the **persistent disk**, not `tmpfs`. ([ch.6](06-rootfs-and-init.md))

## Firmware / flash forensics

- **Read the capture method first.** A dump's own shell-history/README beside it
  tells you its structure (here: cpio + concatenated `mtdblock`s with ASCII
  markers). ([ch.4](04-kernel-from-nand.md))
- **ASCII markers + `grep -abo` beat guessing offsets;** carve with
  `dd ... iflag=skip_bytes,count_bytes skip=<off> count=<len>`. ([ch.4](04-kernel-from-nand.md))
- **Identify a kernel in one line:** `binwalk part.bin` → find the uImage/gzip,
  then `dd bs=64 skip=1 | gunzip > vmlinux && strings vmlinux | grep "Linux version"`.
  ([ch.4](04-kernel-from-nand.md))
- **`strings vmlinux | grep -iE "EXT[234]-fs"`** tells you which filesystems are
  built in (can hold root) vs. modular. ([ch.5](05-malta-kernel.md))
- **`strings` on a kernel exposes its SoC** (`xcode_*` here) — proof it's board-
  specific. ([ch.4](04-kernel-from-nand.md))
- **Verify large copies off network mounts.** A CIFS `soft` mount silently
  truncated a 618 MB `cp` to ~214 MB. Check `ls -l` against the source. ([ch.4](04-kernel-from-nand.md))

## Booting a stock kernel on malta

- **malta caps at 256 MB.** `-m 512` → `bootmem alloc failed` panic. Use `-m 256`.
  ([ch.5](05-malta-kernel.md))
- **A big initramfs panics early.** QEMU loads it at the top of RAM, starving
  bootmem. Boot from a **disk image** for anything but a tiny rootfs. ([ch.5](05-malta-kernel.md))
- **Pick the root fs the kernel has built in** (ext3 here, not ext4). ([ch.5](05-malta-kernel.md))
- **Whole-disk image → `root=/dev/sda`** (no partition table, so no `sda1`);
  malta IDE is `/dev/sda` via libata. ([ch.5](05-malta-kernel.md))
- **`-serial` order maps to `ttyS0, ttyS1, …`.** Use `-serial mon:stdio` +
  `-serial tcp:...,server` and `-display none` (not `-nographic`, which injects
  its own serial). ([ch.5](05-malta-kernel.md), [ch.7](07-radio-handshake.md))

## Turning a snapshot into a bootable rootfs

- **`mke2fs -d <dir> image N`** builds a populated fs with no root and no loop
  mount. ([ch.6](06-rootfs-and-init.md))
- **Snapshots lose symlinks, device nodes and exec bits.** Expect to recreate:
  - `/bin/sh` + busybox applet symlinks (else "No init found"). ([ch.6](06-rootfs-and-init.md))
  - execute bits — **root can't `exec` a file with no `x` bit**, loader included
    ("Permission denied"). ([ch.6](06-rootfs-and-init.md))
  - glibc sonames (`libc.so.6 → libc-2.9.so`) and the hard-coded ELF interpreter
    `/lib/ld.so.1` (missing interpreter = "not found" on an existing binary).
    ([ch.6](06-rootfs-and-init.md))
  - `libnss_*.so.N` sonames + `/etc/nsswitch.conf` (else glibc `getpwnam` /
    `gethostbyname` fail; thttpd: "unknown user - 'root'"). ([ch.6](06-rootfs-and-init.md))
- **Let devtmpfs make `/dev`.** Mount it first in init, then
  `exec </dev/console >/dev/console 2>&1`. ([ch.6](06-rootfs-and-init.md))
- **Don't assume busybox applets exist** — `setsid`, `timeout`, `readlink` were
  absent; `exec setsid sh` killing init → "Attempted to kill init!" panic. Use
  `/bin/busybox sh`. ([ch.6](06-rootfs-and-init.md))
- **Write a minimal custom `/init`** rather than reusing the device's hardware-
  specific `rcS`. ([ch.6](06-rootfs-and-init.md))

## Peripheral handshakes & TLS

- **A device often gates its main function on a sub-component's identity reply.**
  `wsa` won't hit the network until the radio reports FCC ID + serial over serial.
  Emulate just enough of the peripheral to answer. ([ch.7](07-radio-handshake.md))
- **Wire an emulated UART to a scripted host peer** via `-serial tcp:...,server`.
  ([ch.7](07-radio-handshake.md))
- **To meet an ancient TLS client you usually need two changes:** compile the old
  protocol version back in **and** lower the library's version floor. One without
  the other still fails. ([ch.8](08-wsa-sb-and-web.md))
- **Turn on handshake debug before guessing** (`MG_MBEDTLS_DEBUG_LEVEL=4`,
  `openssl s_client -msg`). A record type of `22`/`0x16` is a real handshake;
  `0x47` is someone sending "GET". ([ch.8](08-wsa-sb-and-web.md))
- **Read the client's trust logic.** Hard-coded port + no CN check + self-signed
  accepted → a redirect and a throwaway cert are enough. ([ch.8](08-wsa-sb-and-web.md))

## Methodology

- **Leave the target binary unmodified; change the environment around it.** Every
  obstacle here was solved outside `wsa` (emulate the radio, redirect the host,
  answer on the port, supply the identity) so the observed behaviour is the real
  device's. ([ch.7](07-radio-handshake.md))
- **Find the one decision the device won't make without the cloud**, and drive
  everything toward being able to feed it that answer. ([ch.1](01-the-device.md))
- **Script self-stopping runs** (`SECS=75 ./run-qemu.sh`) and have init print a
  single `RESULT` block, so each iteration is one command and one readable
  output. ([ch.6](06-rootfs-and-init.md))

[← the database cycle & web](08-wsa-sb-and-web.md) · [Guide index](README.md)
