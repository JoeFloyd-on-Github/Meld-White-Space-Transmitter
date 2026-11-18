# 7. The radio handshake: why wsa blocks, and the stand-in that frees it

[← the bootable rootfs](06-rootfs-and-init.md) · [Guide index](README.md) · [Next: the database cycle & web →](08-wsa-sb-and-web.md)

With a kernel booting and a userspace mounted, `wsa` finally *ran* — and then did
nothing. It launched, stayed alive, and **never contacted the database**. This
chapter is the diagnosis of that silence and the radio emulator that resolved it,
and it is the part of the project where leaving `wsa` completely unmodified
mattered most.

## The symptom: alive, silent, not connecting

`wsa` was running (four LinuxThreads PIDs), the generated XML had the right
`HostAddress`, `/etc/hosts` had the redirect — but the stand-in server logged no
connection, `/etc/config/channels` stayed empty, and `wsa` printed nothing, even
with its stdout pointed at the console.

### Ruling things out, one trap at a time

- **Not the redirect / resolver.** Setting `HostAddress=127.0.0.1` (a literal IP
  that needs no name resolution at all) changed nothing — `wsa` still didn't
  connect. So name resolution wasn't the blocker.
- **Not buffering hiding output.** `wsa`'s stdout is fully buffered when it's a
  file, so early runs showed an empty log. Pointing it at `/dev/console` (a tty,
  which is line-buffered) *still* produced nothing — so `wsa` genuinely wasn't
  reaching any `printf`, not merely failing to flush.
- **A self-inflicted red herring.** A diagnostic line in init —
  `echo "GET / HTTP/1.0" | nc 127.0.0.1 443` — was itself opening a **plaintext**
  connection to the TLS port, which made the mbedTLS server log a bogus
  "bad ClientHello" starting with byte `0x47` (`'G'`). That `0x47` looked for a
  while like `wsa` sending plaintext; it was the `nc` probe. **Lesson: remove
  your own diagnostics before interpreting a server's logs — you can easily read
  your probe's traffic as the target's.**

### Finding the real block: `wchan` + open fds

The decisive move was to ask the kernel where each `wsa` thread was stuck
(remembering LinuxThreads = one PID per thread, [chapter 6](06-rootfs-and-init.md)):

```sh
for P in $(ps | grep '[w]hitespaceagent' | awk '{print $1}'); do
  echo "pid $P: wchan=$(cat /proc/$P/wchan) state=$(cut -d' ' -f3 /proc/$P/stat)"
  ls -l /proc/$P/fd
done
```

Two facts jumped out:

- One thread sat in `poll_schedule_timeout`, and the open fds included
  **`fd 6 -> /dev/ttyS1`** and `fd 5 -> /etc/config/fccidSerialInfo` — but **no
  socket**. The network thread had never opened a connection.
- The other threads were in `sys_rt_sigsuspend` — LinuxThreads' way of waiting on
  a condition variable.

The picture: the **serial thread is polling `/dev/ttyS1` for a radio that isn't
there**, and the **database thread is blocked waiting for the serial thread to
signal it.** No radio → no signal → no database call. Ever.

## Why the agent waits for the radio

This is by design, and the decompile (and [`../wsa-overview.md`](../wsa-overview.md)
§6.2) spells it out. On startup the serial thread runs a fixed init sequence
against the NXP radio on `/dev/ttyS1`, and its **final step** is:

> *If both `fccID` and `serialNumber` are non-empty, signal thread A to start.*

The agent **cannot build a registration or channel request without the FCC ID and
serial number** — and it gets them *from the radio*, not from a config file. So
until the radio answers, the database thread is parked. On real hardware the
radio always answers; on malta, `ttyS1` is a bare 16550 with nothing attached.

### The serial protocol (the parts that matter)

The radio speaks a simple line/char protocol at 115200 (documented fully in
`../wsa-overview.md` §6.3). The init handshake, in order:

| Agent sends | Radio must reply | Purpose |
|---|---|---|
| `MELD_TECH_WS_ENABLE\r` | — | enter white-space control mode |
| `h` | info lines, then a line starting `EOF` | info dump (tolerates a 100 ms timeout) |
| `e0\r` | — | transmitter off |
| `l\r` | `Firmware version = ...` | firmware version |
| **`x`** | **`S/N = <int>`** | **serial number → `serialNumber`** |
| **`y`** | **`FCC ID = ...`** | **FCC ID → `fccID`** |
| `b?` | 8 hex digits at offset 3 | streaming IP |
| … | | power / modulation |

The two **critical** exchanges are `x` and `y`: without those two replies the
gate at the end of the sequence never opens. Everything else tolerates a timeout.

## The stand-in radio — `radio-emu.py`

The fix is to put something on the other end of `ttyS1` that answers the
handshake. QEMU exposes guest `ttyS1` as a **TCP server** (the second `-serial`,
[chapter 5](05-malta-kernel.md)):

```
-serial tcp:127.0.0.1:5556,server,nowait
```

and [`radio-emu.py`](../../qemu-env/radio-emu.py) connects to it from the host and
replies to the agent's commands:

```python
if   c == b"x":  send("S/N = %s\r\n" % SERIAL)     # 371
elif c == b"y":  send("FCC ID = %s\r\n" % FCCID)   # OKVMT300
elif c == b"l...": send("Firmware version = ...")
elif c == b"h":  send("MELD WS radio\r\nStatus OK\r\nEOF\r\n")
# e0/e1, c<ch>, p.., m#[..], b#[..]  -> acknowledged / ignored
```

It only needs to be faithful enough to get past the gate — `S/N` and `FCC ID`
are mandatory; the rest are acknowledged so the agent's 100 ms-timeout reads
don't stall. [`run-qemu.sh`](../../qemu-env/run-qemu.sh) starts it automatically
alongside QEMU; it retries the connection until the serial port is listening.

The moment it answers, the guest log confirms the gate opening and the database
thread springing to life:

```
[radio] -> 'S/N = 371\r\n'
[radio] -> 'FCC ID = OKVMT300\r\n'
sb: accepted 127.0.0.1:... -> 127.0.0.1:443     # the agent finally connects
```

## Leaving `wsa` unmodified — and why that was the point

Throughout all of this, **the `wsa` binary was never patched**. Not one byte. The
goal was to observe the *real* software's behaviour, so every obstacle was solved
by changing the *environment around* the binary, never the binary:

- It expects a radio handshake → **emulate the radio**, don't stub out the check
  in the binary.
- It won't load in qemu-user → **run it under a real kernel**, don't rebuild it.
- It connects to a dead host on a hard-coded port → **redirect the host** and
  **answer on that port**, don't re-point the binary.
- It needs an FCC ID and serial → **have the (emulated) radio report them**,
  don't fake the config.

(Earlier, purely to *diagnose* qemu-user, a throwaway copy of `wsa` had its ELF
header byte-patched — but that was a disposable experiment to probe the loader,
never used to run the agent, and the real binary stayed pristine.)

The reward is credibility: when `/etc/config/channels` fills in, it is the
genuine, unmodified agent that put it there, exercising the exact code paths the
device runs — so what the emulator proves about the protocol and the stand-in
server is true of the real device too.

**Tips that generalise:**
- When an embedded agent "runs but does nothing," read `/proc/<pid>/wchan` and
  `/proc/<pid>/fd` for every thread. "Blocked in `poll` on a tty, no socket open"
  is a loud clue that it's waiting on a peripheral, not the network.
- A device that talks to a sub-component over serial often **gates its main
  function on that sub-component's identity reply**. Emulate just enough of the
  sub-component to answer the identity query.
- Wire an emulated UART to a host socket with QEMU's `-serial tcp:...,server` and
  script the peer; remember `-serial` order maps to `ttyS0, ttyS1, …`.
- Strip your own probes before reading the target's logs, and prefer changing the
  environment over patching the binary when the goal is faithful observation.

[← the bootable rootfs](06-rootfs-and-init.md) · [Guide index](README.md) · [Next: the database cycle & web →](08-wsa-sb-and-web.md)
