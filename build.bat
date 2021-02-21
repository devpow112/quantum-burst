@echo off

pushd "%~dp0"

set "TYPE=%1"
set "GDK=%CD%/ext/sgdk"
set "GDK=%GDK:\=/%"
set "GDK_WIN=%CD%\ext\sgdk"
set "PATH=%PATH%;%CD%\sgdk\bin"

if "%TYPE%" EQU "" (
  set "TYPE=release"
)

if "%TYPE%" NEQ "debug" (
  if "%TYPE%" NEQ "asm" (
    if "%TYPE%" NEQ "release" (
      exit /b 1
    )
  )
)

copy /b "src\boot\rom_head.c" +,,"src\boot\rom_head.c" >NUL 2>&1

if "%TYPE%" NEQ "asm" (
  "%GDK_WIN%\bin\make" -f "%GDK_WIN%\makefile.gen" clean
)

"%GDK_WIN%\bin\make" -f "%GDK_WIN%\makefile.gen" %TYPE%

popd
