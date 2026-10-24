# Game version management

The current game version is stored in `<exedir>/.meowlaunch/config.json`.
Installed versions are directories under `<exedir>/.minecraft/versions/`.

## Show help

```
meowlaunchcli game help
```

## Switch

```
meowlaunchcli game switch <version>
```

Writes `{"game_version":"<version>"}` to `.meowlaunch/config.json`.

## Show current

```
meowlaunchcli game current
```

Prints the active version, or `(none)` if not set.

## List installed

```
meowlaunchcli game show
```

Prints each subdirectory under `.minecraft/versions/`.
