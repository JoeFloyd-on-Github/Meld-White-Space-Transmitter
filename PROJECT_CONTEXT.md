# Project context — Meld White Space Transmitter

Persistent notes for future sessions (human or Claude): **why** this repo exists,
what has been done, the decisions made along the way, and what is still open.
Last updated 2026-11-22. Update this file when a goal, decision or open item
changes.

## Goal

Bring a working-but-inert **Meld Technology MT300** TVWS transmitter (FCC ID
`OKVMT300`, unit serial 371) back to life. The hardware is fine; the device will
not transmit because `whitespaceagent` must first get a channel list from the
SpectrumBridge white-space database (`tvws.spectrumbridge.com`), which no longer
exists. The fix: reproduce the database's answer locally and give it back to the
device — without modifying the original binary.

A second, explicit goal is **documentation as advocacy**: the README's essay uses
the MT300 as a clean example of hardware turned into e-waste by a dead cloud
dependency, and argues that ownership is hollow when use needs an always-on
check. Keep that framing intact when editing the README.

## What has been done (in order)

1. **Binary + spoof servers.** Collected `wsa` (v44, Nov 2013) and wrote stand-in
   SpectrumBridge servers in Python (`spectrumbridge_spoof_python/`, plain + SOAP)
   and C (`spectrumbridge_spoof_c/`, Mongoose + mbedTLS 2.28.2).
2. **Decompile.** Full Ghidra 12.2 decompile of `wsa`
   (`WhiteSpaceApp/ghidra/wsa_decompiled.c` raw, `wsa_annotated.c` fully
   annotated, with a variable legend).
3. **Device snapshot + firmware.** `root/` is the running device's rootfs;
   `upgrade/` is the `meldtech` 1.21 upgrade package. The deployed
   `/usr/bin/whitespaceagent` is byte-identical to `WhiteSpaceApp/wsa`.
4. **Boot + OEM binaries.** `docs/boot-process.md` and `oem-analysis/`
   (decompiled `pm_daemon`, `actimage`, `mtd_char_op`, uevent pair, `boxdiag`,
   `load_keys_good`).
5. **On-device stand-in.** `docs/on-device-spoof.md`: cross-compile `sb_mips`
   (static MIPS), redirect the hostname to loopback via `/etc/hosts`, test.
6. **QEMU proof.** `qemu-env/` boots the device userspace on a stock malta kernel
   and runs the real, unmodified `wsa` through register → channel against
   `sb_mips`, with `radio-emu.py` answering the radio handshake over ttyS1.
7. **Emulation guide.** `docs/emulation-guide/` (9 chapters) — the full
   binary → emulator story plus tips and pitfalls.
8. **README** rewrite with the overview, the e-waste/ownership essay and an
   original SVG banner (no copyrighted product photos).
10. **Radio + video analysis (2026-11-21).** `docs/radio-and-video.md`: the
    ADV7842 -> ViXS XCode4 -> NXP "Tx v2" signal chain, the serial RF-control
    protocol (from the MT300-AV `RF_Control.php`), the software-selectable
    modulation formats (8VSB/COFDM-DVB-T/QAM), MPEG-2/H.264 encoding via
    `av_streamer`, and the EDID that pins HDMI input to 720p. Drawn from the
    MT300-AV firmware on the share, not from the repo's white-space snapshot.
11. **Device backup procedure + tooling (2026-11-21).** `docs/device-backup.md`
    and `tools/backup/` (meld_telnet.py, backup-meld.sh): a full, verified backup
    of a running unit (all 11 MTD partitions block-level + live rootfs/`/disk`
    cpio) streamed over nc. A reference backup of the bench unit (sn457, v1.26,
    sb_mips-modified) is on the share under
    `backup-mt300-sn457-v1.26-sbmips-spoofed-2026-11-21/` (sensitive: unredacted
    owner data; not in the repo).
9. **Live device (2026-11-21).** A second, powered bench unit (serial **457**,
   kernel 2.6.29-ts, Meld app v1.26) reached via its passwordless telnet root
   shell (`docs/device-access.md`). Following `docs/on-device-test-plan.md`, with
   `sb_mips` rebuilt to offer only channels **23,42** and the RF output in a
   dummy load, the unmodified agent registered, took channel 23 and keyed the
   radio; a carrier was seen at 524–530 MHz. **The approach works on hardware.**
   The stand-in was then widened to offer **all of 21–51** (owner's choice, for
   bench work into the load); the agent still keys on its preferred channel 23.
12. **Bootloader survey + A/B bank recovery (2026-11-22).** A *third* unit (serial
    **00000601**, MT300-AV, app AV30, kernel 2.6.36) was brought up on serial +
    LAN. It booted but **hung at `do_page_fault() #2: sending SIGSEGV to init`**:
    its saved active bank was **B** (`current_active=1`), whose root filesystem
    `RootFS_1` (`mtd7`) sits on a **destroyed NAND region** — all 209 bad blocks
    fall in `0x3600000–0x058e0000`, exactly that partition (~75% bad). **Bank A
    (`RootFS_0`/`mtd6`) is clean.** With the owner's authorization the default bank
    was switched and persisted (`setenv current_active 0; saveenv`); the unit now
    autoboots the healthy bank to a working shell and runs its AV app. New docs:
    `docs/bootloader.md` (**general** — U-Boot, the 11-partition flash map, the A/B
    bank model, selecting/persisting a bank, and recovering a unit stuck on a bad
    bank, plus the kernel/hardware handoff) and `docs/units/mt300-av-00000601.md`
    (**per-unit** — the bad-block map, bank status, and the recovery for this unit).
    General bootloader mechanism and per-unit facts are kept separate.

