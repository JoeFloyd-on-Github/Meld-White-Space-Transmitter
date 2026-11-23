#!/usr/bin/env python3
"""Split av_streamer_decompiled.c into one file per DWARF compilation unit.

Uses .debug_aranges (CU address ranges) + the CU DW_AT_name to bucket every
'// ==== name @ addr ====' block. Functions outside any CU range (CRT, PLT
thunks, libgcc) go to 'nocu'. Writes ghidra/split/<NN>_<cu>.c and an index.
"""
import re, subprocess, sys, os, collections

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
BIN = os.path.join(ROOT, "extract/Meld/AV30/test_script/av_streamer")
DEC = os.path.join(ROOT, "ghidra/av_streamer_decompiled.c")
OUT = os.path.join(ROOT, "ghidra/split")

def sh(*a):
    return subprocess.run(a, capture_output=True, text=True).stdout

# CU offset -> name
info = sh("readelf", "--debug-dump=info", BIN)
cu_name, cur = {}, None
for line in info.splitlines():
    m = re.match(r"\s*Compilation Unit @ offset 0x([0-9a-f]+):", line)
    if m:
        cur = int(m.group(1), 16); continue
    if cur is not None and cur not in cu_name:
        m = re.search(r"DW_AT_name\s*:\s*(?:\(.*?\):\s*)?(\S+)", line)
        if m:
            cu_name[cur] = m.group(1)

# aranges: debug_info offset (points at CU header) -> [(lo, hi)]
ranges = []
ar = sh("readelf", "--debug-dump=aranges", BIN)
off = None
for line in ar.splitlines():
    m = re.search(r"Offset into .debug_info:\s+0x([0-9a-f]+)", line)
    if m:
        off = int(m.group(1), 16); continue
    m = re.match(r"\s+([0-9a-f]{8}) ([0-9a-f]{8})$", line)
    if m and off is not None:
        lo, ln = int(m.group(1), 16), int(m.group(2), 16)
        if ln:
            ranges.append((lo, lo + ln, cu_name.get(off, hex(off))))
ranges.sort()

def cu_for(addr):
    for lo, hi, n in ranges:
        if lo <= addr < hi:
            return n
    return "nocu"

text = open(DEC).read()
blocks = re.split(r"(?m)^(?=// ==== )", text)
buckets = collections.OrderedDict((n, []) for _, _, n in ranges)
buckets["nocu"] = []
for b in blocks:
    m = re.match(r"// ==== (.+?) @ ([0-9a-f]+) ====", b)
    if not m:
        continue
    buckets[cu_for(int(m.group(2), 16))].append((m.group(1), m.group(2), b))

os.makedirs(OUT, exist_ok=True)
with open(os.path.join(OUT, "INDEX.txt"), "w") as idx:
    for i, (n, fs) in enumerate(buckets.items()):
        safe = n.replace("/", "_")
        fn = f"{i:02d}_{safe}.c"
        with open(os.path.join(OUT, fn), "w") as f:
            for _, _, b in fs:
                f.write(b)
        lines = sum(b.count("\n") for _, _, b in fs)
        idx.write(f"{fn}\t{len(fs)} funcs\t{lines} lines\t" +
                  " ".join(x for x, _, _ in fs) + "\n")
print(open(os.path.join(OUT, "INDEX.txt")).read())
