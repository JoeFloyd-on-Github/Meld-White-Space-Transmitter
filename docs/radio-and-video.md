# The radio and the video path

How the Meld MT300 turns a video input into a modulated UHF signal: the signal
chain, how the RF stage is controlled, what modulation formats it supports, how
video is encoded, and what constrains the input resolution.

Sources: the `whitespaceagent` decompile ([`wsa-overview.md`](wsa-overview.md)
§6), and the **MT300-AV** firmware image on the share
(`cpio-mt-300av-sn677`) — its web RF-control page (`disk/www/RF_Control.php`),
the A/V streamer app and scripts (`disk/Meld/AV30/test_script/`), and the ViXS
driver module. The MT300 and MT300-AV share the same radio hardware and the same
ViXS/ADV7842 video path; they differ in which software front-end is shipped.

---

## 1. The signal chain

```
 HDMI / Component (YPbPr) / Composite (CVBS)
          │
          ▼
   ADV7842            Analog Devices video front-end:
                      input select, A/D, and the EDID presented to an HDMI source
          │  raw video (driven from user space by av_streamer)
          ▼
   ViXS XCode4        the main MIPS SoC's transcoder ("Boxster")
                      kernel driver xcode4drv.ko, device /dev/vixs/xcodedrv
          │  MPEG-2 or H.264 video, muxed into an MPEG-2 Transport Stream
          ▼
   DVO                parallel transport-stream output
          │  the TS
          ▼
   NXP "Tx v2" board  a separate transmitter board with its own firmware
                      and its own processor, commanded over /dev/ttyS1
          │  modulates the TS onto a UHF channel (8VSB / COFDM / QAM)
          ▼
        RF out
```

Two processors are involved:

- **The main MIPS SoC** (a ViXS XCode, running Linux `2.6.36-ts-mipsisa32r2el`)
  captures and encodes the video and builds the transport stream.
- **The NXP transmitter board** does the actual RF modulation. The SoC does not
  modulate anything itself; it hands the NXP board a transport stream and a set
  of text commands over a serial link.

---

## 2. How the RF stage is controlled

The NXP board is driven entirely over **`/dev/ttyS1`, 115200 8N1, no parity, no
flow control**, with a short text protocol. Only one process talks to that port
at a time:

- On the **white-space MT300**, that process is `whitespaceagent` (wsa). It is
  the sole driver of the radio; see [`wsa-overview.md`](wsa-overview.md) §6 for
  the exact init sequence and the subset of commands it uses.
- On the **MT300-AV**, the web page `disk/www/RF_Control.php` can also drive it:
  it runs `wsagent_stop` to free the port, opens `/dev/ttyS1` through a PHP
  serial class, and reads/writes the radio directly.

### 2.1 Command set

Query forms end in `?`; writes use `m#[…]` with a fixed-width hex/decimal
payload. From `RF_Control.php`'s command reference:

| Command | Function | Notes |
|---|---|---|
| `MELD_TECH_WS_ENABLE` | enter white-space control mode | sent once at startup |
| `e1` / `e0` | transmitter on / off | |
| `e?` | read transmitter enable | returns `1`/`0` |
| `m01?` / `m#[01000000dd]` | ATSC channel | `dd` = 21–51 (decimal) |
| `m02?` / `m#[02000000zz]` | power value | `zz` = 0x05–0x35 (see map) |
| `m03?` / `m#[03000000zz]` | **modulation mode** | `zz` = 0x00–0x0d (see §3) |
| `m04?` / `m#[040ddd0000]` | non-ATSC frequency | `ddd` = 500–700 MHz |
| `m07?` / `m#[07000000zz]` | DAC power gain | `zz` = 0x00–0x7f; ATSC default 0xab(*) |
| `m08?` | chassis temperature | `zz` hex °C |
| `m09?` / `m#[0900000000b]` | 24-hour timeout enable | `0`/`1` |
| `pNN` | power level (hex) | also sent on every channel change |
| `c<ch>` | set channel (US/ATSC path, wsa) | |
| `b?` / `b#[01…]` | streaming-in IP address | 8 hex digits |
| `l` / `x` / `y` | firmware version / serial / FCC ID | |
| `q` | full status dump | version, S/N, FCC ID, channel/freq, power, gain, modulation |
| `h` | info dump | read until `EOF` |

