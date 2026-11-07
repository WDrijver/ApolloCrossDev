# exFAT on-disk format

> Source: Microsoft **exFAT file system specification**, revision 1.00
> (<https://learn.microsoft.com/en-us/windows/win32/fileio/exfat-specification>,
> upstream markdown source `MicrosoftDocs/win32`, doc dated 07/08/2025).
> Full text captured locally as `exfat-specification.md` (same folder) —
> that file is authoritative; this one is the working summary.
> Applies to: exFAT major revision 1, any minor revision.

## Summary

Everything needed to read and write exFAT structures on disk: the volume region
layout, boot sector fields, the FAT, the allocation bitmap, the up-case table,
the directory entry types, the four checksum/hash algorithms, and the format
limits. Read this before writing any exFAT parsing or writing code. Section
numbers below refer to the upstream spec so anything can be checked against it.

**All numbers in this document were transcribed from the upstream source, not
from a summary.** Several published third-party summaries of the Stream
Extension, File Directory and Volume Label entries have wrong field offsets —
do not trust any layout that disagrees with the tables here.

## Key rules / constraints

- **Every multi-byte field on disk is little-endian.** The 68080 is big-endian.
  Do NOT overlay a C struct of native `UWORD`/`ULONG` on a directory entry or
  boot sector and read fields directly — every read and write must byte-swap.
  Declare on-disk structures as byte arrays or `le16/le32/le64` wrapper types
  with explicit accessor functions.
- **Sizes and offsets are 64-bit.** `VolumeLength`, `DataLength` and
  `ValidDataLength` are 8 bytes. Cluster arithmetic overflows 32 bits on large
  volumes. The ApolloCrossDev libc `off_t` is 32-bit (`typedef long _off_t`), so
  a project-defined 64-bit offset type is mandatory.
- **Validate the Boot Checksum before trusting any boot-region field**
  (Section 3.4). If the Main Boot region fails, fall back to the Backup Boot
  region at sector 12.
- **Reject unknown major revisions.** Mount only volumes whose
  `FileSystemRevision` high byte is 1 (Section 3.1.12).
- exFAT is **case-insensitive and case-preserving**. Name comparison is done on
  the name up-cased through the volume's own Up-case Table — not through any
  built-in table.
- `NameHash` only ever proves a *mismatch*. Every hash match must still be
  confirmed by comparing the full up-cased name (Section 7.6.4).
- Follow the write ordering in Section 8.1 (below) and maintain `VolumeDirty`,
  or a power loss mid-write leaves an inconsistent volume.
- A cluster can be up to 32 MB. Never size a cluster buffer from an assumption;
  allocate from Fast RAM based on the actual `SectorsPerClusterShift`.

## Details

### Volume structure (Section 2, Table 3)

Offsets in sectors; sector size is `2^BytesPerSectorShift`.

| Sub-region | Offset (sector) | Size (sectors) |
|---|---|---|
| Main Boot Sector | 0 | 1 |
| Main Extended Boot Sectors | 1 | 8 |
| Main OEM Parameters | 9 | 1 |
| Main Reserved | 10 | 1 |
| Main Boot Checksum | 11 | 1 |
| Backup Boot Sector | 12 | 1 |
| Backup Extended Boot Sectors | 13 | 8 |
| Backup OEM Parameters | 21 | 1 |
| Backup Reserved | 22 | 1 |
| Backup Boot Checksum | 23 | 1 |
| FAT Alignment | 24 | `FatOffset - 24` |
| First FAT | `FatOffset` | `FatLength` |
| Second FAT | `FatOffset + FatLength` | `FatLength * (NumberOfFats - 1)` |
| Cluster Heap Alignment | `FatOffset + FatLength * NumberOfFats` | `ClusterHeapOffset - (FatOffset + FatLength * NumberOfFats)` |
| Cluster Heap | `ClusterHeapOffset` | `ClusterCount * 2^SectorsPerClusterShift` |
| Excess Space | `ClusterHeapOffset + ClusterCount * 2^SectorsPerClusterShift` | `VolumeLength - (that)` |

### Main / Backup Boot Sector (Section 3.1, Table 4)

