Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function New-Placement([int]$x, [int]$y, [int]$width, [int]$height, [string]$align = 'TOP_LEFT') {
	[ordered]@{
		Area = [ordered]@{ Offset = [ordered]@{ x = $x; y = $y }; Size = [ordered]@{ x = $width; y = $height } }
		Align = $align
	}
}

function New-Control([int]$x, [int]$y, [int]$width, [int]$height, [string]$text = '', [bool]$interactive = $true, [bool]$hidden = $false) {
	$control = [ordered]@{}
	if($hidden -or -not $interactive) {
		$control.State = [ordered]@{}
		if($hidden) { $control.State.Flag = 1 }
		if(-not $interactive) { $control.State.Mask = 0 }
	}
	$control.Placement = New-Placement $x $y $width $height
	if($text.Length) { $control.Text = [ordered]@{ Text = $text; Align = 'CENTER' } }
	$control
}

function Add-TextLeft($control, [string]$text) {
	$control.Text = [ordered]@{ Text = $text; Align = 'CENTER_LEFT' }
}

function New-CloseScript([string]$rootIdentifier) {
	'{"' + $rootIdentifier + '":{"State":{"Flag":1}}}'
}

function Add-Scroll([System.Collections.IDictionary]$client, [string]$prefix, [int]$clientWidth, [int]$clientHeight, [bool]$vertical) {
	if($vertical) {
		$root = New-Control ($clientWidth - 18) 0 18 $clientHeight '' $false $true
		$root[($prefix + '_decrease')] = New-Control 0 0 18 18 '^'
		$root[($prefix + '_thumb')] = New-Control 0 18 18 24 '' $true
		$root[($prefix + '_increase')] = New-Control 0 ($clientHeight - 18) 18 18 'v'
	}
	else {
		$root = New-Control 0 ($clientHeight - 18) $clientWidth 18 '' $false $true
		$root[($prefix + '_decrease')] = New-Control 0 0 18 18 '<'
		$root[($prefix + '_thumb')] = New-Control 18 0 24 18 '' $true
		$root[($prefix + '_increase')] = New-Control ($clientWidth - 18) 0 18 18 '>'
	}
	$client[$prefix] = $root
}

function New-Viewport([string]$prefix, [string]$title, [int]$x, [int]$y, [int]$width, [int]$height, [bool]$hidden) {
	$root = New-Control $x $y $width $height '' $true $hidden
	$root.Placement.Border = [ordered]@{ Left = 3; Top = 3; Right = 3; Bottom = 3 }
	$root[($prefix + '_title')] = New-Control 0 0 ($width - 30) 22 $title $false
	Add-TextLeft $root[($prefix + '_title')] $title
	$root[($prefix + '_close')] = New-Control ($width - 26) 0 24 22 'X'
	$root[($prefix + '_close')].Script = New-CloseScript $prefix
	$root[($prefix + '_client')] = New-Control 2 24 ($width - 8) ($height - 30) '' $false
	$root
}

function Add-Rows([System.Collections.IDictionary]$parent, [string]$prefix, [int]$count, [int]$width, [int]$rowHeight) {
	$rows = New-Control 0 0 $width ($count * $rowHeight) '' $false
	for($i = 0; $i -lt $count; ++$i) {
		$id = '{0}_{1:D2}' -f $prefix, $i
		$row = New-Control 0 ($i * $rowHeight) $width $rowHeight '' $true $true
		Add-TextLeft $row ''
		$rows[$id] = $row
	}
	$parent[($prefix + '_rows')] = $rows
}

function New-Directory([int]$index) {
	$prefix = 'directory_{0}' -f $index
	$root = New-Viewport $prefix 'FILES / CURRENT DIRECTORY' (40 + 24 * $index) (48 + 24 * $index) 420 600 ($index -ne 0)
	$client = $root[($prefix + '_client')]
	Add-Rows $client $prefix 32 388 20
	Add-Scroll $client ($prefix + '_scroll_y') 412 570 $true
	Add-Scroll $client ($prefix + '_scroll_x') 412 570 $false
	$root
}

function Add-EditorBody([System.Collections.IDictionary]$root, [string]$prefix, [int]$width, [int]$height, [bool]$withToolbar) {
	if($withToolbar) {
		$root[($prefix + '_save')] = New-Control ($width - 180) 0 64 22 'Save'
		$root[($prefix + '_keyboard')] = New-Control ($width - 114) 0 88 22 'Keyboard'
	}
	$client = $root[($prefix + '_client')]
	Add-Rows $client $prefix 40 ($width - 44) 20
	$cursor = New-Control 0 0 8 20 '|' $false $true
	$client[($prefix + '_cursor')] = $cursor
	Add-Scroll $client ($prefix + '_scroll_y') ($width - 8) ($height - 30) $true
	Add-Scroll $client ($prefix + '_scroll_x') ($width - 8) ($height - 30) $false
}

function New-Editor([int]$index) {
	$prefix = 'editor_{0}' -f $index
	$root = New-Viewport $prefix 'SELECT A FILE' (480 + 24 * $index) (72 + 24 * $index) 760 560 $true
	Add-EditorBody $root $prefix 760 560 $true
	$root
}

