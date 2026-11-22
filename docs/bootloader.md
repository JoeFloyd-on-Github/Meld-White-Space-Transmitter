# The MT300 bootloader, flash layout, and A/B firmware banks

This is the **general** reference for the pre-Linux layer of the Meld MT300: the
U-Boot boot monitor, the NAND flash partition scheme, the A/B firmware-bank
mechanism, how to select and persist the active bank, and the kernel/hardware the
bootloader hands off to. It is written to apply to **any** MT300, not one
particular unit.

Facts tied to a specific physical unit — its NAND bad-block map, its serial and
MACs, which bank happens to be healthy — live in the per-unit notes under
[`units/`](units), e.g. [`units/mt300-av-00000601.md`](units/mt300-av-00000601.md),
not here. Getting a shell on a live unit is in
[`device-access.md`](device-access.md); the Linux-side boot scripts are in
[`boot-process.md`](boot-process.md).

A cleaned U-Boot console capture is in
[`captures/uboot-console-2026-11-22.txt`](captures/uboot-console-2026-11-22.txt).

---

## 1. Reaching the bootloader

The 4-pin UART is **115200 8N1, no flow control**. U-Boot reads keystrokes during
a short autoboot window:

```
Hit any key to stop autoboot:  5  4 3 2 1 0
```

`bootdelay` is 5 s. Any key stops autoboot and drops to the prompt:

```
XCode_Viper#
```

Because the window is short, a reliable method is to spam a key (e.g. space every
~40 ms) from power-on until the prompt appears, then send **Ctrl-C + Enter** to
clear the line (queued keypresses otherwise fill U-Boot's line editor and ring the
bell). Once Linux is up the serial console is no longer a shell — see
[`device-access.md`](device-access.md) §2.

## 2. U-Boot identity

| Fact | Value | Source |
|---|---|---|
| Boot monitor | **U-Boot 1.1.3** (build date varies by unit, e.g. Mar 18 2014) | `version` |
| SoC | XCode **Viper** ("XCode NON-USB SOC"), MIPS 74Kc | boot banner |
| Prompt | `XCode_Viper#` | — |
| Vendor note | "MELD Technology: Installed ViXs supplied Genlock patch for serial clocks" | banner |
| `TEXT_BASE` / size | `0x80000000` / `0x100000` (1 MiB) | banner |
| Board_ID | `0x00002F00` | banner / env `Board_ID` |
| NAND | Micron **MT29F4G08AAC**, **512 MiB**, 128 kB (`0x20000`) erase block | `nand info` |
| NOR flash | none (`Flash: 0 kB`) | banner |

## 3. The NAND flash layout (11 partitions)

The partition table is defined by the kernel/driver, **not** stored in NAND
(`nand partition_show` reports "Partition magic doesn't match"). From Linux
(`cat /proc/mtd`) the layout is:

| mtd | Name | Offset range | Size | Role / bank |
|----|------|------|------|------|
| mtd0 | `Boot_FW1_pja` | 0x000000–0x080000 | 512 KB | first-stage / board FW |
| mtd1 | `Boot_loader_0` | 0x080000–0x180000 | 1 MB | U-Boot, bank **A** |
| mtd2 | `Boot_loader_1` | 0x180000–0x280000 | 1 MB | U-Boot, bank **B** |
| mtd3 | `UBOOT_ENV` | 0x280000–0x300000 | 512 KB | U-Boot environment (`saveenv` target) |
| mtd4 | `Linux_Kernel_0` | 0x300000–0xb00000 | 8 MB | kernel, bank **A** |
| mtd5 | `Linux_kernel_1` | 0xb00000–0x1300000 | 8 MB | kernel, bank **B** |
| mtd6 | `RootFS_0` | 0x1300000–0x3600000 | 35 MB | root fs (yaffs2), bank **A** |
| mtd7 | `RootFS_1` | 0x3600000–0x5900000 | 35 MB | root fs (yaffs2), bank **B** |
| mtd8 | `AppFS_0` | 0x5900000–0x12c40000 | 211 MB | app/data (`/disk`), bank **A** |
| mtd9 | `AppFS_1` | 0x12c40000–0x1ff80000 | 211 MB | app/data (`/disk`), bank **B** |
| mtd10 | `BBT` | 0x1ff80000–0x20000000 | 512 KB | bad-block table |

