#!/usr/bin/env bash
#
# Smoke tests for MeowLauncherCli.
#
# Usage:
#   ./scripts/test.sh
#
# Builds the binary via scripts/build.sh, then checks the basic CLI contract:
# help output, version output, and exit codes.

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Deterministic language: expect the English archive regardless of the
# runner's system locale.
export LANG=C

# Match the binary name produced by build.sh on this platform.
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) bin_suffix=".exe" ;;
    *) bin_suffix="" ;;
esac

echo "[INFO] Building..."
"$PROJECT_ROOT/scripts/build.sh" >/dev/null

# Coverage builds put the binary in build_test/, normal builds in build/.
if [ "${COVERAGE:-0}" = "1" ]; then
    out_dir="$PROJECT_ROOT/build_test"
else
    out_dir="$PROJECT_ROOT/build"
fi
BIN="$out_dir/meowlaunchcli${bin_suffix}"
status=0

# 1. No arguments: prints help, exits 0.
out="$("$BIN")"
echo "$out" | grep -q "Usage: meowlaunchcli" \
    || { echo "[FAIL] bare invocation missing usage line"; status=1; }

# 2. help subcommand: exits 0.
"$BIN" help >/dev/null \
    || { echo "[FAIL] help subcommand exited nonzero"; status=1; }

# 3. version: exits 0 and matches "MeowLauncherCli v<semver> build <git>".
out="$("$BIN" version)"
echo "$out" | grep -Eq "^MeowLauncherCli v[0-9]+\.[0-9]+\.[0-9]+ build .+" \
    || { echo "[FAIL] unexpected version output: $out"; status=1; }

# 4. Unknown command: exits nonzero.
if "$BIN" doesnotexist >/dev/null 2>&1; then
    echo "[FAIL] unknown command exited 0"
    status=1
fi

# 5. Locale detection: an unrelated system locale falls back to English.
LANG=fr_FR.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    || { echo "[FAIL] non zh/ja locale did not fall back to English"; status=1; }

# 6. zh_TW locale selects the zh-TW archive (default multi-language build).
LANG=zh_TW.UTF-8 "$BIN" | grep -q "指令：" \
    || { echo "[FAIL] zh_TW locale did not select zh-TW help"; status=1; }

# 7. ja_JP locale selects the Japanese archive.
LANG=ja_JP.UTF-8 "$BIN" | grep -q "コマンド:" \
    || { echo "[FAIL] ja_JP locale did not select Japanese help"; status=1; }

# 8. Single-language build (LANGS=en): any system locale still yields English.
echo "[INFO] Single-language build check..."
LANGS="en" "$PROJECT_ROOT/scripts/build.sh" >/dev/null
LANG=zh_TW.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    || { echo "[FAIL] single-lang build ignored $LANGS"; status=1; }

# 9. Language not compiled in (LANGS=en ja-JP): zh locale falls back English.
echo "[INFO] Uncompiled-language fallback check..."
LANGS="en ja-JP" "$PROJECT_ROOT/scripts/build.sh" >/dev/null
LANG=zh_TW.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    || { echo "[FAIL] uncompiled zh locale did not fall back to English"; status=1; }

# 10. Unit tests: each tests/<module>/test_*.c links against its own module.
echo "[INFO] Unit tests..."
CC="${CC:-cc}"
CFLAGS="${CFLAGS:--std=c11 -Wall -Wextra -Wpedantic -I.}"
for test_src in tests/*/test_*.c; do
    mod="$(basename "$(dirname "$test_src")")"
    unit_bin="$out_dir/test_${mod}${bin_suffix}"
    # shellcheck disable=SC2086
    "$CC" $CFLAGS "$test_src" "internal/$mod/$mod.c" -o "$unit_bin" \
        || { echo "[FAIL] build unit test $mod"; status=1; continue; }
    echo "[INFO] == $mod =="
    "$unit_bin" || { echo "[FAIL] unit test $mod"; status=1; }
done

# Coverage build: emit an HTML + cobertura report into build_test/ if gcovr
# is present (cobertura.xml is what Codecov consumes).
if [ "${COVERAGE:-0}" = "1" ]; then
    if command -v gcovr >/dev/null 2>&1; then
        gcovr -r "$PROJECT_ROOT" --html-details \
            -o "$out_dir/coverage.html" "$out_dir"
        gcovr -r "$PROJECT_ROOT" --xml \
            -o "$out_dir/coverage.xml" "$out_dir"
        echo "[INFO] Coverage report: $out_dir/coverage.html"
    else
        echo "[WARN] gcovr not found; .gcda data left in $out_dir"
    fi
fi

if [ "$status" -eq 0 ]; then
    echo "[INFO] All smoke tests passed"
else
    echo "[ERROR] Smoke tests failed"
fi

exit "$status"
