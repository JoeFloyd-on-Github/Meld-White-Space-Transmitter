# 3. User mode vs system mode: why the switch was necessary

[← qemu-user](02-qemu-user.md) · [Guide index](README.md) · [Next: binwalk the kernel →](04-kernel-from-nand.md)

QEMU has two fundamentally different emulation modes. Understanding the boundary
between them is the single most important architectural decision in this project,
so it gets its own short chapter.

## The two worlds

| | **qemu-user** (`qemu-mipsel`) | **qemu-system** (`qemu-system-mipsel`) |
|---|---|---|
| What it emulates | one **process** | a whole **machine** |
| Kernel | the **host** kernel (syscalls translated) | a **guest** kernel you supply (`-kernel`) |
| CPU | the target CPU for one process | the target CPU + a board (`-M malta`) |
| Devices | host files/sockets by name | emulated UARTs, NICs, IDE, RTC, framebuffer |
| ELF loading | qemu's own user-mode loader | the **guest kernel's** loader |
| Threads/procs | one process, host-scheduled | real processes under the guest kernel |
| Filesystem | the host filesystem | a guest root (disk image or initramfs) |
| Boot | none — just `exec` | real: kernel → init → userspace |
| Setup cost | trivial | build a kernel + a root filesystem |

The slogan: **qemu-user runs a program on your kernel; qemu-system runs a kernel
on your CPU.**

## Why `wsa` needs system mode — three independent reasons

Any *one* of these would force the switch. `wsa` has all three.

1. **It won't load in user mode.** As shown in
   [chapter 2](02-qemu-user.md), qemu-user's ELF loader rejects the binary before
   `main`. Under qemu-system, the **guest kernel** maps and starts the ELF —
   exactly as the real device does — and the exotic uClibc/hard-float/MIPS32r2
   image loads without complaint. The loader problem simply disappears, because
   a real Linux loader is doing the work.

2. **It needs a real serial port with something on the other end.** `wsa`'s
   control loop will not reach the network until the radio on `/dev/ttyS1`
   answers with an FCC ID and serial number ([chapter 7](07-radio-handshake.md)).
   In user mode, `/dev/ttyS1` is whatever host path happens to exist — you cannot
   put an emulated UART under it and feed bytes in. In system mode, the malta
   board *has* a 16550 UART exposed as guest `ttyS1`, and QEMU can wire its other
   end to a socket where a radio emulator lives.

3. **It needs siblings.** The real scenario is several processes at once — the
   agent, the stand-in database server on `:443`, the web server on `:80` — all
   sharing config files and talking over loopback. That is an init-plus-services
   system, which is precisely what qemu-system provides and qemu-user does not.

## What system mode costs you (and this guide pays)

Full-system emulation is not free. You now owe the machine three things the
process-mode world handed you for nothing:

- **A kernel** that boots on the emulated board *and* can run your binaries —
  which, critically, is **not** the device's own kernel ([chapter 4](04-kernel-from-nand.md)
  explains why), so you must find or build a compatible stock one
  ([chapter 5](05-malta-kernel.md)).
- **A root filesystem** the kernel can mount and `init` from — which means
  turning a filesystem *snapshot* into a genuinely *bootable* image, repairing
  everything the snapshot lost ([chapter 6](06-rootfs-and-init.md)).
- **The devices and wiring** — serial for the radio, a NIC for the network,
  host port-forwards to reach the services ([chapter 8](08-wsa-sb-and-web.md)).

The payoff is decisive: under system mode the *actual, unmodified* `wsa` binary
runs its real threads, opens real sockets, speaks real TLS, and writes real
config files — the full behaviour, not a reconstruction.

## A useful hybrid

The two modes are not mutually exclusive. This project used **both**:

- qemu-**user** to run the soft-float `sb_mips` server and replay-test the
  database protocol quickly, with no boot (`test_replay.sh`).
- qemu-**system** to run the real `wsa` end to end against that same server
  inside a booted device userspace (`qemu-env/`).

Use the cheap tool where it works and the heavy tool only where you must.

**Tip that generalises:** decide between the modes by asking what the binary
*touches*. Only CPU + syscalls the host provides → user mode. A device node that
must be emulated, a kernel feature the host lacks, a specific kernel/loader, or
cooperating processes → system mode. "It wouldn't run in qemu-user" is a reason
to reach for qemu-system, not a dead end.

[← qemu-user](02-qemu-user.md) · [Guide index](README.md) · [Next: binwalk the kernel →](04-kernel-from-nand.md)
