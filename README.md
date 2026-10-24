# MeowLauncherCli

[English](README.md) [繁體中文](README-zh_TW.md) [日本語](README-ja_JP.md)

> A Minecraft launcher that Chinese-style parents cannot report — it lives entirely in the terminal, and to the kind of parent who sees a threat to their child in every pixel, it just looks like serious developer work.

## Overview

MeowLauncherCli is a **pure command-line launcher for Minecraft**. It aims to bring together nearly everything a Minecraft player needs — game versions, mods, skins, worlds and more — into a single, scriptable, zero-GUI application.

No windows, no icons, no tray icon. Just a terminal. If someone glances at the screen, it looks like you are busy programming.

## Usage

```sh
meowlaunchcli            # no arguments: print the help list (busybox-style)
meowlaunchcli version    # MeowLauncherCli v0.0.1 build <git>
meowlaunchcli <command>  # dispatch a subcommand
```

## Commands

| Command    | Purpose |
| ---------- | ------- |
| `help`     | Show this help list |
| `version`  | Show MeowLauncherCli version |
| `install`  | Install game, mod loaders, mods, packs, modpacks |
| `remove`   | Remove game, mod loaders, mods, packs, modpacks |
| `update`   | Refresh game metadata sources |
| `search`   | Search available game / loader / mod / pack versions |
| `game`     | Manage and switch game versions |
| `launch`   | Launch the game |
| `mod`      | Manage mods (current game version) |
| `account`  | Manage accounts |
| `world`    | Manage worlds (current game version) |
| `resource` | Manage resource packs (current game version) |
| `config`   | Show or edit configuration |

## Localization

The UI language follows the system locale (`$LANG`) automatically. English, Traditional Chinese and Japanese strings are embedded into the binary at build time; build a single-language binary, e.g. `LANGS="zh-TW" ./scripts/build.sh`.

## Building

```sh
./scripts/build.sh     # Linux: musl-gcc, statically linked
./scripts/test.sh       # build + CLI smoke tests
```

On Windows (MSYS2/MinGW) the same script uses `gcc` and produces `meowlaunchcli.exe`.

Environment overrides: `CC`, `CFLAGS`, `LDFLAGS`, `LANGS`, `GIT_VERSION`. Instrument a coverage build with `COVERAGE=1`.

## Configuration

Copy `config.json.example` to `config.json` and adjust it to your needs.

## Status

Early development. `help` and `version` are implemented; the remaining commands land module by module.

## License

Licensed under the **GNU Affero General Public License v3.0** (AGPL-3.0). See [LICENSE](LICENSE) for the full text.
