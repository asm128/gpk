$ErrorActionPreference = 'Stop'
$root = (Resolve-Path '.').Path
$games = (Resolve-Path '..\gpk_games').Path
$map = @{}
$widths = @{ u8='u0'; u16='u1'; u32='u2'; u64='u3'; i8='s0'; i16='s1'; i32='s2'; i64='s3'; f32='f2'; f64='f3' }
foreach ($width in $widths.Keys) {
    $new = $widths[$width] + '_t'
    foreach ($prefix in @('ast','rect','rect2','tri2','minmaxtri2')) { $map[$prefix + $width] = $prefix + $new }
    $map['v' + $width] = 'v' + $new
    $map['vc' + $width] = 'vc' + $new
    $map['astatic' + $width] = 'ast' + $new
}
$map['astchar'] = 'astsc_t'
$map['astuchar'] = 'astuc_t'
$map['astc'] = 'astsc_t'
$map['astuc'] = 'astuc_t'
$map['rect2f'] = 'rect2f2_t'
$map['rect2d'] = 'rect2f3_t'
$map['tri2char'] = 'tri2sc_t'
$map['tri2uchar'] = 'tri2uc_t'
$map['vchar'] = 'vsc_t'
$map['vuchar'] = 'vuc_t'
$map['vcchar'] = 'vcsc_t'
$map['vcuchar'] = 'vcuc_t'
$map['vi16'] = 'vs1_t'
$map['vu16'] = 'vu1_t'
$map['vu32'] = 'vu2_t'
$map['vi32'] = 'vs2_t'
$map['vci16'] = 'vcs1_t'
$map['vci32'] = 'vcs2_t'
$map['asti16'] = 'asts1_t'
$map['astu32'] = 'astu2_t'
$ownerFiles = @('gpk_array_static.h','gpk_view.h','gpk_rect.h','gpk_rect2.h','gpk_tri2.h','gpk_n2.h')
$encoding = [System.Text.Encoding]::GetEncoding(28591)
$pattern = '\b(?:' + (($map.Keys | Sort-Object Length -Descending | ForEach-Object { [regex]::Escape($_) }) -join '|') + ')\b'
$changed = 0
foreach ($base in @($root, $games)) {
    foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File | Where-Object { $_.Extension -in @('.h','.hpp','.cpp','.c','.inl') -and $_.Name -notin $ownerFiles }) {
        $bytes = [IO.File]::ReadAllBytes($file.FullName)
        $before = $encoding.GetString($bytes)
        $after = [regex]::Replace($before, $pattern, [System.Text.RegularExpressions.MatchEvaluator]{ param($m) $map[$m.Value] })
        if ($after -cne $before) {
            for ($attempt = 0; $attempt -lt 10; $attempt++) {
                try { [IO.File]::WriteAllBytes($file.FullName, $encoding.GetBytes($after)); break }
                catch [IO.IOException] {
                    if ($attempt -eq 9) { throw }
                    Start-Sleep -Milliseconds 200
                }
            }
            $changed++
            Write-Output $file.FullName
        }
    }
}
Write-Output "Changed $changed source files."
