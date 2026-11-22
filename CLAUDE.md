# CLAUDE.md — Meld White Space Transmitter

**Before doing anything in this repo, read [`PROJECT_CONTEXT.md`](PROJECT_CONTEXT.md).**
It holds the goals, history, key decisions, concerns and open items from earlier
sessions — the things that are not obvious from the code or the git log.

## Quick orientation

- Subject: the Meld MT300 TV white-space transmitter and its `whitespaceagent`
  (`wsa`, MIPS little-endian, uClibc, static OpenSSL 1.0.0). The device refuses to
  transmit because its database host `tvws.spectrumbridge.com` is gone; this repo
  reverse-engineers it and replaces that database locally.
- Start points: `README.md` (overview), `HOW-TO.md` (restore a device + FCC
  rules), `docs/wsa-overview.md` (agent behavior and protocol),
  `docs/radio-and-video.md` (the NXP radio, its serial control + modulation
  formats, and the ADV7842→ViXS video path), `docs/emulation-guide/` (binary →
  QEMU narrative), `docs/emulation-guide/09-tips-and-pitfalls.md` (traps already
  hit — check here before debugging).
- The committed binaries (`wsa`, `sb_mips`, busybox, malta kernel, …) are **MIPS**,
  not host executables.

## Working conventions

- Large data lives **outside** the repo on the CIFS share
  `/mnt/RAID2/Meld/spectrumbridge/` (full NAND dump ~618 MB). Copy locally before
  working on it and verify the size afterwards — a CIFS `soft` mount once silently
  truncated a copy.
- Keep large tool output (decompiles, binwalk, hexdumps) in files and read only
  the relevant parts; whole dumps blow up context cost.
- Don't commit private keys (`*.pem` is gitignored), Ghidra project state
  (`*.rep/`, `*.gpr`) or `build/` directories.
- `root/` and the spoof configs contain a real device's registration details
  (owner, contact, GPS). Don't copy them into new public-facing text.
- Commit or push only when asked.
