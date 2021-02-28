@echo off

set "TYPE=%1"
set "VERSION=%2"

if "%TYPE%" EQU "" (
  set "TYPE=Release"
)

if "%VERSION%" EQU "" (
  set "VERSION=100"
)

pushd "%~dp0"
powershell -File build.ps1 -BuildType %Type% -Version %VERSION%
popd
