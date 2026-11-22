# HOW-TO: Bring a Meld MT300 back to life

This is the end-to-end guide to making a **Meld Technology MT300** TV white-space
transmitter work again after its database host (`tvws.spectrumbridge.com`) went
offline. It covers:

1. [What the device is and why it stopped](#1-background-what-broke-and-why)
2. [White-space spectrum, the channels, and the FCC rules](#2-white-space-spectrum-and-the-fcc-rules) — **read this before you transmit**
3. [Building the `sb_mips` stand-in](#3-build-the-sb_mips-stand-in)
4. [Finding the device on the network, over serial](#4-find-the-device-over-the-serial-console)
5. [Transferring the binary to the device](#5-transfer-the-binary-to-the-device)
6. [Making it run at every boot](#6-make-it-run-at-every-boot)
7. [Verifying, and backing out](#7-verify-and-roll-back)

If you only want the quick path and have already read the FCC section, jump to
the [one-shot install](#the-one-shot-install).

> This guide documents work on **your own hardware, into a dummy load, for
> bench testing**. It is not permission to put a signal on the air. Section 2
> explains the line between the two, and it is a line with legal force.

---

## 1. Background: what broke, and why

The MT300 is a UHF TV-band transmitter certified under **FCC ID `OKVMT300`**. It
is a *white-space device*: under FCC rules it may only transmit on TV channels
that are unused at its location, and it learns which channels those are by asking
an FCC-approved **white-space database** over the internet.

The database it was built to use belonged to **Spectrum Bridge, Inc.** at
`tvws.spectrumbridge.com`. Spectrum Bridge wound down its TV white-space database
service, and that host no longer answers. The on-device agent
(`whitespaceagent`, "wsa") will not enable the radio until it has received a
channel list, so with the database gone the device powers up and sits idle
forever. The hardware is fine; it is waiting for an answer that never comes.

This project supplies that answer locally. `sb_mips` is a tiny HTTPS server,
cross-compiled for the device, that speaks the same protocol the old database
did and returns a channel list. The device's own hostname file (`/etc/hosts`) is
pointed at `127.0.0.1`, so the agent talks to `sb_mips` on the device itself. The
original `whitespaceagent` binary is **never modified**.

For the protocol details and the reverse-engineering story, see
[`docs/wsa-overview.md`](docs/wsa-overview.md) and
[`docs/on-device-spoof.md`](docs/on-device-spoof.md).

---

## 2. White-space spectrum and the FCC rules

**This section is the most important one. The database check you are replacing is
the exact mechanism that keeps this transmitter legal. Once it is gone, staying
legal is entirely on you.**

### 2.1 What "TV white space" means

UHF television is divided into 6 MHz channels. In any given place, many of those
channels carry no local station — they are "white space." The FCC's
**Part 15, Subpart H** rules let unlicensed devices use that idle spectrum, on
strict conditions, the central one being: **a white-space device must get its
list of permitted channels from an FCC-approved database, by location, and may
transmit only on channels that database returns.** The database is what protects
licensed TV stations, wireless microphones, and other users from interference.

The MT300 was certified against those rules *with the database in the loop*.
Running it against a local stand-in removes the one safeguard the certification
assumed. The stand-in says "yes" to whatever channels it is configured for; it
does **not** know what is actually on the air where you are. So:

- **For bench work, terminate the RF output into a dummy load or a high-value
  attenuator — never an antenna.** Confirm the chain on a spectrum analyzer or
  SDR before anything can radiate.
- **Before any channel could reach an antenna, confirm it is genuinely vacant at
  your location** using the FCC's TV-query and white-space tools plus a local
  scan, and confirm your intended use actually falls within Part 15 Subpart H.
- Transmitting on an occupied TV channel, or on spectrum that is no longer TV
  band at all (see below), can cause harmful interference and is a violation of
  FCC rules. The responsibility is the operator's.

If you are not prepared to verify legality channel-by-channel, keep the output in
a dummy load and treat this purely as a hardware-restoration exercise.

### 2.2 The channel-to-frequency map

The MT300's firmware allows TV channels **21 through 51**. In the original US
UHF plan each channel is 6 MHz wide, and channel *N* spans
`470 + (N−14)×6` MHz to `476 + (N−14)×6` MHz:

| Ch | MHz | Ch | MHz | Ch | MHz |
|---|---|---|---|---|---|
| 21 | 512–518 | 32 | 578–584 | 43 | 644–650 |
| 22 | 518–524 | 33 | 584–590 | 44 | 650–656 |
| 23 | 524–530 | 34 | 590–596 | 45 | 656–662 |
| 24 | 530–536 | 35 | 596–602 | 46 | 662–668 |
| 25 | 536–542 | 36 | 602–608 | 47 | 668–674 |
| 26 | 542–548 | **37** | **608–614** | 48 | 674–680 |
| 27 | 548–554 | 38 | 614–620 | 49 | 680–686 |
| 28 | 554–560 | 39 | 620–626 | 50 | 686–692 |
| 29 | 560–566 | 40 | 626–632 | 51 | 692–698 |
| 30 | 566–572 | 41 | 632–638 |  |  |
| 31 | 572–578 | 42 | 638–644 |  |  |

### 2.3 What is no longer TV band — the 600 MHz repack

The MT300's firmware dates from 2013, and **the UHF band has been re-carved since
then.** After the FCC's 2017 *broadcast incentive auction* and the 2018–2020
*repack*, the top of this range is no longer television spectrum:

- **Channels 38–51 (614–698 MHz) are now the "600 MHz band," licensed to mobile
  carriers.** The band plan is roughly **617–652 MHz downlink** (cell tower →
  phone) and **663–698 MHz uplink** (phone → tower), with guard bands around
  them. For example, **channel 42 (638–644 MHz) sits inside a carrier's downlink.**
  Transmitting here is transmitting into licensed cellular spectrum. Do not do it
  on an antenna.
- **Channel 37 (608–614 MHz) is reserved** for radio astronomy and the Wireless
  Medical Telemetry Service. It has never been available to white-space devices.
- **Only channels 21–36 (512–608 MHz) remain TV band**, and even there a channel
  is usable only where no licensed TV station (or other protected user, such as a
  wireless-microphone reservation) occupies it.

So although the device will *accept* and key up on any channel from 21 to 51, the
only channels that can be lawful on an antenna today are **vacant channels in the
21–36 range**, verified for your location. Everything from 37 up is off-limits on
the air.

### 2.4 What the stand-in offers, and how the agent chooses

By default `sb_mips` is configured (in `spectrumbridge_spoof_c/sb.c`) to offer the
**whole 21–51 range**, because for bench testing into a load the wider list
exercises the agent fully and the device still selects its own preferred channel.
The agent picks the unit's `PreferredChannel` (23 on the reference unit) whenever
it is in the list, so in practice it operates on 23.

**Before connecting an antenna, narrow that list.** Edit `s_channel_list` in
`sb.c` to contain only vacant channels in 21–36 for your location and rebuild
(section 3). `ChannelCount` is computed automatically from the list, so you only
edit the one line. Keep the whole response under ~1 KB (the agent's receive
buffer); the full 31-channel list is about 210 bytes, so this is never a real
constraint.

---

## 3. Build the `sb_mips` stand-in

`sb_mips` is a static MIPS32 little-endian binary. It is built on a Linux PC with
a self-contained musl cross-toolchain and mbedTLS; **no root and nothing
system-wide is installed** — everything lands under `spectrumbridge_spoof_c/build/`.

### 3.1 Prerequisites

- A Linux x86-64 host (or WSL2). The build script fetches a prebuilt
  `mipsel-linux-muslsf` toolchain from musl.cc and the mbedTLS 2.28.2 source from
  GitHub, so **internet access is needed the first time only.**
- `wget`, `tar`, `make`, and a host C compiler for the small build helpers.

### 3.2 Choose the channel list

Open [`spectrumbridge_spoof_c/sb.c`](spectrumbridge_spoof_c/sb.c) and set the one
line:

```c
static const char *s_channel_list =
    "21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,"
    "37,38,39,40,41,42,43,44,45,46,47,48,49,50,51";
```

- **For bench testing into a load:** leave it as the full range, or trim it to a
  couple of channels — it does not matter, since the output is in a dummy load.
- **For anything that could reach an antenna:** replace it with only the vacant
  channels in **21–36** for your location (section 2.3). For example, if only
  channels 26 and 31 are clear where you are: `static const char *s_channel_list = "26,31";`

You do **not** touch any count variable; the server counts the channels itself at
start-up.

### 3.3 Build

```bash
cd spectrumbridge_spoof_c
./build.sh
```

The result is `spectrumbridge_spoof_c/sb_mips`, a static, stripped MIPS
executable (~840 KB). Confirm it:

```bash
file sb_mips            # ELF 32-bit LSB executable, MIPS, statically linked, stripped
md5sum sb_mips          # note this hash; you will check it again on the device
```

### 3.4 Smoke-test on the PC (optional)

The repo includes a replay test that drives the server with the exact requests
the device's agent sends and checks every field of the reply:

```bash
./test_replay.sh        # expect: "8 passed, 0 failed"
```

---

## 4. Find the device over the serial console

The MT300 ships with a static IP of **`192.168.1.194/24`**, but a given unit may
have been changed to DHCP or another address. The serial console is the surest
way to learn where it is, because it needs no network at all.

### 4.1 The serial cable

The board has a **4-pin UART header**, 3.3 V TTL, at **115200 baud, 8N1, no flow
control.** Connect a USB-TTL adapter (GND, TX, RX — cross TX/RX), and open a
terminal:

```bash
# Linux, e.g. the adapter at /dev/ttyUSB0:
screen /dev/ttyUSB0 115200        # or: picocom -b 115200 /dev/ttyUSB0
```

Power-cycle the device and you will see U-Boot, then the kernel, then the init
log.

### 4.2 Why the console alone will not give you a shell

At the end of boot, `/etc/init.d/rcS` launches the AV encoder application in the
**foreground**, and it takes over the console's input and output. From then on
the console streams the encoder's status lines (`//=== Analog Init Mode...`,
`NO digital video standard detected`, and so on) and your keystrokes go to that
app, not to a shell. This is normal, not a fault.

So the serial port is an excellent **read-only boot monitor**, and to turn it
into an interactive shell you interrupt the boot (next step).

### 4.3 Getting the IP address via serial

You have two good options.

**Option A — read it from a single-user shell (most reliable).**

1. Power-cycle the device and watch the console. During U-Boot's autoboot
   countdown, **press a key** to stop it and reach the U-Boot prompt.
2. Boot the kernel with an init override so the AV app never starts. The exact
   boot command varies, but it amounts to appending `init=/bin/sh` to the kernel
   arguments and booting. (On this U-Boot you typically edit the `bootargs`
   environment variable to add `init=/bin/sh`, then run the normal boot command.)
3. You now have a root shell on the serial console, with the normal startup
   **not** run — so read the configured address directly:

   ```sh
   cat /etc/init.d/setipeth0
   ```

   That script holds the answer:
   - `DHCP=0` with `IPADDR=192.168.1.194` → that static address.
   - `DHCP=1` → the unit uses DHCP; bring the interface up and read the lease:

     ```sh
     /etc/init.d/setipeth0        # runs udhcpc
     ifconfig eth0                # shows the leased inet addr
     ```

**Option B — watch the normal boot log.** If you would rather not interrupt boot,
let it run and read the network section as it scrolls past. You will see the
hostname set, `eth0` brought up, and the gateway pinged (`ping -c 5 ...`). A
static unit shows "Using Static IP Configuration"; a DHCP unit shows "Using DHCP."
This tells you the subnet and gateway even if the device's own address scrolls by
quickly.

### 4.4 Network alternative (no serial)

If you can reach the LAN, the device's Ethernet MAC always begins with the OUI
**`F8:22:85`**. Populate the ARP table with a ping sweep and match that prefix:

```bash
# Linux
for i in $(seq 1 254); do ping -c1 -W1 192.168.1.$i >/dev/null & done; wait
ip neigh | grep -i 'f8:22:85'
```

```powershell
# Windows PowerShell
1..254 | ForEach-Object { (New-Object Net.NetworkInformation.Ping).SendPingAsync("192.168.1.$_",300) } | Out-Null
Start-Sleep 3
arp -a | Select-String 'f8-22-85'
```

### 4.5 The shell you will use for the rest of this guide

Once you know the IP, the device runs an **unauthenticated BusyBox telnet daemon**
on port 23 that drops you straight to a root shell — this is how the OEM firmware
ships:

```bash
telnet 192.168.1.194        # -> "/ #", root, no password
```

Everything in sections 5–7 is run from that telnet shell (and a normal shell on
your PC for the file transfer).

---

## 5. Transfer the binary to the device

The device has BusyBox `nc` and `md5sum` but no `scp`/`ssh`. Push the file by
having the device listen and the PC connect.

**On the device (telnet shell):**

```sh
cd /disk
(nc -l -p 9000 > sb_mips.new &)      # listen on port 9000, save to sb_mips.new
```

**On your PC:**

```bash
nc -q 2 -w 10 192.168.1.194 9000 < spectrumbridge_spoof_c/sb_mips
```

**Verify the copy on both ends** — the hashes must match:

```sh
# device:
md5sum /disk/sb_mips.new
```
```bash
# PC:
md5sum spectrumbridge_spoof_c/sb_mips
```

`/disk` is the writable app partition and survives reboots, so it is a good
staging spot. The permanent install in the next section copies the binary onto
the root filesystem.

If you also want to install at boot, transfer the two helper scripts the same
way:

```bash
nc -q 2 -w 10 192.168.1.194 9000 < spectrumbridge_spoof_c/device/sb_start
nc -q 2 -w 10 192.168.1.194 9000 < spectrumbridge_spoof_c/device/install.sh
```

(Run a fresh `nc -l -p 9000 > <name> &` on the device before each one.) On an
**MT300-AV (v45)** unit send `device/install_av.sh` instead of `install.sh` — see
[§6](#6-make-it-run-at-every-boot).

---

## 6. Make it run at every boot

### Why the root filesystem, and why loopback

The permanent install puts the binary and its launcher on the **root filesystem**
(`/usr/bin`), not `/disk`, for two reasons:

- `/disk` is one of two app partitions (`AppFS_0` / `AppFS_1`) and which one is
  mounted depends on the active firmware image. The rootfs also holds `/etc/hosts`
  and `/etc/init.d/rcS`, so keeping everything together means the whole change
  lives in one place.
- The stand-in listens on **`127.0.0.1` only**, and `/etc/hosts` resolves
  `tvws.spectrumbridge.com` to `127.0.0.1`. Because `rcS` brings the loopback
  interface up before the network, this works **regardless of the device's IP
  address** — a new static address, a DHCP lease, or no network cable at all makes
  no difference. This directly satisfies the "find the server even if the IP
  changes" requirement: the server is always at `127.0.0.1`.

### The one-shot install

The repo ships a device-side installer,
[`spectrumbridge_spoof_c/device/install.sh`](spectrumbridge_spoof_c/device/install.sh),
that does the whole thing and backs up every file it touches. After transferring
`sb_mips`, `sb_start`, and `install.sh` to `/disk` (section 5):

```sh
# on the device:
sh /disk/install.sh
```

It will:

1. Copy `sb_mips` and `sb_start` to `/usr/bin` and make them executable (and print
   the `sb_mips` MD5 so you can check it).
2. Point `tvws.spectrumbridge.com` at `127.0.0.1` in `/etc/hosts` (saving the
   original as `/etc/hosts.orig`).
3. Insert three lines into `/etc/init.d/rcS`, just before `wsagent_start` (saving
   the original as `/etc/init.d/rcS.orig`):

   ```sh
   # SpectrumBridge stand-in (local database for whitespaceagent); see /usr/bin/sb_start
   /usr/bin/sb_start &
   sleep 2
   wsagent_start
   ```

The installer is safe to run more than once — it skips any step already done.

### AV units running the v45 agent (use `install_av.sh`)

The MT300-**AV** (video) units carry a **v45** `whitespaceagent` and boot a
slightly different init path, so `install.sh` above is **not** the right script
for them — use
[`spectrumbridge_spoof_c/device/install_av.sh`](spectrumbridge_spoof_c/device/install_av.sh)
instead (`sh /disk/install_av.sh`). It handles three differences:

- **The boot hook goes in `/etc/init.d/setipeth0`, not `rcS`.** On AV firmware
  `rcS` only starts the encoder; it is `setipeth0` that calls `wsagent_start`.
- **The v45 agent connects to the *reverse name* of the resolved host, not the
  dotted IP.** A plain `127.0.0.1 tvws.spectrumbridge.com` line (and even setting
  `HostAddress=127.0.0.1`) makes it reverse-resolve `127.0.0.1` to `localhost` and
  abort with *"localhost is not a valid IP address."* The installer writes an
  `/etc/hosts` whose **first** name for `127.0.0.1` is a literal dotted quad, which
  the agent accepts:

  ```
  127.0.0.1	127.0.0.1 localhost
  127.0.0.1	tvws.spectrumbridge.com
  ```

- **The agent only runs if `/etc/config/WSAgentRestart` exists.** Otherwise
  `wsagent_start` runs `MeldAgent -O` (a radio read that never contacts the
  database). The installer `touch`es that file — the same thing the web UI's
  "register" button does. On an AV unit the transmitter also will not **key**
  until a live video source is connected, even once registration succeeds.
- **The AV firmware only sends the TX-enable on a video-*connect* event**, so a
  cold power cycle with the source already attached comes up registered and
  powered but with TX disabled (no RF). `install_av.sh` also installs
  [`sb_txkeepalive`](spectrumbridge_spoof_c/device/sb_txkeepalive), a boot
  supervisor that re-asserts `MeldAgent -T` once the radio is armed, so the unit
  comes up transmitting regardless of video-sync timing. Transfer
  `device/sb_txkeepalive` to `/disk` alongside `sb_mips` and `sb_start` before
  running `install_av.sh`. Full story:
  [`docs/av-tx-enable-flaw.md`](docs/av-tx-enable-flaw.md).

Background and the full trace are in
[`docs/on-device-spoof.md`](docs/on-device-spoof.md) (the v45 caveat) and
[`docs/units/mt300-av-677.md`](docs/units/mt300-av-677.md).

`sb_start` itself is a small supervisor: it launches `sb_mips` on
`127.0.0.1:443` and relaunches it 5 seconds after it ever exits, so the server is
always up for the agent's periodic re-checks (the reply tells the agent to come
back in 24 hours).

### Doing it by hand instead

If you prefer to apply the steps manually (or want to understand each one), they
are written out in
[`docs/on-device-spoof.md`](docs/on-device-spoof.md) §3.

---

## 7. Verify, and roll back

### Reboot and confirm it comes up on its own

```sh
reboot
```

Wait for the device to come back (telnet answers again in roughly 15–20 s), then
check that it reached the transmit state **with no manual steps**:

```sh
# the stand-in and its supervisor are running:
ps | grep -E 'sb_start|sb_mips' | grep -v grep
# it listens on loopback:443:
netstat -ltn | grep 127.0.0.1:443
# the agent walked register -> channel -> transmit:
for f in registration channel_status channels operating transmitter; do
  echo "== $f"; grep option /etc/config/$f
done
```

**Success looks like this** (the transmitter typically keys ~30 s after power-up;
give it up to a minute):

| File | Field | Success value |
|---|---|---|
| `registration` | `status` | `0` |
| `channel_status` | `status` | `0` |
| `channels` | `channels` | the offered list (filtered to 21–51) |
| `operating` | `channel` | the chosen channel (e.g. `23`) |
| `transmitter` | `status` | `1` (**RF keyed**) |

Finally, **with the output in a dummy load**, confirm a carrier at the operating
channel's frequency (channel 23 = 524–530 MHz) on a spectrum analyzer or SDR.

The full test runbook, including the RF-safety preconditions, is
[`docs/on-device-test-plan.md`](docs/on-device-test-plan.md).

### Rolling it back

To return the device to its untouched state:

```sh
cp /etc/init.d/rcS.orig /etc/init.d/rcS      # remove the boot hook
cp /etc/hosts.orig /etc/hosts                # restore the hostname mapping
rm -f /usr/bin/sb_start /usr/bin/sb_mips     # remove the binaries
killall sb_start sb_mips 2>/dev/null         # stop them now
wsagent_stop; wsagent_start                  # agent goes back to no answer; RF drops
```

After this the agent can no longer reach a database, the status files return to
their idle values, and the transmitter stays off — exactly as the device behaved
before any of this.

### A note on firmware images and upgrades

The MT300 keeps **two** copies of its kernel, root filesystem, and app partition
(the A/B "image" slots). This install lives on the **currently active** rootfs. If
the device is ever switched to its other image — for instance by a firmware
upgrade — none of these changes will be present there and the transmitter will
stay off until you re-run the install on that image.

---

## Where to read more

| Topic | File |
|---|---|
| The agent's protocol and response parser | [`docs/wsa-overview.md`](docs/wsa-overview.md) |
| The stand-in: redirect options, build, testing | [`docs/on-device-spoof.md`](docs/on-device-spoof.md) |
| Live device access (serial and telnet) | [`docs/device-access.md`](docs/device-access.md) |
| The on-hardware test runbook and RF safety | [`docs/on-device-test-plan.md`](docs/on-device-test-plan.md) |
| Project goals, history, open items | [`PROJECT_CONTEXT.md`](PROJECT_CONTEXT.md) |
