# Quantum Burst

[![CI][CI Badge]][CI Workflows]

A "bullet hell" style space shooter written for the [Sega Genesis].

## Development

Development can be done on **Windows** only, and has been tested only on
**Windows 10**.

### Prerequisites

- [Python](https://www.python.org/downloads/windows)
- [Java](https://java.com/en/download/manual.jsp)

### Build

Build scripts are provided for both `cmd` and `powershell`. They will process
all the files under `res`, `inc` and `src` to produce a `rom.bin` file in the
`out` directory. After this is done the checksum will be calculated and
`rom_cc.bin` generated with the correct chechsum.

#### cmd

```cmd
build.bat [build type] [version]
```

- `[build type]` - Can be any of `Debug`, `Release`, `Clean` or `ASM`. Case does
not matter. If omitted will default to `Release`
- `[version]` - Can be any integer value of 100 or less. If omitted will default
to `100`

#### powershell

```pwsh
.\build.ps1 [-BuildType] [-Version]
```

- `[-BuildType]` - Can be any of `Debug`, `Release`, `Clean` or `ASM`. Case does
not matter. If omitted will default to `Release`
- `[-Version]` - Can be any integer value of 100 or less. If omitted will
default to `100`

<!-- links -->
[CI Badge]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml/badge.svg?branch=main
[CI Workflows]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml
[Sega Genesis]: https://en.wikipedia.org/wiki/Sega_Genesis
