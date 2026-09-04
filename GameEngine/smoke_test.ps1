$ErrorActionPreference = 'Continue'
$env:Path = 'C:\msys64\mingw64\bin;' + $env:Path
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms

$bin  = 'C:\Users\Harah\.gemini\antigravity\scratch\GameEngine\build-mingw\bin'
$out  = 'C:\Users\Harah\.gemini\antigravity\scratch\GameEngine'
$wshell = New-Object -ComObject wscript.shell

function Snap([string]$name, [System.Diagnostics.Process]$proc) {
    Start-Sleep -Seconds 8
    try { [void]$wshell.AppActivate($proc.Id) } catch {}
    Start-Sleep -Milliseconds 800
    try {
        $bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
        $bmp = New-Object System.Drawing.Bitmap $bounds.Width, $bounds.Height
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        $g.CopyFromScreen($bounds.Location, [System.Drawing.Point]::Empty, $bounds.Size)
        $file = Join-Path $out ("smoke_" + $name + ".png")
        $bmp.Save($file, [System.Drawing.Imaging.ImageFormat]::Png)
        $g.Dispose(); $bmp.Dispose()
        Write-Output ("SNAPSHOT saved: " + $file)
    } catch {
        Write-Output ("SNAP FAILED for " + $name + " : " + $_.Exception.Message)
    }
    $proc.Refresh()
    $alive = -not $proc.HasExited
    $cpu = 0
    if ($alive) { $cpu = [math]::Round($proc.TotalProcessorTime.TotalSeconds, 1) }
    Write-Output ("RESULT " + $name + " alive=" + $alive + " title='" + $proc.MainWindowTitle + "' cpuSeconds=" + $cpu)
}

$p1 = Start-Process (Join-Path $bin 'GameEngineEditor.exe') -WorkingDirectory $bin -PassThru
Snap 'editor' $p1
if (-not $p1.HasExited) { Stop-Process -Id $p1.Id -Force }

$p2 = Start-Process (Join-Path $bin 'GameEngineRuntime.exe') -WorkingDirectory $bin -PassThru
Snap 'runtime' $p2
if (-not $p2.HasExited) { Stop-Process -Id $p2.Id -Force }

Write-Output 'SMOKE TEST DONE'