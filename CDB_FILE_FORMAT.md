# ColumnDB File Format Specification

## Overview
The `.cdb` file format is a binary format for storing ColumnDB databases on disk.

## File Structure

```
┌─────────────────────────────────────┐
│ Header (32 bytes)                   │
├─────────────────────────────────────┤
│ Column Metadata (variable)          │
│ - Column names and types            │
├─────────────────────────────────────┤
│ Column Data (variable)              │
│ - Actual column data                │
├─────────────────────────────────────┤
│ Footer (16 bytes)                   │
│ - Checksum and metadata             │
└─────────────────────────────────────┘
```

## Header (32 bytes)

```
Offset  Size  Type        Description
------  ----  --------    -----------
0       4     uint32      Magic number (0x43444201 = "CDB\x01")
4       4     uint32      Format version (1)
8       4     uint32      Number of columns
12      4     uint32      Number of rows
16      8     uint64      Timestamp (seconds since epoch)
24      4     uint32      Flags (reserved, set to 0)
28      4     uint32      Header checksum (CRC32)
```

## Column Metadata (for each column)

```
Offset  Size  Type        Description
------  ----  --------    -----------
0       1     uint8       Data type (0-5)
1       2     uint16      Name length (n)
3       n     char[]      Column name (UTF-8)
3+n     8     uint64      Data offset
11+n    8     uint64      Data size (bytes)
19+n    8     uint64      Null bitmap size
```

## Column Data

Raw columnar data stored sequentially:
- INT32: 4 bytes per value
- INT64: 8 bytes per value
- FLOAT32: 4 bytes per value
- FLOAT64: 8 bytes per value
- STRING: 4-byte length + UTF-8 data for each value
- BOOL: 1 byte per value
- NULL bitmap: 1 bit per value (8 values per byte)

## Footer (16 bytes)

```
Offset  Size  Type        Description
------  ----  --------    -----------
0       4     uint32      Magic number (0x43444245 = "CDBE")
4       8     uint64      Total file size
12      4     uint32      File checksum (CRC32)
```

## Example File Layout

```
File: data.cdb

Bytes 0-31:     Header
                - Magic: 0x43444201
                - Version: 1
                - Columns: 3
                - Rows: 5

Bytes 32-200:   Column Metadata
                - Column 0: "id" (INT64)
                - Column 1: "name" (STRING)
                - Column 2: "score" (FLOAT64)

Bytes 200-1000: Column 0 Data (INT64)
                - 40 bytes (5 rows × 8 bytes)

Bytes 1000-2000: Column 1 Data (STRING + length)
                - Variable length strings

Bytes 2000-2040: Column 2 Data (FLOAT64)
                - 40 bytes (5 rows × 8 bytes)

Bytes 2040-2080: Null Bitmaps
                - 1 bitmap per column

Bytes 2080-2096: Footer
                - Magic: 0x43444245
                - Total size: 2096
                - Checksum
```

## Advantages

- ✅ Self-describing (metadata included)
- ✅ Columnar layout (efficient for analytics)
- ✅ Type-safe (types stored explicitly)
- ✅ Checksums (data integrity)
- ✅ Timestamps (version tracking)
- ✅ Platform-independent (no alignment issues)

## Version History

### Version 1 (Current)
- Initial format
- Support for 6 data types
- CRC32 checksums
