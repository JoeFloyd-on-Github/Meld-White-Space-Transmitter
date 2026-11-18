# Device and firmware context

This document describes the **system around `whitespaceagent`** — the Meld MT300
device, how it boots, how its configuration becomes the XML that `wsa` reads,
the web UI, and the firmware history. It is built from two artifacts in this
repo:

- **`root/`** — a snapshot of the running device's root filesystem.
- **`upgrade/`** — the `meldtech` 1.21 firmware upgrade package (tgz + extracted).

For the behavior of the `whitespaceagent` binary itself, see
[`wsa-overview.md`](wsa-overview.md); for its decompiled source, see
[`../WhiteSpaceApp/ghidra/`](../WhiteSpaceApp/ghidra/).

Everything here is read from those two artifacts; nothing was produced by
running the device.

## 1. The device

A **Meld Technology MT300** TV-white-space (TVWS) transmitter — a fixed/portable
device that transmits on broadcast-TV channels left vacant in a given location,
after checking an online white-space database for channels it is allowed to use.

| Item | Value |
|---|---|
| CPU / OS | 32-bit MIPS (MIPS32r2, little-endian), BusyBox + a small OpenWrt-style userland |
| Root FS | the snapshot in `root/`; application storage is a **yaffs2 NAND** volume mounted at `/disk` (from `/dev/mtdblock8` or `9`) |
| FCC ID | `OKVMT300` |
| Radio | an NXP-based transmitter ("Tx v2"), firmware **1.3.0 v0036**, controlled over `/dev/ttyS1` at 115200 (see `wsa-overview.md` §6) |
| Serial no. | `371` |
| Web UI | `thttpd` serving PHP from `/disk/www` |
| Config version | `sbi` **1.38 / Nov-2013** |

The device had **root with no password** (`/etc/passwd` has `root::0:0:…`, no
`/etc/shadow`) and respawns a root shell on the `ttyS0` serial console
(`/etc/inittab`). Those are security characteristics of the device as shipped,
noted here for completeness.

## 2. Boot sequence

`/etc/inittab` runs `/etc/init.d/rcS` at sysinit. The relevant steps:

```
mount proc / sysfs / usbfs
mount -t yaffs2 -rw /dev/mtdblock8 (or 9) /disk      # application + config storage
/etc/init.d/setipeth0                                # static IP on eth0
clear_config_files                                   # blank the status files wsa writes
wsagent_start                                        # build the XML, launch whitespaceagent
thttpd -C /disk/www/thttpd.conf                      # web UI
```

- `clear_config_files` resets `channels`, `operating`, `channel_status` and
  `transmitter` to empty so the Status page starts clean each boot.
- `wsagent_start` is what actually starts `wsa` (next section).
- The web UI's `registration.php` also calls `wsagent_stop` / `wsagent_start`
  to restart the agent after the user edits settings.

## 3. Configuration → XML pipeline

This is the piece that `wsa-overview.md` originally had to infer. The device
keeps settings in **`/etc/config/sbi`** (OpenWrt UCI format) and converts them
into the XML `wsa` reads, using an awk script plus a fixed template:

```
/etc/config/sbi            (UCI: option 'Key' 'Value')
        +                   ──►  xml_conversion (awk)  ──►  /etc/config/meldTechDeviceCapability.xml
/etc/config/template_file  (XML skeleton)
```

Driven by `wsagent_start`:

```sh
awk -f /disk/usr/bin/xml_conversion /etc/config/sbi /etc/config/template_file \
    > /etc/config/meldTechDeviceCapability.xml &
whitespaceagent cafile.cert /etc/config /etc/config/meldTechDeviceCapability.xml > /dev/null &
```

**`xml_conversion`** (an awk script) splits `sbi` on the `'` character so that
`option 'HostAddress' 'x'` yields key `HostAddress` → value `x`, builds a lookup
table, then copies `template_file` line by line; for each `<Tag>...</Tag>` whose
`Tag` is in the table it substitutes the value, and passes every other line
through unchanged.

Consequences (all relevant to `wsa`):

- **`template_file` fixes the field set.** The device template contains
  `HostAddress`, `HostPortNumber`, `RegulatoryDomain`, `Location/Latitude`,
  `Location/Longitude`, the `TvwsDeviceInfo` block, `PreferredChannel`,
  `EnumeratedTransmitPower` and `StreamingIpAddress`. It has **no `FccID`,
  `MfgSerialNumber` or `ModulationMode`** — consistent with `wsa` taking FCC ID
  and serial from the radio and defaulting modulation to ATSC.
