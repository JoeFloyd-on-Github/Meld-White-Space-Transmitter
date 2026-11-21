# On-device test plan: bringing the transmitter up against the stand-in

This is the step-by-step procedure for exercising the **real MT300** with the
on-device SpectrumBridge stand-in (`sb_mips`) and confirming the agent walks the
full `register -> channel -> transmitter on` cycle on actual hardware.

The protocol mechanics, the redirect options, and the cross-compile are in
[`on-device-spoof.md`](on-device-spoof.md); getting a shell is in
[`device-access.md`](device-access.md). This document is the live runbook and the
**RF-safety preconditions**, which matter here in a way they do not under QEMU.

**Status: planned / partially staged.** Done so far: telnet shell confirmed on the
bench unit; `sb_mips` copied to `/disk/sb_mips` and MD5-verified against the PC
copy. **Not yet done:** rebuild with the reduced channel list, start the server,
restart the agent, confirm RF. The transmitter has **not** been keyed.

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
| `registration` | `status` | `99` | `0` |
| `channel_status` | `status` | `99` | `0` |
| `channels` | — | *(empty)* | `23,42` (or the subset wsa keeps) |
| `operating` | `channel` | *(empty)* | `23` or `42` |
| `transmitter` | `radio_status` | `0` | `1` (**RF keyed**) |

On the server side you should see the two requests the agent makes — a
registration `PUT` and a channel `GET` — each answered `200 / SBI-Status 0`.

## 6. Confirm RF (into the load)

With the output in a dummy load/attenuator, verify a carrier appears at the
operating channel's band (ch 23 = 524-530 MHz, ch 42 = 638-644 MHz) on a spectrum
analyzer or SDR. This is the end-to-end proof on real hardware that the earlier
QEMU result ([`qemu-env/`](../qemu-env)) predicted.

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
