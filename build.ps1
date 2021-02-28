param(
  [Parameter(Mandatory=$false)][Int] $version = 100,
  [Parameter(Mandatory=$false)][String] $buildType = 'Release'
)

$callingLocation = Get-Location

try {
  $buildType = $buildType.ToLower()

  if (@('debug', 'release', 'clean', 'asm') -NotContains $buildType) {
    throw "Invalid 'BuildType', allowed values are: debug, release, clean, asm"
  }
  
  if ($version -Gt 100) {
    throw "Invalid 'Version', must be a value of 100 or less"    
  }
  
  Set-Location $PSScriptRoot

  $year4Digits = (Get-Date -Format 'yyyy')
  $month3Letters = (Get-Date -Format 'MMM').ToUpper()
  $version2Digits = ($version - 1).ToString().PadLeft(2,'0')
  $headerContents = Get-Content -Path src\boot\rom_head.c.in
  $headerContents = $headerContents.Replace('{Year4Digits}', $year4Digits)
  $headerContents = $headerContents.Replace('{Month3Letters}', $month3Letters)
  $headerContents = $headerContents.Replace('{Version2Digits}', $version2Digits)
  
  Set-Content -Path src\boot\rom_head.c -Value $headerContents  
  
  if (Test-Path -Path 'out\rom_cc.bin') {
    Remove-Item 'out\rom_cc.bin'
  }
  
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
