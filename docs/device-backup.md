# Backing up a Meld device: full flash + live filesystem

How to make a complete, verified backup of a running Meld device (MT300,
MT300-AV, or the PB/HDS variants) over the network, with no special tools on the
device beyond what the OEM firmware already ships. The result is a directory on
a file share containing:

- **raw images of every NAND (MTD) partition** — boot loaders, kernels, both
  rootfs slots, both app-fs slots, U-Boot env, bad-block table;
- **live file-level snapshots** of the mounted read-write filesystems (the root
  fs and `/disk`), as gzipped cpio;
- **device metadata** (versions, partition map, mounts, processes) and a record
  of any modifications;
- a **README** and **md5 manifests**.

The capture is **read-only on the device** — it changes nothing — and the unit
keeps running throughout.

> This was written from a backup of the bench MT300 (serial 457). The reference
> output of that run is on the share under
> `backup-mt300-sn457-v1.26-sbmips-spoofed-2026-11-21/`. Helper scripts that
> automate everything below are in [`../tools/backup/`](../tools/backup/).

---

## 0. Prerequisites

- **A root shell on the device.** Meld's firmware runs an unauthenticated
  BusyBox telnet daemon on port 23 that drops straight to root. See
  [`device-access.md`](device-access.md) for finding the device's IP (serial
  console or MAC-OUI scan) and connecting. Everything below runs from that shell.
- **`nc` on both ends.** The device has BusyBox `nc`; your host needs any `nc`.
- **A destination with room.** A full MT300 backup is ~545 MB (two 214 MiB app
  partitions dominate). Put it on a file share or local disk on your host, *not*
  on the device (the device has nowhere near enough free space — which is exactly
  why everything is streamed over the network).
