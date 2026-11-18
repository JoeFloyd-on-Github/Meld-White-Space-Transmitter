# 8. The database cycle and the web UI

[← the radio handshake](07-radio-handshake.md) · [Guide index](README.md) · [Next: tips & pitfalls →](09-tips-and-pitfalls.md)

With the radio handshake answered ([chapter 7](07-radio-handshake.md)), the
agent's database thread finally runs. This chapter covers the last three pieces:
**redirecting** the dead host to a local stand-in, winning the **TLS 1.0 interop**
fight so the handshake actually completes, and how the **web server and the agent
interact** through shared config files. The stand-in server itself —
`sb_mips` — is documented in [`../on-device-spoof.md`](../on-device-spoof.md);
here we focus on making the *emulated device* talk to it.

## Redirecting the dead host

`wsa` resolves `<HostAddress>` (from `/etc/config/sbi`) with `gethostbyname()` and
connects to **port 443, hard-coded**. There are two ways to point that at a local
server, and the emulator uses both for robustness:

1. **`/etc/hosts`** — add `127.0.0.1 tvws.spectrumbridge.com`. Clean, and keeps
   the real hostname in the config. But it depends on the agent's resolver
   actually consulting `/etc/hosts`, which for a static uClibc build is not
   guaranteed.
2. **`HostAddress=127.0.0.1`** in `sbi` — a literal IP that needs *no resolver at
   all*. This is the robust choice and the one the emulator relies on:

   ```sh
   sed -i "s/option 'HostAddress' '[^']*'/option 'HostAddress' '127.0.0.1'/" /etc/config/sbi
   ```

Because the agent does **no certificate hostname/CN check** (confirmed in the
decompile), connecting to `127.0.0.1` while the cert says
`tvws.spectrumbridge.com` is fine. And because its verify callback accepts a
self-signed leaf, no CA setup is needed. Port 443 is free on the device (the web
server is on 80), so the stand-in can take it.

## The TLS 1.0 interop fight

The agent links **OpenSSL 1.0.0** with `SSLv23_client_method()` — it can
negotiate at most **TLS 1.0**, with legacy RSA / AES-CBC / SHA-1 ciphers. The
stand-in is Mongoose + **mbedTLS 2.28**, which by default speaks only TLS 1.2+.
Three changes were needed to meet the old client, all in the stand-in build (the
agent is never touched):

1. **Compile TLS 1.0 back in.** mbedTLS 2.28 builds TLS 1.0/1.1 out by default;
   a user-config overlay re-enables them
   ([`mbedtls_user_config.h`](../../spectrumbridge_spoof_c/mbedtls_user_config.h)):
   `#define MBEDTLS_SSL_PROTO_TLS1` (and `TLS1_1`, `SSL3`).
2. **Lower Mongoose's minimum version.** Mongoose calls
   `mbedtls_ssl_config_defaults(PRESET_DEFAULT)` and never drops the floor below
   TLS 1.2, so even with TLS 1.0 compiled in the server refuses it. The build
   patches a throwaway copy of `mongoose.c` to call
   `mbedtls_ssl_conf_min_version(..., MINOR_VERSION_1)`.
3. **Accept the legacy ClientHello.** For robustness with OpenSSL 1.0.0 clients
   that announce themselves with the old SSLv2-format ClientHello,
   `MBEDTLS_SSL_SRV_SUPPORT_SSLV2_CLIENT_HELLO` is enabled too.

### Reading the handshake when it fails

Debugging a TLS handshake blind is miserable; mbedTLS will narrate it. Building
`sb_mips` with `-DMG_MBEDTLS_DEBUG_LEVEL=4` turns on per-message logging, which
is how the real handshake was confirmed (and how the `nc`-probe red herring from
[chapter 7](07-radio-handshake.md) was caught):

```
client hello v3, message type: 22          # 0x16 = a real TLS handshake record
client hello v3, protocol version: [3:1]   # TLS 1.0
... => parse change cipher spec ... <= handshake wrapup
```

(Turn the debug level back off for the shipped binary — it's noisy and slower.)

## The full cycle, observed

Once the versions meet, the real agent runs the complete protocol against the
stand-in, and the stand-in's log shows both requests `wsa` is documented to send:

```
sb:  PUT  /v3/devices/US/OKVMT300/371                   -> 200, SBI-Status 0   (register)
sb:  GET  /v3/channels/US/38.8132280/-77.0443610/?...   -> 200, 11 channels    (channel request)
```

The agent parses the channel list, filters it to the hardware band (21–51), and
writes the survivors to the config:

```
/etc/config/channels = '21,24,27,30,33,36,39,42,45,48,51'
```

That file is the proof the whole emulator exists to produce: the **unmodified
agent**, on a **real kernel**, having completed a **real TLS 1.0 handshake**,
accepted the stand-in's answer and committed the channels it would transmit on.
The dead `tvws.spectrumbridge.com` has been replaced, end to end, by a 900 KB
program the device could run on itself.

## How the web server and the agent interact

They don't call each other directly — they communicate through **`/etc/config`
files and process control**, which is exactly how the real device works.

```
         writes                         reads
  wsa  ───────────►  /etc/config/channels   ◄────────── status.php  (the web UI)
  wsa  ───────────►  /etc/config/operating          │
  wsa  ───────────►  /etc/config/registration       │  renders the Status page
  wsa  ───────────►  /etc/config/transmitter        │
                                                     
  registration.php ─ writes ─► /etc/config/sbi ─► wsagent_stop; wsagent_start ─► wsa restarts
```

- **The agent is the writer.** As it registers and obtains channels it writes the
  status files (`channels`, `operating`, `registration`, `transmitter`,
  `channel_status`). It reads its *input* from `sbi` (via the generated XML).
- **The web UI is the reader/editor.** `thttpd` (with embedded PHP — the
  SpectrumBridge "thttpd.php" build, so no separate PHP binary) serves the LuCI-
  style pages from `/disk/www`. `status.php` reads the agent's output files and
  shows registration status, the operating channel and TX state.
  `registration.php` and `preferences.php` **edit `sbi`** and then call
  `wsagent_stop; wsagent_start`, which regenerates the XML (via the
  `xml_conversion` awk script) and **relaunches the agent** — i.e. the way a user
  "requests a new channel" from the web UI is to change settings and have the
  agent restarted to act on them.

In the emulator this all works: once the NSS/loader fixes from
[chapter 6](06-rootfs-and-init.md) are in place, thttpd serves, and
`GET /status.php` returns the real page:

```html
<title>MELDTECH - White Space Status</title>
```

reading the very `channels` file the emulated agent just wrote — the web UI and
the agent, both the device's own software, cooperating inside the emulator exactly
as they do on the hardware.

**Tips that generalise:**
- A hard-coded port + no CN check + a permissive verify callback means a redirect
  plus a self-signed cert is enough — read the client's trust logic before
  building a CA.
- To talk to an ancient TLS client you must often **compile old protocol versions
  back in *and* lower the library's version floor** — doing just one leaves the
  server refusing the handshake.
- Turn on your TLS library's handshake debug (`MG_MBEDTLS_DEBUG_LEVEL`,
  `SSL_CTX_set_msg_callback`, `openssl s_client -msg`) before guessing why a
  handshake fails.
- Appliance components usually integrate through **shared files + restart hooks**,
  not IPC. Find the files and the restart command and you understand the whole
  interaction.

[← the radio handshake](07-radio-handshake.md) · [Guide index](README.md) · [Next: tips & pitfalls →](09-tips-and-pitfalls.md)
