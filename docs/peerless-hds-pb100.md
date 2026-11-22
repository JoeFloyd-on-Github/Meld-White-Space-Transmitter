# The Peerless-AV rebrand: PeerAir™ "Pico Broadcaster" HDS-PB100

The Meld MT300 was also sold under another company's brand. **Peerless-AV**
(the mounting/AV-distribution company) resold it as the **PeerAir™ Pico
Broadcaster, model `HDS-PB100`**, part of its short-lived **PeerAir** wireless-
video line (`peerair.peerless-av.com`). It is the same hardware and the same
Spectrum Bridge white-space stack described throughout this repo, in a different
box label.

Sources, both kept in this repo under [`datasheets/`](datasheets):
the official US sell sheet **`LIT-0976` rev.2**
([PDF](datasheets/HDS-PB100_PeerAir_Pico_Broadcaster_LIT-0976-rev2.pdf)) and the
**User Manual and Installation Guide** (sheet `180-9044-1`, issued 2013‑11‑07)
([PDF](datasheets/HDS-PB100_User_and_Installation_Manual_180-9044-1.pdf)); plus
the 2013‑10‑21 Internet Archive capture of the product page. The device shown is
the familiar MT300 form factor — a small black chassis with a single vertical whip
antenna (US Patent 8,063,996). It won a *NewBay Media Product Innovation Award
2013* (honorable mention).

**The sell sheet settles the attribution outright:** its own legal footer states
*"Pico Broadcaster is a trademark of Meld Technologies, Inc."* while "Peerless-AV®"
and "PeerAir™" are trademarks of Peerless Industries, Inc. So the HDS-PB100 is
Meld's device (the MT300), carried and sold through Peerless‑AV's channel.

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