- **Check the device's toolbox** before you start:

  ```sh
  for t in nanddump dd nc md5sum gzip tar cpio cat flash_erase; do
    printf '%s: ' $t; which $t 2>/dev/null || echo MISSING
  done
  ```

  On the MT300 you get `dd nc md5sum gzip tar cpio cat flash_erase`, but
  **`nanddump` is MISSING** — this shapes the whole approach (see
  [§6, OOB/ECC](#6-tips-and-pitfalls)).

---

## 1. The transfer pattern: device serves, host pulls

Every stream uses the same idea: the **device** puts data on TCP port 9000 with
`nc`, and your **host** connects and saves it. Work in **two terminals**:

- **Terminal D** — your open telnet session *on the device*.
- **Terminal H** — a normal shell *on your host*, in the backup directory.

For one transfer:

```sh
# Terminal D (device): serve a source on port 9000 (blocks until the host
# connects and the transfer finishes, then returns to the prompt)
nc -l -p 9000 < /dev/mtdblock0
```

```sh
# Terminal H (host): connect and save
nc 192.168.1.194 9000 > mtdblock0-Boot_FW1.img
```

Start the device side first. BusyBox `nc -l` serves exactly one connection, then
both ends close. That's it — no `scp`, no `ssh`, no agent.

> **Scripting note.** If you drive the device non-interactively (telnet closes
> after each command, as the helper scripts do), the listener must be detached
> so it outlives the session: wrap it as `(nc -l -p 9000 < /dev/mtdblock0 &)`.
> In a hand-typed interactive session you don't need that — just run it in the
> foreground as above.

---

## 2. Image every MTD partition

### 2.1 Read the partition map

```sh
cat /proc/mtd
```

On an MT300 (identical A/B layout on all units seen):

```
dev:    size      erasesize  name
mtd0:   00080000  00020000   "Boot_FW1"          512 KiB  boot firmware
mtd1:   00100000  00020000   "Boot_loader_0"       1 MiB  U-Boot (A)
mtd2:   00100000  00020000   "Boot_loader_1"       1 MiB  U-Boot (B)
mtd3:   00080000  00020000   "UBOOT_ENV"         512 KiB  U-Boot environment
mtd4:   00800000  00020000   "Linux_Kernel_0"      8 MiB  kernel (A)
mtd5:   00800000  00020000   "Linux_kernel_1"      8 MiB  kernel (B)
mtd6:   02000000  00020000   "RootFS_0"           32 MiB  rootfs (A)
mtd7:   02000000  00020000   "RootFS_1"           32 MiB  rootfs (B)
mtd8:   0d640000  00020000   "AppFS_0"           214 MiB  /disk app fs (A)
mtd9:   0d640000  00020000   "AppFS_1"           214 MiB  /disk app fs (B)
mtd10:  00080000  00020000   "BBT"               512 KiB  bad-block table
```

### 2.2 Which slot is live

The device has **two** of each image (A/B). Find out which set is running so you
can label it:

```sh
cat /proc/cmdline        # e.g. root=/dev/mtdblock6  -> RootFS_0 is the active rootfs
/bin/actimage; echo $?   # exit code 0 => image 0 active, 1 => image 1
mount                     # shows which mtdblock is mounted at /disk
```

On the reference unit: `root=/dev/mtdblock6`, `actimage` = 0, `/disk` =
`mtdblock8` — so **image 0** (mtd4/6/8) is active and mtd5/7/9 are the spare
slots.

### 2.3 Pull each partition, with verification

Read partitions through the **block** device `/dev/mtdblockN` (the MTD block
layer's view of the flash). For each N:

```sh
# Terminal D (device): note the source hash, then serve it
md5sum /dev/mtdblock0
nc -l -p 9000 < /dev/mtdblock0
```

```sh
# Terminal H (host): pull, then confirm size and hash match the device
nc 192.168.1.194 9000 > mtdblock0-Boot_FW1.img
ls -l mtdblock0-Boot_FW1.img        # size must equal the /proc/mtd size
md5sum mtdblock0-Boot_FW1.img       # must equal the device's md5sum
```

Repeat for `mtdblock1 … mtdblock10`. Name each file
`mtdblockN-<PartitionName>.img`. Record the hashes in `mtd/MD5SUMS.txt`.

**What to expect:** the A/B bootloader images are usually byte-identical to each
other, as are the A/B kernels — equal md5s there are normal. The **active**
rootfs and app-fs may not reproduce their hash (see the live-snapshot pitfall in
§6); everything else should match the device exactly.

---

## 3. Snapshot the live filesystems

The raw rootfs/app images are yaffs2 and awkward to mount off-device. A
file-level cpio is directly browsable and is the authoritative capture of a
**mounted read-write** filesystem. Use `find -xdev` so each archive stays on one
filesystem (it naturally skips `/proc`, `/sys`, `/dev`, and other mounts), and
`cpio -H newc` (the same format as the other dumps on the share). Compress on the
**host** to spare the device's CPU.

```sh
# Terminal D (device): the root filesystem
cd / && find . -xdev | wc -l                 # note the file count (to verify later)
cd / && find . -xdev | cpio -o -H newc | nc -l -p 9000 2>/dev/null
```

Wait — on an interactive shell run the two commands in sequence; the `wc -l`
first so you have the expected count, then the streaming `cpio`.

```sh
# Terminal H (host): receive and gzip
nc 192.168.1.194 9000 | gzip > rootfs-mtdblock6-live.cpio.gz
gzip -t rootfs-mtdblock6-live.cpio.gz && echo "gzip OK"
zcat rootfs-mtdblock6-live.cpio.gz | cpio -t 2>/dev/null | wc -l   # ~= device count
```

Then the same for `/disk`:

```sh
# Terminal D (device)
cd /disk && find . -xdev | wc -l
cd /disk && find . -xdev | cpio -o -H newc | nc -l -p 9000 2>/dev/null
```

```sh
# Terminal H (host)
nc 192.168.1.194 9000 | gzip > disk-mtdblock8-live.cpio.gz
gzip -t disk-mtdblock8-live.cpio.gz && echo "gzip OK"
zcat disk-mtdblock8-live.cpio.gz | cpio -t 2>/dev/null | wc -l
```

A difference of one between the device's `find | wc -l` and the archive's entry
count is just the `.` root entry — not a problem.

---

## 4. Capture metadata and any modifications

Save a plain-text snapshot of the device's state next to the images, so the
backup is self-describing:

```sh
# Terminal D (device) — run and copy the output into meta/device-state.txt
uname -a
cat /proc/cmdline
cat /proc/mtd
mount
df -h
/bin/actimage; echo "actimage rc=$?"
ps
cat /disk/Meld/meldtech/RELEASE_NOTES 2>/dev/null | head -40   # app version
cat /etc/config/fccidSerialInfo                                # FCC ID + serial
cat /etc/config/queryInfo 2>/dev/null                          # radio info dump
grep -iE 'version|date' /etc/config/sbi                        # config version
```

(You can also just `nc`-stream a cpio of `/etc/config` if you want the raw
files.)

**If the device has been modified** — e.g. it is running the `sb_mips` database
stand-in from this project — record exactly what was changed, so no one later
mistakes the backup for pristine firmware:

```sh
ls -l /usr/bin/sb_mips /usr/bin/sb_start ; md5sum /usr/bin/sb_mips /usr/bin/sb_start
cat /etc/hosts                                    # the redirect
grep -n -A2 -B1 sb_start /etc/init.d/rcS          # the boot hook
ls -l /etc/init.d/rcS.orig /disk/hosts.bak        # the backups of the originals
```

Pristine firmware is still available **inside the same backup**: read the
**inactive** rootfs slot image (`mtdblock7-RootFS_1.img` when image 0 is active),
which the modifications never touched.

---

## 5. Lay it out, write the README, verify

### Directory naming

Use a name that states the model, serial, software version, whether it is
modified, and the date — matching the share's convention
(`cpio-mt-300av-sn677…`):

```
backup-<model>-sn<serial>-<appversion>[-<modnote>]-<YYYY-MM-DD>/
  README.txt
  mtd/            mtdblockN-<Name>.img …   + MD5SUMS.txt
  rootfs-live/    rootfs-…cpio.gz, disk-…cpio.gz   + MD5SUMS.txt
  meta/           device-state.txt, modifications/…
```

e.g. `backup-mt300-sn457-v1.26-sbmips-spoofed-2026-11-21/`.

### Checksums and final read-back

```sh
(cd mtd && md5sum mtdblock*.img > MD5SUMS.txt)
(cd rootfs-live && md5sum *.cpio.gz > MD5SUMS.txt)
```

If the destination is a **CIFS/SMB share, read the files back and re-hash** at
least the large ones (see the CIFS pitfall in §6):

```sh
cd mtd && md5sum -c MD5SUMS.txt      # re-reads every image off the share
```

### README

Write a `README.txt` that states, at minimum:

- model, serial, FCC ID;
- the **software version** (Meld app version/date, AV app, kernel, radio
  firmware) and **which boot image was active**;
- **whether the device was modified**, and exactly how;
- how it was captured (nc streaming; block-level images; **no OOB** because no
  `nanddump`);
- the contents and the md5 manifests;
- the **live-snapshot caveat** for the active rootfs image;
- a **sensitivity note** if nothing was redacted — a Meld unit's
  `/etc/config/sbi` holds the registered owner's name, contact, address and GPS.

A filled-in example is the `README.txt` in the reference backup on the share;
the helper script writes one automatically.

---

## 6. Tips and pitfalls

These are the traps hit while developing this procedure. Read them before you
start.

- **The active rootfs image has an unstable hash.** `mtdblockN` for the *mounted,
  read-write* root (and sometimes `/disk`) is a **moving target**: the running
  system writes to it (logs, atime, config, flash GC/wear-levelling), so a second
  `md5sum /dev/mtdblock6` gives a *different* hash than the first. The raw image
  you capture is a valid point-in-time **snapshot**, but its md5 is not
  reproducible — don't treat a mismatch there as corruption. For a consistent
  file-level view of the running root, rely on the **cpio** from §3. Inactive
  slots and quiet partitions verify byte-for-byte.
  - Quick test on the device: `md5sum /dev/mtdblock6; sleep 2; md5sum /dev/mtdblock6`
    — if they differ, it's live. `sync` first to reduce (not eliminate) churn.

- **CIFS/SMB can silently truncate.** A `soft`-mounted SMB share has, in this
  project's history, silently cut a copy short. **Always** check the received
  size against `/proc/mtd`, compare md5s device-vs-host, and for large files
  `md5sum -c` the manifest afterwards (that re-reads them off the share). The
  stream itself is reliable; the destination filesystem is the risk.

- **No `nanddump` → no OOB/ECC.** These are **block-level** images (the flash as
  the MTD block layer presents it), *not* raw NAND with the spare/out-of-band
  area. That's fine for firmware archival and matches the share's existing
  `…-mtdblocks` dump, but it means you cannot reconstruct ECC or inspect the OOB.
  If you ever need that, you'd have to get `nanddump`/`mtd-utils` onto the device
  (not present in stock firmware).

- **Detaching the listener when scripting.** BusyBox `nc -l` run from a telnet
  session that then closes will be killed with the session. Non-interactive
  drivers must background it in a subshell — `(nc -l -p 9000 < src &)` — which
  survives the session close. Interactive (hand-typed) sessions don't need this.

- **One transfer at a time — and verify the byte count.** Everything uses port
  9000, so each `nc` must finish before the next starts. The real hazard is a
  **lingering listener**: if a partition's device-side `nc -l` does not exit
  cleanly, the next partition's host `nc` can connect to the *stale* listener and
  receive empty or wrong-partition data. This bit unit 677 once — `mtd9` came back
  **empty** and `mtd10` held `mtd9`'s 221 MB. `backup-meld.sh` now guards against
  it: before each pull it runs `killall nc` on the device to clear any stale
  listener, then **checks the received byte count against the partition's
  `/proc/mtd` size and retries** (up to 3×) on a short read, flagging
  `SIZE MISMATCH` if it still can't. If you pull by hand, do the same — one
  partition at a time, and confirm the received size before moving on. (To fix one
  bad partition after the fact: `killall nc` on the device, start a fresh
  `(nc -l -p 9000 < /dev/mtdblockN &)`, pull it, and check the size.)

- **Pulling the device-side md5 out of automated output.** `md5sum /dev/mtdblockN`
  and the *command echo* both contain the string `/dev/mtdblock`. If you grep for
  that substring you'll grab the echo line by mistake. Match the hash line
  instead — a 32-hex field: `awk '$1 ~ /^[0-9a-f]+$/ && length($1)==32 {print $1}'`.

- **Order the big partitions last.** `AppFS_0/1` are 214 MiB each and dominate
  the time (minutes over 100 Mbit/s through SMB). Do the small ones first so a
  stall on a big one doesn't block the quick wins. Give host-side `nc` a generous
  idle timeout (`nc -w 25`).

- **Telnet eats control characters / line endings.** The BusyBox login shell is
  raw; if you automate it, send `\r\n`, disable echo (`stty -echo`), and strip
  telnet IAC negotiation. The helper `meld_telnet.py` does all of this.

- **Don't confuse the backup with a restore.** This procedure only **reads**
  (`cat`, `find`, `md5sum`). Writing an image back (`flash_erase` + `nandwrite`,
  or a U-Boot `nand write`) is a separate, dangerous operation that can brick the
  unit, especially across the A/B slots — out of scope here. If you ever restore,
  write to the **inactive** slot and switch to it, never over the running one.

---

## 7. Helper scripts

[`../tools/backup/`](../tools/backup/) automates all of the above:

| Script | Role |
|---|---|
| `meld_telnet.py` | run commands on the device over its passwordless telnet (handles IAC, echo, prompts) |
| `backup-meld.sh` | the full backup: reads `/proc/mtd`, pulls every partition with md5 verification, streams the live rootfs and `/disk`, captures metadata, writes `MD5SUMS.txt` and a `README.txt` |

Usage:

```sh
tools/backup/backup-meld.sh 192.168.1.194 /mnt/RAID2/Meld/spectrumbridge
```

The script is just the commands in this document, wired together with the same
verification. Read it before running it; it is deliberately small so you can
follow exactly what it does on your device.

---

## See also

- [`device-access.md`](device-access.md) — getting the shell and finding the IP
- [`on-device-spoof.md`](on-device-spoof.md) — the sb_mips modification, if the
  unit you're backing up has it
- [`boot-process.md`](boot-process.md) — what each partition is and how the
  device boots from the A/B slots