(*) the gain field is two hex digits (0x00–0x7f) in the read form; the
documented ATSC default `0xab` appears in the write notes, i.e. the comment and
the stated range disagree in the OEM source — treat the exact default as
unverified and read it back with `m07?` on a live unit.

### 2.2 Power-level map (`m02`)

Power is set as a hex "value," not dBm. The OEM map of level → value:

| Level | Value | Level | Value |
|---|---|---|---|
| 1 | 0x05 | 7 | 0x23 |
| 2 | 0x0a | 8 | 0x28 |
| 3 | 0x0f | 9 | 0x2d |
| 4 | 0x14 | 10 | 0x32 |
| 5 | 0x19 | 11 | 0x35 (default) |
| 6 | 0x1e | | |

### 2.3 Status readout (`q`)

Two firmware builds, from the OEM comments, showing the same command on
different units:

```
MT300 Firmware version = 1.3.2 Tx v2          MT300-AV Firmware version = 1.3.4 Tx v2
S/N = 00000159                                S/N = 00000453
FCC ID = OKVMT300                             FCC ID = OKVMT300
Channel = 23                                  Frequency = 0506
TX power = 0x35                               TX power = 0x35
DAC gain = 0xAB                               DAC gain = 0xAB
Modulation mode = ATSC                        Modulation mode = DVB-T (8 MHz)
```

Note the white-space unit reports its setting as a **channel** and ATSC; the AV
unit reports a **frequency** and DVB-T. Same radio, different configured mode.

---

## 3. Modulation: controllable, not fixed in hardware

**The NXP board is effectively a software-defined modulator. The modulation is a
runtime serial setting (`m#[03…]`), not a fixed property of the radio.** The full
set of modes it accepts:

| Code | Modulation | Family |
|---|---|---|
| `00` | **ATSC (8VSB)** | US terrestrial HDTV |
| `01` | DVB-T 8 MHz, QAM-64 | COFDM (terrestrial) |
| `04` | DVB-T 7 MHz, QAM-64 | COFDM |
| `05` | DVB-T 6 MHz, QAM-64 | COFDM |
| `08` | DVB-T 8 MHz, QAM-16 | COFDM |
| `09` | DVB-T 7 MHz, QAM-16 | COFDM |
| `0a` | DVB-T 6 MHz, QAM-16 | COFDM |
| `0b` | DVB-T 8 MHz, QPSK | COFDM |
| `0c` | DVB-T 7 MHz, QPSK | COFDM |
| `0d` | DVB-T 6 MHz, QPSK | COFDM |
| `02` | US Cable QAM-64 | QAM (cable) |
| `03` | US Cable QAM-256 | QAM (cable) |
| `06` | DVB-C 8 MHz, QAM-64 | QAM (DVB cable) |

So it does both **8VSB** (the US ATSC standard) and **COFDM/DVB-T** (6/7/8 MHz
channel rasters, with QPSK/16-QAM/64-QAM constellations), plus single-carrier QAM
for cable.

`wsa`'s own `MELD_MODULATION_TYPE_ENUM` ([`wsa-overview.md`](wsa-overview.md) §4)
lists the same families (ATSC, DVB-T 6/7/8 MHz, QAM-64/256, DVB-C 8 MHz).

### 3.1 Why it is effectively fixed on the white-space unit

The hardware can switch modulation, but the white-space software never does:

- `whitespaceagent` sets modulation once at init, from `meldModulationMode`,
  which **defaults to `0` (ATSC)** because the device's config XML never
  supplies a `<ModulationMode>` tag (the template omits it; see
  [`wsa-overview.md`](wsa-overview.md) §4 and `system-context.md`).
- The white-space unit's web UI ships `processor.php` but **not**
  `RF_Control.php`, so there is no exposed control to change the mode.

