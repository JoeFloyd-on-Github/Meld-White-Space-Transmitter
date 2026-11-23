#!/usr/bin/env python3
"""Verify an annotated file reproduces the raw Ghidra code exactly.

Strips all C comments (/* */ and //) and all whitespace from both the raw split
file and the annotated file, then compares the remaining token streams. Prints
OK, or the first point of divergence with context.

usage: check_annotated.py <raw_split.c> <annotated.c>
"""
import re, sys

def strip(src):
    out, i, n = [], 0, len(src)
    while i < n:
        c = src[i]
        if c == '"' or c == "'":            # keep string/char literals intact
            q, j = c, i + 1
            while j < n and src[j] != q:
                j += 2 if src[j] == '\\' else 1
            out.append(src[i:j + 1]); i = j + 1
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2); i = n if j < 0 else j + 2
        elif src.startswith("//", i):
            j = src.find("\n", i); i = n if j < 0 else j
        else:
            out.append(c); i += 1
    return re.sub(r"\s+", "", "".join(out))

raw, ann = strip(open(sys.argv[1]).read()), strip(open(sys.argv[2]).read())
if raw == ann:
    print("OK: code identical (comments/whitespace ignored)")
    sys.exit(0)
k = next((i for i, (a, b) in enumerate(zip(raw, ann)) if a != b), min(len(raw), len(ann)))
print(f"MISMATCH at char {k} (raw len {len(raw)}, annotated len {len(ann)})")
print("raw      :", raw[max(0, k - 80):k + 80])
print("annotated:", ann[max(0, k - 80):k + 80])
sys.exit(1)
