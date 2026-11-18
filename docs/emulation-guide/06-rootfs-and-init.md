# 6. Building a bootable userspace from a snapshot

[← a working kernel](05-malta-kernel.md) · [Guide index](README.md) · [Next: the radio handshake →](07-radio-handshake.md)

A filesystem *snapshot* is not a *bootable* filesystem. The snapshot in
[`../../root`](../../root) is a faithful `tar`/`cpio`-style copy of the device's
files, but the process that made it quietly dropped several things a kernel needs
to actually boot and run userspace. This chapter is the work of turning that copy
into a disk image a stock malta kernel will `init` from — and, just as usefully,
*how each missing piece announced itself* so you can recognise it next time. All
of it is scripted in [`build-rootfs.sh`](../../qemu-env/build-rootfs.sh) and
[`init`](../../qemu-env/init).

## Building the image without root

You do not need to be root (or to loop-mount anything) to build a populated
filesystem image. Modern `mke2fs` can populate an image from a directory tree:

```sh
mke2fs -q -F -t ext3 -b 1024 -L meldroot -d "$STAGE" rootfs.ext3 163840
#                              ^^ ext3 (built in)     ^^ source dir   ^^ 160 MB
```

`-d <dir>` copies the tree in as the image is created. For inspecting or
extracting from the image afterwards — including **post-mortem on a crashed
boot**, since the ext3 image persists — `debugfs` reads it without mounting:

```sh
debugfs -R "ls -l /disk/usr/bin" rootfs.ext3
debugfs -R "cat /disk/www/logs/thttpd.log" rootfs.ext3
```

That `debugfs` trick was the single most useful debugging tool in this chapter:
a daemon that dies during boot can't show you its error on a `tmpfs` `/tmp`, but
if it logs to the **ext3 disk**, you read the log from the image after the VM
exits.

## The six things the snapshot lost — and how each showed up

### 1. `/bin/sh` and the busybox applet symlinks

**Symptom:** the kernel mounted root fine, then:

```
Failed to execute /init.  Attempting defaults...
Kernel panic - not syncing: No init found.
```

`/init` is a `#!/bin/sh` script; the kernel couldn't find `/bin/sh`. The snapshot
has the static `/bin/busybox` but **none of the applet symlinks** (`sh`, `mount`,
`ifconfig`, `awk`, …) that are normally created by `busybox --install`. Without
`/bin/sh` the shebang fails, and the kernel then tries `/sbin/init`, `/bin/sh`, …
all missing → "No init found."

**Fix:** recreate the applet symlinks pointing at busybox:

```sh
for a in sh mount umount mkdir ifconfig route udhcpc awk grep sed cat ls \
         sleep ps netstat wget stty ln chmod cp rm ... ; do
  ln -sf busybox "$STAGE/bin/$a"
done
ln -sf /bin/busybox "$STAGE/usr/bin/awk"   # wsagent_start calls /usr/bin/awk
```

### 2. Missing execute bits (root can't run a file with no `x`)

**Symptom:** `/disk/usr/bin/thttpd: Permission denied`, even running as root.

The snapshot stored the binaries mode `0664` — **no execute bit**. A common
misconception is that root can execute anything; in fact, to `execve` a regular
file the kernel requires *at least one* execute bit to be set, even for root.

**Fix:** restore `+x` on everything under the `bin` directories and on scripts:

```sh
find "$STAGE" -type f -path '*bin/*' -exec chmod +x {} +
```

### 3. The glibc dynamic loader and sonames

**Symptom (progression):** first `thttpd: not found` (even though the file
exists), then after fixing the loader, `thttpd: Permission denied`.

`thttpd` is **dynamically linked** against glibc 2.9. Its ELF interpreter is
hard-coded to `/lib/ld.so.1`, and it `NEEDED`s `libc.so.6`, `libm.so.6`,
`libcrypt.so.1`. The snapshot kept only the **versioned** files in `/disk/lib`
(`ld-2.9.so`, `libc-2.9.so`, …) and dropped every **soname symlink** and the
`/lib` directory. "not found" from a shell executing a dynamic binary means the
**interpreter** is missing; "Permission denied" after that meant the loader
itself lacked `+x` (the interpreter is executed, so it needs the bit too).

**Fix:** recreate the sonames, the `/lib/ld.so.1` the ELF asks for, make the
loader executable, and point the runtime at the lib dir:

```sh
( cd "$STAGE/disk/lib"
  ln -sf ld-2.9.so       ld.so.1
  ln -sf libc-2.9.so     libc.so.6
  ln -sf libcrypt-2.9.so libcrypt.so.1
  ln -sf libm-2.9.so     libm.so.6
  # ... libdl, libpthread, libnsl ...
)
mkdir -p "$STAGE/lib"; ln -sf /disk/lib/ld-2.9.so "$STAGE/lib/ld.so.1"
find "$STAGE/disk/lib" -type f -exec chmod +rx {} +
# and in init:  export LD_LIBRARY_PATH=/disk/lib:/lib
```

### 4. `/etc/nsswitch.conf` and the NSS modules