| Field | Offset | Size | Notes |
|---|---|---|---|
| JumpBoot | 0 | 3 | `EB 76 90` |
| FileSystemName | 3 | 8 | `"EXFAT   "` (three trailing spaces) |
| MustBeZero | 11 | 53 | must be 0 — stops FAT12/16/32 drivers mounting it |
| PartitionOffset | 64 | 8 | media-relative sector offset; 0 = ignore |
| VolumeLength | 72 | 8 | volume size in sectors |
| FatOffset | 80 | 4 | ≥ 24 |
| FatLength | 84 | 4 | sectors per FAT |
| ClusterHeapOffset | 88 | 4 | |
| ClusterCount | 92 | 4 | ≤ 2³²−11 |
| FirstClusterOfRootDirectory | 96 | 4 | 2 … `ClusterCount + 1` |
| VolumeSerialNumber | 100 | 4 | |
| FileSystemRevision | 104 | 2 | high byte = major, low byte = minor; 1.00 here |
| VolumeFlags | 106 | 2 | see below |
| BytesPerSectorShift | 108 | 1 | 9…12 (512…4096 bytes) |
| SectorsPerClusterShift | 109 | 1 | 0 … `25 - BytesPerSectorShift` (max cluster 32 MB) |
| NumberOfFats | 110 | 1 | 1, or 2 for TexFAT only |
| DriveSelect | 111 | 1 | INT 13h drive number |
| PercentInUse | 112 | 1 | 0…100, or `FFh` = unknown |
| Reserved | 113 | 7 | |
| BootCode | 120 | 390 | fill with `F4h` if not bootable |
| BootSignature | 510 | 2 | `AA55h` |
| ExcessSpace | 512 | `2^BytesPerSectorShift − 512` | undefined |

`VolumeFlags` (Section 3.1.13, Table 5) — bit 0 `ActiveFat` (0 = first FAT and
first bitmap, 1 = second, only legal when `NumberOfFats == 2`), bit 1
`VolumeDirty`, bit 2 `MediaFailure`, bit 3 `ClearToZero`, bits 4–15 reserved.

Extended Boot Sectors (Section 3.2): `ExtendedBootCode` at 0 for
`2^BytesPerSectorShift − 4` bytes, then `ExtendedBootSignature` = `AA550000h`.

OEM Parameters (Section 3.3): ten 48-byte `Parameters[]` entries at offsets
0, 48, … 432, then reserved. Each begins with a 16-byte `ParametersGuid`;
all-zero GUID = Null Parameters. Flash Parameters GUID is
`{0A0C7E46-3399-4021-90C8-FA6D389C4BA2}`.

### Boot Checksum (Section 3.4, Figure 1)

Computed over the 11 sectors of the boot region (sectors 0–10), skipping byte
offsets 106, 107 and 112 (`VolumeFlags` and `PercentInUse`). The resulting
32-bit value is repeated to fill the whole of sector 11.

```c
UInt32 BootChecksum(UCHAR *Sectors, USHORT BytesPerSector)
{
    UInt32 NumberOfBytes = (UInt32)BytesPerSector * 11;
    UInt32 Checksum = 0;
    UInt32 Index;

    for (Index = 0; Index < NumberOfBytes; Index++)
    {
        if ((Index == 106) || (Index == 107) || (Index == 112))
        {
            continue;
        }
        Checksum = ((Checksum&1) ? 0x80000000 : 0) + (Checksum>>1) + (UInt32)Sectors[Index];
    }
    return Checksum;
}
```

### File Allocation Table (Section 4.1)

32-bit entries. `FatEntry[N]` lives at byte offset `N * 4` within the FAT.

- `FatEntry[0]`: media type in the low byte (should be `F8h`), other three bytes `FFh`.
- `FatEntry[1]`: `FFFFFFFFh`, historical, unused.
- `FatEntry[2] … FatEntry[ClusterCount+1]`: next cluster in the chain
  (2 … `ClusterCount + 1`), or `FFFFFFF7h` = bad cluster, or `FFFFFFFFh` =
  end of chain.

### Cluster addressing (Section 5.1)

The first cluster of the Cluster Heap has **index 2**.

```
sector_of(cluster) = ClusterHeapOffset + (cluster - 2) * 2^SectorsPerClusterShift
byte_of(cluster)   = sector_of(cluster) * 2^BytesPerSectorShift
```

### Allocation Bitmap (Section 7.1.5)

One bit per cluster; `BitmapEntry[2]` is the first cluster of the heap. Bit
value 0 = free, 1 = allocated. **The first bit is the lowest-order bit of the
first byte** — so cluster 2 is bit 0 of byte 0, cluster 9 is bit 7 of byte 0,
cluster 10 is bit 0 of byte 1.