The MT300-AV, by contrast, ships `RF_Control.php` and defaults to DVB-T. Same
radio hardware; the difference is which front-end software is installed.

### 3.2 Changing modulation means changing the transport stream too

Setting a DVB-T mode on the NXP board is necessary but not sufficient: the
transport stream has to match. ATSC carries PSIP tables and a ~19.4 Mbit/s
payload; DVB-T carries DVB SI tables (NIT/SDT/TOT/EIT) and a payload that depends
on the channel width, constellation and guard interval. The A/V streamer can
build DVB tables — `av_streamer` links **dvbpsi** and its symbols include
`dvbpsi_nit_*`, `dvbpsi_sdt_*`, `dvbpsi_eit_*` — but the bitrate must be set
within the chosen mode's capacity (§4).

---

## 4. Video encoding and the transport stream

The encoder is the ViXS XCode4, driven by the user-space app **`av_streamer`**
(`disk/Meld/AV30/test_script/`). `avs_start.sh` loads the driver
(`rc.xcode4 Board_ID=0x2f00`), configures the TS input (`TsConfig meld_ts.cfg`),
then launches:

```
av_streamer -e Meld_Edid_r12_720p_pcm48.txt -c $codec -m $output $scaler -R $bitrate -r $rmxrate
```

Settings come from `avs.config`:

| Field | Meaning | Values |
|---|---|---|
| `CODEC` | video codec | `1` = MPEG-2 (default), `0` = H.264 |
| `OUTPUT` | where the TS goes | `0` = DVO (to the RF modulator), `1`/`2` = UDP (IP streaming) |
| `SCALER` | optional downscale | `0` = off, `1` = scale to 480i |
| `BITRATE` | video bitrate | up to `170` → 17.0 Mbit/s |
| `RMXRATE` | remux (full TS) rate | up to `190` → 19.0 Mbit/s |

So the same box can either feed the RF modulator (DVO) **or** stream the
transport stream over UDP to the network — it is a video-over-IP encoder as well
as a transmitter. The default MPEG-2 at a ~19 Mbit/s remux rate is exactly an
ATSC payload.

### 4.1 The PSIP virtual channel is decoupled from the RF channel

`av_streamer` also builds the stream's **PSIP** (the ATSC `TVCT`, which carries
the `major-minor` *virtual channel* a TV displays). The virtual channel is
independent of the RF channel — on a real station that is deliberate, but on this
firmware the two are set by **different subsystems that never reconcile**, so they
can disagree:

- `whitespaceagent` sets the **RF channel** on the NXP radio (the database's
  granted/preferred channel).
- `av_streamer` reads **`/etc/config/queryInfo`** at startup, takes its
  `Channel = ` value as the PSIP **`major_channel_number`**, and defaults the
  **minor** to `1` (`avs_start.sh` passes no `-M`).

Because `avs_start.sh` launches the encoder as soon as `queryInfo` is non-empty —
which first happens at the radio's **power-on default channel** (the lowest in the
grant), *before* `whitespaceagent` moves it to the preferred channel — the PSIP
**latches that early default and never re-reads it**. The result is a TV that
shows the default channel as its virtual number while the RF is on the preferred
one. Restarting `av_streamer` after the RF channel has settled makes it re-latch
the current channel. A worked example (RF 31 / virtual `21-1`, confirmed by
restart) is in [`units/mt300-av-677.md`](units/mt300-av-677.md).

---

## 5. Inputs and resolution

The input front-end is the **ADV7842**, driven from user space by `av_streamer`
(symbols `ADV7842_t::LoadEDID`, `program_input_source`,
`detect_video_standard`). It auto-detects a wide range of standards:

| Input | Standards the chip reports |
|---|---|
| Composite (CVBS) | 480i, 576i |
| S-Video | 480i |
| Component (YPbPr) | 480i, 576i, 480p, 576p, 720p 50/60, 1080i 50/60, 1080p |
| HDMI | 480i, 576i, 720p, 1080i, 1080p (50/60 Hz) |

