@echo off

set "TYPE=%1"
set "REVISION=%2"

if "%TYPE%" EQU "" (
  set "TYPE=Release"
)

if "%REVISION%" EQU "" (
  set "REVISION=99"
)

pushd "%~dp0"
powershell -File build.ps1 -BuildType %Type% -Revision %REVISION%
popd
