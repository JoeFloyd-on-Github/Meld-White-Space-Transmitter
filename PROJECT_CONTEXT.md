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
13. **Restored unit 00000601 end to end (2026-11-22).** The same MT300-AV (serial
    **601**, app **v1.38**, NXP FW **1.3.9**, hostname `MT300-AV`), after the bank-A
    recovery above, was reached at **192.168.1.195** and brought fully back:
    - **Full verified backup** streamed over nc to the share at
      `backup-mt300-sn601-v1.38-2026-11-22/` (538 MB; all 11 MTD images + live
      rootfs/`/disk` cpio; sensitive owner data, out of the repo). Both RootFS slots
      read back as NAND-noisy snapshots at the `mtdblock` layer (no `nanddump` → no
      OOB/ECC, non-deterministic even on the device itself), so the **cpio archives
      are the authoritative file capture** — noted in the backup's
      `meta/verification-notes.txt`.
    - **`sb_mips` installed + boot-hooked.** The bench 31-channel build
      (md5 `c7ed3cca…`) works **unchanged** against this unit's `whitespaceagent`,
      even though that agent is a **different build** (md5 `8063cb6b…`, its own
      version string is **v45**, vs the repo's analysed **v44** — same
      4,648,238-byte size). On this AV firmware **`rcS` does NOT call
      `wsagent_start`** (it only starts `av_streamer`); the agent is launched from
      **`/etc/init.d/setipeth0`**, so the `sb_start` hook went there (before its
      `wsagent_start`), not in `rcS`. Plus the `/etc/hosts` redirect and the
      `sb_start` supervisor in `/usr/bin`. Backups: `/etc/hosts.orig`,
      `/etc/init.d/setipeth0.presb`.
    - **Confirmed transmitting.** The agent does `PUT /v3/devices/US/OKVMT300/601`
      (register) then `GET /v3/channels/US/<lat>/<lon>/`; registration succeeds
      (`registration_status 0`) and it selects an operating channel (31). **The AV
      video transmitter only keys once a live video input is locked** — with no
      source, `av_streamer` loops "NO digital video standard detected" and
      `radio_status` stays 0; connecting a source locked 1080i@60 and `radio_status`
      went to 1 (web Status page: Transmitter Enabled). The non-AV bench unit keys a
      bare carrier without video; the AV unit does not.
    - **Web UI password.** `/disk/www/.htpasswd` held the factory default `admin:` +
      a traditional DES-crypt hash (not in wordlists, not public). Rather than crack
      it, it was **reset to a known value via `htpasswd -b`** over the root shell
      (value in the owner's local notes, not the repo). Device facts harvested from
      the now-accessible web UI are in the per-unit doc.
    All recorded in `docs/units/mt300-av-00000601.md`.
14. **Hardware-version reference (2026-11-22).** `docs/hardware-versions.md`
    (general) documents the three MT300 boards named in the firmware's own install
    pages: **MT300-AV RevB** and **RevC** (video; `Board_ID 0x2F00`; RevC silk
    "MT350 PCB Rev C") and the **video-less MT300-IT RevB** (`Board_ID 0x00F6`);
    how to identify each (rear A/V connectors, silk screen, `MELD_PLATFORM`); the
    jumper/SW3-1 setup (RevB needs JP2 & JP15 + SW3-1 Manual, RevC just SW3-1
    Automatic); and the factory **serial+TFTP manual Linux update** to **Ver_2.06_d**
    (U-Boot `XCode_Viper#`, per-model kernel image + NandScript, `nopart3` preserves
    `UBOOT_ENV`). Jumper-diagram images are in `docs/images/hardware/`.
15. **Peerless-AV rebrand (2026-11-22).** `docs/peerless-hds-pb100.md` — the same
    MT300 was resold by Peerless-AV as the PeerAir "Pico Broadcaster" **HDS-PB100**,
    specifically the **IP-fed MT300-IT** variant. Its sell sheet (`LIT-0976`) and
    user manual (`180-9044-1`) are mirrored in `docs/datasheets/`. Key facts:
    FCC_Model `MT300v1` / FCC ID `OKVMT300`; **you feed it an MPEG-2 transport stream
    over UDP** to the rear RJ45 LAN port — default config IP **192.168.1.194**,
    streaming/modulator IP **192.168.1.193**, **port 5002** (the .193 matches sn601's
    live `StreamingIpAddress`); MPEG-2 / AC-3 / 720p-1080i, up to 19.2 Mbps; the
    bundled Peerless-AV Streaming Media Server (Lite) transcodes + streams. Its
    advertised "first year of FCC licensing subscription" **is** the Spectrum Bridge
    database dependency — the cloud gating was a built-in, recurring part of the
    product.
16. **FCC authorization mirror (2026-11-22).** `docs/fcc.md` summarizes the grant
    for **FCC ID OKVMT300** (grantee **Meld Technology**, granted 2012-11-28, Part 15
    Subpart H Fixed TVBD / WGF; FCC filing agent **Spectrum Bridge, Inc.**; test firm
    Timco Engineering). Grant conditions: professional install, permanent mount;
    **only TV channels 21-34 and 39-51** authorized; **"approved for operation with
    the Spectrum Bridge TVWS database"** (the dependency is literally in the FCC
    grant); no directional antenna / gain >2 dBi; **0.014 W (14 mW) conducted**;
    40 cm RF separation. All **24 public exhibits** are mirrored in
    `docs/fcc/exhibits/` (4 operator manuals, 7 internal/external photos — the
    internal shows the original **PCB-1814** board with the **ViXS XCode** SoC — 3
    test reports incl. the Database Interface certification, antenna data, MPE,
    labels, attestations, letters). Pulled from `fccid.io` (apps.fcc.gov returns 403
    to bots). Confidential exhibits (schematics/BOM) are not public, not included.