So the device is **dual-bank (A/B)**: two complete copies of bootloader, kernel,
root filesystem, and app partition, plus a shared env and BBT.

## 4. How it boots, and the A/B bank selector

Default boot command (U-Boot env):

```
bootcmd = nboot 81000000 0; bootm 81000000
```

`nboot` loads the active **kernel** image into RAM at `0x81000000`, then `bootm`
boots it. The image is **not** a standard uImage at the raw offset — the vendor
`nboot` resolves the bank. The active bank is chosen by the env variable:

```
current_active = 0   ->  kernel from Linux_Kernel_0 (0x300000), root=/dev/mtdblock6 (RootFS_0)
current_active = 1   ->  kernel from Linux_kernel_1 (0xb00000), root=/dev/mtdblock7 (RootFS_1)
```

This was confirmed live: with `current_active=1` the console shows
`Loading kernel[1] image ... offset: 0xb00000` and `root=/dev/mtdblock6`→`mtdblock7`
changes accordingly; with `current_active=0`, `Loading kernel[0] ... offset:
0x300000` and `root=/dev/mtdblock6`. The `/disk` app partition the active rootfs
mounts also follows the bank.

The loaded kernel image carries a vendor header (first word `0x000055AA`, then a
descriptor table), handled by `nboot`/`bootm`; it is not a Linux uImage magic
(`0x27051956`).

### Selecting and persisting a bank

At the U-Boot prompt:

```sh
setenv current_active 0      # pick bank A (or 1 for bank B) — in RAM only
saveenv                      # persist to the UBOOT_ENV partition (mtd3)
reset                        # reboot; autoboot now uses the saved bank
```

