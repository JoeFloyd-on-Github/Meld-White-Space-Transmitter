# MT300 device boot process and hardware control

This document describes how the Meld MT300 boots, how the front-panel **power/
reset button** is monitored, what happens when it is pressed, how the **IP
address** is assigned, how the **radio** is tied to `whitespaceagent`, and how
the **device hardware** is configured. It also indexes the OEM hardware-control
scripts and binaries and their decompiled/annotated source.

Sources: the `root/` device snapshot (boot scripts under `root/etc/init.d`, the
OEM binaries under `root/bin`, `root/sbin`, `root/disk/usr/bin`) and Ghidra
decompilation of the OEM binaries. Decompiled and annotated source for the OEM
binaries is in [`../oem-analysis/`](../oem-analysis/). Nothing here was produced
by running the device.

## 0. Platform summary

| Item | Value |
|---|---|
| SoC | **ViXS XCode** (XCodePro100 BSP) TV transcoder, MIPS32r2 |
| Kernel | Linux **2.6.36** (MIPS), Timesys toolchain (glibc 2.9, GCC 4.3.2) |
| Bootloader | **U-Boot** (sets GPIO 6 & 7 ON for hardware enables; per the Meld patch-script header) |
| Userland | BusyBox + udev + a handful of OEM daemons |
| NAND layout | dual application images (A/B) plus a U-Boot environment partition; app storage is **yaffs2** |
| A/V path | HDMI/analog video in → ViXS transcode → **NXP TV transmitter** ("Tx v2", FCC ID `OKVMT300`) over `/dev/ttyS1` |
| Front panel | power/standby button via a **box microcontroller** on `/dev/micro`; optional IR remote on `/dev/irc`/`/dev/irc1` |
| Hostname | `xc4viper` (the ViXS reference board name is "XC4"/"Viper") |

## 1. Boot chain

```
U-Boot  ─►  Linux 2.6.36  ─►  /sbin/init  ─►  /etc/inittab  ─►  /etc/init.d/rcS
```

- **U-Boot** selects and loads the kernel, and (per the patch-script header)
  sets **GPIO 6 & 7 to ON** — board-level hardware enables asserted before Linux
  starts. Which application image U-Boot/Linux uses is driven by the
  `current_active` variable in the U-Boot NAND environment (see §6.4).
- **`/etc/inittab`** (BusyBox init) runs `/etc/init.d/rcS` at sysinit, respawns
  a **root shell on the `ttyS0` serial console** (no login), and maps
  ctrl-alt-del to `/sbin/reboot`.

### `/etc/init.d/rcS`, step by step

1. `mount` proc, usbfs, sysfs.
2. **`/bin/actimage`** — returns the active image number (0 or 1). `rcS` mounts
   the matching NAND volume as `/disk`: image 0 → `/dev/mtdblock8`, image 1 →
   `/dev/mtdblock9` (yaffs2, rw). This is the A/B boot selection (§6.4).
3. **`/usr/bin/uevent_daemon`** — the hotplug/uevent broker (§6.5).
4. **`start_udev`** — populate `/dev` from sysfs via `udevd`.
5. `mount` devpts; `sysctl -p` (network buffer tuning, `/etc/sysctl.conf`).
6. `hostname xc4viper`; bring up loopback.
7. **`/etc/init.d/setipeth0`** — configure `eth0` (§4).
8. **`telnetd -l /bin/sh`** — a telnet daemon dropping to a root shell (no auth).
9. **`/bin/pm_daemon &`** — the power/reset-button + IR daemon (§2, §3).
10. **`clear_config_files`** — blank the status files `wsa` writes
    (`channels`, `operating`, `channel_status`, `transmitter`) so the web Status
    page starts clean.
11. **`wsagent_start`** — build the device XML from `sbi` and launch
    `whitespaceagent` (§5).
12. **`thttpd`** — the PHP web UI.

(`rcS.post` exists but is entirely commented out — developer scratch.)

## 2. How the reset / power button is monitored

The front-panel button is handled by **`pm_daemon`** (`root/bin/pm_daemon`),
started from `rcS` as `/bin/pm_daemon &`. It is a small event daemon built
around a `select()` loop; the physical button is **not** a GPIO polled by Linux
directly but an event delivered by the box's **microcontroller** through the
char device **`/dev/micro`**. Annotated source:
[`oem-analysis/pm_daemon.annotated.c`](../oem-analysis/pm_daemon.annotated.c).

