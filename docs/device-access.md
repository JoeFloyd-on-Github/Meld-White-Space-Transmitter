# Getting a shell on the live MT300 (serial and telnet)

This is the first material in the repo taken from a **physical, powered MT300**
rather than from static analysis or QEMU. It records how to reach the running
device, why the serial console alone does **not** give you a usable shell, the
telnet root shell that does, and the live state the device is in before the
database stand-in is run.

> **Privacy note.** A real unit's `/etc/config/sbi` carries the **previous
> owner's** name, email, phone, and street address (the registrant contact the
> FCC database required). None of that is in this repository, and it must stay
> out. If you capture or publish a config dump, blank the `Contact*`,
> `DeviceOwner`, `Latitude`, and `Longitude` fields first. The structure of
> `sbi` is documented in [`system-context.md`](system-context.md) with generic
> values.

## 1. Identifying the unit

Confirmed on the bench unit over telnet:

| Fact | Value | Source |
|---|---|---|
| Hostname | `xc4viper` | `uname -a` |
| Kernel | `2.6.29-ts-mipsisa32r2el #42 Sun Sep 16 2012` | `uname -a` |
| White-space app | Meld **v1.26** (March 2013) | `/disk/Meld/meldtech/RELEASE_NOTES` |
| FCC ID / serial | `OKVMT300` / `457` | `/etc/config/fccidSerialInfo` |
| Radio | NXP `1.1.0 Tx v2` | `/etc/config/fccidSerialInfo` |
| eth0 MAC | `F8:22:85:0A:00:20` | boot log / kernel cmdline |
| IP (as shipped) | **static `192.168.1.194/24`**, gw `192.168.1.1` | `/etc/init.d/setipeth0` |

Note the deployed app here is **v1.26**, older than the `WhiteSpaceApp/wsa`
(v44, Nov 2013) the decompile describes. The TLS/HTTP database protocol is the
same; only build metadata differs.

## 2. The serial console will not give you a shell (by itself)

The 4-pin UART is **115200 8N1, no flow control**. It is the right cable and the
device talks on it — you get the full U-Boot + kernel + init boot log. But typing
at it does nothing, and this is **not** a wiring or bootloader-option problem:

`/etc/init.d/rcS` ends by launching the AV encoder app **in the foreground**:

```sh
#Meld: Auto Start AV software
cd /disk/Meld/AV11/test_script
./test_dvo.sh          # -> ./encoder_sample -m DVO
```

`encoder_sample` owns the console's stdin/stdout from then on. That is the
source of the endless `//=== Analog Init Mode...`, `HDMI 0x5 is ...`,
`NO digital video standard detected`, and `Receive an Event ID 8` lines — it is a
video app scanning its inputs, not an error. Your keystrokes go to that app (which
only wants `q`), never to a shell. So the serial port is a perfect **read-only
boot monitor** and a dead end for interactive work.

(If you ever do need a serial shell: stop autoboot in U-Boot and boot with
`init=/bin/sh`, or comment out the AV block in `rcS` on the writable rootfs. You
do **not** need either for anything below — telnet is simpler.)

## 3. Telnet is the way in: root shell, no password

`rcS` starts, earlier and in the background:

```sh
echo "Starting Telnet Daemon"
telnetd -l /bin/sh         # busybox telnetd, login shell = /bin/sh, NO auth
```

So **TCP 23 drops you straight to a `uid=0(root)` shell with no login prompt and
no password.** This is a property of the shipped OEM firmware, not a change made
here.

```sh
# from a host on the same LAN
telnet 192.168.1.194
# -> / #    (root, immediately)
```

### Finding the device if the IP is unknown

It ships static at `192.168.1.194`, but a unit may have been reconfigured to DHCP
(`/etc/init.d/setipeth0`, `DHCP=1`) or to another static address. Find it by its
fixed MAC prefix instead:

```powershell
# Windows: ping-sweep the subnet to populate ARP, then match the MAC
1..254 | ForEach-Object { (New-Object Net.NetworkInformation.Ping).SendPingAsync("192.168.1.$_",300) } | Out-Null
Start-Sleep 3
arp -a | Select-String 'f8-22-85'      # Meld/Viper eth0 OUI on this unit
```

## 4. Moving files on and off (no scp/ssh on the device)

The device has **busybox `nc`** (BusyBox v1.13.3) and `md5sum`, but no ssh/scp.
Push a file from the PC by having the device listen and the PC connect:

```sh
# on the device (telnet):
(nc -l -p 9000 > /disk/sb_mips &) ; sleep 1
# on the PC: open a TCP socket to 192.168.1.194:9000 and send the file's bytes,
# then verify both ends:
md5sum /disk/sb_mips          # compare against the PC-side hash
```

`/disk` is the app partition (`/dev/mtdblock8`, yaffs2, **rw**, ~158 MB free),
so binaries dropped there persist and survive reboot.

## 5. Live state before the stand-in runs (why RF is off)

With the dead database unreachable, the agent leaves its status files in the
"never succeeded" state. Read straight from `/etc/config`:

| File | Field | Observed | Meaning |
|---|---|---|---|
| `registration` | `status` | `99`, or no options at all | registration never completed |
| `channel_status` | `status` | `99` | no channel list obtained |
| `channels` | — | *(empty)* | no available channels |
| `operating` | `channel` | *(empty)* | no operating channel chosen |
| `transmitter` | `radio_status` → `status` | `0` | **RF output off** |

Running processes of interest (`ps`): four `whitespaceagent cafile.cert
/etc/config` instances, `thttpd` (web UI), and `encoder_sample -m DVO`.

The unit's clock reads **1970** — there is no battery-backed RTC and no time
sync. Per the decompile ([`on-device-spoof.md`](on-device-spoof.md) §1), `wsa`'s
`verify_callback` accepts not-yet-valid/expired certs, so the wrong clock does
**not** block the TLS handshake with the stand-in.

### Already half-redirected

On the bench unit, `/etc/hosts` already contains:

```
192.168.1.249  tvws.spectrumbridge.com
```

i.e. the dead host is pointed at another LAN machine (`192.168.1.249`). That host
answers ping but **nothing listens on 443**, so the agent still gets no response.
Either stand a server up at that address, or repoint the name at the device's own
loopback and run the stand-in on the device — see
[`on-device-test-plan.md`](on-device-test-plan.md).