### Up-case Table (Section 7.2)

An array of 2-byte entries: the entry at index *c* holds the up-cased form of
Unicode character *c*. May be stored compressed: the value `FFFFh` followed by
a count *n* means "the next *n* characters map to themselves". **Reading both
compressed and uncompressed forms is mandatory**; writing compressed is
optional. The mappings for the first 128 characters are fixed by the spec and a
table that deviates from them is invalid. A custom table must cover `0000h` to
`FFFFh`. `TableChecksum` covers the table exactly as stored on the volume
(compressed or not). The recommended table has checksum `E619D30Dh`.

```c
UInt32 TableChecksum(UCHAR *Table, UInt64 DataLength)
{
    UInt32 Checksum = 0;
    UInt64 Index;

    for (Index = 0; Index < DataLength; Index++)
    {
        Checksum = ((Checksum&1) ? 0x80000000 : 0) + (Checksum>>1) + (UInt32)Table[Index];
    }
    return Checksum;
}
```

### Directory entries (Section 6)

Every directory entry is **32 bytes**. `EntryType` is byte 0:

| Value | Meaning |
|---|---|
| `00h` | End of directory. Every subsequent entry is also end-of-directory. |
| `01h`–`7Fh` | Unused entry (`InUse` = 0). Contents undefined; may be overwritten. |
| `80h` | **Invalid** — reserved so that clearing `InUse` can never fabricate an end-of-directory marker. |
| `81h`–`FFh` | Regular entry (`InUse` = 1). Only this range is valid inside an entry set. |

`EntryType` bit fields (Table 15): bits 0–4 `TypeCode`, bit 5 `TypeImportance`
(0 = critical, 1 = benign), bit 6 `TypeCategory` (0 = primary, 1 = secondary),
bit 7 `InUse`. Deleting an entry = clearing bit 7 (so `85h` → `05h`).

Generic **primary** template (Table 16): `EntryType` 0/1, `SecondaryCount` 1/1,
`SetChecksum` 2/2, `GeneralPrimaryFlags` 4/2, `CustomDefined` 6/14,
`FirstCluster` 20/4, `DataLength` 24/8.

Generic **secondary** template (Table 18): `EntryType` 0/1,
`GeneralSecondaryFlags` 1/1, `CustomDefined` 2/18, `FirstCluster` 20/4,
`DataLength` 24/8.

`GeneralPrimaryFlags` / `GeneralSecondaryFlags`: bit 0 `AllocationPossible`,
bit 1 `NoFatChain`, remainder custom/reserved.

- `AllocationPossible` = 0 → `FirstCluster` and `DataLength` are undefined.
- `NoFatChain` = 1 → the allocation is one **contiguous** run of clusters and
  the FAT entries for it are invalid and must not be followed; the run length is
  `ceil(DataLength / (2^SectorsPerClusterShift * 2^BytesPerSectorShift))`.
  When `NoFatChain` = 1, `FirstCluster` must be valid and `DataLength` non-zero.
- `FirstCluster` = 0 means no allocation, and then `DataLength` must be 0 and
  `NoFatChain` must be 0.

The **root directory** is always described by a FAT cluster chain starting at
`FirstClusterOfRootDirectory`, as if `NoFatChain` were 0; its length comes from
walking the chain, not from any `DataLength`.

#### Entry set checksum (Section 6.3.3, Figure 2)

Covers the whole entry set — primary plus `SecondaryCount` secondaries —
skipping bytes 2 and 3 (the `SetChecksum` field itself).

```c
UInt16 EntrySetChecksum(UCHAR *Entries, UCHAR SecondaryCount)
{
    UInt16 NumberOfBytes = ((UInt16)SecondaryCount + 1) * 32;
    UInt16 Checksum = 0;
    UInt16 Index;

    for (Index = 0; Index < NumberOfBytes; Index++)
    {
        if ((Index == 2) || (Index == 3))
        {
            continue;
        }
        Checksum = ((Checksum&1) ? 0x8000 : 0) + (Checksum>>1) + (UInt16)Entries[Index];
    }
    return Checksum;
}
```

#### Allocation Bitmap entry — `81h` (Section 7.1, Table 20)

`EntryType` 0/1, `BitmapFlags` 1/1, `Reserved` 2/18, `FirstCluster` 20/4,
`DataLength` 24/8. `BitmapFlags` bit 0 `BitmapIdentifier`: 0 = First Allocation
Bitmap, 1 = Second (only with `NumberOfFats` = 2). Critical primary, in the root
directory.

