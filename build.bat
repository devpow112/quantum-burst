@echo off
setlocal

set "TYPE=%1"
set "REVISION=%2"

if "%TYPE%" NEQ "" (
  set "TYPE_COMMAND=-BuildType %TYPE%"
)

if "%REVISION%" NEQ "" (
  set "REVISION_COMMAND=-Revision %REVISION%"
)

pushd "%~dp0"
powershell -ExecutionPolicy Bypass -File build.ps1 -Rebuild %TYPE_COMMAND% %REVISION_COMMAND%
popd
