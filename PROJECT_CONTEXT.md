# Project context — Meld White Space Transmitter

Persistent notes for future sessions (human or Claude): **why** this repo exists,
what has been done, the decisions made along the way, and what is still open.
Last updated 2026-11-21. Update this file when a goal, decision or open item
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
9. **Live device (2026-11-21).** A second, powered bench unit (serial **457**,
   kernel 2.6.29-ts, Meld app v1.26) reached via its passwordless telnet root
   shell (`docs/device-access.md`). Following `docs/on-device-test-plan.md`, with
   `sb_mips` rebuilt to offer only channels **23,42** and the RF output in a
   dummy load, the unmodified agent registered, took channel 23 and keyed the
   radio; a carrier was seen at 524–530 MHz. **The approach works on hardware.**
   The stand-in was then widened to offer **all of 21–51** (owner's choice, for
   bench work into the load); the agent still keys on its preferred channel 23.

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