17. **White-space channel finder + data mirror (2026-11-22).** `tools/whitespace-
    finder/` — a dependency-free browser app (`index.html`) that reads a verbatim
    mirror of the **WaveDB** FCC protected-entity CSVs (`data/`, ~7 MB from
    `csv.wavedb.com`, snapshot 2026-11-22: ~8k TV stations, 600 MHz licensees, fixed
    WSDs, wireless mics, BAS links, MVPD/TV receive sites, land-mobile) and, for a
    location, estimates which TV channels look open for white space. First-order
    model (ERP/HAAT contour estimate + point keep-outs; ch37 reserved; ch38-51 =
    600 MHz), **informational, not a certified determination**. Serve with
    `python3 -m http.server` (browsers block `fetch()` over `file://`). The CSVs
    were kept **verbatim** (including third-party contact fields) at the owner's
    explicit instruction, as public records — the one deliberate exception to
    keeping registrant PII out of the repo. A **"jump to a TV market" dropdown**
    (54 states/territories, ~244 markets) is built from the data itself — it
    clusters each state's full-power stations by proximity, ranks by station count,
    lists the top 5, and jumps to each cluster's centroid (the market's geographic
    core) — so no coordinates are hard-coded and it tracks the data on refresh.
18. **Bootloader + firmware survey of unit 677 (2026-11-22).** A fourth physical
    unit (serial **677**, MT300-AV, app AV30, kernel 2.6.36, radio NXP **1.3.9**
    Tx v2, real MAC `F8:22:85:0B:01:74`) documented from its serial console in
    `docs/units/mt300-av-677.md` (+ a console capture under `docs/captures/`).
    Unlike 601, its `bootcmd` is **hardcoded to bank A** (`nand read 81000000
    300000 800000; bootm`, `bootargs root=/dev/mtdblock6`), so the bad-NAND cluster
    in `RootFS_1`/`mtd7` (209 bad blocks, same bank-B zone as 601) is harmless —
    the unit boots fine and needs no recovery. Its U-Boot `mii info` reports a
    **real PHY** (OUI 0x4700) where 601 showed only the phantom 0x0080 (likely a
    different board revision — see `docs/hardware-versions.md`). It shipped on
    **DHCP** (got 192.168.1.25); switched to a persistent static
    **192.168.1.196** in `/etc/init.d/setipeth0` (DHCP=0; original saved as
    `setipeth0.orig`) and verified across a reboot. The **serial console gives a
    root shell** on this unit.

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
- **The database protocol is SpectrumBridge v3 REST:** a `PUT /v3/devices/US/<FccID>/<serial>`
  carrying a `<RegistrationRequest>` (register), then a `GET /v3/channels/US/<lat>/<lon>/`
  (channel list). `sb_mips` answers both the same `200` / `SBI-Status: 0` / channel
  list, which is enough for registration *and* channel selection on both the v44 and
  v45 agents. `registration_status 0` = **success** (99 = unknown/failure).
