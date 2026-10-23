# Security Policy

## Supported Versions

MeowLauncherCli is in early development. Security fixes land on the `main` development branch. No stable release series exists yet; once releases begin, only the latest minor version will receive security patches.

| Version | Supported |
| ------- | --------- |
| dev (`main`) | ✅ |
| `< 1.0` (no releases yet) | ❌ |

## Reporting a Vulnerability

**Do not open a public issue for security problems.**

Please report vulnerabilities privately via **GitHub Security Advisories** — open this repository's **Security → Report a vulnerability** page. Reports are kept confidential until a fix is ready.

Your report should include:

- A description of the vulnerability and its impact
- Steps to reproduce (commands, configuration, environment)
- The affected version or commit hash
- Your severity assessment and, if you have one, a suggested fix

### What to expect

- Acknowledgement within **72 hours**
- Initial assessment within **7 days**
- We will keep you informed of progress. You will be credited in the release notes unless you prefer to remain anonymous.

## Scope

**In scope:**

- Remote code execution or arbitrary file write, including path traversal while downloading or installing game versions, mods or packs
- Leakage of credentials, session tokens or secret values stored in the configuration
- Insecure download channels, silent tampering of downloaded artifacts
- Memory-safety bugs (memory corruption, undefined behavior) reachable from untrusted input

**Out of scope:**

- Attacks that require physical access to the machine
- Social engineering
- Vulnerabilities in third-party software (Java runtime, Minecraft itself, mods, servers) — report those to their upstream projects
- Misuse by someone who already has shell access to your account

## Security Design Goals

These are the project's security commitments for features under development:

- **No passwords, ever.** MeowLauncherCli will never request or store your Mojang/Microsoft account password. Access tokens stay on your machine and are only sent to Minecraft's official authentication endpoints.
- **HTTPS only.** All downloads use HTTPS, and artifacts are verified against official metadata where available.
- **No telemetry.** No usage data leaves your machine.
- **Explicit error handling.** No silent failure paths in file operations; errors are surfaced to the user.
