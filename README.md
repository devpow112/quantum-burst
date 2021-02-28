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

- `[build type]`: Can be any of `Debug`, `Release`, `Clean` or `ASM`. If omitted
it will default to `Release`. Case does not matter
- `[version]`: Can be any 2 digit integer value. If ommited it will default to
`99`

#### powershell

```pwsh
.\build.ps1 [-BuildType] [-Version]
```

- `[-BuildType]`: Can be any of `Debug`, `Release`, `Clean` or `ASM`. If omitted
it will default to `release`. Case does not matter
- `[-Version]`: Can be any 2 digit integer value. If ommited it will default to
`99`

<!-- links -->
[CI Badge]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml/badge.svg?branch=main
[CI Workflows]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml
[Sega Genesis]: https://en.wikipedia.org/wiki/Sega_Genesis
