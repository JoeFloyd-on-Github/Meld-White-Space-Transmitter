# 4. Recovering the device's own kernel with binwalk

[← user vs system](03-user-vs-system.md) · [Guide index](README.md) · [Next: a working kernel →](05-malta-kernel.md)

Full-system emulation needs a kernel. The obvious candidate is the *device's own*
kernel — so the first move was to go get it out of the flash. This chapter is the
binwalk investigation: how the kernel was located and extracted, what it turned
out to be, and the detailed reasons it cannot be booted under QEMU — which is
what forced the stock-kernel detour in [chapter 5](05-malta-kernel.md).

## The raw material: a full NAND dump

Beyond the filesystem snapshot in [`../../root`](../../root), a **complete NAND
flash dump** of a sibling unit existed. (Its exact provenance is recorded in the
project's memory, not in this public repo.) It is a ~618 MB file, and the first
thing to learn about any such dump is *how it was made*, because that dictates how
to take it apart. The dump's own shell history gave it away:

```sh
( find / ... | cpio -ov -H newc;
  for i in 0 1 2 3 4 5 6 7 8 9 10; do echo "MTD_BLOCK_$i"; cat /dev/mtdblock$i; done
) | nc -l -p 443
```

So the file is a `newc` cpio archive of the running rootfs, **followed by the raw
contents of every MTD partition** (`/dev/mtdblock0` … `mtdblock10`), each one
preceded by a plain-ASCII marker line `MTD_BLOCK_N`. That is a gift: the
partitions are delimited by greppable text.

### Carving the partitions

Find the markers and their byte offsets in one pass:

```sh
grep -aboE "MTD_BLOCK_[0-9]+" dump.bin
# 81949552:MTD_BLOCK_0
# 82473852:MTD_BLOCK_1
# 83522440:MTD_BLOCK_2
# 84571028:MTD_BLOCK_3
# 85095328:MTD_BLOCK_4
# 93483948:MTD_BLOCK_5
# ...
```

The gaps between markers give each partition's size, which immediately sketches
the flash layout (each marker line is `MTD_BLOCK_N\n`, 12 bytes, so payload starts
12 bytes after the offset grep reports):

| mtdblock | size | role (inferred from size + content) |
|---|---|---|
| 0 | 512 KB | U-Boot bootloader |
| 1–2 | 1 MB each | U-Boot support / splash |
| 3 | 512 KB | **U-Boot environment** (`fw_env.config` → `/dev/mtd3`) |
| **4** | **8 MB** | **kernel image A** |
| **5** | **8 MB** | kernel image B (the other half of the A/B pair) |
| 6 | 35 MB | application / rootfs image A (yaffs2) |
| 7 | 35 MB | application / rootfs image B |

The **two 8 MB kernel partitions** are the A/B failsafe pair that `actimage`
selects between (see [`../boot-process.md`](../boot-process.md)). Carve one out
with a byte-exact `dd`:

```sh
# payload of MTD_BLOCK_4 starts at 85095328 + 12, runs to the next marker
dd if=dump.bin of=mtd4.bin bs=1M iflag=skip_bytes,count_bytes \
   skip=85095340 count=8388608
```

> **Pitfall — the truncated CIFS copy.** The dump lived on a network (CIFS)
> share mounted `soft`, and a plain `cp` of the 618 MB file returned success but
> produced only ~214 MB — a silent short read. It didn't matter here because the
> kernel partitions sit early in the file and were intact, but **always verify
> the size of a large copy off a network mount** (`ls -l`, or compare to the
> source) before trusting it.

## binwalk: it's a 2.6.36 uImage

```sh
$ binwalk mtd4.bin
DECIMAL   HEX    DESCRIPTION
0         0x0    uImage header, header size: 64 bytes, ... Data Address: 0x8C000000,
                 Entry Point: 0xAC040000, OS: Linux, CPU: MIPS, image type: OS Kernel
                 Image, compression type: gzip, image name: "Mips Linux 2.6.36 Kernel MT300"
64        0x40   gzip compressed data, has original file name: "vmlinux.mem"
```

A U-Boot `uImage`: a 64-byte header followed by a gzip-compressed raw kernel
(`vmlinux.mem`). Extract and inspect the kernel itself:

```sh
dd if=mtd4.bin bs=64 skip=1 | gunzip > vmlinux.mem
strings vmlinux.mem | grep -m1 "Linux version"
# Linux version 2.6.36-ts-mipsisa32r2el (root@Dell-5150) (gcc version 4.5.3 (GCC)) #67 ...
```

So the device runs **Linux 2.6.36**, MIPS32r2, little-endian, built in July 2014.
(The malta kernel we end up using is 2.6.32 — one minor revision away, reassuring
for userspace ABI compatibility.)

## Why this kernel cannot be booted under QEMU

Having the real kernel in hand, the natural hope is `qemu-system-mipsel -kernel
vmlinux.mem`. It does not work, and the reasons are instructive — they are the
same reasons *any* vendor SoC kernel resists a generic emulator.

### 1. It is built for a board QEMU does not model

The uImage load/entry addresses are **`0x8C000000` / `0xAC040000`**. A stock
QEMU-`malta` MIPS kernel loads around `0x80000000` (kseg0) with an entry in the
same region. `0x8C...`/`0xAC...` reflect the **ViXS XCode SoC's** physical memory
map and its use of kseg1 (uncached) for early entry — a layout QEMU's malta
machine does not provide. Load it on malta and it executes against the wrong RAM
base and the wrong peripheral addresses.

### 2. Its drivers are compiled *in*, for hardware that isn't emulated

`strings` on the kernel shows the SoC is welded into the image, not loaded later:

```
xcode_hibernation_enter   xcode_pm_end   xcode_hibernation_restore_cleanup   ...
```

These `xcode_*` routines are the **ViXS power-management and video pipeline built
straight into the kernel**. The earliest boot steps — timer, interrupt
controller, and above all the **console UART** — are initialised for ViXS
peripherals at ViXS addresses. QEMU-malta exposes a *different* interrupt
controller (Malta FPGA + GT64120/PIIX4) and a 16550 UART at the PC-standard
`0x3f8`. The ViXS kernel, probing its own hardware and finding malta's instead,
**never gets a working console and panics (or hangs) before printing a line** —
there is no output to even debug with.

### 3. Built-in vs. loadable — and why both hurt

The device splits its hardware support two ways, and each is a problem:

- **Built into the kernel:** the ViXS core (`xcode_*`), the yaffs2 filesystem
  (the `yaffs.*` module parameters are present in the image), and the board's
  console/timer/IRQ setup. These run unconditionally at boot and assume ViXS
  hardware. You cannot disable them; they are the kernel.
- **Loaded as out-of-tree modules (LKMs):** the video codec driver ships
  separately as **`xcode4drv.ko`** on the rootfs. Even if the base kernel somehow
  booted, the device's full function depends on binary `.ko` modules built
  against *this exact 2.6.36 ViXS kernel's* symbol versions. They will not load
  into any other kernel, and QEMU has no device for them to drive anyway.

The combination is the key insight:

> The device's kernel is **inseparable from the device's silicon.** Its built-in
> drivers assume ViXS hardware that QEMU cannot present, and its out-of-tree
> modules are bound to that one kernel build. Booting it under a generic emulator
> is not a matter of the right command line — there is no machine for it to run
> on.

### What the extraction *was* worth

Even though the kernel can't boot under QEMU, recovering it paid off:

- It **pinned the kernel version** (2.6.36) and toolchain (gcc 4.5.3), which
  guided the choice of a close stock kernel (2.6.32) and gave confidence the
  userspace ABI would match.
- It **confirmed the filesystem type** (yaffs2) and the A/B image scheme,
  corroborating the boot-process analysis.
- It **proved the ViXS dependency** concretely (the `xcode_*` symbols), turning
  "malta won't work" from a guess into a demonstrated fact — and justifying the
  stock-kernel approach instead of a long, doomed fight to boot the original.

**Tips that generalise:**
- A firmware blob's *capture method* (often in a shell-history or README beside
  it) tells you its structure. Read that before carving.
- ASCII delimiters + `grep -abo` beat guessing offsets; `dd` with
  `iflag=skip_bytes,count_bytes` carves at exact byte boundaries.
- `binwalk` a kernel partition to find the uImage/compression; `gunzip` the
  payload and `strings | grep "Linux version"` to identify it in one line.
- A vendor SoC kernel is usually *not* bootable on a generic machine model.
  Extract it to **learn from** (version, drivers, fs), then boot a **stock**
  kernel for the same ISA to host the userspace.

[← user vs system](03-user-vs-system.md) · [Guide index](README.md) · [Next: a working kernel →](05-malta-kernel.md)
