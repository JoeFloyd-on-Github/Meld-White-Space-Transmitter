# OEM hardware-control binary analysis

Decompiled and annotated source for the Meld MT300's **OEM hardware-control
binaries** — the device daemons and tools that run below `whitespaceagent` and
handle the power/reset button, the A/B boot image, NAND flashing, kernel
hotplug events, the ViXS video front-end, and the secure keystore.

The narrative that ties these together — the full boot sequence and the answers
to "how is the reset button monitored / what happens on reset / how is the IP
assigned / how is the radio tied to the WSA / how is the hardware configured" —
is in [`../docs/boot-process.md`](../docs/boot-process.md). This directory is the
binary analysis those answers rest on.

## Files

Each binary has a `.decompiled.c` (raw Ghidra output) and a `.annotated.c`
(the same, with libc calls resolved and every function commented).

| Binary | Source of | What it does |
|---|---|---|
| `pm_daemon` | `root/bin/pm_daemon` | **power/reset button + IR remote** → `/sys/power/state` |
| `actimage` | `root/bin/actimage` | **A/B boot image** select via the U-Boot NAND env |
| `mtd_char_op` | `root/bin/mtd_char_op` | raw **NAND flash** read/write/erase/dump |
| `uevent_daemon` | `root/disk/usr/bin/uevent_daemon` | kernel **hotplug** → app notification broker |
| `uevent_client` | `root/disk/usr/bin/uevent_client` | hotplug client (from `blkdev_detect.sh`) |
| `boxdiag` | `root/bin/boxdiag` | **video front-end** (ADV7185/SAA711x) init via the ViXS driver |
| `load_keys_good` | `root/bin/load_keys_good` | **secure keystore** key loading via the ViXS driver (app subset) |

## How these were produced

- Imported each binary into **Ghidra 12.2** headless and decompiled every
  function (`DecompAll.java`).
- In the `.annotated.c` files, the **libc PLT stubs** that Ghidra left as
  `FUN_<address>` were renamed to their real names, resolved from each ELF's
  `.rel.plt` relocation table (so `FUN_00400ba0` → `open`, etc.). Referenced
  `.rodata` constants were substituted in where useful, and a **purpose comment
  was added above every function**.
- **Function names are original symbols** for the binaries that ship with a
  symbol table / DWARF (`pm_daemon`, `actimage`, `mtd_char_op`, the uevent pair,
  `load_keys_good`); **`boxdiag` is stripped**, so only its libc stubs carry
  names and its own routines appear as `FUN_<address>` — its hardware intent is
  read from the embedded strings. Local variables keep Ghidra's auto-generated
  names throughout; the naming legend is in
  [`../WhiteSpaceApp/ghidra/README.md`](../WhiteSpaceApp/ghidra/README.md).

### Scope notes

- `pm_daemon`, `mtd_char_op`, `uevent_daemon`, `uevent_client`, `boxdiag` are
  **dynamically linked**, so their `.decompiled.c` already contains only the
  program's own functions (libc lives in `libc.so`).
- `actimage` is **statically linked**; its `.decompiled.c`/`.annotated.c`
  include the bundled glibc functions as well (annotated with a default note) —
  only `actimage`'s own routines carry real commentary.
- `load_keys_good` is **~2.7 MB, statically linked, 849 functions**, almost all
  of them standard glibc. Including a full re-decompile of glibc would add ~90k
  lines of noise, so the committed files (`load_keys_good.app.*`) contain **only
  its 17 application functions** — the OEM keystore/GPIO/xcode-driver code —
  exactly as the `WhiteSpaceApp/ghidra` analysis omits libc.

Nothing here was produced by running the binaries; this is static analysis only.

## Key findings (see `../docs/boot-process.md` for detail)

- **Reset/power button:** the physical button is sensed by a **box
  microcontroller** that pushes a one-byte event to the char device
  `/dev/micro`. `pm_daemon` `select()`s on it (`event_loop`) and on a press
  (`micro_evt_handler`, "Power switch is pressed. (reason %c)") calls
  `mode_change`, which `sync()`s and drives `/sys/power/state`. An IR remote
  POWER key does the same, but is disabled on the MT300 (no `-f` config).
- **Boot image:** `actimage` reads/sets `current_active` in the **U-Boot NAND
  environment** (`/dev/mtd3`) to choose between two application images
  (`mtdblock8`/`mtdblock9`) — a failsafe A/B upgrade scheme.
- **Hardware:** the SoC is a **ViXS XCode** transcoder; `boxdiag` initializes
  the ADV7185/SAA711x video decoders and `load_keys_good` loads content-
  protection keys into the SoC keystore, both via `/dev/vixs/xcodedrv`.
