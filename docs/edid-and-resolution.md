# MT300-AV EDID Format, 1080p/1080i Resolution, and Overscan

This note documents how the MT300-AV configures HDMI capabilities via its
EDID `.txt` scripts, how `av_streamer` and the ViXS XCode4 encoder handle
`720p` / `1080i` / `1080p`, why a 1080p laptop source can look cropped and soft
on a receiving TV, and what to change in the EDID to disable source-side
overscan.

Cross-references:
- [`docs/radio-and-video.md`](radio-and-video.md) §5 (signal chain & cold-boot HDMI log)
- [`AvStreamer/README.md`](../AvStreamer/README.md) & [`AvStreamer/ghidra/av_streamer_annotated.c`](../AvStreamer/ghidra/av_streamer_annotated.c)

---

## 1. How the `edid.txt` format works

The EDID files in `/disk/Meld/AV30/test_script/` (mirrored in
[`AvStreamer/extract/Meld/AV30/test_script/`](../AvStreamer/extract/Meld/AV30/test_script/))
are **ADV7842 I2C register-write scripts**, not raw `.bin` EDID blobs. Every
non-empty line has the form:

```text
<8-bit_i2c_addr_hex> <reg_hex> <val_hex> ; optional comment
```

`ADV7842_t::LoadEDID()` ([`06_adv7842.cpp.c`](../AvStreamer/ghidra/annotated/06_adv7842.cpp.c))
parses up to **279 (`0x117`)** lines with `sscanf("%x %x %x")`, writes them
into `adv7842load_EDID_tab`, arms auto-HPA (`0x34:0x69 HPA_AUTO_INT_EDID = 0xA4`),
replays the table over I2C, and commits the EDID to the ADV7842 SPI EEPROM
(`0x32:0x7F = 0x04`, `0x32:0x7E = 0x01`).

### Register groups in the script

| 8-bit addr (7-bit) | ADV7842 Map | Purpose in the EDID script |
|---|---|---|
| `40` (`0x20`) | IO Map | `40 FF 80` resets the chip; `40 F1..FE` maps internal sub-banks (`KSV=64`, `EDID=6C`, `HDMI=68`, etc.) |
| `64` (`0x32`) | KSV / Repeater | `64 77 00` disables internal EDID on all 4 ports while writing; `64 70..76` sets Source Physical Address (SPA); `64 77 0F` re-enables internal EDID |
| `6C` (`0x36`) | EDID RAM | `6C 00..7F` = 128-byte VESA EDID 1.3 base block; `6C 80..FF` = 128-byte CEA-861 extension block |

### Key EDID byte offsets (`6C <reg> <val>`)

- **`0x00..0x7F` (Base EDID 1.3 block):**
  - `0x36..0x7D`: four 18-byte **Detailed Timing Descriptors (DTDs)** (`0x36` = DTD1 preferred timing, `0x48` = DTD2, etc.).
  - `0x7F`: Base-block checksum (`(sum(0x00..0x7F) & 0xFF) == 0`).
- **`0x80..0xFF` (CEA-861 extension block):**
  - `0x80 = 0x02` (CEA extension tag), `0x81 = 0x03` (rev 3), `0x82` = byte offset where DTDs begin (`0x84..dtd_start` holds CEA Data Blocks).
  - **`0x83`: CEA flags byte:**
    - bit 7 (`0x80`): **Underscan** (`1` = IT/pixel-mapped sink, no overscan; `0` = overscanned TV sink)
    - bit 6 (`0x40`): Basic Audio
    - bit 5 (`0x20`): YCbCr 4:4:4
    - bit 4 (`0x10`): YCbCr 4:2:2
    - bits 3..0: number of native DTDs
  - **CEA Video Data Block (tag `2`):** list of 1-byte **VIC** codes; bit 7 (`0x80`) marks a VIC as **native** (`*`).
  - **CEA Audio Data Block (tag `1`):** 3-byte Short Audio Descriptors (`fmt`, `channels`, `sample-rate bitmask`).
  - **`0xFF`: Extension-block checksum** (`(sum(0x80..0xFF) & 0xFF) == 0`).

---

## 2. Shipped EDID files compared

All four scripts in `test_script/` have valid `0x00` checksums on both blocks:

| File | Preferred DTD (`0x36`) | CEA Video VICs | Audio (`tag 1`) | CEA `0x83` |
|---|---|---|---|---|
| `Meld_Edid_r12_720p_pcm48.txt` *(default in `avs_start.sh`)* | `1280×720p@74.25 MHz` (DTD2 `1024×768p`) | **`5*`** (1080i60 native), `4` (720p60), `1` (480p) | 2-ch PCM, **48 kHz only** (`0x04`) | `0x71` (`underscan=0`) |
| `meld_edid.txt` | *(none)* | **`5*`**, `4`, `1` | 2-ch PCM, 32/44.1/48 kHz (`0x07`) | `0x71` (`underscan=0`) |
| `meld_edid_ac3.txt` | *(none)* | **`5*`**, `4`, `1` | 6-ch AC-3, 32/44.1/48 kHz (`0x07`) | `0x71` (`underscan=0`) |
| `edid.txt` | `1920×1080p@148.50 MHz` (DTD2 `1280×720p`) | **`16*`** (1080p60 native), `4`, `3`, `18`, `1`, `19`, `17`, `7`, `5`, `6`, `34`, `20`, `21` | 2-ch PCM, 32/44.1/48 kHz (`0x07`) | `0x71` (`underscan=0`) |

