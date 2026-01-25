# Quantum Burst

[![License][License]](LICENSE)
[![Release][Release Badge]][Release Workflow]

A [bullet hell] style space shooter written for the [Sega Genesis].

## Development

Development is set up for **Windows** only and has been tested only on **Windows
10** and **Windows 11**. It is assumed that you have [Git] and Git Bash
installed.

### Prerequisites

- [Python]
- [Java]

### Build

A build script is provided that will process all the files under `game/res`,
`game/inc` and `game/src` to produce a `rom.bin` file in the `game/out`
directory. After this is done the checksum will be calculated and
`rom_final.bin` generated with the correct checksum.

```bash
./build.sh [-b|--build-type <build-type>] [-r|--revision <revision>] [--rebuild]
```

- `-b|--build-type` - Can be any of `Debug`, `Release`, `Clean` or `ASM`. If
  omitted will default to `Release`.
- `-r|--revision` - Can be any integer value of `99` or less. If omitted will
  default to `99`.
- `--rebuild` - Will run a `Clean` before a `Debug` or `Release` build. This
  has no effect on other build types.

### Debugging

Sadly, most tooling doesn't allow good, feature-rich debugging; it's recommended
to use the `log()` helper along with the [Gens KMod] emulator to do log-based
debugging.

<!-- links -->
[License]: https://img.shields.io/github/license/devpow112/quantum-burst?label=License
[Release Badge]: https://github.com/devpow112/quantum-burst/actions/workflows/release.yml/badge.svg?branch=main
[Release Workflow]: https://github.com/devpow112/quantum-burst/actions/workflows/release.yml?query=branch%3Amain
[Bullet Hell]: https://en.wikipedia.org/wiki/Shoot_%27em_up#Bullet_hell
[Sega Genesis]: https://en.wikipedia.org/wiki/Sega_Genesis
[Git]: https://git-scm.com/downloads
[Python]: https://www.python.org/downloads/windows
[Java]: https://java.com/en/download/manual.jsp
[Gens KMod]: https://segaretro.org/Gens_KMod
