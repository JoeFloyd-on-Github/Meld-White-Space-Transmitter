# 5. Finding and choosing a working kernel

[← binwalk the kernel](04-kernel-from-nand.md) · [Guide index](README.md) · [Next: the bootable rootfs →](06-rootfs-and-init.md)

Since the device's own kernel can't boot under QEMU ([chapter 4](04-kernel-from-nand.md)),
the plan becomes: **boot a stock little-endian MIPS kernel on QEMU's `malta`
board and run the device's userspace on it.** The `malta` machine is QEMU's
best-supported MIPS target — it models a real Malta development board with a
16550 console UART, a PCnet/PCI NIC, PIIX4 IDE, and an RTC, and a wide range of
generic MIPS kernels boot on it. This chapter is about getting such a kernel and
getting past the pitfalls that stop it booting.

## Finding a kernel (the dead ends matter)

The device is **MIPS32, little-endian (`mipsel`)**, so the kernel must match.
That requirement killed the first two sources:

- **OpenWrt.** Current OpenWrt (23.05) still has a `malta` target, but only the
  **big-endian** (`be`) subtarget is published — there is no `le` malta build
  any more, and older releases 404 the same way. Big-endian is useless here: the
  device binaries are little-endian and must run on a little-endian kernel.
- **Debian installer / ports.** Debian's `mipsel` malta netboot kernels have been
  removed from the mirrors (mipsel is a dropped/much-reduced port). The obvious
  `deb.debian.org` and `archive.debian.org` paths 404.

The source that *works* is the long-standing set of **QEMU MIPS test kernels and
images maintained by Aurelien Jarno** at `people.debian.org/~aurel32/qemu/mipsel/`.
These are exactly the "boots on `qemu-system-mipsel -M malta`" kernels everyone
uses for MIPS QEMU work:

```
vmlinux-2.6.32-5-4kc-malta     <-- used here
vmlinux-3.2.0-4-4kc-malta
debian_squeeze_mipsel_standard.qcow2
```

`vmlinux-2.6.32-5-4kc-malta` was the pick: it is a Debian `2.6.32` `4kc-malta`
kernel, little-endian, MIPS32 — and **2.6.32 is one minor revision from the
device's 2.6.36**, so userspace built for the device runs on it without ABI
surprises. It is committed in [`../../qemu-env/`](../../qemu-env) so the
environment is turnkey.

```sh
wget https://people.debian.org/~aurel32/qemu/mipsel/vmlinux-2.6.32-5-4kc-malta
file vmlinux-2.6.32-5-4kc-malta
# ELF 32-bit LSB executable, MIPS, MIPS32 ... statically linked, not stripped
```

### Why this stock kernel can run the device's hard-float binaries

The device's `busybox`, `thttpd` and `wsa` are **hard-float MIPS32r2**. The
`4kc-malta` kernel is a 4Kc (no FPU) build, but two things make hard-float work:

- QEMU's malta default CPU can be set to one *with* an FPU (`-cpu 24Kf`), so FP
  instructions execute natively; and
- Debian's MIPS kernels ship the **kernel FPU emulator**
  (`CONFIG_MIPS_FPU_EMULATOR`), so even a no-FPU CPU would trap-and-emulate.

This is the inverse of the qemu-user problem: the hard-float image that qemu-user
refused to *load* runs fine once a *real kernel* loads it.

## Booting it: the memory-map pitfalls

With a kernel chosen, the first boots **panicked before userspace**, and both
failures are classic QEMU-malta traps worth knowing.

### Pitfall 1 — malta's 256 MB ceiling

First attempt used `-m 512`:

```
bootmem alloc of 2097152 bytes failed!
Kernel panic - not syncing: Out of memory
```

The malta board on a 32-bit MIPS kernel **cannot address more than 256 MB**.
Passing `-m 512` produces a broken memory map and a bootmem failure at time
`0.000000` — before the console is even fully up. Fix: **`-m 256`** (or less).

### Pitfall 2 — a big initramfs at the top of RAM

