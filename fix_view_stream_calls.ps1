$ErrorActionPreference = 'Stop'
$encoding = [Text.Encoding]::GetEncoding(28591)
$files = @(
    'gpk/gpk_connection.cpp',
    'gpk/gpk_ro_gnd.cpp',
    'gpk/gpk_ro_rsm.cpp'
)
foreach ($file in $files) {
    $path = Join-Path (Get-Location) $file
    $lines = [IO.File]::ReadAllLines($path, $encoding)
    $changed = $false
    for ($i = 0; $i -lt $lines.Length; $i++) {
        $line = $lines[$i]
        foreach ($method in @('read_pod', 'write_pod')) {
            $needle = ".$method("
            $start = $line.IndexOf($needle, [StringComparison]::Ordinal)
            if ($start -lt 0) { continue }
            $open = $start + $needle.Length
            $depth = 0
            $comma = -1
            $close = -1
            for ($j = $open; $j -lt $line.Length; $j++) {
                switch ($line[$j]) {
                    '(' { $depth++ }
                    ')' { if ($depth -eq 0) { $close = $j; break } else { $depth-- } }
                    ',' { if ($depth -eq 0 -and $comma -lt 0) { $comma = $j } }
                }
                if ($close -ge 0) { break }
            }
            if ($comma -lt 0 -or $close -lt 0) { continue }
            $args = $line.Substring($open, $close - $open)
            $viewType = if ($method -eq 'write_pod') { '::gpk::vcu0_t' } else { '::gpk::view' }
            $replacement = $viewType + '{' + $args + '}'
            $line = $line.Substring(0, $open) + $replacement + $line.Substring($close)
            $changed = $true
        }
        $lines[$i] = $line
    }
    if ($changed) {
        [IO.File]::WriteAllLines($path, $lines, $encoding)
        Write-Output $file
    }
}
