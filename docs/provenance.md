# Provenance: who wrote what

The MT300 firmware is **two vendors' work meeting at hand-wired seams**, and that
structure is visible in the files themselves — copyright headers, version-control
keywords, branding and config namespaces all line up the same way. This matters
because it predicts where the system is fragile: every bug found in this project so
far sits in the **Meld integration layer**, not in the Spectrum Bridge base.

## The short version

- **Spectrum Bridge, Inc.** wrote the base stack: the white-space **agent**
  (`whitespaceagent`), its **helper scripts** (`wsagent_start` / `wsagent_stop`),
  the `sbi` configuration system, and the **web UI framework and its core pages**.
  All of it is one body of work from 2012, by the same developer, in Spectrum
  Bridge's own version control.
- **Meld Technology, Inc.** *integrated* that work to make a shipping product:
  rebranded the web UI, added pages and a config section that expose the device's
  radio and hardware, wrote the hardware/video layer (`MeldAgent` for the NXP
  radio, `av_streamer` for the ViXS/ADV7842 video path), wired the boot scripts,
  and added the glue that **links registration to the transmitter enable**.

Meld built *on top of* Spectrum Bridge's code rather than replacing it — in several
places you can see Meld's branding sitting directly over Spectrum Bridge's own
copyright.

## The evidence

### Spectrum Bridge wrote the agent, the scripts, and the base web UI

Every one of these files carries the identical header:

```
(c) 2012 Copyright Spectrum Bridge, Inc.
All rights reserved.
This software contains confidential and proprietary information.
The content or any derivative work may be used only as authorized
by a licensing agreement from Spectrum Bridge, Inc.  All copies must
include this header.
```

and the same version-control signature — Spectrum Bridge's SVN, developer
**`mario`**, 2012:

| File | Area | SVN keyword |
|---|---|---|
| `whitespaceagent` (binary) | the white-space agent (SBI v3 REST, TLS) | — (SB protocol throughout) |
| `wsagent_start` | agent launcher | `$Revision: 11 $ … $Author: mario $` (2012) |
| `cbi.js`, `xhr.js` | web UI framework (config-bind + XHR) | `$Author: mario $` 2012-08-09 |
| `cascade.css` | web UI styling | `$Author: mario $` 2012-10-04 |
| `config.php`, `index.php`, `network.php` | base web pages | `$Author: mario $` 2012 |
| `preferences.php`, `processor.php`, `status.php` | base web pages | `$Author: mario $` 2012 |
| `registration.php` | registration page | `$Author: mario $` 2013-07-11 |
| `upgrade.php` | firmware-upgrade page | `$Author: mario $` 2012-10-25 |

The web pages also *render* the attribution — a footer on each page reads:

```
© 2012 Spectrum Bridge, Inc. All Rights Reserved.
```

and the framework is unmistakably Spectrum Bridge's own: the `cbi-*`
("config-bind interface") markup and `cbi_d_update()` JavaScript, the `sbi`
(Spectrum Bridge Info) config namespace, and `registration.php`'s embedded table
of SBI protocol status codes ("Malformed request", "FCC identifier not recognized
or supported", "Location outside of regulatory domain", "Antenna height above 30
meters", …).

(Repo copy of the base web UI:
[`root/disk/Meld/meldtech/disk/www/`](../root/disk/Meld/meldtech/disk/www).)

### Meld integrated it — branding over the top, hardware underneath

Meld's changes are layered onto Spectrum Bridge's files without removing the SB
attribution, which is the clearest tell:

- **Rebranding over SB's own copyright.** The page header logo is
  `images/meld.jpg` and titles were changed to e.g.
  `<title>MELDTECH - Radio Preferences</title>` — while the **Spectrum Bridge
  copyright footer is left in place underneath.** A Meld logo over a Spectrum
  Bridge page that still says "© 2012 Spectrum Bridge" is the integration in a
  single screen.
- **Device features bolted into SB's framework.** Meld added a `meldtech` section
  inside Spectrum Bridge's `sbi` config namespace — `cbi-sbi-meldtech`,
  `cbid.sbi.meldtech.Power`, `…PreferredChannels` — to expose Power Level,
  Available Channels and Preferred Channels through SB's `cbi` UI.
- **New hardware/RF pages** not in SB's base set: `RF_Control.php` (power,
  modulation, the `e1`/`e0` transmitter enable) and `SystemInfo.php` (software
  versions, radio controls).
- **The hardware/video layer is entirely Meld's:** `MeldAgent` (NXP radio over
  `/dev/ttyS1`: channel, power, TX enable/disable, status), `av_streamer` (ViXS
  XCode4 encoder + ADV7842 capture, EDID, PSIP), the `AV30` app, and the boot
  wiring (`rcS` → `setipeth0` → `avs_start.sh`, "MELD Technology Inc., AV30").
- **Dated Meld edits inside SB's scripts.** `wsagent_start` is a Spectrum Bridge
  2012 file with Meld additions pasted in as plain dated comments (not SB's SVN
  keywords): *"Added July 28, 2013"* (clear the `/dev/ttyS1` serial buffer before
  the agent runs) and *"Added February 03, 2015"* (the `WSAgentRestart` /
  `TXAutoEnable` branching that chooses between `whitespaceagent`, `MeldAgent -T`
  and `MeldAgent -O`). This file is the single clearest picture of the two
  codebases meeting.

## Why it matters

The division of labor explains the fragility. Spectrum Bridge's agent speaks a
clean, self-contained protocol and has behaved correctly throughout this project.
The failures all live where **Meld wired SB's "registered / channel granted" state
to the device's own radio and video**:

- **Cold-boot TX stays disabled** — the enable (`MeldAgent -T` / `e1`) is Meld's,
  tied to a video-connect event, and is never re-asserted at boot. See
  [`av-tx-enable-flaw.md`](av-tx-enable-flaw.md).
- **PSIP virtual channel `21-1` vs RF channel 31** — `av_streamer` (Meld) latches
  the channel from `queryInfo` at startup, out of step with the agent's grant. See
  [`radio-and-video.md`](radio-and-video.md) §4.1.
- **Cold-boot HDMI won't lock until re-plug** — an ADV7842/EDID/HPD timing issue
  in Meld's video path. See [`radio-and-video.md`](radio-and-video.md) §5.1.

In each case Spectrum Bridge's base did its job; the seam Meld added around it is
where the behavior breaks.
