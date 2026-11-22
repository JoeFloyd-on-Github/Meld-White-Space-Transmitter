# tools/backup — full-device backup over the network

Scripts that automate [`../../docs/device-backup.md`](../../docs/device-backup.md):
a complete, verified backup of a running Meld device (all NAND partitions + the
live filesystems) streamed over `nc`, using only the stock BusyBox tools on the
device. **Read the doc before using these** — it explains every step and the
pitfalls these scripts guard against.

## Files

| File | Role |
|---|---|
| `meld_telnet.py` | runs commands on the device over its passwordless BusyBox telnet (handles IAC negotiation, echo, prompts). Also useful on its own for poking at a unit. |
| `backup-meld.sh` | the full backup: partition map → every MTD image with md5 verification → live rootfs and `/disk` as cpio.gz → metadata + modification record → `README.txt` and `MD5SUMS.txt`. |

## Use

```sh
# host IP of the device, and the parent dir to drop the backup under
tools/backup/backup-meld.sh 192.168.1.194 /mnt/RAID2/Meld/spectrumbridge
```

It creates `backup-<model>-sn<serial>-v<appver>-<date>/` under the parent and
prints per-partition verification as it goes. A full MT300 backup is ~545 MB and
takes a few minutes (the two 214 MiB app partitions dominate).

Requirements on the host: `bash`, `nc`, `md5sum`, `gzip`, `python3`. The device
must be reachable on telnet 23 (passwordless root, as Meld firmware ships) and
accept an inbound `nc` on port 9000 (override with `PORT=…`).

## Notes

- **Read-only on the device.** The scripts only `cat`/`find`/`md5sum`; nothing is
  written to the unit.
- **The active rootfs image is a live snapshot** — its md5 won't reproduce
  (the running system writes to it). That's flagged, not an error; the
  `rootfs-live/*.cpio.gz` is the consistent file-level capture. See the doc's
  Tips and pitfalls.
- **If the destination is CIFS/SMB**, run the `md5sum -c` the script prints at the
  end — SMB has silently truncated a copy in this project before.
- **Backups are sensitive and are never committed** to this repo: a Meld unit's
  config carries the registered owner's personal details. Keep backups on your
  share only.