- **At least two `whitespaceagent` builds exist** (both 4,648,238 bytes): the repo's
  **v44** (analysed/decompiled) and the AV v1.38 unit's **v45** (`-v` string); they
  differ by md5 but speak the same protocol, so the one `sb_mips` serves all.
- **The AV (video) units require a locked video input to key RF**; the non-AV units
  key a bare carrier without one. `MELD_PLATFORM`/`Board_ID`: AV = `MT300-AV`/`0x2F00`,
  IT = `MT300-IT`/`0x00F6`.
- **FCC-authorized channels (grant OKVMT300) are 21-34 and 39-51** at 14 mW
  conducted — narrower than the firmware's 21-51 offer, and the grant itself is
  conditioned on the Spectrum Bridge database (`docs/fcc.md`).
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
- **Verified full backups** made with `tools/backup/`:
  `backup-mt300-sn457-v1.26-sbmips-spoofed-2026-11-21/` (bench unit 457) and
  `backup-mt300-sn601-v1.38-2026-11-22/` (AV unit 601) — each has `mtd/` (11 raw
  images + MD5SUMS), `rootfs-live/` (rootfs + `/disk` cpio), `meta/`, `README.txt`.
  Unredacted owner PII; kept off the repo.

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
- **sb_mips is now installed on unit 00000601 (601) too**, on its active bank-A
  rootfs (`mtdblock6`): `/usr/bin/sb_mips` + `sb_start`, the `/etc/hosts` redirect,
  and the hook in **`/etc/init.d/setipeth0`** (NOT `rcS` — on AV firmware `rcS`
  doesn't launch the agent). RF only keys with a live video input on this AV unit.
  If the active bank ever changes, none of this is there — re-install (adapting the
  hook to `setipeth0`). Its web password was reset to a known value (owner's notes).
- **Regulatory:** the device is a licensed-band-adjacent transmitter; the original
  database enforced channel legality by location. A spoofed channel list removes
  that check, so real transmission needs a legally valid channel choice.
  The firmware's 21–51 range predates the 600 MHz repack: in the US, channels
  38–51 are now licensed mobile spectrum and 37 is radio astronomy, so only
  vacant channels in **21–36** are usable on air today. (The device's FCC grant,
  `OKVMT300`, authorizes **21-34 and 39-51** — pre-repack; `docs/fcc.md`.) The
  stand-in currently offers all of 21–51 for bench work; narrow it, and check
  `tools/whitespace-finder/` for what is actually vacant, before an antenna goes on.
- **Privacy:** `root/` and spoof configs carry the real registration
  (owner/contact/GPS). Treat as sensitive in anything published.
- **Token cost:** decompiles and dumps are huge. Read slices, not whole files.

## Open items / possible next steps

- `device/install.sh` anchors its boot hook on `^wsagent_start$` in **`rcS`**, which
  is right for the non-AV units but **wrong for AV (AV30) firmware**, where that
  line lives in `/etc/init.d/setipeth0`. The sn601 install was done by hand for
  that reason; adapt the script (detect/choose the file that actually contains
  `wsagent_start`) before relying on it on an AV unit.
- Optionally make the stand-in choose channels from a real, current TVWS
  availability source rather than a fixed list. `tools/whitespace-finder/` now
  computes candidate channels from the live WaveDB data for a location, but it is a
  standalone/informational tool and is **not** wired into `sb_mips` — connecting
  the two (have the stand-in serve only locally-vacant channels) is still open.
- **Unit 00000601 — DONE (2026-11-22):** full backup taken
  (`backup-mt300-sn601-v1.38-2026-11-22/`) and the web-UI password reset to a known
  value via `htpasswd -b` (the factory DES hash was not in any wordlist and not
  publicly documented). See `docs/units/mt300-av-00000601.md`.