`setenv` alone is **volatile** — a power cycle reverts to the saved value.
`saveenv` writes the environment to NAND ("Saving Environment to NAND... Erasing
Nand...Writing to Nand... done") so the choice survives reboots and power loss.

## 5. Recovering a unit stuck on a bad bank

Because each bank has its own rootfs, a bank whose `RootFS_n` partition is
corrupt or sitting on failed NAND will **mount but then crash userspace**, while
the other bank is fine. The signature on the serial console is:

```
VFS: Mounted root (yaffs filesystem) on device 31:7     <- the bad bank's rootfs
Freeing unused kernel memory: 192k freed
do_page_fault() #2: sending SIGSEGV to init              <- init dies, console hangs
```

The kernel mounts the rootfs but `init` (or a library it needs) lands on a bad
block and segfaults; the system hangs with no shell and no network (it will
answer ARP from a *stale* cache entry, but ping/telnet fail). **Recovery is to
switch to the healthy bank** and persist it:

1. Power-cycle and interrupt autoboot (§1).
2. `printenv current_active` — note the current (failing) bank.
3. `setenv current_active <other>` — e.g. `0` if it was stuck on `1`.
4. `run bootcmd` — test-boot the other bank (RAM-only, reversible). A healthy bank
   reaches a shell / starts telnetd with no SIGSEGV.
5. If it boots cleanly, make it the default: back at U-Boot, `setenv current_active
   <other>; saveenv`.

Check which bank is healthy before committing: `nand bad` lists the bad blocks,
and comparing their offsets to the partition map (§3) shows which `RootFS_n` is
affected. A bank whose rootfs region is free of bad blocks is the safe choice.

## 6. The kernel and hardware it hands off to

From the Linux boot log (general to the MT300-AV; exact build strings vary by
unit):

- **Kernel:** `Linux 2.6.36-ts-mipsisa32r2el` (some older units ship 2.6.29),
  gcc 4.5.3, little-endian MIPS.
- **CPU:** MIPS **74Kc**, ~**650 MHz**, FPU present.
- **RAM:** 64 MiB (`memsize 0x4000000`), ~60 MiB usable.
- **Ethernet:** Synopsys **SynopGMAC** (GMAC-UNIV); PHY reset on GPIO 4; the driver
  logs **"Can not find phy"** and eth0 runs on the XCode internal clock — matching
  U-Boot's `mii info`, which returns an identical phantom response at all 32 MDIO
  addresses (no MDIO-accessible PHY). eth0 still links and passes traffic.
- **Serial:** four builtin ARC UARTs `ttyS0`–`ttyS3`. `ttyS0` is the console;
  `ttyS1` (`0xbffff450`) is the NXP radio control port.
- **USB:** ViXs On-Chip **EHCI** + **OHCI** host controllers.
- **SATA:** two ports present (ata1/ata2), normally "link down" (no drives).
- **No RTC:** "unable to open rtc device (rtc0)" — no battery-backed clock, so the
  time starts at 1970 until set. (`wsa` tolerates this; its TLS `verify_callback`
  accepts not-yet-valid/expired certs — see [`on-device-spoof.md`](on-device-spoof.md).)
- **Filesystems compiled in:** yaffs2 (root/app), squashfs 4.0, unionfs 2.5.7,
  NTFS (R/W), XFS, UDF.

## 7. NAND bad blocks (how to read them)

`nand bad` (U-Boot) or the kernel's yaffs layer report factory/worn bad blocks.
**How many and where is per-unit**, so the actual list for a given device is in its
per-unit notes, not here. To assess a unit:

```
XCode_Viper# nand bad          # list bad-block offsets
```

Then map the offsets onto the partition table (§3): bad blocks concentrated inside
one `RootFS_n`/`AppFS_n` range indicate that bank is degraded (see §5). A healthy
chip shows only a handful of scattered bad blocks.

## 8. Command set

`help` lists all commands. Beyond stock U-Boot, several are vendor/XCode
additions (**†**):

**Boot / image:** `bootm`, `go`, `nboot`, `flashboot`† (`flashboot N address` —
boot a bank), `scsiboot`, `bootp`/`dhcp`/`tftpboot`/`nfs`/`rarpboot`, `lzo`†,
`usbioctl`† / `uui`† (USB kernel fetch / memory test), `yuv`† (YUV video data).

**NAND (`nand ...`):** `info`, `device`, `read`/`write`, `read.yaffs2`/
`write.yaffs2` (data+OOB), `erase [clean]`, `read.oob`/`write.oob`, `bad`, and the
vendor `partition_show`† / `partition_read`† / `partition_write`†.

**Environment:** `printenv`, `setenv`, `saveenv`, `run`, `echo`, `savemenu`†
(save a boot menu to on-board EEPROM).

**Memory / test:** `md`, `mw`, `mm`, `nm`, `cp`, `cmp`, `crc32`/`icrc32`, `base`,
`loop`/`iloop`, `mtest`, `mss`† (SSO RAM test), `imd`/`imm`/`imw`/`inm` (I2C),
`iprobe` (I2C scan — typically finds no chips on this board), `mii` (PHY regs).

**Flash (NOR — unused):** `erase`, `protect`, `flinfo`. **Filesystems:** `fatls`/
`fatload`/`fatinfo`, `ext2ls`/`ext2load`. **Misc:** `ping`, `reset`, `version`,
`help`/`?`, `scsi`, USB sub-system. Absent: `bdinfo`.

### Destructive commands
`nand erase`, `nand write*`, `erase`, `mw`/`mm`/`cp` into live regions, `saveenv`
(rewrites the env), `savemenu`, and `nand partition_write` modify the device.
Everything used for the surveys in this repo was read-only except the deliberate,
authorized `saveenv` used to set the default bank (§4).

## 9. Leaving the bootloader

- `reset` — reboot the CPU (re-runs autoboot with the saved bank).
- `run bootcmd` — boot the active bank now.
- For a serial **root shell** instead of the AV app owning the console, boot the
  kernel with `init=/bin/sh` appended — see [`device-access.md`](device-access.md)
  §2. Not needed for normal access (telnet is simpler).