Flow (`main` → `event_loop` → `micro_evt_handler`):

1. `main()` opens **`/dev/micro`** (`O_RDWR`) and **`/sys/power/state`**
   (`O_WRONLY`). If either fails, `pm_daemon` exits.
2. It registers `/dev/micro` with the handler **`micro_evt_handler`** in a small
   `(fd → handler)` table (`dm_add_entry`), then calls **`event_loop()`**.
3. `event_loop()` builds an `fd_set` of the registered fds and blocks in
   `select()` with no timeout. When `/dev/micro` becomes readable (the
   microcontroller reported a button event), `event_dispatcher` calls the
   handler.
4. **`micro_evt_handler()`** reads **one byte** (the "reason" code) from
   `/dev/micro`, logs `Power switch is pressed. (reason %c)`, and calls
   `mode_change()`.

So "monitoring the button" = `pm_daemon` sitting in `select()` on `/dev/micro`,
with the microcontroller as the thing that actually debounces/senses the
physical switch and pushes a one-byte event up to Linux.

### The optional IR remote path

`pm_daemon` can also handle an **infrared remote** (IguanaIR; `/dev/irc` RX and
`/dev/irc1` TX). This path is enabled **only** when `pm_daemon` is started with
`-f <irc-config>`; it decodes NEC/RC6 remote frames (`ir_rx_evt_handler` →
`irr_get_symbol_from_code`) and, when the decoded symbol is **`POWER`**, calls
`mode_change(-1)` — the same power-state change as the physical button.

**On the MT300 as shipped this IR path is disabled:** `rcS` launches
`/bin/pm_daemon &` with **no `-f`**, so `main()` prints
`IRC config file reading error. IRDA power key will not work!` and registers
only `/dev/micro`. The IR remote and transmit features (and the interactive
keyboard commands `s`/`d`/`c` for sending IR codes) are present in the binary
but unused in this configuration.

## 3. What happens when you press the button (reset)

The button is a **soft power / standby** control, not a hard SoC reset:

- `micro_evt_handler` (button) and the IR `POWER` key both call
  **`mode_change()`**, which **writes a mode byte to `/sys/power/state`**
  (the kernel power-state interface). Interactively `mode_change` offers
  `on / active / passive standby / off (o/a/p/x)`; the event-driven paths drive
  it toward the low-power/off state. The microcontroller supplies a "reason"
  byte so firmware can distinguish, e.g., a short press from a long hold.
- A **full reboot** is available separately: `inittab` maps ctrl-alt-del to
  `/sbin/reboot`, and the web UI / scripts can call `reboot`.
- A **hard power cycle** restarts the whole chain in §1; because the active
  image is chosen from the U-Boot environment, a failed upgrade can be recovered
  by booting the other image (§6.4).

**"Factory reset" is a different thing** and is **not** the physical button. It
is a software action in the web UI (per the firmware `RELEASE_NOTES`, v1.20
"reset to factory defaults"): it clears the registration info, resets the IP to
`192.168.1.194`, the streaming IP to `192.168.1.193`, clears preferred channels,
and sets transmit power to 11 — all by rewriting the `/etc/config/*` files, then
restarting `whitespaceagent`. The button does not trigger this.

## 4. How the IP address is assigned

`eth0` is configured by **`/etc/init.d/setipeth0`**, called from `rcS`. As
shipped it uses a **static IP**:

```
DHCP=0
IPADDR=192.168.0.202   SUBNET=255.255.255.0   GATEWAY=192.168.0.1
ifconfig eth0 192.168.0.202 netmask 255.255.255.0 up
route add default gw 192.168.0.1 eth0
ping -c 1 -W 5 192.168.0.1
```

- `setipeth0` has a `DHCP=1` branch that instead runs `udhcpc -i eth0`. It is
  off in this snapshot (`DHCP=0`).
- When DHCP **is** used, **`udhcpc_default.script`** handles lease events:
  - `bound`/`renew` → `ifconfig` the leased address, set the default route(s),
    rewrite `/etc/resolv.conf` from the DHCP-supplied DNS.
  - `leasefail` → **APIPA fallback**: probe `169.254.x.x` addresses with
    `arping -D` and assign the first free one; if none, set `0.0.0.0`.
  - `deconfig` → bring the interface to `0.0.0.0`.
