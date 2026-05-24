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

## Supported types

| Type | C type | Size |
|---|---|---|
| `TYPE_INT` | `int` | 4 bytes |
| `TYPE_DOUBLE` | `double` | 8 bytes |
| `TYPE_BOOL` | `char` | 1 byte |
| `TYPE_STRING` | `char[64]` | 64 bytes |
