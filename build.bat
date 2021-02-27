@echo off

pushd "%~dp0"
set "TYPE=%1"
set "GDK=%CD%/ext/sgdk"
set "GDK=%GDK:\=/%"
set "GDK_WIN=%CD%\ext\sgdk"
set "SGCC=%CD%\ext\sgcc"
set "PATH=%PATH%;%CD%\sgdk\bin"

if "%TYPE%" EQU "" (
  set "TYPE=release" 
) else if /i "%TYPE%" EQU "debug" (
  set "TYPE=debug"
) else if /i "%TYPE%" EQU "release" (
  set "TYPE=release"
) else if /i "%TYPE%" EQU "clean" (
  set "TYPE=clean"
) else if /i "%TYPE%" EQU "asm" (
  set "TYPE=asm"
) else (
  echo Incorrect build type. Must be debug, release, clean or asm
  goto ERROR
)

copy /b "%CD%\src\boot\rom_head.c" +,,"%CD%\src\boot\rom_head.c" >NUL 2>&1
"%GDK_WIN%\bin\make" -f "%GDK_WIN%\makefile.gen" %TYPE%

if %ERRORLEVEL% NEQ 0 (
  echo Build failed!
  goto ERROR
)

if "%TYPE%" EQU "debug" (
  goto CORRECT_CHECKSUM
) else if "%TYPE%" EQU "release" (
  goto CORRECT_CHECKSUM
)

goto END

:CORRECT_CHECKSUM

python "%SGCC%\sgcc.py" "out\rom.bin"

if %ERRORLEVEL% NEQ 0 (
  echo Checksum correction failed!
  goto ERROR
)

goto END

:ERROR
popd
exit /b 1

:END

popd
exit /b 0
