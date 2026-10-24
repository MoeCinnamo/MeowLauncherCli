# Account management

MeowLauncherCli stores local offline accounts under `~/.meowlaunch/data/users.json`.

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

## Storage layout

```json
{
  "users": [
    {"name":"alice","type":"local","uuid":"12345678-1234-..."}
  ]
}
```

Windows path: `C:\Users\<you>\.meowlaunch\data\users.json`.
