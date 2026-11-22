# MT300-AV cold-boot TX-enable flaw — diagnosis, fix, and test

**One-line:** on the MT300-**AV** firmware the transmitter is enabled only in
response to a video-**connect event**, so after a cold power cycle with the HDMI
source already synced the unit comes up *registered, channel-selected and
powered but with TX disabled* — no RF. The fix is a small boot supervisor,
[`sb_txkeepalive`](../spectrumbridge_spoof_c/device/sb_txkeepalive), that
re-asserts the enable once the radio is armed and keeps it enabled.

This is a firmware **logic** flaw, independent of the SpectrumBridge stand-in:
registration against `sb_mips` works correctly throughout. It was found on unit
677; see [`units/mt300-av-677.md`](units/mt300-av-677.md) for that unit's record.

> Not to be confused with the **virtual-channel** quirk (TV shows `21-1` while the
> RF is on ch31), which is a separate PSIP-latch issue — see
> [`radio-and-video.md`](radio-and-video.md) §4.1.

---

## 1. Symptom

After a **hard power cycle**, the unit booted normally (telnet, web UI, static IP
all fine) but **produced no RF**, where before the cycle it had been transmitting
on channel 31.

## 2. The debugging deep-dive

The value of writing this out is the *method*: the firmware spreads the "am I
transmitting?" state across several places that can disagree, and the bug only
shows up when you compare them.

### 2.1 Establish what booted

```
ps                      -> sb_start, sb_mips, av_streamer running; whitespaceagent NOT
netstat -ltn            -> 127.0.0.1:443 LISTEN  (stand-in up)
/etc/config/registration-> status 0  (success), timestamp fresh
/etc/config/channel_status-> status 0, timestamp STALE (first boot pass only)
/etc/config/operating   -> channel ''   (empty!)
/etc/config/transmitter -> radio_status 0
/etc/config/queryInfo   -> Channel = 31, "TX disabled"
av_streamer log         -> "NO digital video standard detected"
logread                 -> whitespaceagent "starting"/"initializeSecurityCtx" every 2-3 s
```

Three red herrings to clear first:

- **"No video."** `av_streamer` reported no lock, and this AV unit genuinely will
  not key without a live input — but that turned out to be only part of it (see
  2.3).
- **Resolver regressed?** No: `/etc/hosts` still had the v45 reverse-name trick,
  and a live capture showed the agent doing a correct `PUT /v3/devices/.../677`
  answered `200 / SBI-Status 0 / 31 channels` by `sb_mips`. Registration works.
- **Agent crash?** `dmesg` showed no segfault; the repeated "starting" is the
  agent being re-run, not a kernel crash.

### 2.2 The telling disagreement

The status files don't agree, and that is the clue:

| Source | Says | Written by |
|---|---|---|
| `registration` | success, **fresh** | whitespaceagent, each cycle |
| `channel_status` | success, **stale** (boot only) | whitespaceagent, on channel request |
| `operating` | **empty** | whitespaceagent, after selecting a channel |
| `queryInfo` | `Channel = 31`, `TX disabled` | MeldAgent/agent, radio readback |
| `transmitter` radio_status | **0** | whitespaceagent |

The agent **keeps registering** but never lands a committed operating channel or a
keyed radio. Once video was reconnected and locked (`Input video is 1080i@60`),
`registration` and `channel_status` still both read success — yet `operating`
stayed empty and `radio_status` stayed 0. So **"no video" was not the whole
story**: with video present and registration succeeding, it still would not key.

### 2.3 `whitespaceagent` is a one-shot that arms but does not enable

Running the boot agent in the foreground and letting it finish cleanly (stand-in
up, video locked) advanced the state exactly one notch:

```
/etc/config/operating -> channel '31'          (now committed)
/etc/config/queryInfo -> Channel = 31, TX power = 0x05   (power LEVEL set)
/etc/config/transmitter radio_status -> still 0
MeldAgent -q          -> ... "TX disabled"      (radio itself: still OFF)
```

Two lessons here, both easy to misread:

1. **`queryInfo: TX power = 0x05` is a power *level*, not "enabled".** The agent
   set the level; the radio's own `q` status still said **`TX disabled`**. The
   authoritative TX on/off state is the radio readback (`MeldAgent -q`), **not**
   `queryInfo` and **not** the `radio_status` file (which is only written by the
   one-shot agent, so it is stale the moment the agent exits).
