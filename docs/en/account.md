# Account management

MeowLauncherCli stores local offline accounts under `<exedir>/.meowlaunch/data/users.json`.

## Add a local account

```
meowlaunchcli account local add <username>
```

Creates an entry with:
- `name`: the username you passed
- `type`: `"local"` (offline)
- `uuid`: a random v4-style UUID (8-4-4-4-12 hex groups)

## Remove a local account

```
meowlaunchcli account local del <username>
```

Removes the matching entry. Prints "Account not found" when no match.

## List local accounts

```
meowlaunchcli account local show
```

Prints each local account name on its own line.

## Storage layout

```json
{
  "users": [
    {"name":"alice","type":"local","uuid":"12345678-1234-..."}
  ]
}
```

Path: `<exedir>/.meowlaunch/data/users.json`.
