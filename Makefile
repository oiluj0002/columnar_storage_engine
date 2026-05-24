# ── Compiler ────────────────────────────────────────────────────────────────
CC     = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic -Werror -O2 -Iinclude

# ── Targets ─────────────────────────────────────────────────────────────────
ENGINE_TARGET = bin/engine
BENCH_TARGET  = bin/bench

# ── Sources ──────────────────────────────────────────────────────────────────
ENGINE_SRCS = src/main.c \
              src/column.c \
              src/table.c \
              src/save.c \
              src/load.c

BENCH_SRCS = src/benchmark.c

# ── Default ──────────────────────────────────────────────────────────────────
all: $(ENGINE_TARGET) $(BENCH_TARGET)

# ── Build rules ──────────────────────────────────────────────────────────────
bin:
	mkdir -p bin

$(ENGINE_TARGET): $(ENGINE_SRCS) | bin
	@echo "[build]  compiling engine..."
	@$(CC) $(CFLAGS) -o $@ $(ENGINE_SRCS)
	@echo "[build]  engine ready -> $@"

$(BENCH_TARGET): $(BENCH_SRCS) | bin
	@echo "[build]  compiling benchmark..."
	@$(CC) $(CFLAGS) -o $@ $(BENCH_SRCS)
	@echo "[build]  benchmark ready -> $@"

# ── Workflow ─────────────────────────────────────────────────────────────────
data:
	@echo "[data]   generating test data..."
	@cd data && uv run main.py
	@echo "[data]   done"

run: all data
	@echo "[engine] running engine..."
	@./$(ENGINE_TARGET)
	@echo "[bench]  running benchmark..."
	@./$(BENCH_TARGET)

# ── Cleanup ──────────────────────────────────────────────────────────────────
clean:
	@echo "[clean]  removing build artifacts..."
	@rm -rf bin/* db/*
	@echo "[clean]  done"

clean-data:
	@echo "[clean]  removing test data..."
	@rm -rf data/*.csv
	@echo "[clean]  done"

# ── Phony ────────────────────────────────────────────────────────────────────
.PHONY: all data run clean clean-data
