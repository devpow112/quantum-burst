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

if "%TYPE%" EQU "Debug" (
  set "TYPE=debug"
) else if "%TYPE%" EQU "Release" (
  set "TYPE=release"
)

if "%TYPE%" NEQ "debug" (
  if "%TYPE%" NEQ "asm" (
    if "%TYPE%" NEQ "release" (
      goto ERROR
    )
  )
)

copy /b "%CD%\src\boot\rom_head.c" +,,"%CD%\src\boot\rom_head.c" >NUL 2>&1

if "%TYPE%" NEQ "asm" (
  "%GDK_WIN%\bin\make" -f "%GDK_WIN%\makefile.gen" clean
  
  if %ERRORLEVEL% NEQ 0 (
    goto ERROR
  )
)

"%GDK_WIN%\bin\make" -f "%GDK_WIN%\makefile.gen" %TYPE%

if %ERRORLEVEL% NEQ 0 (
  goto ERROR
)
  
goto SUCCESS

:ERROR

popd
exit /b 1

:SUCCESS

popd
