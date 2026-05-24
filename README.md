# Columnar Storage Engine

A minimal columnar storage engine written in C23. Reads trade data from CSV, stores each column as a contiguous binary file on disk, and benchmarks binary reads against CSV parsing.

## How it works

Instead of storing rows together (like a CSV or row-store database), each column is stored as its own flat binary file. This allows the engine to read a single column — such as `Price` — without touching any other data, which is significantly faster for analytical queries.

```
db/
├── metadata.bin       ← table schema (column names, types, row counts)
└── columns/
    ├── Trade_ID.bin
    ├── Symbol.bin
    ├── Price.bin
    ├── Quantity.bin
    └── Is_Valid.bin
```

## Requirements

- `gcc` with C23 support
- [`uv`](https://github.com/astral-sh/uv) — for generating test data

## Usage

```bash
# Compile both binaries
make

# Generate 10M rows of test data
make data

# Full pipeline: compile → generate data → run engine → run benchmark
make run
```

### Individual commands

```bash
make               # compile engine and benchmark
make data          # generate test CSV via Python
make run           # full pipeline
make clean         # remove build artifacts and db
make clean-data    # remove generated CSV files
```

## Project structure

```
├── src/
│   ├── main.c          # entry point — loads CSV and saves binary
│   ├── column.c        # column memory management and value parsing
│   ├── table.c         # table container
│   ├── load.c          # CSV parser
│   ├── save.c          # binary serializer
│   └── benchmark.c     # compares binary vs CSV read performance
├── include/            # header files
├── data/
│   └── main.py         # test data generator (10M rows)
├── db/                 # binary output (created at runtime)
└── bin/                # compiled binaries (created at runtime)
```

## Benchmark

The benchmark measures the performance difference between reading a single column from a binary file versus parsing the same column from the original CSV.

Both approaches compute the **average `Price`** across all rows — a typical analytical query that only touches one column out of five.

| Approach | What it does |
|---|---|
| Binary | Opens `db/columns/Price.bin`, loads raw `double` values directly into memory with a single `fread` call |
| CSV | Opens `trades.csv`, tokenizes every row line by line, extracts the price field, and converts it from text to `double` |

The binary approach is faster for two reasons:
- **No parsing** — values are already stored in their native binary format, so no text-to-number conversion is needed
- **No wasted reads** — only `Price.bin` is loaded; the other four columns are never touched

Sample output:

```
--- Benchmark Results ---
CSV Time:    3.421 seconds (Avg: $30012.48)
Binary Time: 0.087 seconds (Avg: $30012.48)
-------------------------
The binary reader is 39.3 times faster!
Net time reduced by 97.5%
```

## Supported types

| Type | C type | Size |
|---|---|---|
| `TYPE_INT` | `int` | 4 bytes |
| `TYPE_DOUBLE` | `double` | 8 bytes |
| `TYPE_BOOL` | `char` | 1 byte |
| `TYPE_STRING` | `char[64]` | 64 bytes |
