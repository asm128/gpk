$ErrorActionPreference = 'Stop'

$isAdministrator = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if(-not $isAdministrator) {
	throw 'Run this script as administrator.'
}

$projectDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$releaseBinary = [IO.Path]::GetFullPath((Join-Path $projectDirectory '..\..\x64.Release\gpk_applock.exe'))
$installedBinary = Join-Path $env:SystemRoot 'System32\gpk_applock.exe'

if(-not (Test-Path -LiteralPath $releaseBinary -PathType Leaf)) {
	throw "Release binary not found: $releaseBinary"
}

Copy-Item -LiteralPath $releaseBinary -Destination $installedBinary -Force
& $installedBinary --install
if($LASTEXITCODE) {
	exit $LASTEXITCODE
}

Write-Host "Installed and configured: $installedBinary"