Values from the `LIT-0976` sell sheet (the web page's shorter list agrees):

| Spec | Value |
|---|---|
| Model | HDS-PB100 (PeerAir Pico Broadcaster) |
| Input | Audio/Video via 10/100 LAN port (**MPEG‑2 transport stream**) |
| Output | ATSC-modulated HD digital-TV signal; stereo / AC‑3 audio |
| Transmit power | **40 mW** (variable, up to 40 mW) |
| Channel selection | **White-space-database compliant** |
| Broadcast frequency | **500–700 MHz** (UHF TV band) |
| Channel range | **21 to 51** |
| Broadcast range | 350 ft (106 m) maximum radius, omnidirectional |
| Encoding | MPEG‑2 |
| Video resolution | 720p / 1080i |
| Video streaming latency | ~1 sec |
| Control | LAN |
| Power supply | Input 100–240 V, 50/60 Hz, 1 A → output **12 V, 3 A** wall adapter |
| Operating temperature | +32 to +131 °F (0 to 55 °C) |
| Compliance | Safety **UL60065**; **FCC Class B** |
| Product dimensions | 14.19 × 7.50 × 1.74 in (360 × 190 × 44 mm) |
| Product weight | 3.36 lb (1.52 kg) |
| Package contents | broadcaster, AC power adapter, startup disc, quick-start guide, instruction manual |
| Package / ship weight | 4.87 lb; box 20.5 × 11.5 × 3.75 in (521 × 292 × 70 mm) |
| UPC | 735029292180 |
| Color | Black |

The **Channel range 21–51** and **white-space-database-compliant channel
selection** on the sell sheet match this repo's firmware findings exactly — the
agent offers channels 21–51 and will not transmit until the database hands it a
list (see [`hardware-versions.md`](hardware-versions.md) and
[`HOW-TO.md`](../HOW-TO.md)). The **40 mW** maximum is the device's rated EIRP,
set by the adjustable power control the web UI exposes as power levels 1–11.

**Features list (paraphrased):** omnidirectional broadcast to ~350 ft; serves an
unlimited number of DTVs in range; LAN transport-stream input with local/remote
control; bundled streaming-media-server transcoder to MPEG‑2; adjustable output
power; web GUI; configurable IP addressing to join an existing network;
wall/ceiling mounting flanges — and, tellingly, it is sold as an **"FCC compliant
broadcast device which includes [the] first year of FCC licensing subscription."**

**Companion accessory:** `HDS-ATA100`, a passive multi-directional
over-the-air HD **receive** antenna recommended for the DTVs picking up the
broadcast.

## How you actually stream content to it

The manual answers the questions the spec sheet left open — how content gets into
the box, in what format, and with what software.

**The input is IP, not A/V.** The only content input is the rear **RJ45 LAN
port**. The device expects an **MPEG‑2 transport stream delivered over UDP** (the
manual's FAQ is explicit: *"the only input … is an RJ45 network port as the
Broadcaster requires a UDP stream"*). There are no HDMI/component inputs on this
model — it is fed entirely over the network. Default addressing (all user-settable
in the web UI):

| Role | Default |
|---|---|
| Config / device IP (web UI) | `192.168.1.194` |
| Streaming IP — the "digital modulator" that receives the transport stream | `192.168.1.193` |
| Streaming (base) port | `5002` |

That streaming address `192.168.1.193` is the same value that shows up as the live
**`StreamingIpAddress`** on the bench MT300‑AV in this repo — i.e. the UDP target
the encoder sends the transport stream to, confirming the mechanism end to end. If
several units share a LAN, each needs a **unique** streaming IP.

**Media format.** The device broadcasts **MPEG‑2 TS** with **stereo / AC‑3**
audio at **720p / 1080i**, and each white‑space channel can carry a stream of up
to **19.2 Mbps**. Source material in other formats must be transcoded to MPEG‑2
first.

**Special software — yes, it's bundled.** The unit ships with a **USB flash drive**
containing the **Peerless‑AV Streaming Media Server (Lite)** application (Windows;
`setup.exe`). It both transcodes arbitrary files to MPEG‑2 and streams them to the
modulator. Typical flow:

1. **Settings** — set a data folder and a **Channel Name + Channel Number** (the
   virtual channel a DTV tunes to, e.g. 71), then **New Device** → enter
   **Config IP `192.168.1.194`**, **Streaming IP `192.168.1.193`**, **Port `5002`**.
2. **Preparation** — drag in or **Import Files**, then **Transcode** to the MPEG‑2
   broadcast format (the app decides whether a given file needs transcoding; its
   Help lists the accepted source types), and optionally assemble a **Playlist**.
3. **Playback** — **Browse**, pick a file or playlist, **Play** (optionally **Loop
   the video**).

**You are not strictly locked to their app.** Because the device just consumes an
MPEG‑2 UDP transport stream at `192.168.1.193:5002`, anything that can emit a
compliant stream to that address will feed it — the manual's FAQ notes that while
the bundled software streams *pre-encoded* AV files, *"there is open source
software that has the ability to stream from a desktop,"* i.e. to push a live
desktop/transport stream to the broadcaster. The bundled app is simply the
supported, turnkey path.

This is the same pipeline the firmware exposes: the **AV** units add an HDMI/
component capture front end (`av_streamer` encodes to a transport stream), while
the **IT / HDS‑PB100** has *only* this IP/UDP transport-stream input — the
`av_streamer` UDP-output path (its `avs.config` `IPADDR` / `IPPORT`) described in
[`radio-and-video.md`](radio-and-video.md).

**It still needs the internet, and still needs the database.** The web UI and
registration flow are exactly those documented elsewhere in this repo: browse to
`192.168.1.194`, fill in the *Who You Are* / *Where You Are* (latitude/longitude)
registration, and the transmitter stays disabled until the Spectrum Bridge
database returns a channel list — polled periodically to "verify the current
license." The manual quotes the annual FCC white-space fee at ~$55 with the first
year included — the recurring dependency this repo exists to replace.

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
- **The manual confirms the FCC identity outright.** Its FCC interference
  statement lists **`FCC_Model: MT300v1`** and **`FCC ID: OKVMT300`** — the same
  Meld grant used by the units analysed in this repo. Peerless-AV resold that
  certified hardware under the `HDS-PB100` SKU; it is a Part 15 device.

The practical upshot: the local Spectrum Bridge stand-in (`sb_mips`) and the
restore procedure in this repo apply to a Peerless **HDS-PB100** exactly as they
do to a Meld-badged **MT300-IT** — it is the same device.
