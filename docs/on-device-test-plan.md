# On-device test plan: bringing the transmitter up against the stand-in

This is the step-by-step procedure for exercising the **real MT300** with the
on-device SpectrumBridge stand-in (`sb_mips`) and confirming the agent walks the
full `register -> channel -> transmitter on` cycle on actual hardware.

The protocol mechanics, the redirect options, and the cross-compile are in
[`on-device-spoof.md`](on-device-spoof.md); getting a shell is in
[`device-access.md`](device-access.md). This document is the live runbook and the
**RF-safety preconditions**, which matter here in a way they do not under QEMU.

**Status: passed on hardware (2026-11-21), into a dummy load.** Steps 1–6 were run
on the bench unit (serial 457). The unmodified agent registered, got channels
`23,42`, chose channel 23, and keyed the radio; a carrier was seen on the
analyzer at channel 23 (524–530 MHz). See [Results](#results-2026-11-21). The
setup does **not** yet survive a reboot (the stand-in is not started at boot).

---

## 0. RF safety preconditions (do these first)

The MT300 is a real, FCC-certified UHF TV transmitter. The database check it is
missing is the exact mechanism that normally keeps it from transmitting on an
occupied channel. Standing in for that database **removes that protection** — the
stand-in says "yes" without checking what is actually on the air at your
location. Transmitting on a channel in use by a licensed TV station, or outside
Part 15 Subpart H rules for white-space devices, can cause interference and is
unlawful.

So, before keying the radio:

1. **Terminate the RF output into a dummy load or a high-value attenuator**, not
   an antenna, for the first bring-up. Confirm the chain on a spectrum analyzer or
   SDR before anything radiates.
2. **Offer only channels you have verified are vacant at your location.** This
   plan uses **channels 23 and 42** (`sb.c`: `s_channel_list = "23,42"`,
   `s_channel_count = 2`), matching the unit's `PreferredChannel`. Confirm both
   are clear via the FCC TV-query / white-space tools and a local scan before use.
   - UHF center bands for reference: **ch 23 = 524-530 MHz**, **ch 42 = 638-644
     MHz**.
3. Treat the whole thing as bench testing of your own hardware into a load, not as
   putting a station on the air.

## 1. Rebuild `sb_mips` for the reduced channel list

The `sb_mips` committed in the repo (and the copy already on the device) is the
**11-channel** build (`21..51`). After narrowing `sb.c` to `23,42` it must be
rebuilt — the server's channel list is compiled in, not configurable at runtime:

```sh
cd spectrumbridge_spoof_c
rm -rf build && ./build.sh          # needs Linux: musl MIPS toolchain + mbedTLS
```

Then copy the fresh `sb_mips` to the device and verify, per
[`device-access.md`](device-access.md) §4:

```sh
# device:
(nc -l -p 9000 > /disk/sb_mips &) ; sleep 1 ; chmod +x /disk/sb_mips
# after transfer, both ends:
md5sum /disk/sb_mips
```

## 2. Point the dead host at the on-device server

Two equivalent options (details and rationale in
[`on-device-spoof.md`](on-device-spoof.md) §2). To run the stand-in **on the
device**, send the name to loopback. The bench unit's `/etc/hosts` currently maps
it to `192.168.1.249`; repoint it:

```sh
sed -i 's/^[0-9. ]*tvws\.spectrumbridge\.com/127.0.0.1   tvws.spectrumbridge.com/' /etc/hosts
grep tvws /etc/hosts        # confirm -> 127.0.0.1
```

(Alternatively, leave `/etc/hosts` at `192.168.1.249` and run the server on *that*
machine instead of the device. The steps below assume on-device / loopback.)

## 3. Start the stand-in

Port 443 is free (the web UI `thttpd` is on 80). Root shell can bind it:

```sh
/disk/sb_mips https://0.0.0.0:443 &
```

It serves the same `200` / `SBI-Status: 0` / channel-list reply to every request;
cert and key are embedded, so there are no files to install. Leave its stdout
visible (or redirect to a log) so you can see each request it answers.

## 4. Restart the agent so it re-resolves and re-queries

```sh
wsagent_stop ; wsagent_start
```

`wsagent_start` rebuilds the XML from `sbi` and relaunches `wsa`, which will now
resolve `tvws.spectrumbridge.com` to `127.0.0.1`, TLS-handshake with the
stand-in, register, and request channels.

## 5. Watch the status files flip

```sh
# busybox: a simple poll loop
while : ; do
  clear
  for f in registration channel_status channels operating transmitter ; do
    echo "=== $f" ; cat /etc/config/$f ; echo
  done
  sleep 1
done
```

**Success looks like:**

| File | Field | Before | After success |
|---|---|---|---|
| `registration` | `status` | `99` (or no options yet) | `0` |
| `channel_status` | `status` | `99` | `0` |
| `channels` | — | *(empty)* | `23,42` (or the subset wsa keeps) |
| `operating` | `channel` | *(empty)* | `23` or `42` |
| `transmitter` | `radio_status` → `status` | `0` | `1` (**RF keyed**) |

On the server side you should see the two requests the agent makes — a
registration `PUT` and a channel `GET` — each answered `200 / SBI-Status 0`.

## 6. Confirm RF (into the load)

With the output in a dummy load/attenuator, verify a carrier appears at the
operating channel's band (ch 23 = 524-530 MHz, ch 42 = 638-644 MHz) on a spectrum
analyzer or SDR. This is the end-to-end proof on real hardware that the earlier
QEMU result ([`qemu-env/`](../qemu-env)) predicted.

## Results (2026-11-21)

Bench unit: serial 457, kernel `2.6.29-ts`, Meld app v1.26, RF output into a
dummy load. Run from a PC over telnet.

| Step | What happened |
|---|---|
| 1. Rebuild + transfer | `sb_mips` rebuilt for `23,42` (MD5 `abeee51e5ec8512d142fcdcaaa51d632`), pushed with `nc`, MD5 matched on the device. Old 11-channel build kept as `/disk/sb_mips.11ch`. A TLS 1.0 handshake against it on a spare port succeeded on the device. |
| 2. Redirect | `/etc/hosts` backed up to `/disk/hosts.bak`, then `tvws.spectrumbridge.com` → `127.0.0.1`. |
| 3. Stand-in | `/disk/sb_mips https://0.0.0.0:443 > /tmp/sb.log 2>&1 &` |
| 4. Agent restart | `wsagent_stop`, then `wsagent_start`'s two commands run by hand with the agent's output sent to `/tmp/wsa.log` instead of `/dev/null` (it logged nothing). |
| 5. Status files | all flipped within about a second of the agent starting: `registration` 0, `channel_status` 0, `channels` `23,42`, `operating` 23, `transmitter` 1. |
| 6. RF | carrier seen at channel 23 (524–530 MHz) on the analyzer. |

The stand-in logged exactly the two expected requests, a registration `PUT
/v3/devices/US/OKVMT300/457` and a channel `GET /v3/channels/US/<lat>/<lon>/`,
both answered `200` / `SBI-Status 0`.

Notes against the plan as written:
- On this unit `/etc/config/registration` had **no options** before the first
  successful registration, rather than `status 99`.
- In `transmitter`, `radio_status` is the section name; the option is `status`.
- **Not persistent.** The `/etc/hosts` change survives a reboot (the rootfs is
  writable yaffs2), but `sb_mips` is not started at boot, so after a power cycle
  the agent finds nothing on 443 and RF stays off. The stand-in's reply sets
  `RefreshIn` to 24 h, so it must also stay running for the agent's re-checks.

## 7. Rollback

```sh
pkill sb_mips                       # stop the stand-in
# restore the original hosts mapping (or whatever the site used)
sed -i 's/^127\.0\.0\.1[ \t]*tvws\.spectrumbridge\.com/192.168.1.249  tvws.spectrumbridge.com/' /etc/hosts
wsagent_stop ; wsagent_start        # agent goes back to getting no answer; RF drops
```

Nothing here is destructive: the status files return to `99`/`0` on their own
once the agent can no longer reach a database.

## Troubleshooting

| Symptom | Likely cause | Check |
|---|---|---|
| Status stays `99` | name not redirected, or server not up | `grep tvws /etc/hosts`; is `sb_mips` running and bound to 443? |
| TLS handshake fails | server not speaking TLS 1.0 | the build must re-enable TLS 1.0 (see `build.sh` / `mbedtls_user_config.h`) |
| `channel_status 0` but `channels` empty | offered channels fell outside wsa's 21-51 filter | keep `s_channel_list` within 21-51 |
| Registration ok, no RF | radio didn't accept the channel | watch the agent's serial chatter to the NXP radio on `/dev/ttyS1` |
