# QEMU environment: running the MT300 userspace and the full wsa ↔ sb cycle

This directory boots the **Meld MT300 userspace under QEMU** and drives the real
`whitespaceagent` ("wsa") through a complete register → channel cycle against the
on-device SpectrumBridge stand-in (`sb_mips`), with a stand-in radio so wsa gets
past its hardware handshake. It is the end-to-end proof behind
[`../docs/on-device-spoof.md`](../docs/on-device-spoof.md).

## Why a generic kernel (not the device's own)

The device's real kernel was recovered from the NAND dump — it is
`Mips Linux 2.6.36 Kernel MT300` (`Linux 2.6.36-ts-mipsisa32r2el`, a uImage at
NAND `mtdblock4`, gzip `vmlinux.mem`, load `0x8C000000`/entry `0xAC040000`). It
is built for the **ViXS XCode SoC** (its symbol table is full of `xcode_*`
routines) and QEMU has no ViXS machine model, so it cannot boot on QEMU. Instead
we boot a stock little-endian **malta** kernel (`vmlinux-2.6.32-5-4kc-malta`,
Debian — 2.6.32 is one minor rev off the device's 2.6.36) and run the device's
*userspace* on it. The device binaries are static (busybox, wsa) or dynamic
glibc (thttpd); both run under a real kernel, which is why wsa works here even
though it will not load under `qemu-user` (its uClibc/hard-float image is
rejected by the user-mode ELF loader).

What's absent on malta — the ViXS video pipeline, `/dev/micro`, and the real
radio — does not matter for the database path. The one piece wsa *requires* is
the radio handshake (it will not contact the database until the radio reports an
FCC ID and serial, see `../docs/wsa-overview.md` §6.2), so we emulate just that.

## Architecture

```
  qemu-system-mipsel -M malta
  ┌─────────────────────────────────────────────── guest ───────────────┐
  │  init → busybox userspace                                            │
  │    sb_mips  :443  ──TLS1.0──┐                                        │
  │    thttpd   :80  (web UI)   │                                        │
  │    whitespaceagent ─────────┘  register PUT + channel GET            │
  │         │  serial (/dev/ttyS1)                                       │
  └─────────┼───────────────────────────────────────────────────────────┘
            │ ttyS1 ⇄ TCP :5556
       radio-emu.py (host)  → answers x/y/l/h: S/N 371, FCC ID OKVMT300
```

- Root is an **ext3** disk image built from [`../root`](../root) (the malta
  kernel has ext2/3 built in, not ext4). `build-rootfs.sh` also recreates the
  bits the snapshot lost: `/bin/sh` and other busybox applet symlinks, the exec
  bits, the glibc soname symlinks + `/lib/ld.so.1`, and an `/etc/nsswitch.conf`
  (without which glibc `getpwnam("root")` fails and thttpd won't start).
- The **radio** is emulated on the host by `radio-emu.py`, wired to guest
  `/dev/ttyS1` through a QEMU serial TCP socket.
- The **redirect** of `tvws.spectrumbridge.com` to loopback is done in `init`
  (both `/etc/hosts` and, since wsa's resolver can't be relied on, the robust
  `HostAddress → 127.0.0.1` in `sbi`).

## Run it

Needs `qemu-system-mipsel` (`sudo apt-get install qemu-system-mips`), `python3`,
and `mke2fs`. `sb_mips` must be built first (`../spectrumbridge_spoof_c/build.sh`).

```sh
./build-rootfs.sh     # build the ext3 root image from ../root + sb_mips
./run-qemu.sh         # boot; starts radio-emu.py automatically
```

`init` prints a RESULT block and drops to a shell. For a scripted, self-stopping
run: `SECS=75 ./run-qemu.sh`. Host port-forwards while it runs:
`http://localhost:8080` (web UI) and `https://localhost:8443` (sb_mips).

## What it proves (observed)

```
radio:   S/N = 371     FCC ID = OKVMT300
sb:      PUT /v3/devices/US/OKVMT300/371        -> 200, SBI-Status 0
sb:      GET /v3/channels/US/38.8132280/-77.0443610/?... -> 200, 11 channels
wsa:     /etc/config/channels = '21,24,27,30,33,36,39,42,45,48,51'
web:     GET /status.php -> <title>MELDTECH - White Space Status</title>
```

The real wsa binary resolved the (redirected) database host, completed a TLS 1.0
handshake with `sb_mips`, registered, requested channels, parsed the response,
and wrote the channel list it would drive the radio with — i.e. **`sb` satisfied
wsa's database lookup**, start to finish.

## Files

| File | What it is |
|---|---|
| `build-rootfs.sh` | builds the ext3 root image (and a cpio initramfs) from `../root` + `sb_mips`, repairing the symlinks/perms the snapshot dropped |
| `init` | guest init: mounts, network, redirect, starts sb_mips/thttpd, runs wsa, prints the RESULT |
| `radio-emu.py` | host-side NXP-radio stand-in on guest ttyS1 (answers `x`/`y`/`l`/`h`/`b?`) |
| `run-qemu.sh` | launches QEMU (malta) + the radio emulator with the serial/net wiring |
| `vmlinux-2.6.32-5-4kc-malta` | the stock mipsel malta kernel (Debian) used to boot |
| `build/` | generated toolchain-free artifacts (root image, staging, extra kernels) — gitignored |

Nothing here runs the device's own kernel; it runs the device's userspace on a
compatible stock kernel. The real kernel is documented above for reference.