Even at `-m 256`, booting with a 25 MB `initrd` panicked the same way:

```
Initial ramdisk at: 0x8e7b0000 (25406422 bytes)
bootmem alloc of 2097152 bytes failed!
Kernel panic - not syncing: Out of memory
```

QEMU loads the initramfs at the **top** of the 256 MB region (`0x8e7b0000` ≈
231 MB). A 25 MB ramdisk there leaves the kernel almost no high memory, and early
`bootmem` (which allocates low) fails to find a contiguous 2 MB. The ramdisk ate
the headroom.

The robust fix is architectural: **don't use an initramfs for a large rootfs —
boot from a root *disk image* instead.** A disk image isn't copied into RAM, so
it doesn't compete for bootmem. That decision drives the whole of
[chapter 6](06-rootfs-and-init.md). (A small initramfs — a few MB — is fine; this
rootfs is ~60 MB uncompressed, far too big to live in RAM on a 256 MB board.)

## The working command line

The result, from [`run-qemu.sh`](../../qemu-env/run-qemu.sh):

```sh
qemu-system-mipsel -M malta -cpu 24Kf -m 256 \
  -kernel vmlinux-2.6.32-5-4kc-malta \
  -append "console=ttyS0 root=/dev/sda rw rootfstype=ext3 init=/init nokaslr" \
  -drive file=rootfs.ext3,format=raw,if=ide \
  -netdev user,id=n0,hostfwd=tcp::8080-:80,hostfwd=tcp::8443-:443 \
  -device pcnet,netdev=n0 \
  -serial mon:stdio \
  -serial tcp:127.0.0.1:5556,server,nowait \
  -display none -no-reboot
```

Notes that each earn their place:

- **`-cpu 24Kf`** — an FPU-equipped CPU for the hard-float binaries.
- **`root=/dev/sda ... if=ide`** — malta's IDE appears as `/dev/sda` via libata in
  this kernel; the whole-disk ext3 image has no partition table, so `root=/dev/sda`
  (not `sda1`).
- **`rootfstype=ext3`** — *not* ext4; see the next section.
- **`-serial mon:stdio` then `-serial tcp:...`** — the **order matters**: the
  first `-serial` is guest `ttyS0` (console), the second is `ttyS1`, which we
  point at a TCP socket for the radio emulator ([chapter 7](07-radio-handshake.md)).
  `-display none` replaces `-nographic` so these explicit serials aren't
  overridden.
- **`hostfwd`** — forwards host `localhost:8080→:80` and `:8443→:443` so you can
  poke the guest's web server and database stand-in from the host browser/curl.

### ext3, not ext4 — because the kernel says so

`root=/dev/sda rootfstype=ext4` failed to mount root. The reason is that this
2.6.32 Debian kernel builds **ext4 as a module**, and with no initrd there is
nothing to load it from. Only filesystems compiled *into* the kernel can hold
root. Confirm what's built in by grepping the kernel:

```sh
strings vmlinux-2.6.32-5-4kc-malta | grep -iE "EXT[234]-fs|cramfs"
# EXT2-fs ...   EXT3-fs ...   (no EXT4)
```

ext2 and ext3 are built in; ext4 is not. So the root image is **ext3**. This is a
general move: *ask the kernel which filesystems it can mount for root before you
pick the one you format with.*

**Tips that generalise:**
- Match endianness first; a `be` kernel will never run `le` userspace.
- `people.debian.org/~aurel32/qemu/` is the reliable source of QEMU-malta MIPS
  kernels when OpenWrt/Debian mirrors fail you.
- malta is capped at 256 MB — size `-m` accordingly.
- Large rootfs → boot from a **disk image**, not an initramfs, to avoid the
  top-of-RAM bootmem squeeze.
- `strings vmlinux | grep EXT` tells you which root filesystems are built in;
  format your image to match.

[← binwalk the kernel](04-kernel-from-nand.md) · [Guide index](README.md) · [Next: the bootable rootfs →](06-rootfs-and-init.md)
