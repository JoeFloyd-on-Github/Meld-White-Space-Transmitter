# 1. The device and the stranding

[← Guide index](README.md) · [Next: qemu-user →](02-qemu-user.md)

## What the MT300 is

The Meld Technology MT300 is a **TV-white-space (TVWS) transmitter**: a fixed
broadcast radio that transmits on UHF television channels left vacant in a given
location. White space is regulated spectrum — you may only use a channel that is
genuinely unused near you, and only with permission. In the US that permission
comes from an FCC-approved **white-space database**; the MT300 used
**SpectrumBridge**.

| Attribute | Value |
|---|---|
| CPU / SoC | ViXS XCode (XCodePro100), 32-bit MIPS32r2, little-endian |
| Kernel | Linux **2.6.36** (`2.6.36-ts-mipsisa32r2el`), built with gcc 4.5.3 |
| Userland | BusyBox + a small OpenWrt-style tree, glibc 2.9 on `/disk` |
| Radio | NXP-based transmitter ("Tx v2", firmware `1.3.0 v0036`) on `/dev/ttyS1` |
| Web UI | `thttpd` (with embedded PHP) serving `/disk/www` |
| Boot | U-Boot → A/B kernel images in NAND → yaffs2 app partition on `/disk` |
| Identity | `FCC ID OKVMT300`, serial `371` |

The agent that ties it together is **`whitespaceagent`** (the binary everyone
here calls `wsa`): a statically linked MIPS executable, **compiled with full
DWARF debug info**, which is what made this whole investigation tractable. Its
behaviour is documented in [`../wsa-overview.md`](../wsa-overview.md).

## How it decides to transmit

Simplified, the agent runs two cooperating threads:

- **Thread B (serial/radio)** talks to the NXP radio over `/dev/ttyS1`. On
  startup it interrogates the radio for its **firmware version, serial number
  and FCC ID**, and only once it has a serial number *and* an FCC ID does it
  signal the other thread to start. (This detail matters enormously later — see
  [chapter 7](07-radio-handshake.md).)
- **Thread A (database client)** builds the device's location/identity into an
  XML document, opens a **TLS connection to `tvws.spectrumbridge.com:443`**,
  **registers** the device, then **requests the list of channels** it is allowed
  to use. It parses the reply, filters it to the hardware's capability band
  (channels 21–51), writes the surviving channels to `/etc/config/channels`, and
  tells the radio to transmit on one of them.

The crucial property: **no valid channel list, no transmission.** If the database
call fails — bad DNS, refused connection, non-200 response, malformed body — the
agent empties its channel map, the transmitter stays off, and it waits ~23 hours
before trying again. This is not a bug. It is the regulatory contract: a
white-space device that cannot confirm its channels is legally required to be
silent.

## The stranding

`tvws.spectrumbridge.com` no longer exists. The service was retired; the
hostname does not resolve. So every 23 hours the MT300 wakes up, tries to ask a
server that is gone, fails, and goes back to sleep with its transmitter off —
forever.

Walk the failure chain and notice how little is actually wrong:

```
radio: OK        CPU: OK        web UI: OK        config: intact
                     │
        gethostbyname("tvws.spectrumbridge.com")  ──►  NXDOMAIN
                     │
        establishASecureSession() fails ──► channel map emptied
                     │
                transmitter stays OFF ──► device is inert
```

Every box is green except one DNS lookup for a third party's server. The owner
holds a fully working, FCC-certified radio that is forbidden by its own firmware
from doing the one thing it exists to do.

### Why this is the archetypal "cloud-shutdown e-waste" story

The MT300 is a clean example of a pattern that strands all kinds of hardware:

- **The dependency is external and non-optional.** The check is not a license
  nag that can be ignored; it is wired into the core control loop.
- **The dependency is a single point of failure the owner doesn't control.** One
  company's decision to decommission a server disables every device in the field.
- **Nothing degrades gracefully.** There is no offline mode, no cached last-known
  answer that keeps working, no local override. The device is binary: answered,
  or inert.
- **The hardware long outlives the service.** Silicon from 2012–2014 still runs
  perfectly; the web service behind it did not survive the decade.

The result is a multi-thousand-dollar instrument reduced to a paperweight not by
wear, but by the quiet retirement of a URL.

## The way out

The device doesn't need a working *SpectrumBridge*; it needs a working *answer*.
Three things make recovery possible, and this guide uses all three:

1. **The agent is analysable.** Static, unstripped, DWARF-annotated — so the
   exact protocol it expects can be read out of the binary rather than guessed.
   (See the decompilation under [`../../WhiteSpaceApp/ghidra`](../../WhiteSpaceApp/ghidra).)
2. **The device is reproducible.** A filesystem snapshot and a full NAND dump
   exist, so the userspace — and the real kernel — can be rebuilt in software.
3. **The dependency is replaceable.** The database protocol is small and the
   device's trust checks are permissive, so a tiny local server can stand in for
   the dead one ([`../on-device-spoof.md`](../on-device-spoof.md)).

The rest of the guide is the engineering of turning those three facts into a
running emulator and a working on-device fix.

**Tip that generalises:** before emulating anything, find the *decision* the
device refuses to make without the cloud. In a white-space radio it is "which
channels are legal"; in a thermostat it might be "is my subscription valid." That
one refusal is the whole ballgame — everything else is plumbing to reach the
point where you can feed it the answer.

[← Guide index](README.md) · [Next: qemu-user →](02-qemu-user.md)
