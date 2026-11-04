# Compiles a sketch and flashes it to the servo adapter over the UPDI Friend.
#
# Usage, from the firmware folder:
#   .\flash.ps1 servo_adapter
#   .\flash.ps1 bringup\bringup_01_leds
#   .\flash.ps1 servo_adapter -Port COM5
#
# The Arduino IDE's Upload Using Programmer doesn't work for the AVR DU in
# DxCore 1.6.2 (some DU fuse values are missing from boards.txt), so this
# calls DxCore's SerialUPDI script directly. It writes flash only and leaves
# the chip's fuses at their factory defaults. 57600 baud is used because
# writes at 230400 silently fail with the UPDI Friend.

param(
    [Parameter(Mandatory = $true)][string]$Sketch,
    [string]$Port = "COM3"
)

$ErrorActionPreference = "Stop"

$ArduinoCli = "C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
$Python = "$env:LOCALAPPDATA\Arduino15\packages\megaTinyCore\tools\python3\3.7.2-post1\python3.exe"
$ProgPy = "$env:LOCALAPPDATA\Arduino15\packages\DxCore\hardware\megaavr\1.6.2\tools\prog.py"
$Fqbn = "DxCore:megaavr:avrdu:chip=avr64du32,clock=24internal,flmap=unlocked"

foreach ($tool in @($ArduinoCli, $Python, $ProgPy)) {
    if (-not (Test-Path $tool)) {
        throw "Not found: $tool"
    }
}

$SketchDir = Resolve-Path (Join-Path $PSScriptRoot $Sketch)
$SketchName = Split-Path $SketchDir -Leaf
$BuildDir = Join-Path $env:TEMP "servo-adapter-build\$SketchName"

Write-Host "Compiling $SketchName..."
& $ArduinoCli compile -b $Fqbn --build-path $BuildDir $SketchDir
if ($LASTEXITCODE -ne 0) { throw "Compile failed" }

Write-Host "Flashing $SketchName on $Port..."
& $Python -u $ProgPy -t uart -u $Port -b 57600 -wd 2 -d avr64du32 "-f$BuildDir\$SketchName.ino.hex" -a write
if ($LASTEXITCODE -ne 0) { throw "Flash failed" }

Write-Host "Done."
