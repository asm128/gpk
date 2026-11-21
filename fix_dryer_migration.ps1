$ErrorActionPreference = 'Stop'
$encoding = [Text.Encoding]::GetEncoding(28591)
[xml]$project = Get-Content dryer/dryer.vcxproj
$names = @($project.Project.ItemGroup.ClCompile.Include) + @($project.Project.ItemGroup.ClInclude.Include)
$replacements = [ordered]@{
    '\bcirclef32\b' = 'circlef2_t'
    '\bspheref32\b' = 'spheref2_t'
    '\btri3f32\b' = 'tri3f2_t'
    '\bimgu32\b' = 'imgu2_t'
    '\bpngFileLoad\b' = 'pngLoadFile'
    '\bview_array\b' = 'view'
    '::gpk::vc\b' = '::gpk::vsc_t'
    '\.u8\(\)' = '.bytes()'
    '::gpk::APPLICATION_STATE_EXIT\b' = '1'
    '\bretval_ginfo_if\b' = 'if_true_vif'
    '\bcef_if\b' = 'if_true_cef'
    'jsonString\.append_string\(' = '::gpk::append_strings(jsonString, '
    'gui\.Controls\.Images\[controlIndex\]\.Image' = 'gui.Controls.ImageViews[controlIndex]'
    'controlImage\.Image\b' = 'gui.Controls.ImageViews[controlIndex]'
}
foreach ($name in ($names | Where-Object { $_ } | Sort-Object -Unique)) {
    $path = [IO.Path]::GetFullPath((Join-Path (Get-Location) ('dryer/' + $name)))
    if (-not (Test-Path -LiteralPath $path)) { continue }
    $before = [IO.File]::ReadAllText($path, $encoding)
    $after = $before
    foreach ($pattern in $replacements.Keys) { $after = [regex]::Replace($after, $pattern, $replacements[$pattern]) }
    if ($after -cne $before) {
        [IO.File]::WriteAllText($path, $after, $encoding)
        Write-Output $name
    }
}
