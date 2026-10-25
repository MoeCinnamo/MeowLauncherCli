# Architecture Decision Records

Newest record on top.

## 2026-10-25 JSON injection fix

- **Reason**: `Game_Switch` concatenated the user-supplied version into a
  JSON string, so quotes/backslashes/control characters could corrupt
  config.json or inject extra fields; the fixed 256-byte buffer could also
  truncate long escaped values.
- **Decisions**:
  - New `internal/json/json_escape.c`: `Json_Escape()` escapes `"`, `\`,
    `\b \f \n \r \t` and other control chars as `\u00XX` (RFC 8259); bytes
    >= 0x20 (UTF-8) pass through unchanged.
  - `Game_Switch` escapes the version and writes through an exactly-sized
    malloc'd buffer.
  - `Json_FindValue` skips `\"`/`\\` so escaped values read back in full.
  - Tests: new `tests/internal/json/test_json_escape.c`; escape cases added
    to `tests/internal/json/test_json_walk.c` and
    `tests/internal/game/test_game.c`; `test.sh` links the new test with
    `--wrap,malloc`.
- **Trade-off**: `game current` shows raw escaped text (e.g. `1.0\"x`) for
  versions containing quotes; values are not unescaped on display.

## 2026-10-25 config module and shared paths

- **Reason**: User wants `game switch` echo, full `config` subcommand, and
  deduplicate exedir across modules.
- **Decisions**:
  - `Game_Switch` prints `game.switched: <ver>` after saving.
  - `internal/config/` split into config_common/java/memory/window/args/
    commands/gfx/help.c; stores keys in config.json.
  - `internal/paths/paths.c`: shared `exedir()`; account/game/config call it.
  - `config help`, `config java|memory|window|args|precmd|postcmd|gfx`
    dispatched in main.c.
  - i18n keys: `game.switched`, `config.usage`, `config.help.*`.
  - Tests: tests/internal/{config,paths,account/local}.
- **Trade-off**: None.

## 2026-10-25 skip malloc-fail tests on macOS

- **Reason**: macOS ld doesn't support `--wrap`; malloc-fail injection tests
  fail without it.
- **Decisions**:
  - `test_json_io.c` and `test_usersfile.c` skip malloc-fail assertions
    under `#ifndef __APPLE__`.
- **Trade-off**: malloc-fail branches not covered on macOS.

## 2026-10-25 cross-platform Codecov upload

- **Reason**: macOS clang test results should also report to Codecov.
- **Decisions**:
  - Each macOS CI job uploads `build_test/junit.xml` via codecov/test-results-action.
  - `__wrap_malloc`/`__real_malloc` guarded by `#ifndef __APPLE__` in tests.
- **Trade-off**: None.

## 2026-10-25 macOS exedir in game.c, skip --wrap on Darwin

- **Reason**: game.c also needed `_NSGetExecutablePath`; Apple ld rejects
  `-Wl,--wrap`.
- **Decisions**:
  - game.c adds `_NSGetExecutablePath` under `__APPLE__`.
  - test.sh skips `--wrap` flags on Darwin.
- **Trade-off**: malloc-fail injection tests skipped on macOS.

## 2026-10-25 macOS exe path resolution

- **Reason**: macOS lacks `/proc/self/exe`, exedir() returned -1.
- **Decisions**:
  - usersfile.c adds `_NSGetExecutablePath` under `__APPLE__`.
- **Trade-off**: None.

## 2026-10-25 macOS bash compatibility

- **Reason**: macOS default bash 3.2 lacks `mapfile`; `nproc` unavailable.
- **Decisions**:
  - build.sh/test.sh replace all `mapfile` with `while read` loop
    (including lang archive collection).
  - `nproc` fallback to `sysctl -n hw.ncpu` on macOS.
- **Trade-off**: None.

## 2026-10-25 always-local strcasestr, macOS CI chmod fix

- **Reason**: Ubuntu 26.04 glibc doesn't declare strcasestr under
  `_POSIX_C_SOURCE=200809L`; macOS chmod `.*` glob errors.
- **Decisions**:
  - Always define local `strcasestr` (no `__GLIBC__` guard).
  - macOS CI jobs removed `chmod 755 * .* -R`.
- **Trade-off**: None.

## 2026-10-25 macOS clang CI, cross-platform error injection

- **Reason**: Need macOS cross-platform test coverage with clang; read-only
  dir test unreliable across platforms.
