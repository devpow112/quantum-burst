param(
  [ValidateRange(0, 99)]
  [Parameter(Mandatory = $false)]
  [Int] $revision = 99,
  [ValidateSet('Debug', 'Release', 'Clean', 'ASM')]
  [Parameter(Mandatory = $false)]
  [String] $buildType = 'Release',
  [Parameter(Mandatory = $false)]
  [Switch] $rebuild = $false
)

$callingLocation = Get-Location

try {
  $buildRoot = $PSScriptRoot

  # generate ROM header
  $year4Digits = (Get-Date -Format 'yyyy')
  $month3Letters = (Get-Date -Format 'MMM').ToUpper()
  $revision2Digits = $revision.ToString().PadLeft(2, '0')
  $contents = Get-Content -Path "$buildRoot\src\boot\rom_head.c.in"
  $contents = $contents.Replace('{Year4Digits}', $year4Digits)
  $contents = $contents.Replace('{Month3Letters}', $month3Letters)
  $contents = $contents.Replace('{Revision2Digits}', $revision2Digits)

  Set-Content -Path "$buildRoot\src\boot\rom_head.c" -Value $contents

  $buildType = $buildType.ToLower()
  $isBuild = @('debug', 'release') -Contains $buildType

  # remove checksum corrected ROM
  if ($isBuild -And $rebuild -Or $buildType -Eq 'clean') {
    if (Test-Path -Path "$buildRoot\out\rom_final.bin") {
      Remove-Item "$buildRoot\out\rom_final.bin"
    }
  }

  $extRoot = Join-Path $buildRoot ext

  # run build
  Set-Location $buildRoot

  if ($isBuild -And $rebuild) {
    & "$extRoot\sgdk\bin\make" -f "$extRoot\sgdk\makefile.gen" clean

    if ($lastExitCode -Ne 0 -Or -Not $?) {
      throw "Clean failed!"
    }
  }

  & "$extRoot\sgdk\bin\make" -f "$extRoot\sgdk\makefile.gen" $buildType

  if ($lastExitCode -Ne 0 -Or -Not $?) {
    throw "Build failed!"
  }

  # correct ROM checksum
  if ($isBuild) {
    Write-Information 'Correcting checksum'

    & python "$extRoot\sgcc\sgcc.py" -s final "$buildRoot\out\rom.bin"
  }

  if ($lastExitCode -Ne 0 -Or -Not $?) {
    throw "Checksum correction failed!"
  }
} catch {
  Write-Error $_

  exit 1
} finally {
  Set-Location $callingLocation
}
