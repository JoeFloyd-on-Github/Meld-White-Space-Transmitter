# `av_streamer` — reverse engineering (MT300-AV)

Reverse-engineering and full annotated decompilation of `av_streamer`, the
user-space application on the Meld MT300-AV that drives the ADV7842 video
front end, programs the ViXS XCode4 encoder (`/dev/vixs/xcodedrv`), and
injects ATSC PSIP / DVB-T SI tables into the transport stream.

## Provenance

- Source unit: **MT300-AV serial 677** (`192.168.1.196`), app **v1.38 / AV30**.
- Extracted from the verified backup
  `/mnt/RAID2/Meld/spectrumbridge/backup-mt300-sn677-v1.38-2026-11-22/rootfs-live/disk-mtdblock9-live.cpio.gz`,
  path `/disk/Meld/AV30/test_script/` (`AppFS_1`, the `/disk` that unit mounts).
- `av_streamer` md5 **`65023c5fd5c3f99af36c8057e1acc75d`**, 427,080 bytes, dated
  2015-10-09 (`av_streamer -v`: version `2.14`, `libadv7842 0x108`,
  `libxcodeencoder 0x10a`). The `sb_mips` install did not touch this directory.
- **Not yet verified against the live unit** (192.168.1.196 was offline at
  extraction time). When reachable:
  `tools/backup/meld_telnet.py -H 192.168.1.196 'md5sum /disk/Meld/AV30/test_script/av_streamer'`.

## Binary & files

ELF32 MIPS32r2 little-endian, dynamically linked (glibc `ld.so.1`, `libstdc++`,
`libpthread`, `libm`), **unstripped with full DWARF** (GCC 4.5.3).

| Path | What |
|---|---|
| [`ghidra/av_streamer_annotated.c`](ghidra/av_streamer_annotated.c) | **Complete annotated decompilation** (all 19 units combined, ~14.4k lines) |
| [`ghidra/annotated/`](ghidra/annotated/) | Per-compilation-unit annotated C files (`00_main.c.c` .. `18_nocu.c`), each verified identical to raw Ghidra code via `check_annotated.py` |
| [`ghidra/av_streamer_decompiled.c`](ghidra/av_streamer_decompiled.c) | Raw Ghidra 12.2 decompile |
| [`ghidra/av_streamer_functions.tsv`](ghidra/av_streamer_functions.tsv) | Address / size / name / signature for all 378 functions |
| [`ghidra/av_streamer_strings.tsv`](ghidra/av_streamer_strings.tsv) | Defined strings |
| [`ghidra/scripts/`](ghidra/scripts/) | `ExportDecomp.java`, `split_by_cu.py`, `check_annotated.py` |
| [`extract/Meld/AV30/test_script/`](extract/Meld/AV30/test_script/) | OEM directory: `av_streamer`, `xcode4drv.ko`, `TsConfig`, `avs_start.sh` & friends, EDIDs, `avs.config` |

Ghidra project state (`ghidra/project/`) is gitignored; open `AvStreamer.gpr` in
`ghidraRun` or re-run headless:

```sh
cd AvStreamer
~/Downloads/ghidra_12.2/support/analyzeHeadless ghidra/project AvStreamer \
  -import extract/Meld/AV30/test_script/av_streamer -overwrite \
  -scriptPath ghidra/scripts -postScript ExportDecomp.java ghidra
```

## Architecture (compilation units)

| Unit | Functions | Role |
|---|---|---|
| `00_main.c.c` | 9 | CLI parsing, ADV7842 init & input priority (`HDMI > Component > CVBS`), event callback, encoder start/restart, SIGTERM / `q`/`g` loop |
| `01_libxcode4encoder.c.c` | 30 | High-level encoder wrapper, send/recv threads, `/etc/config/{queryInfo,sbi,company}` parsers, ATSC PSIP & DVB-T SI table builders |
| `02_vixs_encoder.c.c` | 10 | Direct `/dev/vixs/xcodedrv` ioctl (`0x17241724`) engine: 3 fds (`hXcode`, `hXcodeRecvVideo`, `hXcodeSendData`), video/audio/mux parameters |
| `03_util_common.c.c` | 5 | UDP multicast/unicast TS socket (`7 × 188 = 1316`-byte `writev` with null-packet padding) and pthread wrappers |
| `04_libadv7842.cpp.c` | 14 | 200 ms ADV7842 polling thread, audio clock GPIO 5 muxing, input auto-reprogramming, event subscription |
| `05_xcodeio.cpp.c` | 18 | `xcode_io_t` singleton: I2C, MMR (`0x1100`/`0x1104`), and framebuffer helpers |
| `06_adv7842.cpp.c` | 19 | `ADV7842_t` class: I2C register programming, EDID/SPI EEPROM load, `HPA_AUTO_INT_EDID`, STDI/SDP video standard detection |
| `07_xcodeapi.cpp.c` | 24 | ViXS SDK C wrappers over `/dev/vixs/xcodedrv` (only I2C/MMR/open/close are active; streaming wrappers are dead code) |
| `08`–`15` (`libdvbpsi`) | 89 | Statically linked VideoLAN `libdvbpsi-1.1.0` (only `genNIT`/`genSDT`/`genTDT`/`genEIT` table generation is active; demux/decoders are dead) |
| `16`–`18` (CRT) | 9 | glibc `elf-init.c`, `stat64.c`, and `.init`/`.fini`/`__start` |

## Key technical findings

