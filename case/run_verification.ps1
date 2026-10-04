param(
    [string]$Scad = ".\alarm_case.scad",
    [string]$OpenSCAD = "",
    [string]$Reference = ".\qa_mesh_v4.json",
    [string]$Output = ".\verification_output",
    [switch]$NoInstall
)

$ErrorActionPreference = "Stop"
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path

function Find-Python {
    if (Get-Command py -ErrorAction SilentlyContinue) {
        return @{ Exe = "py"; Prefix = @("-3") }
    }
    if (Get-Command python -ErrorAction SilentlyContinue) {
        return @{ Exe = "python"; Prefix = @() }
    }
    throw "No encuentro Python 3. Instala Python 3 y marca 'Add python.exe to PATH'."
}

$Py = Find-Python
$Venv = Join-Path $Here ".venv"

if (-not (Test-Path $Venv)) {
    Write-Host "Creando entorno virtual en $Venv"
    & $Py.Exe @($Py.Prefix) -m venv $Venv
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

$VenvPython = Join-Path $Venv "Scripts\python.exe"
if (-not (Test-Path $VenvPython)) {
    # Linux/macOS fallback if this script is run under pwsh
    $VenvPython = Join-Path $Venv "bin/python"
}

if (-not $NoInstall) {
    Write-Host "Instalando/actualizando dependencias..."
    & $VenvPython -m pip install --disable-pip-version-check -r (Join-Path $Here "requirements.txt")
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

$ArgsList = @(
    (Join-Path $Here "verify_all.py"),
    "--scad", $Scad,
    "--out", $Output
)

if ($OpenSCAD) {
    $ArgsList += @("--openscad", $OpenSCAD)
}
if ($Reference -and (Test-Path $Reference)) {
    $ArgsList += @("--reference", $Reference)
}

& $VenvPython @ArgsList
exit $LASTEXITCODE
