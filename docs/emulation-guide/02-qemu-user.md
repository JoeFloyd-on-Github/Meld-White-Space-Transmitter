# 2. qemu-user: the fast path and its wall

[← The device](01-the-device.md) · [Guide index](README.md) · [Next: user vs system →](03-user-vs-system.md)

The quickest way to run a foreign-architecture Linux binary on an x86-64 host is
**qemu-user** (user-mode emulation): it translates one process's instructions and
syscalls, using the *host* kernel and filesystem. No kernel, no disk image, no
boot — you just run the binary. It is the right first thing to try, and it is
where this project started.

## Setup

```sh
sudo apt-get install qemu-user qemu-user-binfmt
```

That installs `qemu-mipsel` (little-endian MIPS, which the MT300 is) and registers
it with the kernel's `binfmt_misc` so a MIPS ELF can be run "natively":

```sh
$ qemu-mipsel ./some-mips-binary          # explicit
$ ./some-mips-binary                       # via binfmt_misc, transparently
```

Verify binfmt is active:

```sh
$ cat /proc/sys/fs/binfmt_misc/qemu-mipsel
enabled
interpreter /usr/bin/qemu-mipsel
```

### Running and tracing

qemu-user's headline debugging feature is `-strace`, which prints the guest's
syscalls — invaluable for seeing exactly what a binary opens, connects to, reads
and writes. In principle:

```sh
qemu-mipsel -strace ./binary arg1 arg2
```

## What worked: the soft-float server

The purpose-built stand-in server, `sb_mips` (a static MIPS build of Mongoose +
mbedTLS, see [chapter 8](08-wsa-sb-and-web.md)), ran under qemu-user with no
trouble:

```sh
$ qemu-mipsel spectrumbridge_spoof_c/sb_mips "https://127.0.0.1:8443"
Meld white-space DB stand-in (SpectrumBridge spoof)
listening on https://127.0.0.1:8443
```

We even validated the full database protocol this way: run the MIPS server under
qemu-user on a high port, then drive it with an `openssl s_client` forced to TLS
1.0 replaying the *exact* bytes `wsa` emits. That is the replay harness,
[`test_replay.sh`](../../spectrumbridge_spoof_c/test_replay.sh), and it passes
8/8. qemu-user is genuinely useful.

## The wall: `wsa` will not load

The real agent, though, refused to run:

```sh
$ qemu-mipsel WhiteSpaceApp/wsa -v
$ echo $?
1                          # no version string, no output, just exit 1
```

Every attempt — with arguments, without, with different `-cpu` models, with
`-R` reserved-VA tweaks — produced the same silent `exit 1`, **before `main`
ever ran**. (A `wsa -v` should print `whitespaceagent Version:44 ...`; it printed
nothing.)

### Diagnosing it (and two traps that wasted time)

**Trap 1 — the pipe exit-code trap.** Early on it *looked* like `wsa` ran,
because:

```sh
$ qemu-mipsel WhiteSpaceApp/wsa 2>&1 | head -8 ; echo "exit $?"
exit 0
```

That `0` is **`head`'s** exit code, not qemu's. `$?` after a pipeline is the last
command's status. The binary had been failing all along; the pipe hid it. Always
capture the exit of the process you care about:

```sh
qemu-mipsel WhiteSpaceApp/wsa -v >/tmp/o 2>/tmp/e; echo "rc=$?"; wc -c </tmp/o
```

**Trap 2 — strace that prints nothing.** `qemu-mipsel -strace` and
`-d strace -D file` both produced *empty* output for `wsa` on this build. The
fix was to trace qemu *itself* from the host — the host-level syscalls include
the translated guest calls and qemu's own loader activity:

```sh
strace -f -qq qemu-mipsel WhiteSpaceApp/wsa -v 2>/tmp/full.txt
tail -25 /tmp/full.txt
```

That revealed the truth. qemu read the 52-byte ELF header, reserved a 128 MB
guest address space, read `/proc/sys/vm/mmap_min_addr`, and then:

```
exit_group(1)
```

It bailed **during ELF loading, before mapping the program's `PT_LOAD`
segments** — a hard rejection by qemu's user-mode ELF loader, not a crash in the
guest.

### Why qemu-user rejects it

Comparing the two binaries' ELF headers tells the story:

| | `wsa` (rejected) | `sb_mips` (runs) |
|---|---|---|
| libc | uClibc | musl |
| float ABI | **hard float** (double precision) | soft float |
| ISA | MIPS32r2 (`EF_MIPS_ARCH_32R2`) | MIPS1 |
| `EI_ABIVERSION` | 1 | 0 |

Patching the arch bits and `ABIVERSION` to match the working binary did **not**
help — the rejection is deeper than any single header field this build of
qemu-user accepts. The practical conclusion:

> **qemu-user 10.2.1 will not load this particular uClibc / hard-float /
> MIPS32r2 static binary.** It runs on the real device and under a real kernel
> (see [chapter 3](03-user-vs-system.md)); it is qemu's *user-mode loader*,
> specifically, that won't take it.

This is not unusual. qemu-user re-implements the kernel's ELF loader and syscall
layer in userspace, and coverage of older/edge toolchains is imperfect —
particularly around MIPS hard-float FP modes and uClibc's LinuxThreads startup.

## Limitations of qemu-user (the general list)

Even when it *does* run a binary, qemu-user cannot emulate a *system*:

- **No real kernel.** The guest's `uname`, `/proc`, devices, scheduling and
  threading all come from the host kernel via translation. Anything that depends
  on kernel behaviour the host doesn't provide (or qemu doesn't translate) breaks.
- **No device nodes with real drivers.** `/dev/ttyS1`, `/dev/micro`, framebuffers
  — a user-mode process opens the *host's* path of that name, if any. There is no
  emulated UART to drive a radio.
- **No init, no multi-process services.** You run one program. The web server,
  the agent, and the radio all needing to coexist and talk over sockets and
  serial is not a user-mode scenario.
- **Threading is fragile.** uClibc LinuxThreads (one process per thread, a
  manager thread, signal-based condition variables) stresses qemu-user's clone
  emulation.
- **Load compatibility is not guaranteed**, as we just saw.

qemu-user is perfect for "run this one static tool and watch its syscalls." It is
the wrong tool for "boot this appliance and watch its services interact." For
`wsa`, which needs a real kernel to even load, a real UART to unblock, and
sibling processes to talk to, the answer was full-system emulation.

**Tip that generalises:** try qemu-user first — it's minutes of setup and often
enough. But the moment a binary won't load, or needs a device/kernel/sibling
process, stop fighting it and move to qemu-system. The time sink is trying to
make user-mode do a system-mode job.

[← The device](01-the-device.md) · [Guide index](README.md) · [Next: user vs system →](03-user-vs-system.md)
