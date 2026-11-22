# CLAUDE.md — Meld White Space Transmitter

**Before doing anything in this repo, read [`PROJECT_CONTEXT.md`](PROJECT_CONTEXT.md).**
It holds the goals, history, key decisions, concerns and open items from earlier
sessions — the things that are not obvious from the code or the git log.

## Quick orientation

- Subject: the Meld MT300 TV white-space transmitter and its `whitespaceagent`
  (`wsa`, MIPS little-endian, uClibc, static OpenSSL 1.0.0). The device refuses to
  transmit because its database host `tvws.spectrumbridge.com` is gone; this repo
  reverse-engineers it and replaces that database locally. The hardware shipped in
  AV (HDMI/component video) and IT (IP/UDP transport-stream) variants and was also
  sold rebranded by Peerless-AV as the PeerAir "Pico Broadcaster" HDS-PB100 — all
  the same `OKVMT300`-certified radio with the same dead dependency.
- Start points: `README.md` (overview), `HOW-TO.md` (restore a device + FCC
  rules), `docs/wsa-overview.md` (agent behavior and protocol),
  `docs/radio-and-video.md` (the NXP radio, its serial control + modulation
  formats, and the ADV7842→ViXS video path),
  `docs/device-backup.md` (full-device backup over nc; scripts in `tools/backup/`),
  `docs/device-access.md` (reaching a live unit), `docs/bootloader.md` (U-Boot,
  the 11-partition flash map, the A/B firmware-bank model, and recovering a unit
  stuck on a bad bank), `docs/units/` (per-unit facts),
  `docs/hardware-versions.md` (the MT300-AV RevB/RevC and video-less MT300-IT
  boards, how to tell them apart, and the factory serial+TFTP software update),
  `docs/fcc.md` (the `OKVMT300` grant and its use conditions, with a mirror of all
  24 public FCC exhibits in `docs/fcc/exhibits/`),
  `docs/peerless-hds-pb100.md` (the same hardware resold by Peerless-AV as the
  PeerAir "Pico Broadcaster"; its sell sheet + user manual are in
  `docs/datasheets/`), `tools/whitespace-finder/` (a dependency-free browser app,
  plus a mirror of the WaveDB FCC protected-entity CSVs, that estimates which TV
  channels are open for white space at a location), `docs/emulation-guide/`
  (binary → QEMU narrative), `docs/emulation-guide/09-tips-and-pitfalls.md` (traps
  already hit — check here before debugging).
- The committed binaries (`wsa`, `sb_mips`, busybox, malta kernel, …) are **MIPS**,
  not host executables.

## Working conventions

- **Device backups** live **outside** the repo on the CIFS share
  `/mnt/RAID2/Meld/spectrumbridge/` (full NAND dumps ~0.5–0.6 GB each; they carry
  the owner's PII, so they never go in git). Copy locally before working on one and
  verify the size afterwards — a CIFS `soft` mount once silently truncated a copy.
- **Public reference data, by contrast, is committed on purpose** and can be large:
  the FCC exhibit PDFs/photos (`docs/fcc/exhibits/`, ~20 MB), the Peerless
  datasheet + manual (`docs/datasheets/`), and the WaveDB FCC protected-entity CSVs
  (`tools/whitespace-finder/data/`, ~7 MB, kept **verbatim** as public records at
  the owner's instruction). These are public records with no connection to a
  specific owned unit; mirror and refresh them freely.
- Keep large tool output (decompiles, binwalk, hexdumps) in files and read only
  the relevant parts; whole dumps blow up context cost.
- **Separate general from device-specific.** Reusable mechanism (protocol,
  bootloader, flash layout, bank model, procedures) goes in the topic docs;
  facts tied to one physical unit (NAND bad-block map, serial, MACs, which bank
  is healthy, its web password hash) go in `docs/units/`, never in the general
  docs. Password hashes and a unit's PII stay out of the repo entirely.
- Don't commit private keys (`*.pem` is gitignored), Ghidra project state
  (`*.rep/`, `*.gpr`) or `build/` directories.
- `root/` and the spoof configs contain a real device's registration details
  (owner, contact, GPS). Don't copy them into new public-facing text.
- Commit or push only when asked.
