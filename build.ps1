param(
  [ValidateRange(0, 99)]
  [Parameter(Mandatory = $false)]
  [Int] $revision = 99,
  [ValidateSet('Debug', 'Release', 'Clean', 'ASM')]
  [Parameter(Mandatory = $false)]
  [String] $buildType = 'Release'
)

$callingLocation = Get-Location

try {
  Set-Location $PSScriptRoot

  $year4Digits = (Get-Date -Format 'yyyy')
  $month3Letters = (Get-Date -Format 'MMM').ToUpper()
  $revision2Digits = $revision.ToString().PadLeft(2, '0')
  $headerContents = Get-Content -Path src\boot\rom_head.c.in
  $headerContents = $headerContents.Replace('{Year4Digits}', $year4Digits)
  $headerContents = $headerContents.Replace('{Month3Letters}', $month3Letters)
  $headerContents = $headerContents.Replace(
    '{Revision2Digits}',
    $revision2Digits
  )

  Set-Content -Path src\boot\rom_head.c -Value $headerContents

  if (Test-Path -Path 'out\rom_cc.bin') {
    Remove-Item 'out\rom_cc.bin'
  }

  $buildType = $buildType.ToLower()

  Invoke-Expression "ext\sgdk\bin\make -f ext\sgdk\makefile.gen $buildType"

  if ($lastExitCode -Ne 0 -Or -Not $?) {
    throw "Build failed!"
  }

  if (@('debug', 'release') -Contains $buildType) {
    Write-Host 'Correcting checksum'
    Invoke-Expression 'python ext\sgcc\sgcc.py out\rom.bin'
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
