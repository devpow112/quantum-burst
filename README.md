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

```cmd
build.bat [type]
```

The `[type]` parameter can be either `debug`, `release`, `clean` or `asm`. If
omitted it will default to `release`. This will process all the files under
`res`, `inc` and `src` to produce a `rom.bin` file in the `out` directory. After
this is done the checksum will be calculated and the `rom.bin` patched.

<!-- links -->
[CI Badge]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml/badge.svg?branch=main
[CI Workflows]: https://github.com/devpow112/quantum-burst/actions/workflows/ci.yml
[Sega Genesis]: https://en.wikipedia.org/wiki/Sega_Genesis
