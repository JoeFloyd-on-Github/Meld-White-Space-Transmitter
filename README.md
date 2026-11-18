# Meld White Space Transmitter

Analysis and documentation of the **Meld Technology MT300** TV-white-space (TVWS)
transmitter and its `whitespaceagent` ("wsa") software. The MT300 is a MIPS/
BusyBox device that transmits on vacant broadcast-TV channels after checking the
**SpectrumBridge** white-space database (`tvws.spectrumbridge.com`) for channels
it is permitted to use.

This repository collects the device's software, a reverse-engineering writeup of
it, stand-in ("spoof") database servers, and a snapshot of a running device.

## What's here

| Path | What it is |
|---|---|
| [`WhiteSpaceApp/wsa`](WhiteSpaceApp) | the `whitespaceagent` binary (MIPS, v44, Nov 2013) — the main subject of analysis |
| [`WhiteSpaceApp/ghidra/`](WhiteSpaceApp/ghidra) | its decompiled source: `wsa_decompiled.c` (raw Ghidra) and `wsa_annotated.c` (fully annotated), plus a `README` with a variable-name legend and an ezxml explanation |
| [`docs/wsa-overview.md`](docs/wsa-overview.md) | **behavior of the `wsa` binary** — startup, the two threads, the TLS/HTTP database protocol, the response format a server must send, the serial radio commands, and the files it writes |
| [`docs/system-context.md`](docs/system-context.md) | **the device around it** — boot sequence, the `sbi`→XML config pipeline, the web UI, the firmware upgrade package, and the version history |
| [`docs/boot-process.md`](docs/boot-process.md) | **the device boot + hardware** — U-Boot/kernel, `rcS`, the reset/power button, IP assignment, how the radio is tied in, and how the ViXS hardware is configured |
| [`docs/on-device-spoof.md`](docs/on-device-spoof.md) | **running the database stand-in on the device** — redirecting the dead `tvws.spectrumbridge.com` to loopback, cross-compiling the MIPS server, and how it was tested |
| [`oem-analysis/`](oem-analysis) | decompiled + annotated source for the **OEM hardware-control binaries** (`pm_daemon`, `actimage`, `mtd_char_op`, the uevent pair, `boxdiag`, `load_keys_good`) |
| [`spectrumbridge_spoof_c/`](spectrumbridge_spoof_c) | a C (Mongoose + mbedTLS) stand-in for the SpectrumBridge database server — builds to a static **MIPS** binary (`sb_mips`) that runs on the device; includes `build.sh`, `test_replay.sh`, and a `sniff` tool (see [`docs/on-device-spoof.md`](docs/on-device-spoof.md)) |
| [`spectrumbridge_spoof_python/`](spectrumbridge_spoof_python) | a Python stand-in for the same server (plain + SOAP variants) |
| [`upgrade/`](upgrade) | the `meldtech` 1.21 firmware upgrade package (tgz + extracted); bundles an older `whitespaceagent` (v27) |
| [`root/`](root) | a snapshot of the **running device's** root filesystem — live `/etc/config`, boot scripts, web UI, and the deployed `whitespaceagent` |

## How the pieces fit

```
  /etc/config/sbi  ──(xml_conversion awk + template_file)──►  meldTechDeviceCapability.xml
                                                                      │
                                                                      ▼
   radio (NXP Tx)  ◄──/dev/ttyS1──  whitespaceagent (wsa)  ──TLS:443──►  tvws.spectrumbridge.com
        MT300                               │                              (spoofable: see
                                            ▼                               spectrumbridge_spoof_*)
                               /etc/config/{channels,operating,
                                transmitter,registration,...}  ──►  thttpd web UI (status page)
```

- `wsagent_start` builds the XML from `sbi` and launches `wsa` with the CA cert,
  CA path, and that XML (see `docs/system-context.md`).
- `wsa` reads the device info, asks the database over TLS which channels are
  available, picks one, and drives the radio over serial; it writes status files
  that the web UI displays (see `docs/wsa-overview.md`).
- The spoof servers replace the real database endpoint so the device/software
  can be exercised offline.

## Key confirmed facts

- The `whitespaceagent` running on the device (`root/disk/usr/bin/whitespaceagent`)
  is **byte-identical** to `WhiteSpaceApp/wsa`, so the analysis describes the
  exact deployed software.
- The database host is **`tvws.spectrumbridge.com`** over TLS on port 443
  (hard-coded port), trusted via the Go Daddy Class 2 root CA.
- `wsa` was built with GCC 4.3.2 / uClibc and statically links **OpenSSL 1.0.0**
  (TLS 1.0 max) and the bundled **ezxml** parser.

## Notes

- The analysis is static (DWARF debug info + Ghidra 12.2), cross-checked against
  the `root/` snapshot and `upgrade/` package. Nothing was produced by running
  the binary or the device.
- `root/` and the spoof configs contain a specific device's real registration
  details (owner, contact, GPS) as captured from the live device.
- The committed binaries (`wsa`, `thttpd`, BusyBox, etc.) are MIPS executables
  for the device, not for the host you clone onto.