## Key technical facts and decisions

- **Why the redirect is easy:** `wsa` uses `gethostbyname` (honors `/etc/hosts`),
  does **no hostname/CN check**, and accepts a **self-signed** (even expired) leaf.
  Port **443 is hard-coded**; `HostPortNumber` in config is ignored.
- **Server constraints:** TLS **1.0** max (OpenSSL 1.0.0, legacy RSA/AES-CBC/SHA),
  `Connection: close` then close the socket, whole response **< ~1 KB** (1 KB
  receive buffer overflows), `SBI-Status: 0` header (space after colon),
  `ChannelCount` = number of **distinct** channels, channels within **21–51**.
- **wsa won't contact the database until the radio reports FCC ID + serial** over
  `/dev/ttyS1` — hence the radio emulator in QEMU.
- **Decision: stock malta kernel, not the device's own.** The device kernel
  (Linux 2.6.36, uImage in NAND `mtdblock4`) is built for the ViXS XCode SoC
  (`xcode_*` symbols) and QEMU has no such machine. A Debian
  `vmlinux-2.6.32-5-4kc-malta` (little-endian) hosts the userspace instead.
- **Decision: qemu-system, not qemu-user,** for `wsa` — its uClibc/hard-float ELF
  is rejected by the user-mode loader. qemu-user is still used for fast server
  replay tests.
- malta: `-m 256` max, boot from an ext3 disk image (`root=/dev/sda`), not a big
  initramfs.

## Data outside the repo

- Full NAND dump: `/mnt/RAID2/Meld/spectrumbridge/cpio-mt-300av-sn677-mtdblocks`
  (newc cpio of the rootfs, then raw `mtdblock0..10` each preceded by an ASCII
  `MTD_BLOCK_N` marker). Layout: 0 U-Boot, 3 U-Boot env, **4/5 A/B kernels (8 MB)**,
  **6/7 A/B rootfs (35 MB)**.
- Same share: `cpio-hds-pb100-sn371`, `backup2_with_mtdblocks/`,
  `OnDeviceSpoofingServer/` (musl toolchain + mbedTLS used for `sb_mips`).

## Concerns

- **Installed permanently on the bench unit (457).** The rootfs (RootFS_0,
  `/dev/mtdblock6`) now has `/usr/bin/sb_mips`, `/usr/bin/sb_start` (restart
  loop, loopback only), the `/etc/hosts` redirect (original in
  `/disk/hosts.bak`, which pointed at 192.168.1.249) and a 3-line `rcS` hook
  (original `/etc/init.d/rcS.orig`). The transmitter keys unattended ~30 s after
  boot. If the active image ever flips to RootFS_1/AppFS_1 (e.g. a firmware
  upgrade), none of this is there and RF stays off: re-run
  `spectrumbridge_spoof_c/device/install.sh`.
- The bench unit (457) differs from the unit the repo's `root/` snapshot came
  from (371): older kernel and app. Don't assume they match file-for-file.
- **Unit 00000601 has a failing NAND region.** Its bank-B rootfs (`RootFS_1`,
  `mtd7`, `0x3600000–0x058e0000`) is ~75% bad blocks and its userspace is
  unbootable (init SIGSEGV). The default was moved to the clean **bank A** and
  saved; **do not flip it back to bank B** (a firmware upgrade might). Details:
  `docs/units/mt300-av-00000601.md`.
- **Regulatory:** the device is a licensed-band-adjacent transmitter; the original
  database enforced channel legality by location. A spoofed channel list removes
  that check, so real transmission needs a legally valid channel choice.
  The firmware's 21–51 range predates the 600 MHz repack: in the US, channels
  38–51 are now licensed mobile spectrum and 37 is radio astronomy, so only
  vacant channels in **21–36** are usable on air. The stand-in currently offers
  all of 21–51 for bench work; narrow it before an antenna goes on.
- **Privacy:** `root/` and spoof configs carry the real registration
  (owner/contact/GPS). Treat as sensitive in anything published.
- **Token cost:** decompiles and dumps are huge. Read slices, not whole files.

## Open items / possible next steps

- `device/install.sh` was written from the hand-run steps and has not itself
  been run on a device yet; try it on the next unit or after a reflash.
- Optionally make the stand-in choose channels from a real, current TVWS
  availability source rather than a fixed list.
- **Unit 00000601:** a full NAND backup has not been taken yet (use
  `tools/backup/`), and the web-UI password (`admin`, DES-crypt in
  `/disk/www/.htpasswd`) has not been recovered — crack it with `john`/`hashcat`
  (`-m 1500`) on a machine with the tools, or reset it via the root shell. The
  hash is kept out of the repo (crackable).