function New-JSONTool {
	$prefix = 'json_expression_tool'
	$root = New-Viewport $prefix 'JSON EXPRESSIONS' 72 72 600 650 $true
	$client = $root[($prefix + '_client')]
	$inputLabel = New-Control 0 0 540 22 'INPUT FILE' $false
	Add-TextLeft $inputLabel 'INPUT FILE'
	$client[($prefix + '_input_label')] = $inputLabel
	$inputs = New-Control 0 24 540 160 '' $false
	for($i = 0; $i -lt 8; ++$i) {
		$id = 'json_input_{0}' -f $i
		$row = New-Control 0 ($i * 20) 540 20 '' $true $true
		Add-TextLeft $row ''
		$inputs[$id] = $row
	}
	$client[($prefix + '_inputs')] = $inputs
	$presetLabel = New-Control 0 190 540 22 'PREBUILT JSON EXPRESSIONS' $false
	Add-TextLeft $presetLabel 'PREBUILT JSON EXPRESSIONS'
	$client[($prefix + '_preset_label')] = $presetLabel
	$presets = New-Control 0 214 540 180 '' $false
	for($i = 0; $i -lt 9; ++$i) {
		$id = 'preset_{0}' -f $i
		$row = New-Control 0 ($i * 20) 540 20 '' $true $true
		Add-TextLeft $row ''
		$row.Script = '{"json_expression_tool":{"command":{"Text":{"Text":"{json_expression_tool.' + $id + '.Text.Text}"}}}}'
		$presets[$id] = $row
	}
	$client[($prefix + '_presets')] = $presets
	$expressionLabel = New-Control 0 430 540 22 'EXPRESSION' $false
	Add-TextLeft $expressionLabel 'EXPRESSION'
	$client[($prefix + '_expression_label')] = $expressionLabel
	$commandRoot = New-Control 0 454 448 26 '' $false
	$commandText = New-Control 0 0 448 26 '' $true
	Add-TextLeft $commandText ''
	$commandRoot.command = $commandText
	$client.command_input = $commandRoot
	$client[($prefix + '_run')] = New-Control 452 454 88 26 'RUN'
	$suggestions = New-Control 0 480 448 240 '' $false $true
	for($i = 0; $i -lt 12; ++$i) {
		$id = 'suggestion_{0:D2}' -f $i
		$row = New-Control 0 ($i * 20) 448 20 '' $true $true
		Add-TextLeft $row ''
		$suggestions[$id] = $row
	}
	$client[($prefix + '_suggestions')] = $suggestions
	Add-Scroll $client ($prefix + '_scroll_y') 592 620 $true
	Add-Scroll $client ($prefix + '_scroll_x') 592 620 $false
	$root
}

function New-JSONOutput {
	$prefix = 'json_expression_output'
	$root = New-Viewport $prefix 'JSON EXPRESSION OUTPUT' 696 72 560 650 $true
	Add-EditorBody $root $prefix 560 650 $true
	$root
}

function New-Keyboard {
	$prefix = 'virtual_keyboard'
	$root = New-Control 230 470 820 220 '' $true $true
	$root.Placement.Border = [ordered]@{ Left = 3; Top = 3; Right = 3; Bottom = 3 }
	$root[($prefix + '_title')] = New-Control 0 0 790 22 'KEYBOARD' $false
	$root[($prefix + '_close')] = New-Control 792 0 24 22 'X'
	$root[($prefix + '_close')].Script = New-CloseScript $prefix
	$characters = New-Control 102 24 512 160 '' $false
	$values = @(1..254) + @(33, 63)
	for($i = 0; $i -lt $values.Count; ++$i) {
		$value = $values[$i]
		$label = if($value -ge 32 -and $value -le 126) { [char]$value } else { '{0:X2}' -f $value }
		$id = 'key_{0:D3}' -f $i
		$characters[$id] = New-Control (($i % 32) * 16) ([Math]::Floor($i / 32) * 20) 16 20 $label
	}
	$root[($prefix + '_characters')] = $characters
	$actionNames = @('Escape','Enter','Backspace','Clear','Left','Right','Up','Down','Front','Back','Delete','Insert','Home','End','Page Up','Page Down','Left Shift','Right Shift','Alt','Alt Gr','Left Ctrl','Right Ctrl','System')
	$actions = New-Control 0 24 816 184 '' $false
	for($i = 0; $i -lt $actionNames.Count; ++$i) {
		$left = $i -lt 12
		$x = if($left) { 0 } else { 622 }
		$y = if($left) { $i * 15 } else { ($i - 12) * 15 }
		$id = 'action_{0:D2}' -f $i
		$actions[$id] = New-Control $x $y 96 15 $actionNames[$i]
	}
	$root[($prefix + '_actions')] = $actions
	$root
}

$gui = [ordered]@{}
$menu = New-Control 0 0 1280 24 '' $false
$menu.menu_files = New-Control 0 0 64 22 'FILES'
$menu.menu_json = New-Control 66 0 64 22 'JSON'
$tabs = New-Control 132 0 920 22 '' $false
for($i = 0; $i -lt 8; ++$i) {
	$tab = New-Control ($i * 112) 0 110 22 '' $true $true
	Add-TextLeft $tab ''
	$tabs[('tab_{0}' -f $i)] = $tab
}
$menu.editor_tabs = $tabs
$menu.menu_exit = New-Control 1214 0 64 22 'EXIT'
$gui.menu = $menu
for($i = 0; $i -lt 4; ++$i) { $gui[('directory_{0}' -f $i)] = New-Directory $i }
for($i = 0; $i -lt 8; ++$i) { $gui[('editor_{0}' -f $i)] = New-Editor $i }
$gui.json_expression_tool = New-JSONTool
$gui.json_expression_output = New-JSONOutput
$gui.virtual_keyboard = New-Keyboard

$json = $gui | ConvertTo-Json -Depth 100 -Compress
$encoding = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText((Join-Path $PSScriptRoot 'file_viewer_gui.json'), $json, $encoding)