2. The agent **registers → selects channel 31 → sets the power level → exits**,
   *without ever enabling the transmitter.* Its 6-line log ends after the TLS
   setup; it is a one-shot, not a resident daemon.

### 2.4 Where the enable actually comes from

`/disk/www/RF_Control.php` documents the radio command set, including:

```
'e1'  :  Enable TX Power
```

and the firmware's own transmit tool is `MeldAgent`:

```
-t, -T   : TX Power Enable        (the TXAutoEnable boot path runs this)
-o, -O   : TX Power Disable
-q/-s    : Print NXP and TX Status
NXP MUST BE IN AUTOMATIC MODE!!!
```

So the chain to actually emit is: **register → select channel → set power level →
`e1` (enable) → valid TS from `av_streamer`.** The agent does everything *except*
the `e1`. Running it by hand fixed it instantly:

```
MeldAgent -T
MeldAgent -q  ->  Channel = 31, TX power = 0x05, Automatic mode   # RF on, confirmed on analyzer
```

### 2.5 Root cause — the flawed logic

The stock firmware ties the `e1` enable to a **video-connect event**: the unit
keys when a source is *plugged in* (the original "I connected a video source and
the output turned on"). On a **cold boot with the source already attached**, there
is no connect transition, so the enable is never sent. `whitespaceagent` arms the
radio and exits; `av_streamer` locks the already-present video but nothing issues
`e1`. The unit sits **armed-but-disabled** indefinitely. A soft reboot often hid
the bug because the separately-powered NXP board **retained** its enabled state; a
hard power cycle resets that board, exposing it.

## 3. The fix — `sb_txkeepalive`

A ~40-line `/bin/sh` supervisor, launched at boot from `/etc/init.d/setipeth0`
(alongside `sb_start`), that closes the gap:

- waits for the radio to be **armed** (`registration` status 0 **and** a non-empty
  `operating` channel — i.e. a real database grant);
- then, whenever `MeldAgent -q` reports `TX disabled`, issues `MeldAgent -T`;
- re-checks every 30 s, so it also recovers if TX ever drops;
- stays off `/dev/ttyS1` while `whitespaceagent`'s own serial thread may hold it
  (it only acts after the one-shot agent has exited);
- if a boot pass left the unit un-armed, re-runs `wsagent_start` to arm it.

It **never** enables TX without a committed `operating` channel, so it can only key
a database-authorized channel. Source:
[`spectrumbridge_spoof_c/device/sb_txkeepalive`](../spectrumbridge_spoof_c/device/sb_txkeepalive);
installed and hooked by
[`install_av.sh`](../spectrumbridge_spoof_c/device/install_av.sh).

## 4. Test and result

Validated on unit 677 (2026-11-22):

1. **Reboot.** After `reboot`, `sb_txkeepalive` launches from the boot hook; the
   unit comes up registered on ch31 with `MeldAgent -q` reporting `TX power =
   0x05`, video locked.
2. **Fault injection** (reproduces the hard-cycle symptom without a power cycle):
   `MeldAgent -O` to force `TX disabled`, then watch. Done twice; both times the
   keepalive caught it within its poll interval and re-enabled —

   ```
   sb_txkeepalive: radio armed (ch 31) but TX disabled -- enabling
   sb_txkeepalive: radio armed (ch 31) but TX disabled -- enabling
   ```

   with the radio returning to `TX power = 0x05` after each.

`MeldAgent -O` leaves the radio in exactly the armed-but-disabled state a hard
power cycle produces, so this is a faithful test. A hard power cycle remains the
final real-world confirmation (done at the bench).

## 5. Notes and caveats

- **Posture.** With this installed the unit **transmits automatically at
  power-up** once it has a valid grant. That is the stated goal here; it is a
  deliberate change from the stock "operator must trigger" behavior. The
  `operating`-channel gate keeps it from ever keying without a database grant.
- **Manual equivalents** (no reinstall): `MeldAgent -T` enables TX now; `MeldAgent
  -O` disables; `MeldAgent -q` prints the authoritative radio state.
- **Don't trust these for TX state:** the `radio_status` file and `queryInfo`
  (`TX power = 0xNN` is a level, set before the enable). Use `MeldAgent -q`.
- The `wsagent_start` re-arm branch in the keepalive is a secondary safety net for
  a boot pass that failed to arm; the primary, tested path is the enable branch.