#### Up-case Table entry — `82h` (Section 7.2, Table 23)

`EntryType` 0/1, `Reserved1` 1/3, `TableChecksum` 4/4, `Reserved2` 8/12,
`FirstCluster` 20/4, `DataLength` 24/8. Critical primary, in the root directory;
exactly one required.

#### Volume Label entry — `83h` (Section 7.3, Table 26)

`EntryType` 0/1, `CharacterCount` 1/1, `VolumeLabel` 2/22, `Reserved` 24/8.
`CharacterCount` is 0…**11** Unicode characters. Zero or one such entry, in the
root directory. Same invalid characters as `FileName`.

#### File entry — `85h` (Section 7.4, Table 27)

Critical primary. Must be immediately followed by exactly one Stream Extension
entry and at least one File Name entry.

| Field | Offset | Size |
|---|---|---|
| EntryType | 0 | 1 |
| SecondaryCount | 1 | 1 |
| SetChecksum | 2 | 2 |
| FileAttributes | 4 | 2 |
| Reserved1 | 6 | 2 |
| CreateTimestamp | 8 | 4 |
| LastModifiedTimestamp | 12 | 4 |
| LastAccessedTimestamp | 16 | 4 |
| Create10msIncrement | 20 | 1 |
| LastModified10msIncrement | 21 | 1 |
| CreateUtcOffset | 22 | 1 |
| LastModifiedUtcOffset | 23 | 1 |
| LastAccessedUtcOffset | 24 | 1 |
| Reserved2 | 25 | 7 |

`FileAttributes` (Table 28): bit 0 `ReadOnly`, bit 1 `Hidden`, bit 2 `System`,
bit 3 reserved, bit 4 `Directory`, bit 5 `Archive`, bits 6–15 reserved.

#### Stream Extension entry — `C0h` (Section 7.6, Table 33)

Critical secondary. Exactly one per file entry set, immediately after the `85h`
entry.

| Field | Offset | Size |
|---|---|---|
| EntryType | 0 | 1 |
| GeneralSecondaryFlags | 1 | 1 |
| Reserved1 | 2 | 1 |
| NameLength | 3 | 1 |
| NameHash | 4 | 2 |
| Reserved2 | 6 | 2 |
| ValidDataLength | 8 | 8 |
| Reserved3 | 16 | 4 |
| FirstCluster | 20 | 4 |
| DataLength | 24 | 8 |

- `AllocationPossible` must be 1 for this entry.
- `NameLength`: 1…255 Unicode characters.
- `ValidDataLength`: how far real user data has been written. Data between
  `ValidDataLength` and `DataLength` is undefined on the media — **reads beyond
  `ValidDataLength` must return zeroes.** For a directory, `ValidDataLength`
  must equal `DataLength`.
- For a directory, `DataLength` is the whole allocation size and is capped at
  256 MB.

#### File Name entry — `C1h` (Section 7.7, Table 34)

`EntryType` 0/1, `GeneralSecondaryFlags` 1/1, `FileName` 2/30.

15 UTF-16 characters per entry; `ceil(NameLength / 15)` entries, up to 17, so
the maximum name is 255 characters. They must immediately follow the Stream
Extension entry as a consecutive run. Unused character slots should be `0000h`.
Names within one directory must be unique **after up-casing**.

