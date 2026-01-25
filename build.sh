#!/bin/bash

set -e

# Default values
REVISION=99
BUILD_TYPE="release"
REBUILD=false

# Parse arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -r|--revision)
      REVISION="$2"
      if ! [[ "$REVISION" =~ ^[0-9]+$ ]] || [ "$REVISION" -lt 0 ] || [ "$REVISION" -gt 99 ]; then
        echo "Error: Revision must be between 0 and 99" >&2
        exit 1
      fi
      shift 2
      ;;
    -b|--build-type)
      BUILD_TYPE="${2,,}" # lowercase
      if [[ ! "$BUILD_TYPE" =~ ^(debug|release|clean|asm)$ ]]; then
        echo "Error: Build type must be Debug, Release, Clean, or ASM" >&2
        exit 1
      fi
      shift 2
      ;;
    --rebuild)
      REBUILD=true
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [-r|--revision <0-99>] [-b|--build-type <Debug|Release|Clean|ASM>] [--rebuild]"
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
  esac
done

# Get script directory
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXTERNALS_ROOT="$ROOT/externals"
GAME_ROOT="$ROOT/game"

# Generate ROM header
YEAR_4_DIGITS=$(date +%Y)
MONTH_3_LETTERS=$(date +%b | tr '[:lower:]' '[:upper:]')
REVISION_2_DIGITS=$(printf "%02d" "$REVISION")
CONTENTS=$(<"$GAME_ROOT/cfg/rom_header.c.in")
CONTENTS="${CONTENTS//\{Year4Digits\}/$YEAR_4_DIGITS}"
CONTENTS="${CONTENTS//\{Month3Letters\}/$MONTH_3_LETTERS}"
CONTENTS="${CONTENTS//\{Revision2Digits\}/$REVISION_2_DIGITS}"

echo "$CONTENTS" > "$GAME_ROOT/src/rom_header.c"

if [[ "$BUILD_TYPE" == "debug" || "$BUILD_TYPE" == "release" ]]; then
  IS_BUILD=true
else
  IS_BUILD=false
fi

# Remove checksum corrected ROM
if { [ "$IS_BUILD" = true ] && [ "$REBUILD" = true ]; } || [ "$BUILD_TYPE" = "clean" ]; then
  if [ -f "$GAME_ROOT/out/rom_final.bin" ]; then
    rm "$GAME_ROOT/out/rom_final.bin"
  fi
fi

# Build lib
SGDK_ROOT="$EXTERNALS_ROOT/sgdk"
PATH="$SGDK_ROOT/bin:$PATH"

cd "$SGDK_ROOT"

make -f "$SGDK_ROOT/makelib.gen" "$BUILD_TYPE"

# Build game
cd "$GAME_ROOT"

if [ "$IS_BUILD" = true ] && [ "$REBUILD" = true ]; then
  make -f "$SGDK_ROOT/makefile.gen" clean
fi

make -f "$SGDK_ROOT/makefile.gen" "$BUILD_TYPE"

# Correct ROM checksum
if [ "$IS_BUILD" = true ]; then
  echo "Correcting checksum"

  python "$EXTERNALS_ROOT/sgcc/sgcc.py" -s final "$GAME_ROOT/out/rom.bin"
fi

echo "Build completed successfully"
