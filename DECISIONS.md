# Architecture Decision Records

Newest record on top.

## 2026-10-24 Languages auto-discovered; CI warnings and coverage fixed

- **Reason**: Language list was hardcoded in two places; CI kept tripping on
  warnings/Windows/coverage.
- **Decisions**:
  - build.sh scans `lang/*.json` (no hardcoded list), emits English first as
    fallback, generates a `builtin_langs[]` table `{prefix, json}` with the
    locale prefix derived from the filename (zh-TW→"zh"). Adding a language
    is dropping a file into `lang/`.
  - `GIT_VERSION` is the full commit hash (`git rev-parse HEAD`).
  - i18n locale detection walks the generated table (no `#ifdef` language
    blocks); tests use a `set_lang()` helper (`putenv` on `_WIN32`,
    `setenv` elsewhere); test.sh removes stale `*.gcda` after each rebuild
    so gcovr only reads the final build.
  - Workflow token permissions explicit at workflow level
    (`contents: read`, `id-token: write`).
- **Trade-off / Risk**: Filename convention `<langcode>-<region>.json` is
  the contract; multi-LANGS rebuild coverage is discarded (branches covered
  by unit tests); Codecov allowed to fail CI.

## 2026-10-24 Fix CI warnings and unit-test failure

- **Reason**: CI surfaced three issues: unused `lang` in single-language
  builds, implicit `setenv` declaration, and the zh-TW unit test failing
  because steps 8-9 left the generated header without the zh archive.
- **Trade-off**: `lang` is guarded by `#if defined(...Z_TW)||...JA_JP)`;
  unit-test CFLAGS carry `-D_POSIX_C_SOURCE=200809L`; test.sh rebuilds the
  default all-languages binary right before compiling unit tests.
- **Risk**: None.

## 2026-10-24 Warnings zeroed, per-module unit tests, dual-platform CI

- **Reason**: Fix CI warnings (`-Wcomment`, `-Waddress`), move unit tests
  under `tests/<module>/`, and extend CI to Windows.
- **Decisions**:
  - Zero warnings enforced: block comments must not contain `/*`; English
    fallback compared through a `fallback_json` pointer variable instead of
    string-literal addresses.
  - Internal macros carry no project prefix (`LANG_JSON_*`, `LANG_*`) —
    internal use only.
  - Unit tests live in `tests/<module>/test_<module>.c`, each linking only
    its own module; `test.sh` compiles and runs them into `build_test/`.
    Smoke tests additionally rebuild with `LANGS=en` and
    `LANGS=en ja-JP` to hit the i18n `#ifdef` branches.
  - CI: `.github/workflows/ci.yml` — ubuntu (musl + coverage → Codecov,
    upload requires `CODECOV_TOKEN` because `main` is a protected branch)
    plus a windows-latest MinGW smoke job.
- **Trade-off / Risk**: gcovr report covers both smoke and unit paths;
  Codecov is allowed to fail CI (`fail_ci_if_error: false`).

## 2026-10-24 Project foundations (compressed record)

- **Reason**: Lock down the shape of MeowLauncherCli after the first
  iterations so later modules stay consistent.
- **Decisions**:
  - Entry point is `cmd/meowlaunchcli/main.c`, busybox-style and
    non-interactive: no arguments (or `help`) prints the help list; the
    first argument dispatches a subcommand. No REPL.
  - Internal modules live under `internal/` (`help`, `version`, `i18n`)
    with the `meow_` prefix; translatable UI strings never live in C code.
  - Localization: `lang/*.json` (en, zh-TW, ja-JP) are embedded into the
    binary at build time by `scripts/build.sh`; runtime selects by `$LANG`
    (`zh*`→zh-TW, `ja*`→ja-JP), English is the fallback — unless the build
    contains only one language, which is used unconditionally.
  - Build: Linux → `musl-gcc -static`; Windows/MinGW → `gcc` producing
    `build/meowlaunchcli.exe`; normal products land in `build/`, coverage
    products (objects, `.gcda`, gcovr HTML report) in `build_test/`;
    `-Wall -Wextra -Wpedantic -D_POSIX_C_SOURCE=200809L`;
    `-DGIT_VERSION="$(git describe --always --dirty)"`; `LANGS` selects the
    embedded languages. `scripts/test.sh` runs CLI smoke tests;
    `COVERAGE=1 ./scripts/test.sh` produces the coverage build.
    CI: `.github/workflows/test.yml` (ubuntu-latest, musl + coverage);
    coverage uploaded to Codecov (app.codecov.io) via cobertura XML.
  - Symbol naming: exported module functions start with a capital letter
    (`I18n_Init`, `I18n`, `Help_Print`, `Version_Print`); file-private
    helpers stay lowercase `static`.
  - `version` subcommand prints `MeowLauncherCli v<semver> build <git>`;
    current semver is `0.0.1`.
- **Trade-off / Risk**: Tiny hand-rolled JSON walker reads the embedded
  archives (controlled subset, no parser dependency); archives must avoid
  quotes/braces inside values. Generated
  `internal/i18n/gen/lang_embedded.h` is build output, not source.