Invalid characters (Table 35): `0000h`–`001Fh` (all control codes), plus
`"` (`0022h`), `*` (`002Ah`), `/` (`002Fh`), `:` (`003Ah`), `<` (`003Ch`),
`>` (`003Eh`), `?` (`003Fh`), `\` (`005Ch`), `|` (`007Ch`).

`.` and `..` must never be *stored* in a `FileName` field, but an
implementation may synthesise them in directory listings.

#### Name hash (Section 7.6.4, Figure 4)

Computed over the **up-cased** name as raw bytes of the UTF-16 (little-endian
on disk) string, `NameLength * 2` bytes:

```c
UInt16 NameHash(WCHAR *FileName, UCHAR NameLength)  // FileName is already up-cased
{
    UCHAR  *Buffer = (UCHAR *)FileName;
    UInt16  NumberOfBytes = (UInt16)NameLength * 2;
    UInt16  Hash = 0;
    UInt16  Index;

    for (Index = 0; Index < NumberOfBytes; Index++)
    {
        Hash = ((Hash&1) ? 0x8000 : 0) + (Hash>>1) + (UInt16)Buffer[Index];
    }
    return Hash;
}
```

On a big-endian host the buffer must be the little-endian byte image, not the
host's native `WCHAR` array, or the hash will not match the volume.

#### Other entry types

`A0h` Volume GUID (benign primary), `C2h` Vendor Extension and `C3h` Vendor
Allocation (benign secondary), and TexFAT Padding (Section 7.10). Section 8.2
governs what an implementation may do with entries it does not recognise: never
modify them, and an unrecognised *critical secondary* entry makes its whole
entry set unrecognised. An unrecognised critical primary entry in the root
directory makes the **volume** invalid.

### Timestamps (Sections 7.4.8 – 7.4.10)

`Timestamp` is a 32-bit value describing **local** time to 2-second resolution
(Table 29):

| Field | Bit offset | Bits | Range |
|---|---|---|---|
| DoubleSeconds | 0 | 5 | 0…29 (0…58 seconds) |
| Minute | 5 | 6 | 0…59 |
| Hour | 11 | 5 | 0…23 |
| Day | 16 | 5 | 1…last day of month |
| Month | 21 | 4 | 1…12 |
| Year | 25 | 7 | 0 = 1980 … 127 = 2107 |

`10msIncrement`: 0…199, adding 0…1990 ms to the corresponding timestamp.

`UtcOffset` (Table 30): bits 0–6 `OffsetFromUtc`, bit 7 `OffsetValid`.
`OffsetFromUtc` is a 7-bit two's-complement count of **15-minute** intervals:
`00h` = UTC, `3Fh` (+63) = UTC+15:45, `7Fh` (−1) = UTC−00:15, `40h` (−64) =
UTC−16:00. If `OffsetValid` is 0 the offset is unknown and `OffsetFromUtc`
shall be `00h`; treat the timestamp as having the current local offset.

### Recommended write ordering (Section 8.1)

Creating entries or allocating clusters:
1. set `VolumeDirty` = 1
2. update the active FAT if needed
3. update the active Allocation Bitmap
4. create/update the directory entry
5. clear `VolumeDirty` to 0 if it was 0 to begin with

Deleting entries or freeing clusters:
1. set `VolumeDirty` = 1
2. delete/update the directory entry
3. update the active FAT if needed
4. update the active Allocation Bitmap
5. clear `VolumeDirty` to 0 if it was 0 to begin with

### Limits (Section 9) and partition types (Section 10.2)

- Sector: 512 … 4096 bytes. Cluster: 1 sector … 32 MB.
- Cluster Heap: at least `2 + NumberOfFats` clusters; at most 2³²−11 clusters
  (spec *recommends* no more than 2²⁴−2).
- Volume: at least 1 MB; architecturally up to 2⁶⁴−1 sectors.
- Directory: 0 … 256 MB, i.e. up to 8,388,608 entries, up to 2,796,202 files
  (three entries minimum per file).
- MBR partition type **`07h`**; GPT partition type GUID
  **`{EBD0A0A2-B9E5-4433-87C0-68B6B72699C7}`**.

## Gotchas / common mistakes

- **Little-endian on a big-endian CPU.** This is the single largest source of
  bugs for this port. Every field above needs a swap on the 68080.
- Published summaries of the layout are frequently wrong. In particular the
  Stream Extension entry has a one-byte `Reserved1` at offset 2, so
  `NameLength` is at 3 and `NameHash` is a **2-byte** field at 4; and the File
  entry's `CreateTimestamp` is at offset **8**, not 10. Check
  `exfat-specification.md` (same folder) rather than a summary.
- `DataLength` is the *allocated* size; `ValidDataLength` is the *readable*
  size. Reporting `DataLength` as the file size is wrong, and reading past
  `ValidDataLength` must yield zeroes rather than stale media content.
- `NoFatChain` = 1 means the FAT entries for that file are **garbage**. Walking
  the chain anyway will produce nonsense.
- The bitmap's first bit is the *lowest* bit of the first byte. Getting the bit
  order backwards silently corrupts allocation.
- Clearing `InUse` deletes an entry — but the value `80h` must never appear, or
  the directory looks terminated.
- The up-case table on the volume is authoritative for case folding. A built-in
  ASCII `toupper()` will mismatch names on volumes with a custom table.
- Both the Boot Checksum and the Entry Set Checksum skip specific byte offsets.
  Forgetting the skips yields a checksum that never validates.
- The root directory is *always* FAT-chained, regardless of any flag.