**Audio sample-rate caveat:** `adv7842_thread()` ([`04_libadv7842.cpp.c`](../AvStreamer/ghidra/annotated/04_libadv7842.cpp.c)) only drives XCode4 GPIO 5 HIGH (selecting the HDMI I2S audio clock) when incoming HDMI audio is **48.0 kHz**. If using `edid.txt` (or a custom EDID), keep the source at 48 kHz or restrict the Audio Data Block to `0x04` (48 kHz only).

---

## 3. How `av_streamer` handles `720p`, `1080i`, and `1080p`

### HDMI front end (`04_libadv7842.cpp.c`, `06_adv7842.cpp.c`)
- The ADV7842 accepts **up to `1080p60`**: `detect_video_standard()` classifies `1080p60`, `map_video_std()` maps it to `VIDEO_STD_1080P_60HZ` (`9`), and `adv7842_program_input_internal()` switches HDMI to `XC_INPUT_HDMI_TO_HDMI_1080P` (`45`).
- *(Component bug note: `adv7842_get_input()` uses bitmask `0x5af0`, which omits bit 13 for `XC_INPUT_HDMI_COMPONENT_1080P`, so 1080p60 **Component** is misreported as `NONE`. HDMI is unaffected.)*

### MPEG-2 encoder (`00_main.c.c`, `02_vixs_encoder.c.c`)
- In `main.c`, `start_encoder()` sets `outVideoStandard = std` (1:1 with the detected input), unless `-o 480i` (`SCALER=1` in `avs.config`) is passed to downscale to `480i`.
- `vixs_encoder_init()` has explicit input and output cases for `720p50/60`, **`1080i50/60`** (`1920×1080` @ 25/30 fps interlaced), and **`1080p60`** (`1920×1080` @ 60 fps, progressive `video_in_flags |= 0x100`), and only enforces `outVideoStandard <= inVideoStandard` (no upscaling).
- **Can `av_streamer` take `1080p` in and convert to `1080i`?** Not unmodified: `start_encoder()` has no `-o 1080i` flag (only `-o 480i`). Patching the `-o 480i` branch in `start_encoder()` from `VIDEO_STD_480I` (`1`) to `VIDEO_STD_1080I_60HZ` (`7`) would pass `in=1080p60` / `out=1080i60` to `xcode4drv.ko`, though hardware re-interlacing in `xcode4drv.ko` is unverified. For ATSC broadcast (which supports `1080i30` and `720p60`, not `1080p60`), having the source send `1080i60` or `720p60` directly via EDID is the clean path.

### Why unit 677 locked at `1080i@60` and looked cropped/soft
During the unit-677 debug session ([`docs/radio-and-video.md`](radio-and-video.md#L284-L294)), `av_streamer` logged:

```text
00:06:04 adv7842 : Input video is 1080i@60
```

Even though `avs_start.sh` passed `Meld_Edid_r12_720p_pcm48.txt`, its CEA block marks **VIC `5*` (`1920×1080i@60Hz`) as native**, so the laptop negotiated `1080i@60` over HDMI while rendering a `1080p` desktop. Combined with `underscan = False` (`0x71` at `0x83`), that produces both symptoms observed on the TV:
1. **Cropping:** `underscan = False` signals an overscanned TV sink, so the GPU and/or TV crops ~5% around the edges.
2. **Softness:** scaling + interlacing a 1080p computer desktop into `1080i@60` blurs fine 1-pixel UI text and lines. (For a computer desktop, `720p60` progressive is typically sharper than `1080i60`.)

---

## 4. Fixing overscan in the EDID (`6C 83` & `6C FF`)

Modern digital displays are pixel-mapped and should not overscan. To tell the
HDMI source **not** to apply overscan/underscan compensation, set **bit 7
(`0x80`, Underscan) of CEA byte `0x83`** and update the Block 1 checksum at
**`0xFF`**:

| File | Current bytes | Pixel-mapped (no overscan) |
|---|---|---|
| `Meld_Edid_r12_720p_pcm48.txt` | `6C 83 71` / `6C FF 97` | **`6C 83 F1`** / **`6C FF 17`** |
| `edid.txt` | `6C 83 71` / `6C FF A5` | **`6C 83 F1`** / **`6C FF 25`** |

*(Adding `0x80` to byte `0x83` requires subtracting `0x80` from checksum byte `0xFF` modulo `0x100` so the 128-byte block `0x80..0xFF` still sums to `0x00` mod 256.)*

**Two-sided caveat:**
- Changing `6C 83 F1` fixes **source-side** (PC/GPU) overscan/underscan scaling before the signal enters the ADV7842.
- On the **receiving TV**, ATSC over-the-air `720p`/`1080i` tuners often still apply default TV overscan unless the TV's own picture/aspect setting is switched to *Just Scan* / *1:1 Pixel* / *Full Pixel*, because the ATSC broadcast stream itself has no flag forcing a TV to disable overscan.
