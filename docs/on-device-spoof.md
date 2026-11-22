# On-device SpectrumBridge stand-in: redirect, build, and test

`tvws.spectrumbridge.com` — the white-space database the Meld MT300 contacts
before it will transmit — no longer exists. Without a successful database
response the device's `whitespaceagent` ("wsa") empties its channel map and the
transmitter stays off. This document shows how to run a stand-in ("spoof")
database **on the device itself** so wsa gets a valid response, how the request
is redirected to it, and how the stand-in was cross-compiled and tested.

It builds on:

- [`wsa-overview.md`](wsa-overview.md) §5 — the exact TLS/HTTP protocol wsa speaks.
- [`../WhiteSpaceApp/ghidra/wsa_annotated.c`](../WhiteSpaceApp/ghidra/wsa_annotated.c) — the decompiled client.
- [`../spectrumbridge_spoof_c/`](../spectrumbridge_spoof_c) — the stand-in server, build script, and tests.

Everything below was derived from static analysis and tested on the host and
under QEMU (see [Testing](#5-testing-and-its-limits)). Work against a **physical
MT300** has since begun — shell access and the live status files are documented
in [`device-access.md`](device-access.md), and the step-by-step hardware runbook,
with RF-safety preconditions, is in
[`on-device-test-plan.md`](on-device-test-plan.md). As of writing, `sb_mips` has
been deployed and verified on a bench unit but the transmitter has not yet been
keyed.

## 1. What wsa does, from the decompile

Three facts from `establishASecureSession()` / `get_ip()` / `verify_callback()`
in `wsa_annotated.c` make an on-device redirect simple:

| Behavior | Code | Consequence |
|---|---|---|
| Resolves the host with **`gethostbyname(HostAddress)`** | `get_ip()` | Honors `/etc/hosts` (uClibc default `files`→`dns`), so the name can be pointed at loopback without DNS. |
| **No hostname / CN verification** | `verify_callback()` does "no hostname matching" | The server certificate's name is irrelevant. |
| Accepts a **self-signed leaf** (`X509_V_ERR 18`), also expired (10) / not-yet-valid (9) | `verify_callback()` whitelist | A throwaway self-signed cert is trusted with **no CA setup**. |
| Connects to **port 443, hard-coded** (`htons(0x1bb)`) | `establishASecureSession()` | The stand-in must listen on 443; `HostPortNumber` in config is ignored. |
| OpenSSL 1.0.0, `SSLv23_client_method()` | — | Max **TLS 1.0**, legacy RSA/AES-CBC/SHA ciphers. The server must speak TLS 1.0. |
| Reads until the peer **closes** the socket | `sslReceiveMessage()` | The server must send `Connection: close` and close. |
| `receiveBuffer` is **1 KB** and overflowed by larger replies | §5.2 | Keep the whole response under ~1 KB. |

The response wsa requires (checked in order): HTTP status `200`; a
`SBI-Status: 0` header (with a space after the colon); and, for the channel
request, `<ChannelCount>`/`<ChannelList>`/`<RefreshIn>` where `ChannelCount`
equals the count of **distinct** channels and the channels fall in wsa's
**21–51** capability filter.

## 2. Redirecting the device to a local server

The port is fixed at 443 and `thttpd` (the web UI) is on port 80, so **443 is
free**. Only the hostname needs to point at the device itself. Two ways, both
relying only on `gethostbyname`:

### Option A — `/etc/hosts` (recommended; keeps the real hostname)

Leave `/etc/config/sbi` as shipped (`HostAddress 'tvws.spectrumbridge.com'`) and
add a hosts entry:

```sh
echo "127.0.0.1   tvws.spectrumbridge.com" >> /etc/hosts
```

uClibc has no `/etc/nsswitch.conf` on this device, so its built-in order is
`files` then `dns`: `gethostbyname("tvws.spectrumbridge.com")` matches
`/etc/hosts` first and returns `127.0.0.1` with no DNS query.

### Option B — point the config at loopback (no resolver involved)

```sh
sed -i "s/option 'HostAddress' '[^']*'/option 'HostAddress' '127.0.0.1'/" /etc/config/sbi
wsagent_stop; wsagent_start     # regenerate the XML and relaunch wsa
```

Because wsa does no CN check, the `127.0.0.1` literal is accepted by TLS all the
same. (The `ln -sf … 3ef04004.0` CA-hash trick in `notes.txt` is **not**
needed — the self-signed leaf is already trusted.)

### Caveat — the v45 agent needs a reverse-resolvable numeric (AV units)

The `get_ip` above is from the **v44** decompile. The **v45** agent shipped on the
MT300-AV units (serials 601/677) behaves differently: after `gethostbyname` it
**reverse-resolves the address and connects to the PTR name**, so it needs that
name to itself be a valid dotted quad. With a plain `127.0.0.1
tvws.spectrumbridge.com` line it reverse-resolves `127.0.0.1` to `localhost` and
aborts with *"localhost is not a valid IP address"* — and pointing the config
straight at `127.0.0.1` (Option B) fails the same way. The fix is to make the
**first** `/etc/hosts` name for `127.0.0.1` a literal dotted quad:

```sh
printf '127.0.0.1\t127.0.0.1 localhost\n127.0.0.1\ttvws.spectrumbridge.com\n' > /etc/hosts
```

See [`units/mt300-av-677.md`](units/mt300-av-677.md) for the full trace. On AV
units `whitespaceagent` is also only started by `wsagent_start` when
`/etc/config/WSAgentRestart` exists (else it runs `MeldAgent -O`, which never
contacts the database), so a never-registered unit needs `touch
/etc/config/WSAgentRestart`. Both steps are wired into
[`../spectrumbridge_spoof_c/device/install_av.sh`](../spectrumbridge_spoof_c/device/install_av.sh).

### Persistence

Confirmed on the bench unit: the root filesystem is read-write yaffs2
(`/dev/mtdblock6`, "RootFS_0"), so edits to `/etc/hosts` and `/etc/init.d/rcS`
survive a reboot. Nothing in the boot scripts rewrites `/etc/hosts`.

Option A is also what makes the setup **independent of the device's IP
address**: the name resolves to `127.0.0.1`, the stand-in listens on loopback
only, and `rcS` brings `lo` up before `eth0`. A new static address, a DHCP lease,
or no network at all changes nothing on that path.

## 3. Deploying the stand-in on the device

### Permanent install (starts at every boot)

`spectrumbridge_spoof_c/device/` holds two small device-side scripts:

| File | Installed as | Does |
|---|---|---|
| `sb_start` | `/usr/bin/sb_start` | runs `/usr/bin/sb_mips https://127.0.0.1:443` and restarts it 5 s after it ever exits |
| `install.sh` | (run once) | copies `sb_mips` + `sb_start` to `/usr/bin`, points `tvws.spectrumbridge.com` at `127.0.0.1` in `/etc/hosts`, and adds `/usr/bin/sb_start &` just before `wsagent_start` in `/etc/init.d/rcS`. Backs up `/etc/hosts.orig` and `/etc/init.d/rcS.orig` first; safe to run again. |
| `install_av.sh` | (run once, **MT300-AV / v45 agent**) | same idea, but hooks `/etc/init.d/setipeth0` (AV firmware starts the agent there, not `rcS`), writes the reverse-name `/etc/hosts` the v45 resolver needs (see the caveat above), and `touch`es `/etc/config/WSAgentRestart` so `wsagent_start` actually launches `whitespaceagent`. Backs up `/etc/hosts.orig` and `/etc/init.d/setipeth0.presb`; safe to run again. |

1. Copy `sb_mips`, `device/sb_start` and `device/install.sh` to `/disk` with
   `nc` and check their MD5s ([`device-access.md`](device-access.md) §4).
2. On the device: `sh /disk/install.sh`
3. Reboot. The agent registers, lists channels and keys the radio with no
   further action.

The binaries go on the **root filesystem** (`/usr/bin`), not `/disk`, because
`/disk` is either AppFS_0 or AppFS_1 depending on the active image; the rootfs
also holds `/etc/hosts` and `rcS`, so the whole change lives in one place.

Verified on the bench unit (2026-11-21, done by hand with the same steps as
`install.sh`; the script itself has not been run on a device yet): after
`reboot`, telnet was back in about 16 s, the agent had registered and stored
all channels by 13 s of uptime, and the transmitter was keyed on channel 23 by
about 30 s.

The rcS change is three lines:

```sh
# SpectrumBridge stand-in (local database for whitespaceagent); see /usr/bin/sb_start
/usr/bin/sb_start &
sleep 2
wsagent_start
```

To undo: `cp /etc/init.d/rcS.orig /etc/init.d/rcS`, restore `/etc/hosts` from
its backup, `rm /usr/bin/sb_start /usr/bin/sb_mips`, reboot.

### Running it by hand (testing)

1. Copy `spectrumbridge_spoof_c/sb_mips` to the device (e.g. `/disk`),
   `chmod +x` it, and apply one redirect from §2.
2. `/disk/sb_mips https://127.0.0.1:443 &` (or `0.0.0.0:443` to reach it from
   the LAN for testing). It serves the same 200 / `SBI-Status: 0` /
   channel-list reply to every request; certificate and key are embedded.
   To change the channels, edit `s_channel_list` in `sb.c` (21–51;
   `ChannelCount` is computed) and rebuild.
3. Restart the agent (`wsagent_stop; wsagent_start`) and watch
   `/etc/config/channels` populate.

No clock change is required: wsa accepts expired/not-yet-valid certs, and the
`Date` header the server sends is used only for display.

## 4. Building `sb_mips`

`spectrumbridge_spoof_c/build.sh` produces the static MIPS binary with no root.
It is Mongoose 7.20 + mbedTLS 2.28.2, cross-compiled with a self-contained musl
toolchain.

```sh
cd spectrumbridge_spoof_c
./build.sh            # downloads toolchain + mbedTLS into ./build/ (gitignored)
```

Three non-obvious things the build gets right, all needed for this legacy target:

- **TLS 1.0 must be re-enabled.** mbedTLS 2.28 compiles TLS 1.0/1.1 out by
  default; `mbedtls_user_config.h` (`-DMBEDTLS_USER_CONFIG_FILE`) switches them
  back on so the OpenSSL-1.0.0 client can handshake.
- **Mongoose never lowers its minimum version.** It calls
  `mbedtls_ssl_config_defaults(PRESET_DEFAULT)` and stops, leaving a TLS 1.2
  floor. `build.sh` patches a throwaway copy of `mongoose.c` to call
  `mbedtls_ssl_conf_min_version(... MINOR_VERSION_1)` (TLS 1.0). The committed
  `mongoose.c` is left untouched.
- **Classic `ET_EXEC`, not static-pie, soft-float.** `-no-pie` makes the binary
  a plain executable like the device's own `wsa`, so the MT300's Linux 2.6.36
  loads it; `-msoft-float` avoids any FPU-ABI mismatch. (`mongoose.c`'s vendored
  header had `MG_ENABLE_TCPIP 1`, which would select Mongoose's built-in TCP/IP
  stack; it is set to `0` so the OS socket API is used.)

Result: `ELF 32-bit LSB executable, MIPS, MIPS-I, statically linked` (~830 KB).

## 5. Testing and its limits

### What passes

`spectrumbridge_spoof_c/test_replay.sh` runs the **shipped MIPS `sb_mips` under
qemu-mipsel** and drives it with the byte-for-byte requests wsa emits
(`buildRegistrationRequest` / `buildChannelRequest`, with the device's real
`RegulatoryDomain`/FCC ID/serial/lat-long), over a forced **TLS 1.0** OpenSSL
client. It asserts every check in wsa's parser:

```
registration PUT : HTTP 200, SBI-Status: 0
channel GET      : HTTP 200, SBI-Status: 0, <ChannelResponse>,
                   ChannelCount == distinct channels, channels in 21-51,
                   body < 1 KB
=> 8 passed, 0 failed
```

This confirms the TLS 1.0 handshake and the exact wire protocol work end to end
against the real server binary.

### Full-system run against the real wsa

The real wsa **does not load under `qemu-user`** (its uClibc/hard-float MIPS32r2
image is rejected by the user-mode ELF loader before `main`), so it was run under
**full-system emulation** instead — a stock mipsel `malta` kernel booting the
device's userspace, with a stand-in radio on `/dev/ttyS1`. See
[`../qemu-env/`](../qemu-env). Observed end to end:

```
radio:  S/N = 371   FCC ID = OKVMT300          (wsa's serial handshake satisfied)
sb:     PUT /v3/devices/US/OKVMT300/371                  -> 200, SBI-Status 0
sb:     GET /v3/channels/US/38.8132280/-77.0443610/?...  -> 200, 11 channels
wsa:    /etc/config/channels = '21,24,27,30,33,36,39,42,45,48,51'
```

So the redirect (§2) and the live register→channel cycle are confirmed with the
**actual wsa binary**, not only by replay. Two notes: wsa will not contact the
database until the radio reports an FCC ID and serial (hence the radio stand-in);
and the device's own 2.6.36 ViXS kernel — recovered from the NAND dump — cannot
boot on qemu's malta machine (no ViXS model), so a compatible stock kernel hosts
the userspace. Running on the physical MT300 is the only way to also exercise
the real radio and ViXS hardware; that was done on 2026-11-21 and the radio keyed
(results in [`on-device-test-plan.md`](on-device-test-plan.md#results-2026-11-21)).

### Host-side quick check (no device)

```sh
cd spectrumbridge_spoof_c
./build.sh
./test_replay.sh
```

## 6. Files

| Path | What it is |
|---|---|
| `spectrumbridge_spoof_c/sb.c` | the stand-in server (unified 200 / `SBI-Status: 0` / channel reply, port 443, `Connection: close`, embedded self-signed cert) |
| `spectrumbridge_spoof_c/sb_mips` | the built static MIPS binary to copy to the device |
| `spectrumbridge_spoof_c/build.sh` | cross-compile (musl toolchain + mbedTLS, TLS 1.0, `-no-pie`) |
| `spectrumbridge_spoof_c/mbedtls_user_config.h` | re-enables TLS 1.0/1.1 in mbedTLS 2.28 |
| `spectrumbridge_spoof_c/test_replay.sh` | runs `sb_mips` under qemu, replays wsa's exact requests, asserts the response |
| `spectrumbridge_spoof_c/notes.txt` | the original author's build/redirect notes |
| `spectrumbridge_spoof_python/sb.py` | a Python equivalent (port 443, same response) for a normal host |
