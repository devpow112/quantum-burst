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
  Set-Location $PSScriptRoot

  # generate ROM header
  $year4Digits = (Get-Date -Format 'yyyy')
  $month3Letters = (Get-Date -Format 'MMM').ToUpper()
  $revision2Digits = $revision.ToString().PadLeft(2, '0')
  $contents = Get-Content -Path src\boot\rom_head.c.in
  $contents = $contents.Replace('{Year4Digits}', $year4Digits)
  $contents = $contents.Replace('{Month3Letters}', $month3Letters)
  $contents = $contents.Replace('{Revision2Digits}', $revision2Digits)

  Set-Content -Path src\boot\rom_head.c -Value $contents

  $build = @('debug', 'release') -Contains $buildType

  # remove checksum corrected ROM
  if (Test-Path -Path 'out\rom_final.bin') {
    Remove-Item 'out\rom_final.bin'
  }

  # run build
  $buildType = $buildType.ToLower()

  if ($build -And $rebuild) {
    Invoke-Expression "ext\sgdk\bin\make -f ext\sgdk\makefile.gen clean"
  }

  Invoke-Expression "ext\sgdk\bin\make -f ext\sgdk\makefile.gen $buildType"

  if ($lastExitCode -Ne 0 -Or -Not $?) {
    throw "Build failed!"
  }

  # correct ROM checksum
  if ($build) {
    Write-Host 'Correcting checksum'
    Invoke-Expression 'python ext\sgcc\sgcc.py -s final out\rom.bin'
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