- `setipeth0_orig` / `setipeth1` are the stock DHCP-only versions (with
  placeholder `888.888.888.888` values), kept but unused.

The **configured IP** is also what the firmware's factory-default and the sbi
versions manage; note the device-level `setipeth0` static value (`192.168.0.202`)
is independent of the white-space `StreamingIpAddress` (`192.168.0.203`) that
`wsa` sends to the radio.

## 5. How the radio is tied to `whitespaceagent`

> The radio and video hardware itself — signal chain, serial control
> protocol, modulation formats, and video encoding — is documented in
> [`radio-and-video.md`](radio-and-video.md).

The "radio" is the **NXP TV transmitter** ("Tx v2", firmware `1.3.0 v0036`). It
is tied to `whitespaceagent` in three layers:

1. **Hardware enable (boot):** U-Boot asserts **GPIO 6 & 7** (per the patch-
   script header) before Linux starts, powering/enabling the transmitter board
   so it is present as a serial device once Linux is up.
2. **Serial transport:** `whitespaceagent`'s serial thread opens **`/dev/ttyS1`**
   at 115200 and speaks the Meld text protocol (`MELD_TECH_WS_ENABLE`, `c`/`m#`/
   `p`/`e0`/`e1`/`x`/`y`/`b?`/`h` commands) to query identity and firmware, set
   channel/power/modulation, and enable/disable transmission. This is documented
   in [`wsa-overview.md`](wsa-overview.md) §6; the serial dumps captured in
   `root/etc/config/queryInfo` and `fccidSerialInfo` confirm it.
3. **Launch wiring (software):** `rcS` → **`wsagent_start`** builds
   `/etc/config/meldTechDeviceCapability.xml` from `/etc/config/sbi` (§6 of
   `system-context.md`) and launches
   `whitespaceagent cafile.cert /etc/config …XML`. `whitespaceagent` is the
   **only** process that talks to `/dev/ttyS1`; the web UI changes settings in
   `sbi` and restarts the agent via `wsagent_stop`/`wsagent_start` rather than
   touching the radio directly.

So the radio is bound to `wsa` by the serial port plus the boot-time GPIO
enable; nothing else drives it.

## 6. How the device hardware is configured

Hardware bring-up is split between U-Boot, a video-path diagnostic/init tool, a
secure-keystore loader, the A/B image selector, and the uevent broker.

### 6.1 U-Boot / kernel level

U-Boot sets **GPIO 6 & 7 ON** (board enables, incl. the transmitter path),
loads the Linux 2.6.36 kernel for the ViXS XCode SoC, and applies a
"genlock" patch (video sync). The ViXS transcoder driver exposes
**`/dev/vixs/xcodedrv`**, which the userland hardware tools talk to via `ioctl`.

### 6.2 Video front-end — `boxdiag`

[`root/bin/boxdiag`](../root/bin/boxdiag) is a **hardware diagnostic / init**
tool for the A/V capture path. Through `/dev/vixs/xcodedrv` it initializes and
tests the video decoder/encoder chips — **ADV7185** and **SAA711x** video
decoders (and a "Mot" device) — issuing register read/write sequences and DMA
tests between host and frame buffer (its strings: `Adv_7185_Init`,
`Ic106_Sa711x_Init`, `Sa711x_Init`, `Mot_Init`, `DMA From Host to Frame Buffer
Failed`). It is the clearest window into the TV-input/transcode hardware. The
binary is stripped, so its decompile is by address, not symbol; see
[`oem-analysis/boxdiag.annotated.c`](../oem-analysis/boxdiag.annotated.c).

### 6.3 Secure keystore / GPIO / power — `load_keys_good`

[`root/bin/load_keys_good`](../root/bin/load_keys_good) loads keys into the ViXS
secure **keystore** (via `/dev/vixs/xcodedrv`) and exercises the SoC's control
commands — its strings include `CMD2_SDEV_RESERVE_KEYSTORE_SLOT`,
`CMD2_SDEV_SET_KEYSTORE_KEY`, `CMD_SDEV_SET_POWER_MODE`, and `CMD_GPIO_UTIL`.
This is how content-protection keys and SoC power/GPIO state are provisioned at
the hardware level. (It is a large, statically linked binary; see
[`oem-analysis/`](../oem-analysis/) for scope.)

### 6.4 Application image selection — `actimage`

