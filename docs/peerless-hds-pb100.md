# The Peerless-AV rebrand: PeerAir™ "Pico Broadcaster" HDS-PB100

The Meld MT300 was also sold under another company's brand. **Peerless-AV**
(the mounting/AV-distribution company) resold it as the **PeerAir™ Pico
Broadcaster, model `HDS-PB100`**, part of its short-lived **PeerAir** wireless-
video line (`peerair.peerless-av.com`). It is the same hardware and the same
Spectrum Bridge white-space stack described throughout this repo, in a different
box label.

Source: the product page as archived 2013‑10‑21 (Internet Archive capture of
`peerless-av.com/en-us/professional/products/hds-pb100`), plus its linked US
"Sell Sheet" (`LIT0976`). The device shown is the familiar MT300 form factor — a
small black chassis with a single vertical whip antenna.

## Which MT300 variant this is

The HDS-PB100's input is a **LAN port carrying an MPEG transport stream** — "this
device accepts content from a computer or media player with IP transport stream
output" — and the spec list has **no HDMI / component / composite inputs at all**.
That makes the HDS-PB100 the **`MT300-IT`** variant from
[`hardware-versions.md`](hardware-versions.md) (the IP-fed, video-less board,
`Board_ID 0x00F6`, `MELD_PLATFORM MT300-IT`), *not* the `MT300-AV` units that have
the ADV7842 A/V front end. "IT" is the IP-transport personality of the radio; the
rebrand is that personality productized for digital-signage integrators.

Everything else is the MT300 we know: a ViXS XCode MIPS board running the
`whitespaceagent` / Spectrum Bridge stack, transmitting **ATSC (8VSB)** on a
vacant UHF TV "white space" channel, modulating an **MPEG‑2** transport stream.

## What it is, in Peerless's words

Marketed as a digital-signage tool: feed it a transport stream over IP and it
broadcasts that HD content over the air to **any number of ordinary DTVs** (ones
with an ATSC tuner and an antenna) within range — no per-screen receiver or
cabling. A bundled "Streaming Media Server" application transcodes arbitrary media
to the MPEG‑2 broadcast format, and a web GUI handles setup. List price **US
$2,249**; marked **Patented**.

## Specifications (as published)

| Spec | Value |
|---|---|
| Model | HDS-PB100 (PeerAir Pico Broadcaster) |
| Wireless technology / connection | ATSC broadcast (8VSB) |
| Wireless frequency | 500000–700000 kHz (**500–700 MHz**, UHF TV band) |
| Wireless range | 350 ft (106 m) radius, omnidirectional |
| Encoding | MPEG‑2 |
| Video resolution | 720p / 1080i |
| Audio | AC‑3, analog stereo |
| Video streaming latency | ~1 sec |
| Input | LAN port — IP transport-stream in (+ network control) |
| Power supply | 12 V, 3 A wall adapter |
| Plug type | US |
| Operating temperature | 32–131 °F (0–55 °C) |
| Color | Black |
| Ship dimensions | 11.5 × 3.75 × 20.5 in (292.1 × 95.3 × 520.7 mm) |
| Shipping weight | 4.87 lb (the page's "4.87 kg" is a typo) |
| UPC | 735029292180 |

**Features list (paraphrased):** omnidirectional broadcast to ~350 ft; serves an
unlimited number of DTVs in range; LAN transport-stream input with local/remote
control; bundled streaming-media-server transcoder to MPEG‑2; adjustable output
power; web GUI; configurable IP addressing to join an existing network;
wall/ceiling mounting flanges — and, tellingly, it is sold as an **"FCC compliant
broadcast device which includes [the] first year of FCC licensing subscription."**

**Companion accessory:** `HDS-ATA100`, a passive multi-directional
over-the-air HD **receive** antenna recommended for the DTVs picking up the
broadcast.

## Why this rebrand matters to this project

- **It confirms the white-space-database dependency was baked into the product,
  not an afterthought.** Peerless advertised that the price *includes the first
  year of "FCC licensing subscription"* — i.e. the paid Spectrum Bridge
  (`tvws.spectrumbridge.com`) registration/channel service this repo reverse-
  engineers and replaces. The transmitter was always gated behind that recurring
  cloud check (see [`README.md`](../README.md) and
  [`wsa-overview.md`](wsa-overview.md)); the marketing copy says so in plain terms.
  When Spectrum Bridge retired the service, every HDS-PB100 went inert for the
  same reason every MT300 did.
- **It pins the HDS-PB100 to the `MT300-IT` hardware variant** (IP input, no A/V),
  which complements the `MT300-AV` bench units analysed here and the three-variant
  breakdown in [`hardware-versions.md`](hardware-versions.md).
- **The specs line up with the firmware findings:** ATSC/8VSB on 500–700 MHz UHF
  matches the channel map the agent offers (and the 600 MHz-repack caveats in
  [`HOW-TO.md`](../HOW-TO.md)); MPEG‑2 encoding and 720p/1080i match the
  `av_streamer` pipeline and EDID behaviour in
  [`radio-and-video.md`](radio-and-video.md).
- The underlying radio is Meld's FCC-certified MT300 (FCC ID `OKVMT300`, grantee
  code `OKV` = Meld Technology); Peerless-AV resold that certified hardware under
  the `HDS-PB100` SKU.

The practical upshot: the local Spectrum Bridge stand-in (`sb_mips`) and the
restore procedure in this repo apply to a Peerless **HDS-PB100** exactly as they
do to a Meld-badged **MT300-IT** — it is the same device.