**Symptom:** `thttpd: unknown user - 'root'`.

`thttpd.conf` has `user=root`, so thttpd calls `getpwnam("root")` to drop
privilege. glibc resolves users through **NSS**, which `dlopen`s
`libnss_files.so.2` *by soname at runtime* and reads `/etc/nsswitch.conf` to know
to use it. The snapshot had neither the `libnss_files.so.2` symlink nor an
`nsswitch.conf`, so `getpwnam` returned "unknown user" and thttpd refused to
start — even though `/etc/passwd` contained `root` all along.

**Fix:** create the NSS sonames and a policy file:

```sh
ln -sf libnss_files-2.9.so libnss_files.so.2     # (+ dns, compat)
cat > "$STAGE/etc/nsswitch.conf" <<'NSS'
passwd: files
group:  files
hosts:  files dns
NSS
```

This is how the device's dynamic, glibc-based web server was brought up: not by
touching thttpd at all, but by restoring the NSS plumbing the snapshot stripped.
Only *after* this does thttpd bind `:80` and serve the PHP UI
([chapter 8](08-wsa-sb-and-web.md)).

### 5. Device nodes and the initial console

**Symptom:** `Warning: unable to open an initial console.`

The snapshot's `/dev` has no device nodes (you can't `mknod` them without root,
and they aren't regular files to copy). The kernel tries to open `/dev/console`
for init's stdio and can't.

**Fix:** rely on **devtmpfs** — this kernel auto-populates `/dev` once it's
mounted. The custom init mounts it *first thing* and then reopens its own stdio:

```sh
/bin/busybox mount -t devtmpfs none /dev
exec </dev/console >/dev/console 2>&1
```

The kernel warning is harmless — init still `exec`s — and after the devtmpfs
mount, `/dev/console`, `/dev/ttyS1`, `/dev/urandom` etc. all exist.

### 6. A custom `init` (don't reuse the device's)

The device's real boot is `/etc/inittab` → `rcS`, which mounts yaffs2 from NAND,
pokes `/dev/micro`, runs `actimage`, and other hardware-specific steps that fail
on malta. Rather than fight it, the environment ships a **small purpose-built
`/init`** ([`qemu-env/init`](../../qemu-env/init)) that does only what the demo
needs: mount the pseudo-filesystems, bring up the network, apply the database
redirect, start `sb_mips` and `thttpd`, launch `wsa`, and drop to a shell. It
uses `/bin/busybox <applet>` explicitly for the earliest commands (before `PATH`
and symlinks are guaranteed) and sets `PATH`/`LD_LIBRARY_PATH` for the rest.

> **A smaller trap:** `setsid` and `timeout` are **not** busybox applets in this
> build. An `exec setsid sh` at the end of init failed, init exited, and the
> kernel panicked with *"Attempted to kill init!"*. Use `exec /bin/busybox sh`
> and avoid assuming any particular applet exists — check with
> `qemu-mipsel root/bin/busybox` (its no-arg output lists the applets) or just
> test.

## A note on uClibc LinuxThreads

When diagnosing why `wsa` wasn't progressing ([chapter 7](07-radio-handshake.md)),
`ps` showed it as **four separate PIDs** with the same command line. That is
**uClibc LinuxThreads**, not NPTL: each thread is its own process with its own
PID and a manager thread, rather than tasks sharing one TGID. Practical
consequences while debugging:

- `/proc/<pid>/task/` only lists the one thread — to see all threads you iterate
  the separate PIDs from `ps`.
- Read each thread's blocking point with `cat /proc/<pid>/wchan`
  (`poll_schedule_timeout`, `sys_rt_sigsuspend`, …) and its open files with
  `ls -l /proc/<pid>/fd` — this is how the radio block was located.

## The build, end to end

[`build-rootfs.sh`](../../qemu-env/build-rootfs.sh) does all of the above
deterministically: copy `../root` (minus the redundant nested firmware trees),
overlay `sb_mips` and the custom `init`, fix exec bits, create the busybox applet
symlinks, recreate the glibc + NSS sonames and `/lib/ld.so.1`, write
`nsswitch.conf`, and emit both a cpio initramfs (for reference) and the **ext3
disk image** that is actually booted.

**Tips that generalise:**
- A snapshot loses symlinks, device nodes and sometimes permission bits. Expect
  to recreate `/bin/sh` + applet links, exec bits, library sonames + the ELF
  interpreter path, and NSS config.
- "not found" when executing an existing binary = missing **interpreter**;
  "Permission denied" as root = missing **execute bit** (including on the loader).
- `getpwnam`/`gethostbyname` failures in a glibc binary usually mean missing
  `libnss_*` sonames and/or `nsswitch.conf`.
- Let **devtmpfs** create `/dev`; mount it first in init and reopen `/dev/console`.
- Use **`debugfs -R`** to read logs and files out of the disk image for
  post-mortem debugging without mounting it.

[← a working kernel](05-malta-kernel.md) · [Guide index](README.md) · [Next: the radio handshake →](07-radio-handshake.md)