1. **No transmitter control:** `av_streamer` never calls `MeldAgent` or any shell
   command (`system`/`popen`). The video-connect → `MeldAgent -T` path in
   [`docs/av-tx-enable-flaw.md`](../docs/av-tx-enable-flaw.md) lives outside
   `av_streamer`.
2. **Command line & hidden flags** (`getopt "c:m:e:n:P:T:d:t:r:R:io:p:M:gl:f:F:v?hH"`):
   - Documented: `-c MPEG2|H264`, `-m UDP|DVO|FILE`, `-e <EDID>`, `-n <ip>`
     (`192.168.1.193`), `-P <port>` (`5002`), `-T <ttl>` (`32`), `-d/-t` data
     insertion, `-r` RMX rate (`1.5–19` Mbps), `-R` video bitrate (`1–17` Mbps),
     `-i` null packets, `-o 480i` scaler, `-p` PCR PID, `-M` PSIP minor channel,
     `-g` table generation.
   - Undocumented: `-l <n>` sets `g_llc_delay` (patches ADV7842 IO map `0x40` reg
     `0x19` `LLC_DLL[4:0]`); `-f <file>` captures TS to `/mnt/sdcard/...`
     (requires `/dev/mmcblk0p1` mounted at `/mnt/sdcard`); `-F <file>` captures
     without the SD check.
3. **Why PSIP virtual channel latches at boot** (`docs/radio-and-video.md` §4.1):
   `encoder_send_data_thread_entry()` reads `/etc/config/queryInfo`,
   `/etc/config/sbi`, and `/etc/config/company` **once** before its loop. In ATSC
   mode (`modulationmode == 1`) it emits 11 TS packets (`2068` bytes) on PIDs
   `0x1FFB` (STT, MGT, TVCT), `0x1000..0x1003` (EIT-0..3), and `0x1100..0x1103`
   (ETT-0..3). `FormATSC_TVCT()` uses `conf.chan_freq` as `major_channel_number`,
   `sti_minor` (default `1`) as minor, `"MELD300"` as short name, and builds
   Extended Channel Name (`0xA0`) from `"<company>,<fcc>(<sn>,<lng>, <lat>)"`
   with `Service Location (0xA1)` hardcoded to MPEG-2 video PID `0x30`, AC-3
   audio PID `0x31`, PCR PID `0x30`. In DVB-T mode it emits 4 packets (`752`
   bytes: NIT `0x10`, SDT `0x11`, TDT `0x14`, EIT `0x12`) via `libdvbpsi`.
4. **HDMI EDID & cold-boot HPA** (`ADV7842_t::LoadEDID`, `program_input_source`):
   - `LoadEDID()` parses up to `279` (`0x117`) `"%x %x %x"` entries into
     `adv7842load_EDID_tab`, sets HDMI map `0x34:0x69` (`HPA_AUTO_INT_EDID`) =
     `0xA4`, replays the table, and stores it to SPI EEPROM via KSV map
     `0x32:0x7F=0x04` / `0x32:0x7E=0x01`.
   - `program_input_source()` enables internal EDID on all 4 ports (`0x32:0x77 =
     0x0F`), sleeps 1 s, and verifies `0x32:0x7D == 0x0F`, without toggling HPD
     afterward.
   - `adv7842_thread()` polls every 200 ms: `0x20:0x6F` bit 1 (HDMI 5V detect →
     `ADV7842_EVT_HDMI_HPD`), `0x34:0x04` bit 4 (encryption → forced bluescreen),
     and `0x34:0x39[3:0]` (sample rate → toggles XCode4 MMR `0x1100`/`0x1104`
     GPIO 5 `0x20` HIGH for 48 kHz HDMI I2S audio, LOW otherwise).
5. **ViXS XCode4 encoder & mux** (`02_vixs_encoder.c.c`):
   Opens `/dev/vixs/xcodedrv` three times (`hXcode`, `hXcodeRecvVideo`,
   `hXcodeSendData`) and issues ioctl `0x17241724` (`CMD_SDEV_OPEN_CAPTURE`
   `0x1f`, `CLOSE` `0x11`, `CTRL` `0x12`, `SEND` `0x13`, `RECV` `0x14`,
   `TSCTRL_GET_INFO` `0x102`). Program `3`, PMT `0x20`, Video `0x30`, Audio `0x31`
   (hardcoded MP2 192 kbps @ 48 kHz), default RMX rate 19.3 Mbps.
6. **Notable bugs found in the OEM binary**:
   - **100% CPU busy-spin (`main`)**: when `stdin` is not a tty (always true under
     `avs_start.sh`), `while (SigTerm == 0)` spins with no `sleep()`, repeatedly
     copying `"main.c : Caught SIGTERM"` into a stack buffer.
   - **`adv7842_get_input` 1080p60 Component bug**: bitmask `0x5af0` omits bit
     13 (`XC_INPUT_HDMI_COMPONENT_1080P = 35`), reporting 1080p60 component as
     `NONE`.
   - **`adv7842_enum_input` side effects**: tears down the active input on exit
     and forgets to reset `std` before probing HDMI.
   - **`util_thread_notify_ready` race**: calls `pthread_mutex_unlock` instead of
     `pthread_mutex_lock` before `pthread_cond_signal`.
   - **`vixs_encoder_update_stream`**: live-bitrate path multiplies `newBitrate *
     1000` (would overflow bps) while inactive path uses bps directly; prints
     `"Encoder is NOT Active!"` on normal resolution restarts (`flag = 6`).
