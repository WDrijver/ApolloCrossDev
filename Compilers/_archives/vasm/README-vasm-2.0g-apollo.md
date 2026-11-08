# vasm 2.0g for Apollo 68080: base and patches

From Renaud Schweingruber (TuKo1982), 2026-11-08. This is the vasm I use in my
GCC-6.50-Dev toolchain. ApolloCrossDev `main` still builds 2.0f.

## Contents

| File | What it is |
|---|---|
| `vasm2_0g-daily-20260903.tar.gz` | Unmodified vasm 2.0g daily snapshot of 2026-09-03 (m68k backend 2.8d) |
| `patches/vasm2_0g-apollo-pcdest.patch` | Fix 1 |
| `patches/vasm2_0g-apollo-subflags-moveq.patch` | Fixes 2 and 3, apply after fix 1 |
| `scripts/GCC-6.50-Dev-scripts.patch` | Change to `GCC-6.50-Dev-Linux.sh` and `GCC-6.50-Dev-MacOS.sh`, against `main` at 3311406 |
| `check-vasm.sh` | Assembles small cases and compares the bytes |

The archive stays separate from the patches because the vasm licence allows redistribution
"without modifications". For the same reason there is no patched source tree or binary here.
The snapshot comes from the Wayback Machine copy of
`sun.hasenbraten.de/vasm/daily/vasm.tar.gz`, as the official server was unavailable.
SHA-1 `ffea7742f02430aa85f18a8439a2f32ab72b548c`, which matches the archive record.

## The fixes

All three only change `-m68080` output and are in `cpus/m68k/cpu.c`.

**1. `-m68080` made PC-relative destinations by default.**
`move.x <src>,label` became `move.x <src>,label(pc)`, also for addq/subq, bchg/bclr/bset, eor,
fmove and AMMX stores. The PRM68080 says a PC-relative destination is not calculated correctly
after some extended source formats. In my ApolloFloppy this printed garbage and corrupted memory.
The option meant for this is `OPT OA+`, off by default, but `APOLLO_PCDEST()` misses operand
types DN, MA, MI, BS and CT, so those fell through to the normal PC optimisation.
With the patch they need `OPT OA+`, which still works.

**2. `sub.l #w,<ea>` and `subi.l #w,<ea>` became `addiw.l #-w,<ea>`, which inverts C and X.**
Measured on a V4 (core 12847), 2000 - 1000 gives C=0 with `sub` and C=1 with `addiw.l #-1000`.
Any code that reads C or X after such a `sub` goes wrong. With the patch `sub` stays `sub`.
`add.l #w` still becomes `addiw.l`, as its flags are the same.

**3. `move.l #-128..127,Dn` did not become `moveq`.**
The Apollo `move` entry is a separate table entry, so the `OC_MOVE` test never matched it.
The patch enables the plain `moveq` form only: same flags, X kept. The other rewrites in that
branch stay off for Apollo, because `moveq` + `add.w` would change X.

| Source (`-m68080`) | Stock 2.0g | Patched |
|---|---|---|
| `move.b (table.l,d0.w),dest` | `15F0 ...` (PC) | `13F0 ...` (abs.l) |
| `addq.l #1,dest` | `52BA` (PC) | `52B9` (abs.l) |
| `sub.l #1000,d1` | `06C1 FC18` | `92BC 0000 03E8` |
| `subi.l #1000,d1` | `06C1 FC18` | `0481 0000 03E8` |
| `move.l #5,d0` | `203C 0000 0005` | `7005` |

**Side effect:** a `sub.l #w` is now 6 bytes instead of 4, and a label destination is abs.l
instead of (d16,PC), so existing `-m68080` asm projects can change size and branch offsets.
GCC output is not affected: GCC assembles with `m68k-amigaos-as`.

## Build by hand

```sh
tar -xzf vasm2_0g-daily-20260903.tar.gz
cd vasm
patch -p1 < ../patches/vasm2_0g-apollo-pcdest.patch
patch -p1 < ../patches/vasm2_0g-apollo-subflags-moveq.patch
make CPU=m68k SYNTAX=mot
cd ..
./check-vasm.sh vasm/vasmm68k_mot     # ends with ALL PASS, stock 2.0g fails 6 cases
```

## Put it in ApolloCrossDev

From the repository root, with the zip extracted to `$ZIP`:

```sh
cp $ZIP/vasm2_0g-daily-20260903.tar.gz $ZIP/patches/*.patch Compilers/_archives/vasm/
git apply $ZIP/scripts/GCC-6.50-Dev-scripts.patch
```

The scripts now start with `rm -rf vasm`, and this matters. `main` tracks
`Compilers/_archives/vasm/vasm/` with its Mach-O `.o` files and binaries. Those are newer than
the sources extracted from the 2.0g archive, so `make` keeps them. On Linux the link then stops
with `file format not recognized`. You may want to remove that tree from git as well.

Separately, `tar -xzf vasm.tar` fails on Linux with GNU tar, because `vasm.tar` is not gzipped.
The unchanged Linux script then builds the tracked tree instead. The script patch removes that
line.

## What was tested

- On Linux x86-64, on a fresh clone of `main` at 3311406: script patch applied, then the vasm
  section of `GCC-6.50-Dev-Linux.sh` run as it stands, and `check-vasm.sh` gives ALL PASS.
- The MacOS script has the same change but was not run: I have no Mac.
- `check-vasm.sh` on stock 2.0g: the 6 fix cases fail, the 5 control cases pass.
- Patched against stock on 10 real sources with `-m68020`: byte-identical.

Not reported to Frank Wille yet.