**Does the input have to be 720p/1080i?** The *chip* handles up to 1080p. In the
shipped configuration the input is **pinned to 720p**, and the mechanism is the
EDID:

- `av_streamer` is launched with `-e Meld_Edid_r12_720p_pcm48.txt`. That EDID is
  written into the ADV7842's EEPROM and is what the device advertises to an HDMI
  source, so the source outputs **720p with 48 kHz PCM audio**. Swap the EDID
  file (several are present: `meld_edid.txt`, `meld_edid_ac3.txt`, `edid.txt`)
  and you change what an HDMI source sends.
- **Component and composite inputs are not constrained by the EDID** — they are
  auto-detected ("Component signal detected with video standard %d"), so they
  lock to whatever standard arrives, up to 1080p.

In other words, the 720p limit is a configuration choice (the EDID handed to
HDMI sources), not a hardware ceiling. Audio is PCM 48 kHz in the default EDID;
an AC-3 variant EDID is also provided.

---

## 6. Kernel and modules

| Item | Value |
|---|---|
| Kernel | **Linux `2.6.36-ts-mipsisa32r2el`** (MIPS32r2 LE), the ViXS XCodePro BSP |
| Kernel image | `uImage` in NAND (`mtdblock4`/`5`), not in the rootfs |
| Shipped module | **`xcode4drv.ko`** only — the ViXS XCode4 transcoder driver (GPL, ~11 MB, not stripped). Registers char device "boxster", creates `/dev/vixs/xcodedrv[2-4]` |

Everything else (Ethernet, USB, storage) is built into the kernel; `xcode4drv.ko`
is the only loadable module. The ADV7842 is handled in user space by
`av_streamer`, not by a kernel module.

---

## 7. Is the radio firmware in the backup?

**No.** The NXP transmitter runs its own firmware — `1.3.0` / `1.3.2` / `1.3.4`
"Tx v2" — on the transmitter board's **own processor**. It is queried and
controlled over serial, but its image is **not** present in the Linux rootfs or
the NAND dump: there is no `.bin`/`.hex`/`.srec` for it anywhere in the backup,
and nothing in Linux loads it. It lives in the NXP board's own flash and is
updated through that board, independently of the Linux system. All the backup
preserves of it is the version string and the serial command interface it
exposes (§2).

By contrast, the main-SoC software **is** fully present: U-Boot, the 2.6.36
kernel in NAND, the rootfs, the ViXS driver, `av_streamer`, and the web UI.

---

## 8. Going deeper with Ghidra

Everything above was established from higher-level artifacts — the serial command
reference, `av_streamer`'s symbols/usage/strings, the EDID and encoder configs,
the start scripts, and the module metadata — which are more reliable than
register-level reversing for these questions.

Ghidra would pay off for a **narrower** question that these sources do not
answer: the exact register programming of the ViXS encoder or the ADV7842
(clocking, input muxing, PSIP/SI table construction, DVO timing). For that,
point Ghidra at **`av_streamer`** first — it is dynamically linked, unstripped,
and carries C++ symbols, so it decompiles cleanly — rather than the large
`xcode4drv.ko`.

---

## Summary

- **Radio control:** a text protocol over `/dev/ttyS1` (115200 8N1). Channel,
  power, modulation, frequency, and on/off are all runtime settings on the NXP
  board. On the white-space unit, `wsa` is the only driver.
- **Modulation:** software-selectable — ATSC/8VSB, DVB-T 6/7/8 MHz
  (QPSK/16-/64-QAM COFDM), and QAM-64/256 for cable. Fixed to ATSC on the
  white-space unit only because the software never changes it, not because the
  hardware can't.
- **Radio firmware:** on the NXP board, not in the backup.
- **Video path:** ADV7842 input → ViXS XCode4 (kernel 2.6.36, `xcode4drv.ko`) →
  MPEG-2/H.264 transport stream → DVO → NXP modulator. Can also stream the TS
  over UDP instead of RF.
- **Resolution:** the ADV7842 handles up to 1080p; HDMI is pinned to 720p by the
  shipped EDID, while component/composite are auto-detected.