[`root/bin/actimage`](../root/bin/actimage) manages the **A/B dual-image** boot.
It reads and writes the **`current_active`** variable in the **U-Boot NAND
environment** (partition `/dev/mtd3`), using its own U-Boot-compatible env
reader/writer (`nandenv_open`, `nand_read_env`, `nand_write_env`,
`env_crc_update`, `getfirstgoodblock`, `crc32`):

- `actimage` (no args) → prints/returns `current_active` (0 or 1, default 0).
  `rcS` uses the **exit code** to decide whether `/disk` is `/dev/mtdblock8`
  (image 0) or `/dev/mtdblock9` (image 1).
- `actimage -w <0|1>` → sets `current_active`. An upgrade writes the new image
  to the inactive bank and flips this variable, so a bad upgrade can fall back to
  the previous image. Annotated source:
  [`oem-analysis/actimage.annotated.c`](../oem-analysis/actimage.annotated.c).

Low-level flashing of the NAND partitions (boot, kernel) is done by
[`root/bin/mtd_char_op`](../root/bin/mtd_char_op) — a MTD utility with
`dump` / `erase mtdx` / `write mtdx <file>` / `read mtdx size <file>`
(`mtd_open`/`mtd_erase`/`mtd_read_page`/`mtd_write_page`, bad-block aware).
Annotated source:
[`oem-analysis/mtd_char_op.annotated.c`](../oem-analysis/mtd_char_op.annotated.c).

### 6.5 Hotplug / device events — `uevent_daemon` + `uevent_client`

[`root/disk/usr/bin/uevent_daemon`](../root/disk/usr/bin/uevent_daemon) is a
**uevent broker**. It daemonizes, listens on a kernel uevent/udev socket and a
UNIX domain socket (`notify_socket`), keeps a small table of device slots, and
**notifies registered client apps** when devices appear/disappear
(`reg_thread`, `mainloop`, `notify_apps`). The matching
[`uevent_client`](../root/disk/usr/bin/uevent_client) is invoked by
**`blkdev_detect.sh`** on block-device hotplug (udev rule), so plugging in USB/SD
storage is routed to whatever app registered interest (used by the firmware's
upgrade-from-media and media features). Annotated source:
[`oem-analysis/uevent_daemon.annotated.c`](../oem-analysis/uevent_daemon.annotated.c)
and [`uevent_client.annotated.c`](../oem-analysis/uevent_client.annotated.c).

## 7. OEM binary / script index

| Component | Type | Role | Source |
|---|---|---|---|
| `bin/pm_daemon` | MIPS ELF (DWARF) | power/reset button + IR remote → `/sys/power/state` | `oem-analysis/pm_daemon.*` |
| `bin/actimage` | MIPS ELF (static) | A/B image select via U-Boot NAND env | `oem-analysis/actimage.*` |
| `bin/mtd_char_op` | MIPS ELF | NAND MTD read/write/erase/dump | `oem-analysis/mtd_char_op.*` |
| `bin/boxdiag` | MIPS ELF (stripped) | video decoder (ADV7185/SAA711x) init/diag via ViXS driver | `oem-analysis/boxdiag.*` |
| `bin/load_keys_good` | MIPS ELF (static) | ViXS secure keystore / GPIO / power-mode provisioning | `oem-analysis/load_keys_good.*` |
| `disk/usr/bin/uevent_daemon` | MIPS ELF | kernel uevent → app notification broker | `oem-analysis/uevent_daemon.*` |
| `disk/usr/bin/uevent_client` | MIPS ELF | hotplug client (from `blkdev_detect.sh`) | `oem-analysis/uevent_client.*` |
| `etc/init.d/rcS` | shell | top-level boot script | — |
| `etc/init.d/setipeth0` | shell | eth0 IP config (static 192.168.0.202) | — |
| `etc/init.d/udhcpc_default.script` | shell | DHCP lease handling + APIPA fallback | — |
| `sbin/start_udev` | shell | udev `/dev` population | — |
| `disk/usr/bin/blkdev_detect.sh` | shell | block-hotplug → `uevent_client` | — |

The remaining binaries (`busybox`, `e2fsck`, `mke2fs`, `flash_erase`, `iconv`,
`ntpdate`, `dosfsck`/`mkdosfs`, the IguanaIR `igdaemon`/`igclient`/`irsend`/`irw`
stack, `strace`) are stock utilities, not Meld/ViXS-specific device control.
