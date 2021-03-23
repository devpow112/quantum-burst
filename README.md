# Quantum Burst

[![License]](LICENSE)
[![CI][CI Badge]][CI Workflow]

A [bullet hell] style space shooter written for the [Sega Genesis].

## Development

Development can be done on **Windows** only, and has been tested only on
**Windows 10**.

### Prerequisites

- [Python](https://www.python.org/downloads/windows)
- [Java](https://java.com/en/download/manual.jsp)

### Build

Build scripts are provided for both `powershell` and `cmd`. They will process
all the files under `res`, `inc` and `src` to produce a `rom.bin` file in the
`out` directory. After this is done the checksum will be calculated and
`rom_final.bin` generated with the correct checksum.

#### powershell

```pwsh
.\build.ps1 [-BuildType] [-Revision] [-Rebuild]
```

- `-BuildType` - Can be any of `Debug`, `Release`, `Clean` or `ASM`. If omitted
will default to `Release`
- `-Revision` - Can be any integer value of `99` or less. If omitted will
default to `99`
- `-Rebuild` - Will run a `Clean` before a `Debug` or `Release` build. This has
no effect on other build types.

#### cmd

```cmd
build.bat [build-type] [revision]
```

- `build-type` - Can be any of `Debug`, `Release`, `Clean` or `ASM`. If omitted
will default to same default as **powershell** script.
- `revision` - Can be any integer value of `99` or less. If omitted will default
to same default as **powershell** script.

> **Note** this method will always do a rebuild.

<!-- links -->
[License]: https://img.shields.io/github/license/devpow112/quantum-burst?label=License
[CI Badge]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml/badge.svg?branch=main
[CI Workflow]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml?query=branch%3Amain
[Bullet Hell]: https://en.wikipedia.org/wiki/Shoot_%27em_up#Bullet_hell
[Sega Genesis]: https://en.wikipedia.org/wiki/Sega_Genesis
