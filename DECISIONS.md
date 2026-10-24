# Architecture Decision Records

Newest record on top.

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
