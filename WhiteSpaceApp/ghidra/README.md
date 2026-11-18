# Ghidra decompilation of `wsa`

This directory holds the decompiled source of the `wsa` binary and an annotated
copy that explains it.

| File | What it is |
|---|---|
| `wsa_decompiled.c` | **Raw** Ghidra 12.2 output for all 127 application + ezxml functions, unedited. Reference/diff only. |
| `wsa_annotated.c` | The **application** functions (not ezxml internals) with my comments added: a header per function and inline notes mapping Ghidra's auto-named locals to what they actually hold. |
| `README.md` | This file: how the decompile was produced, the variable-naming legend, and how the program uses the ezxml XML library. |

The companion document `docs/wsa-overview.md` (repo root) describes the runtime
behavior end to end. This directory is about the *code itself*.

Nothing here was produced by running the binary; it is static analysis only.

## How it was generated

- `WhiteSpaceApp/wsa` is ELF32 MIPS32r2, little-endian, statically linked,
  **not stripped and compiled with full DWARF** (`-g`, GCC 4.3.2, uClibc).
- Imported into Ghidra 12.2 headless with auto-analysis, then the decompiler
  was run over every function in `0x400290`..`0x40f800` (the application code
  plus the bundled ezxml parser). The statically linked libc and OpenSSL code
  after that range is not decompiled here.
- Because the binary carries DWARF, **function names, parameter names, and the
  application struct/enum types are the original symbols**, not Ghidra
  inventions. The `WHITESPACE_CONTEXT_STRUCT`, the `WS_STATE_*` / `WS_EVENT_*`
  enums, the `SBI`/`WS_*` status codes, and the source file names all come
  straight from the debug info.

## Reading Ghidra's local variable names

Local variables were generally **not** in the debug info, so Ghidra generated
names from each variable's storage and inferred type. Once you know the scheme
the "cryptic" names are quite readable:

| Pattern | Meaning | Example |
|---|---|---|
| `iVar1`, `uVar3` | an `int` / `unsigned` temporary, numbered in order of appearance | return values, loop scratch |
| `pcVar2`, `peVar1` | a **p**ointer to `char` / to an `ezxml_t` | `pcVar2 = wsCtx->receiveBuffer` |
| `sVar1` | a `size_t` (usually a `strlen`/`write` result) | `sVar1 = strlen(...)` |
| `local_10`, `local_18` | a stack variable; the number is its **negative stack offset in hex** (so `local_18` sits below `local_10`) | `local_10` is very often the function's return value |
| `acStack_90` | an **a**rray of **c**har on the **Stack** at offset `0x90`; `auStack_` is an array of `undefined`/bytes | `acStack_90 [100]` = a 100-byte buffer |
| `fStack_98` | an `fd_set` on the stack (used by every `select()` loop) | socket/serial readiness |
| `in_f0_1`, `in_v0` | a value Ghidra believes arrives in a register (`$f0`, `$v0`) but whose definition it could not see | often a decompiler artifact |
| `DAT_0051a0a8`, `&DAT_...` | a constant in `.rodata`/`.data` Ghidra did not name; usually a short literal (`"e1\r"`, `","`, a double) | serial command bytes |
| `name::var` | a `static` local (file/function scope), e.g. `processWS...::httpFailureCount` | persists across calls |

`undefined`/`undefined1`/`undefined2` are just Ghidra's "I know the size but not
the type" placeholders (1/2-byte etc.). `ezxml_t_conflict` is Ghidra's name for
`ezxml_t` — the "conflict" suffix only means the type name appeared in more than
one compilation unit; it is the same ezxml node type.

The annotated source resolves the names that matter in each function, so you
should rarely need this table, but it is here for the raw file.

## The ezxml XML library

`wsa` parses XML with **ezxml**, a small single-file public-domain parser
(`ezxml.c`/`ezxml.h`, by Aaron Voisine). It builds an in-memory tree of nodes
and is used in two places: reading the local device-info file
(`readXmlIntoContext`) and parsing the channel-list response from the database
server (`processXmlResponseMessage`).

### The node type

Every element is an `ezxml_t` (a pointer to `struct ezxml`). The fields the
program actually touches:

| Field | Meaning |
|---|---|
| `->name` | tag name (e.g. `"ChannelList"`) |
| `->txt` | the element's text content as a NUL-terminated C string. **This is what the code reads almost everywhere** — e.g. `peVar1->txt`. For an empty/self-closing element it is `""`, never NULL. |
| `->child` | first nested child element |
| `->sibling` / `->next` | next element with the same / with any tag at this level |
| `->attr` | `NULL`-terminated array of alternating name/value attribute strings (wsa never reads attributes) |

### The API calls wsa uses

| Call | What it does | Where |
|---|---|---|
| `ezxml_parse_file(path)` | read a whole file, return the **root** node (or NULL on failure) | `readXmlIntoContext` |
| `ezxml_parse_str(buf, len)` | parse an in-memory buffer; **skips any bytes before the first `<`**, which is why the raw HTTP response (status line + headers, then the XML body) can be handed to it directly | `processXmlResponseMessage` |
| `ezxml_child(parent, "Tag")` | return the first **direct child** of `parent` whose tag is `"Tag"`, or NULL | walking the device-info tree |
| `ezxml_get(xml, ...)` | variadic path walk: alternating `"Tag", index` pairs descend the tree; called here as `ezxml_get(xml)` with no path, which just returns the node passed in. *(The exact variadic arguments are not visible in the decompile; this is inferred from the ezxml API and the surrounding code.)* | response parsing |
| `ezxml_free(xml)` | free the whole tree. Must be called on the **root** returned by parse; freeing is done once per parse | both |

### How the program leans on it

- **Only direct children are read.** `readXmlIntoContext` calls `ezxml_child` on
  the root for each top-level tag and, for the nested `<Location>` and
  `<TvwsDeviceInfo>` blocks, calls `ezxml_child` again on that sub-node. There is
  no XPath and no recursion; the tag tables (`responseTags`, `locationTags`,
  `tvwsDeviceTags`) are walked by index.
- **A missing element is treated as empty, not an error.** The pattern
  everywhere is `peVar = ezxml_child(...); text = peVar ? peVar->txt : "";`.
  So an absent tag silently yields `""` (or `0` after `atoi`).
- **Text is copied out with `strcpy`/`atoi` immediately** into fixed context
  buffers, and non-numeric values are re-XML-escaped by
  `postProcessDataForEscapeSequence` before storage. ezxml owns the parsed
  strings only until `ezxml_free`, so everything needed is copied first.
- **The response parser does not validate structure deeply.** It pulls
  `ChannelCount`, `ChannelList`, `RefreshIn`, then cross-checks the count
  against the distinct channel numbers it parsed. A missing `ChannelList`
  dereferences NULL (see `docs/wsa-overview.md` §5.5).

The ezxml internals (`ezxml_parse_str`, `ezxml_decode`, `ezxml_open_tag`, the
DTD/entity handling, etc.) are standard upstream ezxml and are included in
`wsa_decompiled.c` for completeness but are **not** annotated here — they are
third-party library code, not part of the Meld application logic.