- **Decisions**:
  - CI: added `macos-15` and `macos-26` jobs.
  - i18n prefix is full locale tag (`zh_TW`, `ja_JP`, `zh_HK`); `zh_CN`
    falls back to English.
  - test_account: trigger usersfile_save failure by rm -rf data dir then
    creating it as a file, cross-platform.

## 2026-10-25 read-only dir test, zh_HK locale, coverage fixes

- **Reason**: CI runs as root so chmod doesn't block writes; `0o` prefix is
  Python octal not C; need zh_HK locale entry.
- **Decisions**:
  - test_account skips read-only dir test when running as root (`getuid()==0`).
  - Use C octal `0555`/`0755` not `0o555`/`0o755`.
  - Added `lang/zh-HK.json`, `README-zh_HK.md`, `docs/zh-HK/`.
  - i18n prefix is full locale tag (`zh_TW`, `ja_JP`, `zh_HK`) so `zh_CN`
    falls back to English.
- **Trade-off**: None.

## 2026-10-25 strict locale, CRLF→LF, CC env override

- **Reason**: Windows mojibake with non-UTF-8 locales; need LF on commit;
  user wants CC env override for musl/mingw/clang; coverage report output.
- **Decisions**:
  - i18n: require UTF-8 encoding in `$LANG`; prefix stored as full locale tag
    (`zh_TW`, `ja_JP`) not just `zh`, so `zh_CN` falls back to English;
    non-UTF-8 also falls back.
  - `.gitattributes`: `* text=auto eol=lf`, `*.sh`/`scripts/*` executable.
  - build.sh/test.sh respect `CC` env var; print `[INFO] Compiler: $CC`;
    defaults: musl-gcc on Linux (static), gcc on Alpine/MinGW.
  - `strcasestr` implemented locally when `__GLIBC__` is not defined.
  - test.sh coverage: `--coverage` flag + gcovr HTML/XML report; falls back
    to `gcov` directly when gcovr is missing.
  - test_account/test_game cover missing users.json / missing versions dir.
  - Added `lang/zh-HK.json`, `README-zh_HK.md`, `docs/zh-HK/`.
- **Trade-off**: Not glibc-compatible by design.

## 2026-10-25 coverage: game help, game show dirs, main.c dispatch

- **Reason**: Raise code coverage on `internal/game/help.c`,
  `internal/game/game.c` (Game_Show listing branch, malformed config), and
  `cmd/meowlaunchcli/main.c` (game/account subcommand dispatch).
- **Decisions**:
  - `tests/internal/game/test_help.c`: calls `Game_Help()` with i18n init.
  - `tests/internal/game/test_game.c`: creates fake
    `build_test/.minecraft/versions/{1.21.11,1.21.11-Neoforge}` dirs to cover
    the readdir/S_ISDIR loop; writes malformed `config.json` to cover the
    `Json_FindValue` miss branch in `Game_Current()`.
  - `tests/cmd/meowlaunchcli/test_main.c`: black-box runs `game help`,
    `game` (no subcommand → usage), `game switch/current/show`, and
    `account local show`.
  - `game` with no subcommand now prints `game.usage` and exits nonzero
    instead of falling through to `Game_Show()`.
- **Trade-off**: None.

## 2026-10-24 Project foundations, module split, tests, i18n, CI (compressed)

- **Reason**: Bootstrap MeowLauncherCli: busybox-style CLI, embedded i18n,
  split internal modules, local account persistence, cross-platform build/test.
- **Decisions**:
  - Entry: `cmd/meowlaunchcli/main.c`, non-interactive dispatch, no REPL.
  - `internal/` modules: `json/{io,walk}`, `account/{account,usersfile,uuid,help}`,
    `game/{game,help}`, `i18n`, `help`, `version`. PascalCase exports,
    lowercase static internals.
  - Data paths: `<exedir>/.meowlaunch/data/users.json`, config.json;
    `.minecraft/versions/`.
  - i18n: `lang/*.json` embedded at build time; English fallback; `$LANG`
    selects by prefix.
  - Build: Linux musl-gcc -static, MinGW gcc; `-Wall -Wextra -Wpedantic
    -D_POSIX_C_SOURCE=200809L`; `-DGIT_VERSION`; `LANGS` selects languages.
  - Tests mirror source tree; `#include .c` to reach statics; `build/` for
    product, `build_test/` for tests/coverage; gcovr HTML/XML; Codecov.
  - `usersfile_save` recursive mkdir per component; insert rebuilds buffer
    in malloc (no NUL gap).
- **Trade-off**: Hand-rolled JSON walker for controlled subset only.
