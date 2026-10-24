#!/usr/bin/env bash
#
# Smoke tests for MeowLauncherCli.
#
# Usage:
#   ./scripts/test.sh
#
# Builds the binary via scripts/build.sh, then checks the basic CLI contract:
# help output, version output, exit codes, locale selection, and per-module
# unit tests. Emits a JUnit report to build_test/junit.xml for Codecov test
# analytics.

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

# Collect "PASS|name" / "FAIL|name" rows; rendered into JUnit XML at exit.
results=()

check() {
    local name="$1"; shift
    if "$@" >/dev/null 2>&1; then
        results+=("PASS|$name")
    else
        echo "[FAIL] $name"
        results+=("FAIL|$name")
        status=1
    fi
}

# 1. No arguments: prints help, exits 0.
out="$("$BIN")"
echo "$out" | grep -q "Usage: meowlaunchcli" \
    && check "bare invocation prints help" true \
    || { echo "[FAIL] bare invocation missing usage line"; results+=("FAIL|bare invocation prints help"); status=1; }

# 2. help subcommand: exits 0.
check "help subcommand exits 0" "$BIN" help

# 3. version: matches "MeowLauncherCli v<semver> build <git>".
out="$("$BIN" version)"
echo "$out" | grep -Eq "^MeowLauncherCli v[0-9]+\.[0-9]+\.[0-9]+ build .+" \
    && check "version output format" true \
    || { echo "[FAIL] unexpected version output: $out"; results+=("FAIL|version output format"); status=1; }

# 4. Unknown command: exits nonzero.
if "$BIN" doesnotexist >/dev/null 2>&1; then
    echo "[FAIL] unknown command exited 0"
    results+=("FAIL|unknown command nonzero exit")
    status=1
else
    results+=("PASS|unknown command nonzero exit")
fi

# 5. Locale detection: an unrelated system locale falls back to English.
LANG=fr_FR.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    && results+=("PASS|fr locale falls back to english") \
    || { echo "[FAIL] fr locale did not fall back to English"; results+=("FAIL|fr locale falls back to english"); status=1; }

# 6. zh_TW locale selects the zh-TW archive (default multi-language build).
LANG=zh_TW.UTF-8 "$BIN" | grep -q "指令：" \
    && results+=("PASS|zh_TW selects chinese") \
    || { echo "[FAIL] zh_TW locale did not select zh-TW help"; results+=("FAIL|zh_TW selects chinese"); status=1; }

# 7. ja_JP locale selects the Japanese archive.
LANG=ja_JP.UTF-8 "$BIN" | grep -q "コマンド:" \
    && results+=("PASS|ja_JP selects japanese") \
    || { echo "[FAIL] ja_JP locale did not select Japanese help"; results+=("FAIL|ja_JP selects japanese"); status=1; }

# 8. Single-language build (LANGS=en): any system locale still yields English.
echo "[INFO] Single-language build check..."
LANGS="en" "$PROJECT_ROOT/scripts/build.sh" >/dev/null
rm -f "$out_dir"/*.gcda 2>/dev/null || true
LANG=zh_TW.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    && results+=("PASS|single-lang build ignores locale") \
    || { echo "[FAIL] single-lang build ignored LANGS"; results+=("FAIL|single-lang build ignores locale"); status=1; }

# 9. Language not compiled in (LANGS=en ja-JP): zh locale falls back English.
echo "[INFO] Uncompiled-language fallback check..."
LANGS="en ja-JP" "$PROJECT_ROOT/scripts/build.sh" >/dev/null
rm -f "$out_dir"/*.gcda 2>/dev/null || true
LANG=zh_TW.UTF-8 "$BIN" | grep -q "Usage: meowlaunchcli" \
    && results+=("PASS|uncompiled language falls back to english") \
    || { echo "[FAIL] uncompiled zh locale did not fall back to English"; results+=("FAIL|uncompiled language falls back to english"); status=1; }

# 10. Unit tests: each tests/<module>/test_*.c links against its own module.
echo "[INFO] Unit tests..."
# Rebuild the default all-languages binary: steps 8-9 rebuilt with reduced
# LANGS, which would leave the unit tests without the zh-TW archive.
"$PROJECT_ROOT/scripts/build.sh" >/dev/null
rm -f "$out_dir"/*.gcda 2>/dev/null || true
# Exercise the final coverage binary once: the gcda we just deleted belongs
# to this build, and gcovr needs it before generating the report.
"$BIN" help >/dev/null
"$BIN" version >/dev/null
# Unit-test compiler: prefer gcc (present on Alpine, MinGW, Ubuntu).
if [ -z "${CC:-}" ]; then
    if command -v gcc >/dev/null 2>&1; then CC="gcc"; else CC="cc"; fi
fi
CFLAGS="${CFLAGS:--std=c11 -Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L -I.}"
[ "${COVERAGE:-0}" = "1" ] && CFLAGS="$CFLAGS --coverage"
for test_src in tests/*/test_*.c; do
    mod="$(basename "$(dirname "$test_src")")"
    unit_bin="$out_dir/test_${mod}${bin_suffix}"
    # shellcheck disable=SC2086
    if ! "$CC" $CFLAGS "$test_src" "internal/$mod/$mod.c" -o "$unit_bin"; then
        echo "[FAIL] build unit test $mod"
        results+=("FAIL|unit test $mod")
        status=1
        continue
    fi
    echo "[INFO] == $mod =="
    if "$unit_bin"; then
        results+=("PASS|unit test $mod")
    else
        echo "[FAIL] unit test $mod"
        results+=("FAIL|unit test $mod")
        status=1
    fi
done

# Emit JUnit XML for Codecov test analytics.
mkdir -p "$out_dir"
junit="$out_dir/junit.xml"
{
    echo '<?xml version="1.0" encoding="UTF-8"?>'
    echo '<testsuites>'
    echo "  <testsuite name=\"meowlaunchcli\" tests=\"${#results[@]}\">"
    for row in "${results[@]}"; do
        state="${row%%|*}"
        name="${row#*|}"
        # XML-escape the name (only & < > matter for our names).
        esc="${name//&/&amp;}"
        esc="${esc//</&lt;}"
        esc="${esc//>/&gt;}"
        echo "    <testcase classname=\"smoke\" name=\"$esc\">"
        if [ "$state" = "FAIL" ]; then
            echo '      <failure/>'
        fi
        echo '    </testcase>'
    done
    echo '  </testsuite>'
    echo '</testsuites>'
} > "$junit"
echo "[INFO] JUnit report: $junit"

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
    echo "[INFO] All tests passed"
else
    echo "[ERROR] Tests failed"
fi

exit "$status"
