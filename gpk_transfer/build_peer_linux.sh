#!/bin/sh
set -eu

SCRIPT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
GPK_ROOT=$(CDPATH= cd -- "$SCRIPT_ROOT/.." && pwd)
OUTPUT_ROOT=$(CDPATH= cd -- "$GPK_ROOT/.." && pwd)/x64.Release
mkdir -p "$OUTPUT_ROOT"

c++ \
	-std=c++20 -O2 -s -static -pthread -Wall -Wextra -Werror -Wno-type-limits \
	-I"$GPK_ROOT/llc" \
	-I"$GPK_ROOT/gpk" \
	"$SCRIPT_ROOT/gpk_transfer.peer.cpp" \
	"$SCRIPT_ROOT/gpk_transfer.cpp" \
	"$GPK_ROOT/llc/gpk_crc.cpp" \
	"$GPK_ROOT/llc/gpk_debug.cpp" \
	"$GPK_ROOT/llc/gpk_platform_error.cpp" \
	"$GPK_ROOT/llc/gpk_log.cpp" \
	"$GPK_ROOT/llc/gpk_chrono.cpp" \
	-o "$OUTPUT_ROOT/gpk_transfer.peer.linux"

test -x "$OUTPUT_ROOT/gpk_transfer.peer.linux"
