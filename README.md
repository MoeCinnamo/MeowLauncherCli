# MeowLauncherCli

[English](README.md) [繁體中文](README-zh_TW.md) [日本語](README-ja_JP.md)

> A Minecraft launcher that Chinese-style parents cannot report — it lives entirely in the terminal, and to the kind of parent who sees a threat to their child in every pixel, it just looks like serious developer work.

## Overview

MeowLauncherCli is a **pure command-line launcher for Minecraft**. It aims to bring together nearly everything a Minecraft player needs — game versions, mods, skins, worlds and more — into a single, scriptable, zero-GUI application.

No windows, no icons, no tray icon. Just a terminal. If someone glances at the screen, it looks like you are busy programming.

## Features

- **Version management** — install and switch between Minecraft releases and snapshots.
- **Mod management** — install, enable and disable mods from the command line.
- **Resource packs & shaders** — manage texture packs, resource packs and shader packs.
- **Skin management** — change skins without opening any GUI.
- **World & save management** — list, back up and restore worlds.
- **Accounts** — offline accounts supported; Microsoft account login planned.
- **Launch configuration** — JVM arguments, game directory and memory limits via `config.json`.
- **Server bookmarks** — save server addresses and join them quickly.
- **Localized UI** — English, Traditional Chinese and Japanese, driven by `lang/*.json`.
- **Scriptable** — every action is a subcommand, so it composes naturally with shell scripts and automation.

## Architecture

```
cmd/main/main.c           entry point; only calls app_run()
internal/app/             application lifecycle: init, event loop, cleanup
internal/                 internal modules, hidden from external consumers
lang/                     UI strings: en.json, zh-TW.json, ja-JP.json
scripts/                  build and test scripts
tests/                    test suite
```

MeowLauncherCli is written in **C11** and targets **musl libc**, so the result can ship as a small, statically linked binary. Modules are isolated under `internal/`, internal functions are `static` by default, and the project prefers the C standard library over third-party dependencies.

## Building

```sh
./scripts/build.sh
```

A C11 compiler is required; a musl toolchain (e.g. `musl-gcc`) is recommended for the target environment. The build scripts are still taking shape during early development.

## Configuration

Copy `config.json.example` to `config.json` and adjust it to your needs. All user-facing strings are externalized under `lang/`, and the interface language follows your locale selection.

## Status

Early development. The module boundaries and skeleton are in place; individual features land module by module.

## License

Licensed under the **GNU Affero General Public License v3.0** (AGPL-3.0). See [LICENSE](LICENSE) for the full text.
