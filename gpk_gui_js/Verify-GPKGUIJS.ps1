$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$scripts = @(
    'gpk.js',
    'gpk_align.js',
    'gpk_input.js',
    'gpk_expression.js',
    'gpk_json_expression.js',
    'gpk_gui_control_flag.js',
    'gpk_gui_text_flag.js',
    'gpk_gui_control.js',
    'gpk_font.js',
    'gpk_gui_theme.js',
    'gpk_gui.js',
    'gpk_gui_text.js',
    'gpk_gui_draw.js',
    'gpk_gui_keyboard.js',
    'gpk_gui_control_list.js',
    'gpk_gui_system.js',
    'gpk_gui_inputbox.js',
    'gpk_gui_viewport.js',
    'gpk_gui_host.js',
    'gpk_gui_document.js',
    'gui_type_explorer_document.js',
    'demo.js',
    'test.js'
)

foreach($script in $scripts) {
    $path = Join-Path $root $script
    if(-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing script: $script"
    }
    if((Get-Item -LiteralPath $path).Length -eq 0) {
        throw "Empty script: $script"
    }
}

$expressionSource = [IO.File]::ReadAllText((Join-Path $root 'gpk_expression.js'))
$evaluatorSource = [IO.File]::ReadAllText((Join-Path $root 'gpk_json_expression.js'))
$coreSource = [IO.File]::ReadAllText((Join-Path $root 'gpk.js'))
$testSource = [IO.File]::ReadAllText((Join-Path $root 'test.js'))
if(-not $coreSource.Contains('(value & mask) === mask') -or -not $coreSource.Contains('(value & mask) !== mask')) {
    throw 'Core bit_all/bit_false semantics do not match gpk_bit.h.'
}
$behavioralCheckCount = [regex]::Matches($testSource, '\btest\("').Count
if($behavioralCheckCount -ne 19) { throw "Expected 19 browser checks; found $behavioralCheckCount." }
if($expressionSource.Contains('eval(') -or $expressionSource.Contains('Function(') -or $evaluatorSource.Contains('eval(') -or $evaluatorSource.Contains('Function(')) {
    throw 'Expression translation contains dynamic JavaScript execution.'
}
foreach($required in @('EXPRESSION_READER_TYPE', 'expressionReaderParse', 'expressionStringTokens')) {
    if(-not $expressionSource.Contains($required)) { throw "Expression parser is missing $required." }
}
foreach($required in @('jsonTruth', 'jsonDeepEqual', 'jsonExpressionResolve')) {
    if(-not $evaluatorSource.Contains($required)) { throw "JSON evaluator is missing $required." }
}

$index = [IO.File]::ReadAllText((Join-Path $root 'index.html'))
$test = [IO.File]::ReadAllText((Join-Path $root 'test.html'))
if($index.IndexOf('src="gpk_expression.js"') -gt $index.IndexOf('src="gpk_json_expression.js"')) { throw 'index.html loads the evaluator before the parser.' }
if($test.IndexOf('src="gpk_expression.js"') -gt $test.IndexOf('src="gpk_json_expression.js"')) { throw 'test.html loads the evaluator before the parser.' }
foreach($script in $scripts) {
    if($script -eq 'test.js') {
        if(-not $test.Contains('src="test.js"')) { throw 'test.html does not load test.js.' }
        continue
    }
    if($script -eq 'demo.js' -or $script -eq 'gpk_gui_host.js') {
        if(-not $index.Contains(('src="' + $script + '"'))) { throw "index.html does not load $script." }
        continue
    }
    if(-not $index.Contains(('src="' + $script + '"'))) { throw "index.html does not load $script." }
    if(-not $test.Contains(('src="' + $script + '"'))) { throw "test.html does not load $script." }
}

$fontSource = [IO.File]::ReadAllText((Join-Path $root 'gpk_font.js'))
$fontMatches = [regex]::Matches($fontSource, '"x":(\d+),"y":(\d+),"b":"([A-Za-z0-9+/=]+)"')
if($fontMatches.Count -ne 13) { throw "Expected 13 embedded raster fonts; found $($fontMatches.Count)." }
foreach($match in $fontMatches) {
    [void][Convert]::FromBase64String($match.Groups[3].Value)
}

$documentSource = [IO.File]::ReadAllText((Join-Path $root 'gui_type_explorer_document.js'))
if(-not $documentSource.Contains('"menu_bar"') -or -not $documentSource.Contains('"workspace"')) {
    throw 'The supplied GUI document roots are missing.'
}

Write-Output "PASS: $($scripts.Count) scripts, 19 browser checks registered, bit semantics, parser/evaluator boundaries, 13 raster fonts, index/test load graphs, and GUI document roots verified."
