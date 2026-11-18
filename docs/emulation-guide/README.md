# From a binary called `wsa` to a QEMU test bench — a definitive guide

This is the long-form story of how a single MIPS binary, `wsa`, pulled off a
dead device was turned into a **fully emulated Meld MT300** running under QEMU,
where its behaviour could be observed and its dependence on a shut-down cloud
service replaced with a local stand-in.

It is written two ways at once: as a **narrative** of what was tried, what
broke, and how each wall was climbed; and as a **reusable playbook** for anyone
emulating an embedded Linux appliance from a firmware image. Nearly every
section ends with the concrete tip that generalises beyond this device.

## Why this device matters: a $-thousand radio turned to e-waste

The Meld MT300 is a TV-white-space transmitter — a real, FCC-certified
(`FCC ID OKVMT300`) broadcast radio that cost thousands of dollars. It is not
broken. Its CPU boots, its radio powers on, its web UI serves. Yet it will not
transmit a single watt, because before every transmission it must ask an online
database — **`tvws.spectrumbridge.com`** — which channels are legal to use at
its location. That service was shut down. The host no longer resolves. The
device, by design, refuses to transmit without an answer, so it does nothing.

This is the quiet way a lot of "smart" hardware dies: not a failed capacitor, but
a failed *dependency*. The silicon outlives the company, the certificate, or the
API. A device that is 100% functional becomes e-waste the day a server is
decommissioned — and the owner, who paid for the hardware, never owned the one
piece that mattered.

The work documented here is the opposite move: take the dependency back. Stand up
the database locally, prove the exact wire protocol the device needs, and build
an emulator faithful enough to run the device's own software and watch it come
back to life. The same approach applies to any appliance stranded by a dead
cloud service.

## The journey (read in order, or jump)

1. **[The device and the stranding](01-the-device.md)** — what the MT300 is,
   how it decides to transmit, and the precise mechanism by which a service
   shutdown bricks it.
2. **[qemu-user: the fast path and its wall](02-qemu-user.md)** — setting up
   user-mode emulation, straceing MIPS binaries, what ran, and the hard limit
   that `wsa` would not load at all.
3. **[User mode vs system mode](03-user-vs-system.md)** — the real difference
   between the two QEMU worlds, and why only full-system emulation could run the
   agent.
4. **[Recovering the device's own kernel with binwalk](04-kernel-from-nand.md)**
   — the full NAND dump, carving the MTD partitions, extracting the 2.6.36
   kernel, and why its built-in ViXS features and out-of-tree modules make it
   impossible to boot under QEMU.
5. **[Finding and choosing a working kernel](05-malta-kernel.md)** — the dead
   ends (OpenWrt, Debian ports), the canonical QEMU malta kernels, and the
   memory-map pitfalls that panic the kernel before it starts.
6. **[Building a bootable userspace](06-rootfs-and-init.md)** — turning a
   filesystem *snapshot* into a *bootable* root image: the symlinks, exec bits,
   loaders and config files the snapshot silently dropped, and a custom init.
7. **[The radio handshake](07-radio-handshake.md)** — why the agent blocks
   forever without a radio, the NXP serial protocol, and the stand-in that
   unblocks it — all while leaving `wsa` byte-for-byte unmodified.
8. **[The database cycle and the web UI](08-wsa-sb-and-web.md)** — redirecting
   the dead host, the TLS 1.0 interop fight, the full register→channel cycle
   against the stand-in, and how the web server and the agent talk to each other.
9. **[Tips, tricks and pitfalls](09-tips-and-pitfalls.md)** — a consolidated
   field guide: the traps that cost the most time and the one-liners that got
   out of them.

## The companions

This guide is the *story*. The *reference* material it builds on lives next to it:

- [`../wsa-overview.md`](../wsa-overview.md) — the behaviour of the `wsa` binary.
- [`../system-context.md`](../system-context.md) — the device, config pipeline, firmware.
- [`../boot-process.md`](../boot-process.md) — boot, reset button, IP, hardware.
- [`../on-device-spoof.md`](../on-device-spoof.md) — the on-device database stand-in.
- [`../../qemu-env/`](../../qemu-env) — the runnable environment this guide produced.
- [`../../spectrumbridge_spoof_c/`](../../spectrumbridge_spoof_c) — the stand-in server.

## The one-paragraph ending

A static binary with debug symbols, a filesystem snapshot, and a full flash dump
were enough to rebuild the device in software, prove what its dead server used to
say, and feed it that answer again from a 900 KB program running on the device
itself. Nothing about the hardware needed fixing. Everything about the
*dependency* did. That is both the good news (these devices are recoverable) and
the warning (they shouldn't have needed recovering).
