# Security Policy

## Supported versions

Only the latest commit on `main` receives security fixes.

## Reporting a vulnerability

Please open a private security advisory on GitHub, or email the maintainer
listed in the repository. Do not file a public issue for undisclosed
vulnerabilities.

## Scope

- The launcher writes only to `~/.meowlaunch/` (`%USERPROFILE%\.meowlaunch\`
  on Windows). It does not contact any network service in the current
  release.
- Local accounts are stored in plaintext in `users.json`; passwords are not
  stored (offline mode only).
- UUIDs are generated with `rand()` — fine for local offline identifiers,
  not for cryptographic use.

## Out of scope

- Compromised Minecraft session tokens (handled by Mojang/Microsoft).
- Vulnerabilities in third-party launchers or mods.
