$ErrorActionPreference = 'Stop'
$encoding = [Text.Encoding]::GetEncoding(28591)
$llcId = '{830AB681-B788-47D4-9767-BA4E0792F682}'
$oldIds = @(
    '{39953D8D-A830-46BD-917E-1A96C8BDADC6}', # gpk
    '{976FE3EA-9FD1-4066-9003-0E8513F9D900}', # gpk_engine
    '{8FD826F8-3739-44E6-8CC8-997122E53B8D}'  # zlibvc
)
$solutions = @('gpk.sln', '..\gpk_games\gpk_games.sln', '..\gpk_samples\gpk_samples.sln')
$projects = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)

foreach ($solution in $solutions) {
    $solutionPath = [IO.Path]::GetFullPath((Join-Path (Get-Location) $solution))
    $lines = [IO.File]::ReadAllLines($solutionPath, $encoding)
    foreach ($line in $lines) {
        if ($line -match '^Project\("[^"]+"\) = "[^"]+", "([^"]+\.vcxproj)"') {
            $projectPath = [IO.Path]::GetFullPath((Join-Path (Split-Path $solutionPath) $Matches[1]))
            if (Test-Path -LiteralPath $projectPath) { [void]$projects.Add($projectPath) }
        }
    }

    $result = [Collections.Generic.List[string]]::new()
    $skipProject = $false
    $globalSection = ''
    $dependencySection = $false
    $seenDependencies = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($line in $lines) {
        if ($skipProject) {
            if ($line -eq 'EndProject') { $skipProject = $false }
            continue
        }
        if ($line -match '^Project\("[^"]+"\) = "(gpk|gpk_engine|zlibvc)",') {
            $skipProject = $true
            continue
        }
        if ($line -match '^\s*GlobalSection\(([^)]+)\)') { $globalSection = $Matches[1] }
        if ($line -match '^\s*EndGlobalSection') { $globalSection = '' }
        if ($line -match '^\s*ProjectSection\(ProjectDependencies\)') {
            $dependencySection = $true
            $seenDependencies.Clear()
        }
        if ($line -match '^\s*EndProjectSection') { $dependencySection = $false }

        $oldReference = $false
        foreach ($oldId in $oldIds) {
            if ($line.Contains($oldId)) { $oldReference = $true; break }
        }
        if ($oldReference -and $globalSection -in @('ProjectConfigurationPlatforms', 'NestedProjects')) { continue }

        $updated = $line
        foreach ($oldId in $oldIds) { $updated = $updated.Replace($oldId, $llcId) }
        if ($dependencySection -and $updated -match '^\s*\{[0-9A-Fa-f-]+\}\s*=') {
            if (-not $seenDependencies.Add($updated.Trim())) { continue }
        }
        $result.Add($updated)
    }
    [IO.File]::WriteAllLines($solutionPath, $result, $encoding)
    Write-Output "Solution: $solution"
}

foreach ($projectPath in $projects) {
    if ($projectPath -match '[\\/]gpk[\\/]gpk\.vcxproj$|[\\/]gpk_engine[\\/]gpk_engine\.vcxproj$|[\\/]zlib[\\/].*zlibvc\.vcxproj$') { continue }
    $before = [IO.File]::ReadAllText($projectPath, $encoding)
    $after = [regex]::Replace($before, '(?i)(?<![\w])(?:gpk|gpk_engine|zlibvc)\.lib;\s*', '')
    $after = [regex]::Replace($after, '(?i)\.\./\.\./gpk/(?:gpk|gpk_engine);\s*', '')
    $after = [regex]::Replace($after, '(?i)\.\./\.\./zlib;\s*', '')
    if ($after -cne $before) {
        [IO.File]::WriteAllText($projectPath, $after, $encoding)
        Write-Output "Project: $projectPath"
    }
}