- The XML **root element is `<DeviceCapability>`** (confirms the "root name not
  checked" note in `wsa-overview.md` §4).
- `wsa`'s launch arguments line up exactly with the documented `main()`:
  `CAfile=cafile.cert`, `CApath=/etc/config`, XML = the generated file.

### `/etc/config` at a glance

| File | Direction | Produced / consumed by |
|---|---|---|
| `sbi` | input | edited via the web UI; source of truth for settings |
| `template_file` | input | XML skeleton for `xml_conversion` |
| `meldTechDeviceCapability.xml` | generated | written by `wsagent_start`, read by `wsa` |
| `cafile.cert` | input | TLS CA (Go Daddy Class 2 root) for verifying the DB server |
| `channels` / `operating` / `transmitter` | output | written by `wsa`, read by the Status page |
| `channel_status` / `registration` | output | written by `wsa`, read by the Status page |
| `fccidSerialInfo` / `queryInfo` | output | written by `wsa` from the radio |

`/etc/config_backup/` holds a copy of the same set (a factory/known-good
backup).

## 4. Web UI

`thttpd` serves PHP pages from `/disk/www`, a LuCI-style (CBI) admin interface:

| Page | Purpose |
|---|---|
| `status.php` | shows registration status, available/operating channel, TX state — reads the `wsa` output files |
| `registration.php` | edits the registration/contact fields in `sbi`, then restarts `wsa` |
| `preferences.php` | preferred channels, transmit power, device type |
| `network.php` | IP configuration (`setipeth0`) |
| `upgrade.php` | uploads and applies a firmware `.tgz` |
| `index.php` / `header.php` / `processor.php` / `config.php` | framing, form processing, config helpers |

The status/preferences pages filter displayed channels to **21–51**, matching
`reAdjustChannelMapToMeldCapabilities()` in `wsa`.

## 5. Firmware upgrade package (`upgrade/`)

`meldtech_upgrade_1.21.tgz` unpacks to `upgrade/meldtech/` and is applied by
`upgrade.sh`. For version **1.21** (Oct 2012) it:

1. Edits `/etc/config/sbi`: sets `ContactCountry` to `US`, `Version` to `1.21`,
   `Date`, adds a `StreamingIpAddress` entry if missing, and — **only if
   `ChLookupDeviceType` is `8` (fixed device)** — points `HostAddress` at
   `tvws.spectrumbridge.com` (the live DB). Type `3` devices stay on the demo DB.
2. Copies in a newer `whitespaceagent` (**v27** in this package) and the whole
   `/disk/www` web UI.
3. Patches `setipeth0` to `killall udhcpc` before bringing the interface up.
4. Installs the `template_file` that includes `StreamingIpAddress`.
5. Disables `thttpd` request logging.
6. Restarts the agent (`wsagent_stop; wsagent_start`).

The package's `RELEASE_NOTES` records the DB target moving **DEMO → TEST → LIVE**
across versions and the device-type default moving from 3 to 8 ("fixed only").

## 6. Version timeline

Pulled together from the binaries, `sbi` versions, and `RELEASE_NOTES`:

| Component | Seen here | Notes |
|---|---|---|
| `whitespaceagent` | **v27** (Oct 24 2012) in `upgrade/`; **v44** (Nov 22 2013) on the device = `WhiteSpaceApp/wsa` | a **v45** is referenced by patch scripts under `root/disk/Meld/` but the binary is not in this snapshot |
| `sbi` config | `1.14` → `1.38` (device is 1.38, Nov 2013); `upgrade/` sets 1.21 | RELEASE_NOTES covers 1.14–1.21 |
| Radio firmware | `1.3.0 v0036 Tx v2` | the NXP transmitter firmware, separate from `whitespaceagent` |
| DB target | DEMO → TEST → LIVE (`tvws.spectrumbridge.com`) | per RELEASE_NOTES and the on-device `sbi` |

The **running device binary is byte-identical to `WhiteSpaceApp/wsa`** (md5
`79f86dba926e284a0fc46614f9d6f201`), so the decompilation and `wsa-overview.md`
describe the exact deployed software.

## 7. Relationship to the spoof servers

`spectrumbridge_spoof_c/` and `spectrumbridge_spoof_python/` reimplement the
**`tvws.spectrumbridge.com`** endpoint that `wsa` connects to, so the device (or
`wsa` run off-device) can be exercised without the real service. The protocol
they must speak — the registration `PUT`, the channel `GET`, the required HTTP
status / `SBI-Status` / `Date` headers, the `ChannelCount`/`ChannelList`/
`RefreshIn` body, the OpenSSL-1.0.0 TLS constraints, and the "server must close
the connection" and "keep the response under ~1 KB" requirements — is documented
in [`wsa-overview.md`](wsa-overview.md) §5.
