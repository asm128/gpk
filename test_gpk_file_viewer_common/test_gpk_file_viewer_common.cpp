#include "test_gpk_file_viewer_common.h"

#include "gpk_file.h"
#include "gpk_framework_module.h"
#include "gpk_gui_system.h"
#include "gpk_label.h"
#include "gpk_path.h"

stxp u2_c JSON_EXPRESSION_PRESET_LIMIT = 9;
stxp u2_c JSON_EXPRESSION_SUGGESTION_LIMIT = 12;
stxp vcst_c JSON_EXPRESSION_TOOL_IDENTIFIER = "json_expression_tool";
stxp vcst_c JSON_EXPRESSION_COMMAND_IDENTIFIER = "command";
stxp vcst_c JSON_EXPRESSION_PRESET_IDENTIFIERS[JSON_EXPRESSION_PRESET_LIMIT] = {
	"preset_0", "preset_1", "preset_2", "preset_3", "preset_4", "preset_5", "preset_6", "preset_7", "preset_8",
};

sttc gpk::err_t fileViewerViewportCreate(SFileViewer & viewer, cid_t & idViewport, vcst_t title, cnst rect2s1_t & area) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(viewportCreate(*viewer.Dialog, viewport));
	idViewport = viewport->IdGUIControl;
	SGUI & gui = *viewer.Dialog->GUI;
	SControlTable		& controls = gui.Controls;
	SControlPlacement	& root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Text[viewport->IdTitle].Text = title;
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn controlMetricsInvalidate(gui, viewport->IdGUIControl);
}

sttc gpk::err_t fileViewerViewportAreaSet(SFileViewer & viewer, cid_t idViewport, cnst rect2s1_t & area) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, idViewport, viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.Placement[viewport->IdGUIControl].Area = area;
	gui.Controls.Placement[viewport->IdClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	rtrn controlMetricsInvalidate(gui, viewport->IdGUIControl);
}

sttc gpk::err_t fileViewerLayoutSet(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	if(FILE_VIEWER_LAYOUT_FLOATING == layout)
		rtrn 0;
	SGUI & gui = *viewer.Dialog->GUI;
	n2s1_c windowSize = gui.LastSize.s1();
	cnst s1_t margin = 12;
	cnst s1_t gap = 8;
	cnst s1_t top = max(margin, gui.Controls.Placement[viewer.IdMenuFiles].Area.Size.y);
	cnst s1_t leftWidth = min((s1_t)360, (s1_t)((windowSize.x - margin * 2 - gap) / 3));
	cnst s1_t height = (s1_t)(windowSize.y - top - margin);
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory)
		if_fail_fe(::fileViewerViewportAreaSet(viewer, viewer.Directories[iDirectory]->IdViewport, {{margin, top}, {leftWidth, height}}));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerViewportAreaSet(viewer, viewer.Editors[iEditor]->IdViewport, {{(s1_t)(margin + leftWidth + gap), top}, {(s1_t)(windowSize.x - margin * 2 - leftWidth - gap), height}}));
	rtrn 0;
}

sttc gpk::err_t fileViewerListCreate(SFileViewer & viewer, SControlList & list, cid_t idViewport) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, idViewport, viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	if_fail_fe(controlListInitialize(gui, list));
	list.Orientation = DIRECTION_DOWN;
	if_fail_fe(controlSetParent(gui, list.IdControl, viewport->IdClient));
	gui.Controls.Constraints[list.IdControl].AttachSizeToControl.x = list.IdControl;
	gui.Controls.States[list.IdControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States[list.IdControl].Mask &= ~GUI_CONTROL_FLAG_Pressed;
	gui.Controls.States[list.IdControl].Mask &= ~GUI_CONTROL_FLAG_Dragging;
	gui.Controls.States[list.IdControl].Mask &= ~GUI_CONTROL_FLAG_Action;
	rtrn 0;
}

sttc void fileViewerListRowConfigure(SFileViewer & viewer, cid_t idControl, b8_t interactive, b8_t buttonPalette = true) {
	SControlTable & controls = viewer.Dialog->GUI->Controls;
	controls.Draw[idControl].Palettes = buttonPalette ? viewer.Dialog->Colors->Button : viewer.Dialog->Colors->Control;
	controls.Text[idControl].Align = ALIGN_CENTER_LEFT;
	if(false == interactive) {
		controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
		controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Pressed;
		controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Dragging;
		controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Action;
	}
}

sttc gpk::err_t fileViewerListRowPush(SFileViewer & viewer, SControlList & list, vcst_t text, b8_t interactive, b8_t buttonPalette = true) {
	if_fail_fe(controlListPush(*viewer.Dialog->GUI, list, label(text)));
	cnst cid_t idControl = list.IdControls[list.IdControls.size() - 1];
	::fileViewerListRowConfigure(viewer, idControl, interactive, buttonPalette);
	rtrn 0;
}

sttc gpk::err_t fileViewerScrollCreate(SFileViewer & viewer, cid_t idViewport, b8_t vertical) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, idViewport, viewport));
	if_fail_fe(sliderCreate(*viewer.Dialog, scrollbar));
	scrollbar->Vertical = vertical;
	scrollbar->FuncValueFormat = [](vcst_t & format, s3_t, cnst minmax<s3_t> &) { format = ""; rtrn 0; };
	scrollbar->ValueLimits = {0, 1};
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	if(vertical) {
		viewport->IdScrollBarY = scrollbar->IdGUIControl;
		viewport->Settings.ScrollBarY = false;
	}
	else {
		viewport->IdScrollBarX = scrollbar->IdGUIControl;
		viewport->Settings.ScrollBarX = false;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorCreate(SFileViewer & viewer, SFileEditor & editor, cnst rect2s1_t & area) {
	if_fail_fe(::fileViewerViewportCreate(viewer, editor.IdViewport, "SELECT A FILE", area));
	if_fail_fe(::fileViewerListCreate(viewer, editor.Lines, editor.IdViewport));
	if_fail_fe(::fileViewerScrollCreate(viewer, editor.IdViewport, true));
	if_fail_fe(::fileViewerScrollCreate(viewer, editor.IdViewport, false));
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, editor.IdViewport, viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	if_fail_fe(editor.IdSave = controlCreate(gui, [&](SControlState &, apod<STextState> &, vcst_t &, SControlConstraints & constraints, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {64, gui.Controls.Placement[viewport->IdClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.AttachSizeToControl = {-1, -1};
		constraints.DockToControl.Right = viewport->IdClose;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Save";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, editor.IdSave, viewport->IdGUIControl));
	gui.Controls.SetHidden(editor.IdSave, true);

	if_fail_fe(editor.IdKeyboard = controlCreate(gui, [&](SControlState &, apod<STextState> &, vcst_t &, SControlConstraints & constraints, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {88, gui.Controls.Placement[viewport->IdClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.DockToControl.Right = editor.IdSave;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Keyboard";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, editor.IdKeyboard, viewport->IdGUIControl));
	gui.Controls.Constraints[viewport->IdTitle].DockToControl.Right = editor.IdKeyboard;
	gui.Controls.SetHidden(editor.IdKeyboard, true);

	if_fail_fe(editor.IdCursor = controlCreate(gui, [](SControlState & state, apod<STextState> &, vcst_t &, SControlConstraints &, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Border = {};
		placement.Margin = {};
		draw.NoBorder = true;
		draw.NoClient = true;
		text.Text = "|";
		text.Align = ALIGN_CENTER_LEFT;
		state.Mask &= ~GUI_CONTROL_FLAG_Hovered;
		state.Mask &= ~GUI_CONTROL_FLAG_Pressed;
		state.Mask &= ~GUI_CONTROL_FLAG_Dragging;
		state.Mask &= ~GUI_CONTROL_FLAG_Action;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, editor.IdCursor, viewport->IdClient));
	gui.Controls.SetHidden(editor.IdCursor, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuCreate(SFileViewer & viewer) {
	SGUI & gui = *viewer.Dialog->GUI;
	SControlList menuViewports = {};
	sttc vcst_c viewportText[] = {"FILES", "JSON"};
	if_fail_fe(controlListCreate(gui, menuViewports, view<vcst_c>{viewportText}, CID_INVALID, DIRECTION_RIGHT));
	gui.Controls.Placement[menuViewports.IdControl].Area.Offset = {};
	gui.Controls.Placement[menuViewports.IdControl].Align = ALIGN_TOP_LEFT;
	viewer.IdMenuFiles = menuViewports.IdControls[0];
	viewer.IdMenuJSON = menuViewports.IdControls[1];
	if_fail_fe(viewer.Dialog->Buttons.push_back(viewer.IdMenuFiles));
	if_fail_fe(viewer.Dialog->Buttons.push_back(viewer.IdMenuJSON));
	if_fail_fe(controlListInitialize(gui, viewer.EditorTabs));
	viewer.EditorTabs.Orientation = DIRECTION_RIGHT;
	gui.Controls.Placement[viewer.EditorTabs.IdControl].Area.Offset.x = gui.Controls.Placement[menuViewports.IdControl].Area.Size.x;
	gui.Controls.Placement[viewer.EditorTabs.IdControl].Align = ALIGN_TOP_LEFT;
	gui.Controls.States[viewer.EditorTabs.IdControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States[viewer.EditorTabs.IdControl].Mask &= ~GUI_CONTROL_FLAG_Pressed;
	gui.Controls.States[viewer.EditorTabs.IdControl].Mask &= ~GUI_CONTROL_FLAG_Dragging;
	gui.Controls.States[viewer.EditorTabs.IdControl].Mask &= ~GUI_CONTROL_FLAG_Action;

	SControlList menuActions = {};
	sttc vcst_c actionText[] = {"EXIT"};
	if_fail_fe(controlListCreate(gui, menuActions, view<vcst_c>{actionText}, CID_INVALID, DIRECTION_RIGHT));
	gui.Controls.Placement[menuActions.IdControl].Area.Offset = {};
	gui.Controls.Placement[menuActions.IdControl].Align = ALIGN_TOP_RIGHT;
	viewer.IdMenuExit = menuActions.IdControls[0];
	if_fail_fe(viewer.Dialog->Buttons.push_back(viewer.IdMenuExit));
	rtrn 0;
}

sttc b8_t fileViewerFolderHasParent(cnst SPathTree & paths) {
	vcst_t parent = {};
	rtrn 0 <= pathDirectory(paths.Root, parent) && parent.size() && parent != paths.Root;
}

sttc gpk::err_t fileViewerFolderLoad(SFileViewer & viewer, SPathList & directory, vcst_t path) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, directory.IdViewport, viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	for(u2_t iControl = 0; iControl < directory.Files.IdControls.size(); ++iControl)
		if_fail_fe(controlTextSet(gui, directory.Files.IdControls[iControl], {}));
	gui.Controls.Text[viewport->IdTitle].Text = {};
	string absolutePath = {};
	if_fail_fe(pathAbsolute(path, absolutePath));
	if_fail_fe(pathList(directory.PathContents, absolutePath, [](b8_t folder, vcst_c &) { rtrn folder ? 2 : 0; }));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	gui.Controls.Text[viewport->IdTitle].Text = directory.PathContents.Root;
	for(u2_t iControl = 0; iControl < directory.Files.IdControls.size(); ++iControl)
		gui.Controls.SetSelected(directory.Files.IdControls[iControl], false);
	rtrn 0;
}

sttc gpk::err_t fileViewerListWindowUpdate(SFileViewer & viewer, SControlList & list, cnst SPathTree & items, cid_t idViewport) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, idViewport, viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	b8_c hasParent = ::fileViewerFolderHasParent(items);
	u2_c itemCount = items.Name.size() + one_if(hasParent);
	if(itemCount && 0 == list.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, list, {}, true));
	cnst s1_t rowHeight = list.IdControls.size() ? max((s1_t)1, gui.Controls.Placement[list.IdControls[0]].Area.Size.y) : 1;
	cnst s3_t visibleCount = max((s3_t)1, (s3_t)gui.Controls.Placement[viewport->IdClient].Area.Size.y / rowHeight);
	while(list.IdControls.size() < min(itemCount, (u2_t)visibleCount))
		if_fail_fe(::fileViewerListRowPush(viewer, list, {}, true));
	cnst s3_t maxOffset = max((s3_t)0, (s3_t)itemCount - visibleCount);
	scrollbar->ValueLimits = {0, max((s3_t)1, maxOffset)};
	if_fail_fe(sliderSetValue(*scrollbar, maxOffset ? scrollbar->ValueCurrent : 0));
	viewport->Settings.ScrollBarY = 0 != maxOffset;
	cnst s3_t firstVisible = maxOffset ? scrollbar->ValueCurrent : 0;
	for(u2_t iControl = 0; iControl < list.IdControls.size(); ++iControl) {
		cnst cid_t idControl = list.IdControls[iControl];
		u2_c iItem = (u2_t)firstVisible + iControl;
		vcst_t text = {};
		if(iControl < (u2_t)visibleCount && iItem < itemCount)
			text = hasParent && 0 == iItem ? vcst_t{".."} : items.Name[iItem - one_if(hasParent)];
		if(gui.Controls.Text[idControl].Text != text)
			if_fail_fe(controlTextSet(gui, idControl, text));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerTextViewsClear(SFileViewer & viewer, SFileEditor & editor) {
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl)
		if_fail_fe(controlTextSet(*viewer.Dialog->GUI, editor.Lines.IdControls[iControl], {}));
	editor.FileLines.clear();
	editor.FileLineWidthMax = 0;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextViewsRebuild(SFileEditor & editor) {
	editor.FileLines.clear();
	editor.FileLineWidthMax = 0;
	vcst_t remaining = editor.FileText;
	do {
		vcst_t line = {};
		vcst_t next = {};
		cnst gpk::err_t splitResult = split('\n', remaining, line, next);
		if(line.size() && '\r' == line[line.size() - 1])
			if_fail_fe(line.slice(line, 0, line.size() - 1));
		if_fail_fe(editor.FileLines.push_back(line));
		editor.FileLineWidthMax = max(editor.FileLineWidthMax, line.size());
		if(0 > splitResult)
			break;
		remaining = next;
	} while(true);
	rtrn 0;
}

sttc gpk::err_t fileViewerTextLoad(SFileViewer & viewer, SFileEditor & editor, vcst_t path) {
	if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
	editor.FilePath = path;
	editor.FileText.clear();
	editor.TextSelections.clear();
	editor.SelectionAnchor = (u2_t)-1;
	editor.SelectionActive = (u2_t)-1;
	if_fail_fef(fileToMemory(path, editor.FileText), "Failed to load '%.*s'.", GPK_VIEW_ARGS(path));
	if_fail_fe(::fileViewerTextViewsRebuild(editor));
	editor.Cursor = {0, (u2_t)-1};
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
	gui.Controls.SetHidden(editor.IdSave, false);
	gui.Controls.SetHidden(editor.IdKeyboard, false);
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, editor.IdViewport, viewport));
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	gui.Controls.Text[viewport->IdTitle].Text = label(path);
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionLabelCreate(SFileViewer & viewer, cid_t iParent, vcst_t textValue, n2s1_t offset) {
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idLabel = -1;
	if_fail_fe(idLabel = controlCreate(gui));
	gui.Controls.Placement[idLabel].Area = {offset, {540, 22}};
	gui.Controls.Placement[idLabel].Border = {1, 1, 1, 1};
	gui.Controls.Draw[idLabel].Palettes = viewer.Dialog->Colors->Control;
	gui.Controls.Text[idLabel].Text = label(textValue);
	gui.Controls.Text[idLabel].Align = ALIGN_CENTER_LEFT;
	gui.Controls.States[idLabel].Mask &= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States[idLabel].Mask &= ~GUI_CONTROL_FLAG_Pressed;
	gui.Controls.States[idLabel].Mask &= ~GUI_CONTROL_FLAG_Dragging;
	gui.Controls.States[idLabel].Mask &= ~GUI_CONTROL_FLAG_Action;
	rtrn controlSetParent(gui, idLabel, iParent);
}

sttc b8_t fileViewerJSONExpressionIdentifier(vcst_t viewKey) {
	if(0 == viewKey.size())
		rtrn false;
	cnst sc_t first = viewKey[0];
	if(false == ((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z') || '_' == first))
		rtrn false;
	for(u2_t iChar = 1; iChar < viewKey.size(); ++iChar) {
		cnst sc_t character = viewKey[iChar];
		if(false == ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || '_' == character))
			rtrn false;
	}
	rtrn true;
}

sttc gpk::err_t fileViewerJSONExpressionPathKeyAppend(string & expression, vcst_t viewKey) {
	if(expression.size())
		if_fail_fe(expression.push_back('.'));
	rtrn expression.append(viewKey);
}

sttc gpk::err_t fileViewerJSONExpressionPresetScriptSet(SGUI & gui, cid_t idPreset, u2_t iPreset) {
	if_true_fe(iPreset >= size(JSON_EXPRESSION_PRESET_IDENTIFIERS));
	gui.Controls.Identifier[idPreset] = JSON_EXPRESSION_PRESET_IDENTIFIERS[iPreset];
	string expression = "{";
	if_fail_fe(expression.append(JSON_EXPRESSION_TOOL_IDENTIFIER));
	if_fail_fe(expression.push_back('.'));
	if_fail_fe(expression.append(JSON_EXPRESSION_PRESET_IDENTIFIERS[iPreset]));
	if_fail_fe(expression.append(".Text.Text}"));
	string & script = gui.Controls.Script[idPreset];
	script.clear();
	if_fail_fe(script.push_back('{'));
	if_fail_fe(appendOpenKey(script, JSON_EXPRESSION_TOOL_IDENTIFIER));
	if_fail_fe(script.push_back('{'));
	if_fail_fe(appendOpenKey(script, JSON_EXPRESSION_COMMAND_IDENTIFIER));
	if_fail_fe(script.push_back('{'));
	if_fail_fe(appendOpenKey(script, "Text"));
	if_fail_fe(script.push_back('{'));
	if_fail_fe(appendOpenKey(script, "Text"));
	if_fail_fe(appendJSONString(script, expression));
	rtrn script.append("}}}}");
}

sttc gpk::err_t fileViewerJSONExpressionLeafPath(cnst SJSONNode & node, view<vcst_c> views, string & expression) {
	cnst SJSONNode * current = &node;
	while(current && current->Token) {
		if(JSON_TYPE_ARRAY == current->Token->Type) {
			if(0 == current->Children.size())
				rtrn 0;
			if_fail_fe(expression.append("['0']"));
			current = current->Children[0];
			continue;
		}
		if(JSON_TYPE_OBJECT != current->Token->Type || current->Children.size() < 2)
			rtrn 0;
		b8_t found = false;
		for(u2_t iChild = 0; iChild + 1 < current->Children.size(); iChild += 2) {
			cnst SJSONNode * key = current->Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= views.size() || false == ::fileViewerJSONExpressionIdentifier(views[key->ObjectIndex]))
				continue;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, views[key->ObjectIndex]));
			current = current->Children[iChild + 1];
			found = true;
			break;
		}
		if(false == found)
			rtrn 0;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionBranchCollect(cnst SJSONNode & node, view<vcst_c> views, vcst_t basePath, aobj<string> & expressions) {
	if(JSON_TYPE_OBJECT == node.Token->Type) {
		for(u2_t iChild = 0; iChild + 1 < node.Children.size(); iChild += 2) {
			cnst SJSONNode * key = node.Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= views.size() || false == ::fileViewerJSONExpressionIdentifier(views[key->ObjectIndex]))
				continue;
			string expression = basePath;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, views[key->ObjectIndex]));
			if_fail_fe(::fileViewerJSONExpressionLeafPath(*node.Children[iChild + 1], views, expression));
			if_fail_fe(expressions.push_back(expression));
		}
	}
	else {
		string expression = basePath;
		if_fail_fe(::fileViewerJSONExpressionLeafPath(node, views, expression));
		if_fail_fe(expressions.push_back(expression));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionPresetsBuild(SFileViewer & viewer, cnst SJSONReader & reader) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	tool.PresetExpressions.clear();
	if(0 == reader.Tree.size() || 0 == reader.Tree[0] || 0 == reader.Tree[0]->Token)
		rtrn 0;
	cnst SJSONNode & root = *reader.Tree[0];
	aobj<aobj<string>> expressionGroups = {};
	if(JSON_TYPE_OBJECT == root.Token->Type) {
		for(u2_t iBranch = 0; ; ++iBranch) {
			b8_t foundBranch = false;
			for(u2_t iRoot = 0; iRoot + 1 < root.Children.size(); iRoot += 2) {
				cnst SJSONNode * keyRoot = root.Children[iRoot];
				cnst SJSONNode * valueRoot = root.Children[iRoot + 1];
				if(0 == keyRoot || 0 == valueRoot || keyRoot->ObjectIndex < 0 || (u2_t)keyRoot->ObjectIndex >= reader.View.size() || false == ::fileViewerJSONExpressionIdentifier(reader.View[keyRoot->ObjectIndex]))
					continue;
				if(JSON_TYPE_OBJECT == valueRoot->Token->Type) {
					cnst u2_t iChild = iBranch * 2;
					if(iChild + 1 >= valueRoot->Children.size())
						continue;
					cnst SJSONNode * keyBranch = valueRoot->Children[iChild];
					if(0 == keyBranch || keyBranch->ObjectIndex < 0 || (u2_t)keyBranch->ObjectIndex >= reader.View.size() || false == ::fileViewerJSONExpressionIdentifier(reader.View[keyBranch->ObjectIndex]))
						continue;
					string branchPath = reader.View[keyRoot->ObjectIndex];
					if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(branchPath, reader.View[keyBranch->ObjectIndex]));
					if_fail_fe(expressionGroups.resize(expressionGroups.size() + 1));
					if_fail_fe(::fileViewerJSONExpressionBranchCollect(*valueRoot->Children[iChild + 1], reader.View, branchPath, expressionGroups[expressionGroups.size() - 1]));
					foundBranch = true;
				}
				else if(0 == iBranch) {
					if_fail_fe(expressionGroups.resize(expressionGroups.size() + 1));
					if_fail_fe(::fileViewerJSONExpressionBranchCollect(*valueRoot, reader.View, reader.View[keyRoot->ObjectIndex], expressionGroups[expressionGroups.size() - 1]));
					foundBranch = true;
				}
			}
			if(false == foundBranch)
				break;
		}
	}
	else {
		if_fail_fe(expressionGroups.resize(1));
		if_fail_fe(::fileViewerJSONExpressionBranchCollect(root, reader.View, {}, expressionGroups[0]));
	}
	for(u2_t iExpression = 0; tool.PresetExpressions.size() < JSON_EXPRESSION_PRESET_LIMIT; ++iExpression) {
		b8_t foundExpression = false;
		for(u2_t iGroup = 0; iGroup < expressionGroups.size() && tool.PresetExpressions.size() < JSON_EXPRESSION_PRESET_LIMIT; ++iGroup) {
			if(iExpression >= expressionGroups[iGroup].size())
				continue;
			vcst_t output = {};
			if(-1 != jsonExpressionResolve(expressionGroups[iGroup][iExpression], reader, 0, output))
				if_fail_fe(tool.PresetExpressions.push_back(expressionGroups[iGroup][iExpression]));
			foundExpression = true;
		}
		if(false == foundExpression)
			break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionPresetsRefresh(SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	tool.PresetExpressions.clear();
	if(tool.InputJSON.Tree.size() && tool.InputJSON.Tree[0] && tool.InputJSON.Tree[0]->Token)
		if_fail_fe(::fileViewerJSONExpressionPresetsBuild(viewer, tool.InputJSON));
	SGUI & gui = *viewer.Dialog->GUI;
	while(tool.Presets.IdControls.size() < tool.PresetExpressions.size())
		if_fail_fe(::fileViewerListRowPush(viewer, tool.Presets, {}, true));
	for(u2_t iPreset = 0; iPreset < tool.Presets.IdControls.size(); ++iPreset) {
		cnst b8_t visible = iPreset < tool.PresetExpressions.size();
		gui.Controls.SetHidden(tool.Presets.IdControls[iPreset], false == visible);
		if(visible) {
			if_fail_fe(controlTextSet(gui, tool.Presets.IdControls[iPreset], tool.PresetExpressions[iPreset]));
			if_fail_fe(::fileViewerJSONExpressionPresetScriptSet(gui, tool.Presets.IdControls[iPreset], iPreset));
		}
	}
	if_fail_fe(controlListArrange(gui, tool.Presets));
	gui.Controls.Placement[tool.Presets.IdControl].Area.Size.x = 540;
	rtrn 0;
}

sttc b8_t fileViewerJSONExpressionStartsWith(vcst_t text, vcst_t prefix) {
	if(prefix.size() > text.size())
		rtrn false;
	for(u2_t iCharacter = 0; iCharacter < prefix.size(); ++iCharacter)
		if(text[iCharacter] != prefix[iCharacter])
			rtrn false;
	rtrn true;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsBuild(SFileViewer & viewer, cnst SJSONNode * node, vcst_t baseExpression) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	tool.SuggestionNode = node;
	tool.SuggestionBase = baseExpression;
	tool.SuggestionText.clear();
	tool.SuggestionExpressions.clear();
	tool.SuggestionChildSlots.clear();
	if(0 == node || 0 == node->Token)
		rtrn 0;
	if(JSON_TYPE_OBJECT == node->Token->Type) {
		for(u2_t iChild = 0; iChild + 1 < node->Children.size(); iChild += 2) {
			cnst SJSONNode * key = node->Children[iChild];
			if(0 == key || key->ObjectIndex < 0 || (u2_t)key->ObjectIndex >= tool.InputJSON.View.size())
				continue;
			vcst_c keyText = tool.InputJSON.View[key->ObjectIndex];
			if(false == ::fileViewerJSONExpressionIdentifier(keyText))
				continue;
			string expression = tool.SuggestionBase;
			if_fail_fe(::fileViewerJSONExpressionPathKeyAppend(expression, keyText));
			if_fail_fe(tool.SuggestionText.push_back(string{keyText}));
			if_fail_fe(tool.SuggestionExpressions.push_back(expression));
			if_fail_fe(tool.SuggestionChildSlots.push_back(iChild + 1));
		}
	}
	else if(JSON_TYPE_ARRAY == node->Token->Type)
		for(u2_t iChild = 0; iChild < node->Children.size(); ++iChild) {
			sc_t indexText[32] = {};
			cnst err_t textLength = ::gpk::sprintf_s(indexText, "['%u']", iChild);
			if_fail_fe(textLength);
			string labelText = {};
			if_fail_fe(labelText.append({indexText, (u2_t)textLength}));
			string expression = tool.SuggestionBase;
			if_fail_fe(expression.append({indexText, (u2_t)textLength}));
			if_fail_fe(tool.SuggestionText.push_back(labelText));
			if_fail_fe(tool.SuggestionExpressions.push_back(expression));
			if_fail_fe(tool.SuggestionChildSlots.push_back(iChild));
		}
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsRefresh(SFileViewer & viewer, b8_t filter) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	tool.SuggestionSource = tool.Command.Text;
	tool.SuggestionVisible.clear();
	vcst_t partial = {};
	if(filter) {
		u2_t offset = 0;
		if(tool.SuggestionBase.size() && ::fileViewerJSONExpressionStartsWith(tool.Command.Text, tool.SuggestionBase)) {
			offset = tool.SuggestionBase.size();
			if(tool.Command.Text.size() > offset && '.' == tool.Command.Text[offset])
				++offset;
		}
		partial = {tool.Command.Text.begin() + offset, tool.Command.Text.size() - offset};
	}
	for(u2_t iSuggestion = 0; iSuggestion < tool.SuggestionText.size() && tool.SuggestionVisible.size() < JSON_EXPRESSION_SUGGESTION_LIMIT; ++iSuggestion)
		if(false == filter || ::fileViewerJSONExpressionStartsWith(tool.SuggestionText[iSuggestion], partial))
			if_fail_fe(tool.SuggestionVisible.push_back(iSuggestion));
	SGUI & gui = *viewer.Dialog->GUI;
	acid & idOptions = gui.Controls.Children[tool.IdSuggestions];
	for(u2_t iOption = 0; iOption < idOptions.size(); ++iOption) {
		cnst b8_t visible = iOption < tool.SuggestionVisible.size();
		gui.Controls.SetHidden(idOptions[iOption], false == visible);
		if(visible)
			if_fail_fe(controlTextSet(gui, idOptions[iOption], tool.SuggestionText[tool.SuggestionVisible[iOption]]));
	}
	if_fail_fe(guiControlListArrange(gui, tool.IdSuggestions, DIRECTION_DOWN));
	if(tool.Command.Editing && tool.SuggestionVisible.size()) {
		if(gui.Controls.IsHidden(tool.IdSuggestions)) {
			if_fail_fe(dropdownExpand(gui, tool.IdSuggestions));
		}
		else {
			if_fail_fe(controlMetricsInvalidate(gui, tool.IdSuggestions));
		}
	}
	else if(false == gui.Controls.IsHidden(tool.IdSuggestions))
		if_fail_fe(dropdownCollapse(gui, tool.IdSuggestions));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionSuggestionsFocus(SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(0 == tool.InputJSON.Tree.size() || 0 == tool.InputJSON.Tree[0] || 0 == tool.InputJSON.Tree[0]->Token)
		rtrn ::fileViewerJSONExpressionSuggestionsBuild(viewer, 0, {});
	cnst SJSONNode * node = tool.InputJSON.Tree[0];
	string base = {};
	b8_t filter = tool.Command.Text.size() != 0;
	if(tool.Command.Text.size()) {
		vcst_t resolvedText = {};
		cnst err_t iResolved = jsonExpressionResolve(tool.Command.Text, tool.InputJSON, 0, resolvedText);
		if(iResolved >= 0 && (u2_t)iResolved < tool.InputJSON.Tree.size() && tool.InputJSON.Tree[iResolved] && tool.InputJSON.Tree[iResolved]->Token) {
			cnst SJSONNode * resolved = tool.InputJSON.Tree[iResolved];
			if(JSON_TYPE_OBJECT == resolved->Token->Type || JSON_TYPE_ARRAY == resolved->Token->Type) {
				node = resolved;
				base = tool.Command.Text;
				filter = false;
			}
			else
				filter = false;
		}
		u2_t iSeparator = tool.Command.Text.size();
		while(iSeparator && '.' != tool.Command.Text[iSeparator - 1])
			--iSeparator;
		if(iSeparator && node == tool.InputJSON.Tree[0]) {
			vcst_c parentExpression = {tool.Command.Text.begin(), iSeparator - 1};
			cnst err_t iParent = jsonExpressionResolve(parentExpression, tool.InputJSON, 0, resolvedText);
			if(iParent >= 0 && (u2_t)iParent < tool.InputJSON.Tree.size() && tool.InputJSON.Tree[iParent] && tool.InputJSON.Tree[iParent]->Token) {
				node = tool.InputJSON.Tree[iParent];
				base = parentExpression;
			}
		}
	}
	if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(viewer, node, base));
	rtrn ::fileViewerJSONExpressionSuggestionsRefresh(viewer, filter);
}

sttc gpk::err_t fileViewerJSONExpressionCreate(SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if_fail_fe(::fileViewerViewportCreate(viewer, tool.IdViewport, "JSON EXPRESSIONS", {{72, 72}, {600, 650}}));
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, tool.IdViewport, viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.Identifier[tool.IdViewport] = JSON_EXPRESSION_TOOL_IDENTIFIER;
	if_fail_fe(::fileViewerJSONExpressionLabelCreate(viewer, viewport->IdClient, "INPUT FILE", {0, 0}));
	if_fail_fe(::fileViewerListCreate(viewer, tool.Inputs, tool.IdViewport));
	gui.Controls.Placement[tool.Inputs.IdControl].Area.Offset = {0, 24};
	gui.Controls.Placement[tool.Inputs.IdControl].Area.Size.x = 540;
	if_fail_fe(::fileViewerJSONExpressionLabelCreate(viewer, viewport->IdClient, "PREBUILT JSON EXPRESSIONS", {0, 190}));
	if_fail_fe(::fileViewerListCreate(viewer, tool.Presets, tool.IdViewport));
	gui.Controls.Placement[tool.Presets.IdControl].Area.Offset = {0, 214};
	gui.Controls.Placement[tool.Presets.IdControl].Area.Size.x = 540;
	if_fail_fe(::fileViewerJSONExpressionLabelCreate(viewer, viewport->IdClient, "EXPRESSION", {0, 430}));
	if_fail_fe(inputBoxCreate(tool.Command, gui, viewport->IdClient));
	gui.Controls.Identifier[tool.Command.IdText] = JSON_EXPRESSION_COMMAND_IDENTIFIER;
	tool.Command.MaxLength = 4096;
	gui.Controls.SetHidden(tool.Command.VirtualKeyboard.IdRoot, true);
	gui.Controls.Placement[tool.Command.IdRoot].Area = {{0, 454}, {448, 26}};
	gui.Controls.Placement[tool.Command.IdText].Area = {{0, 0}, {448, 26}};
	gui.Controls.Text[tool.Command.IdText].Align = ALIGN_CENTER_LEFT;
	if_fail_fe(tool.GUISystems.resize(GUI_TYPE_COUNT));
	SControlPlacement suggestionPlacement = {};
	suggestionPlacement.Area.Size = {448, 24};
	suggestionPlacement.Border = {1, 1, 1, 1};
	suggestionPlacement.Margin = {4, 2, 4, 2};
	sttc vcst_c emptySuggestions[JSON_EXPRESSION_SUGGESTION_LIMIT] = {};
	if_fail_fe(tool.IdSuggestions = guiCreateDropdown(gui, tool.GUISystems, view<vcst_c>{emptySuggestions}, tool.Command.IdText, suggestionPlacement, ALIGN_CENTER_LEFT));
	for(u2_t iOption = 0; iOption < gui.Controls.Children[tool.IdSuggestions].size(); ++iOption) {
		cnst cid_t idOption = gui.Controls.Children[tool.IdSuggestions][iOption];
		gui.Controls.Draw[idOption].Palettes = viewer.Dialog->Colors->Button;
		gui.Controls.SetHidden(idOption, true);
	}
	if_fail_fe(tool.IdRun = controlCreate(gui));
	gui.Controls.Placement[tool.IdRun].Area = {{452, 454}, {88, 26}};
	gui.Controls.Draw[tool.IdRun].Palettes = viewer.Dialog->Colors->Button;
	gui.Controls.Text[tool.IdRun].Text = "RUN";
	gui.Controls.Text[tool.IdRun].Align = ALIGN_CENTER;
	if_fail_fe(controlSetParent(gui, tool.IdRun, viewport->IdClient));
	if_fail_fe(::fileViewerEditorCreate(viewer, tool.Draft, {{696, 72}, {560, 650}}));
	tool.Draft.FileText = "[]";
	if_fail_fe(::fileViewerTextViewsRebuild(tool.Draft));
	pobj<SDialogViewport> draftViewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, tool.Draft.IdViewport, draftViewport));
	gui.Controls.Text[draftViewport->IdTitle].Text = "JSON EXPRESSION OUTPUT";
	gui.Controls.SetHidden(tool.Draft.IdKeyboard, true);
	gui.Controls.SetHidden(tool.Draft.IdSave, true);
	gui.Controls.SetHidden(tool.IdViewport, true);
	gui.Controls.SetHidden(tool.Draft.IdViewport, true);
	rtrn 0;
}

sttc vcst_t fileViewerJSONExpressionInputView(vcst_t input) {
	if(input.size() >= 3
	&& 0xEF == (u0_t)input[0]
	&& 0xBB == (u0_t)input[1]
	&& 0xBF == (u0_t)input[2])
		rtrn {input.begin() + 3, input.size() - 3};
	rtrn input;
}

sttc gpk::err_t fileViewerJSONExpressionInputSelect(SFileViewer & viewer, cid_t iEditor) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(iEditor < 0 || (u2_t)iEditor >= viewer.Editors.size())
		rtrn 0;
	tool.InputEditor = iEditor;
	SGUI & gui = *viewer.Dialog->GUI;
	tool.InputJSON.Tree.clear();
	if_fail_fe(tool.InputJSON.Reset());
	cnst vcst_t input = ::fileViewerJSONExpressionInputView(viewer.Editors[iEditor]->FileText);
	cnst bool inputParsed = input.size() && succeeded(jsonParse(tool.InputJSON, input));
	if_fail_fe(::fileViewerJSONExpressionPresetsRefresh(viewer));
	if(inputParsed && tool.InputJSON.Tree.size() && tool.InputJSON.Tree[0]) {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(viewer, tool.InputJSON.Tree[0], {}));
	}
	else {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(viewer, 0, {}));
	}
	if_fail_fe(::fileViewerJSONExpressionSuggestionsRefresh(viewer, false));
	for(u2_t iInput = 0; iInput < tool.Inputs.IdControls.size(); ++iInput)
		gui.Controls.SetSelected(tool.Inputs.IdControls[iInput], iInput == (u2_t)iEditor);
	tool.Draft.FilePath = viewer.Editors[iEditor]->FilePath;
	if_fail_fe(tool.Draft.FilePath.append(".expressions.json"));
	gui.Controls.SetHidden(tool.Draft.IdSave, false);
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, tool.IdViewport, viewport));
	gui.Controls.Text[viewport->IdTitle].Text = label(viewer.Editors[iEditor]->FilePath);
	if_fail_fe(dialogControlGet(*viewer.Dialog, tool.Draft.IdViewport, viewport));
	gui.Controls.Text[viewport->IdTitle].Text = label(tool.Draft.FilePath);
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionInputsUpdate(SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	while(tool.Inputs.IdControls.size() < viewer.Editors.size())
		if_fail_fe(::fileViewerListRowPush(viewer, tool.Inputs, {}, true));
	SGUI & gui = *viewer.Dialog->GUI;
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if(gui.Controls.Text[tool.Inputs.IdControls[iEditor]].Text != viewer.Editors[iEditor]->FilePath)
			if_fail_fe(controlTextSet(gui, tool.Inputs.IdControls[iEditor], viewer.Editors[iEditor]->FilePath));
	gui.Controls.Placement[tool.Inputs.IdControl].Area.Size.x = 540;
	if(tool.InputEditor < 0 && viewer.IdEditorCurrent >= 0)
		if_fail_fe(::fileViewerJSONExpressionInputSelect(viewer, viewer.IdEditorCurrent));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionRun(SFileViewer & viewer) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	if(tool.InputEditor < 0 || (u2_t)tool.InputEditor >= viewer.Editors.size() || 0 == tool.Command.Text.size())
		rtrn 0;
	SJSONReader inputJSON = {};
	cnst vcst_t input = ::fileViewerJSONExpressionInputView(viewer.Editors[tool.InputEditor]->FileText);
	if(0 == input.size() || failed(jsonParse(inputJSON, input)))
		rtrn 0;
	vcst_t output = {};
	cnst err_t iResult = jsonExpressionResolve(tool.Command.Text, inputJSON, 0, output);
	if(-1 == iResult)
		rtrn 0;
	string outputJSON = {};
	if(iResult >= 0) {
		if((u2_t)iResult >= inputJSON.Tree.size() || 0 == inputJSON.Tree[iResult])
			rtrn 0;
		if_fail_fe(jsonWrite(inputJSON.Tree[iResult], inputJSON.View, outputJSON));
	}
	else if(-5 == iResult) {
		if_fail_fe(outputJSON.append(output));
	}
	else {
		if_fail_fe(appendJSONString(outputJSON, output));
	}
	tool.Draft.FileText.clear();
	if_fail_fe(tool.Draft.FileText.append("[\n"));
	if_fail_fe(tool.Draft.FileText.push_back('\t'));
	if_fail_fe(tool.Draft.FileText.append(outputJSON));
	if_fail_fe(tool.Draft.FileText.append("\n]"));
	tool.Draft.TextSelections.clear();
	if_fail_fe(::fileViewerTextViewsRebuild(tool.Draft));
	tool.Draft.Cursor = {0, (u2_t)-1};
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.SetHidden(tool.Draft.IdViewport, false);
	if_fail_fe(controlToFront(gui, tool.Draft.IdViewport));
	rtrn 0;
}

sttc gpk::err_t fileViewerJSONExpressionUpdate(SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	SJSONExpressionTool & tool = viewer.JSONExpressions;
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsHidden(tool.IdViewport))
		rtrn 0;
	if_fail_fe(::fileViewerJSONExpressionInputsUpdate(viewer));
	for(u2_t iInput = 0; iInput < tool.Inputs.IdControls.size() && iInput < viewer.Editors.size(); ++iInput)
		if(gui.Controls.IsAction(tool.Inputs.IdControls[iInput]))
			if_fail_fe(::fileViewerJSONExpressionInputSelect(viewer, (cid_t)iInput));
	for(u2_t iPreset = 0; iPreset < tool.Presets.IdControls.size() && iPreset < tool.PresetExpressions.size(); ++iPreset)
		if(gui.Controls.IsAction(tool.Presets.IdControls[iPreset]))
			if_fail_fe(tool.Command.SetText(gui, tool.PresetExpressions[iPreset]));
	b8_t suggestionSelected = false;
	cnst acid & idOptions = gui.Controls.Children[tool.IdSuggestions];
	for(u2_t iOption = 0; iOption < idOptions.size() && iOption < tool.SuggestionVisible.size(); ++iOption)
		if(gui.Controls.IsAction(idOptions[iOption])) {
			cnst u2_t iSuggestion = tool.SuggestionVisible[iOption];
			if(iSuggestion >= tool.SuggestionExpressions.size() || iSuggestion >= tool.SuggestionChildSlots.size())
				continue;
			if_fail_fe(tool.Command.SetText(gui, tool.SuggestionExpressions[iSuggestion]));
			cnst u2_t iChild = tool.SuggestionChildSlots[iSuggestion];
			if(tool.SuggestionNode && iChild < tool.SuggestionNode->Children.size()) {
				cnst SJSONNode * selectedNode = tool.SuggestionNode->Children[iChild];
				if(selectedNode && selectedNode->Token && (JSON_TYPE_OBJECT == selectedNode->Token->Type || JSON_TYPE_ARRAY == selectedNode->Token->Type))
					if_fail_fe(::fileViewerJSONExpressionSuggestionsBuild(viewer, selectedNode, tool.SuggestionExpressions[iSuggestion]));
			}
			tool.Command.Editing = true;
			gui.Controls.SetFocused(tool.Command.IdText, true);
			if_fail_fe(::fileViewerJSONExpressionSuggestionsRefresh(viewer, false));
			suggestionSelected = true;
			break;
		}
	if(gui.Controls.IsAction(tool.Command.IdText)) {
		tool.Command.Editing = true;
		if_fail_fe(::fileViewerJSONExpressionSuggestionsFocus(viewer));
	}
	else if(tool.Command.Editing && false == suggestionSelected && false == gui.Controls.IsFocused(tool.Command.IdText))
		tool.Command.Editing = false;
	b8_t run = gui.Controls.IsAction(tool.IdRun);
	if(tool.Command.Editing) {
		astcid<1> processableControls = {tool.Command.IdText};
		cnst err_t inputResult = tool.Command.Update(gui, tool.Command.VirtualKeyboard, frameEvents, processableControls);
		run |= INT_MAX == inputResult;
	}
	if(tool.SuggestionSource != tool.Command.Text && false == suggestionSelected) {
		if_fail_fe(::fileViewerJSONExpressionSuggestionsRefresh(viewer, true));
	}
	else if(false == tool.Command.Editing && false == gui.Controls.IsHidden(tool.IdSuggestions)) {
		if_fail_fe(dropdownCollapse(gui, tool.IdSuggestions));
	}
	gui.Controls.SetHidden(tool.Command.IdRoot, false);
	gui.Controls.SetHidden(tool.Command.VirtualKeyboard.IdRoot, true);
	if(run)
		if_fail_fe(::fileViewerJSONExpressionRun(viewer));
	if(gui.Controls.IsAction(tool.Draft.IdSave) && tool.Draft.FilePath.size())
		if_fail_fef(fileFromMemory(tool.Draft.FilePath, tool.Draft.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(tool.Draft.FilePath));
	rtrn 0;
}

sttc u2_t fileViewerLineOffset(cnst SFileEditor & editor, u2_t line) {
	u2_t offset = 0;
	u2_t iLine = 0;
	while(offset < editor.FileText.size() && iLine < line)
		if('\n' == editor.FileText[offset++])
			++iLine;
	rtrn offset;
}

sttc u2_t fileViewerCursorOffset(cnst SFileEditor & editor) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn editor.FileText.size();
	vcst_c line = editor.FileLines[editor.Cursor.y];
	rtrn ::fileViewerLineOffset(editor, editor.Cursor.y) + min(editor.Cursor.x, line.size());
}

sttc void fileViewerCursorOffsetSet(SFileEditor & editor, u2_t offset) {
	editor.Cursor = {};
	for(u2_t iCharacter = 0, countCharacter = min(offset, editor.FileText.size()); iCharacter < countCharacter; ++iCharacter) {
		if('\n' == editor.FileText[iCharacter]) {
			++editor.Cursor.y;
			editor.Cursor.x = 0;
		}
		else if('\r' != editor.FileText[iCharacter] || iCharacter + 1 >= editor.FileText.size() || '\n' != editor.FileText[iCharacter + 1])
			++editor.Cursor.x;
	}
}

sttc gpk::err_t fileViewerTextInsert(SFileViewer & viewer, SFileEditor & editor, sc_t character) {
	u2_c offset = ::fileViewerCursorOffset(editor);
	if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
	if_fail_fe(editor.FileText.insert(offset, character));
	editor.TextSelections.clear();
	if_fail_fe(::fileViewerTextViewsRebuild(editor));
	if('\n' == character) {
		++editor.Cursor.y;
		editor.Cursor.x = 0;
	}
	else
		++editor.Cursor.x;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextBackspace(SFileViewer & viewer, SFileEditor & editor) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(editor);
	if(editor.Cursor.x) {
		if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
		if_fail_fe(editor.FileText.remove(cursorOffset - 1));
		--editor.Cursor.x;
	}
	else if(editor.Cursor.y) {
		vcst_c previousLine = editor.FileLines[editor.Cursor.y - 1];
		u2_c separatorOffset = ::fileViewerLineOffset(editor, editor.Cursor.y - 1) + previousLine.size();
		u2_c separatorCount = cursorOffset - separatorOffset;
		editor.Cursor.x = previousLine.size();
		--editor.Cursor.y;
		if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
		for(u2_t iCharacter = 0; iCharacter < separatorCount; ++iCharacter)
			if_fail_fe(editor.FileText.remove(separatorOffset));
	}
	editor.TextSelections.clear();
	if_fail_fe(::fileViewerTextViewsRebuild(editor));
	rtrn 0;
}

sttc gpk::err_t fileViewerTextDelete(SFileViewer & viewer, SFileEditor & editor) {
	if(editor.TextSelections.size()) {
		apod<rangeu2_t> deletions = {};
		u2_t cursorOffset = editor.FileText.size();
		for(u2_t iSelection = 0; iSelection < editor.TextSelections.size(); ++iSelection) {
			rangeu2_c selection = editor.TextSelections[iSelection];
			u2_c selectionBegin = min(selection.Offset, editor.FileText.size());
			u2_c selectionEnd = (u2_t)min((u3_t)editor.FileText.size(), (u3_t)selection.Offset + selection.Count);
			if(selectionBegin < selectionEnd) {
				if_fail_fe(deletions.push_back({selectionBegin, selectionEnd - selectionBegin}));
				cursorOffset = min(cursorOffset, selectionBegin);
			}
		}
		if(deletions.size()) {
			for(u2_t iDeletion = 1; iDeletion < deletions.size(); ++iDeletion) {
				rangeu2_c deletion = deletions[iDeletion];
				u2_t iPrevious = iDeletion;
				while(iPrevious && deletions[iPrevious - 1].Offset > deletion.Offset) {
					deletions[iPrevious] = deletions[iPrevious - 1];
					--iPrevious;
				}
				deletions[iPrevious] = deletion;
			}
			u2_t countMerged = 1;
			for(u2_t iDeletion = 1; iDeletion < deletions.size(); ++iDeletion) {
				rangeu2_t & previous = deletions[countMerged - 1];
				rangeu2_c current = deletions[iDeletion];
				u3_c previousEnd = (u3_t)previous.Offset + previous.Count;
				u3_c currentEnd = (u3_t)current.Offset + current.Count;
				if(current.Offset <= previousEnd)
					previous.Count = (u2_t)(max(previousEnd, currentEnd) - previous.Offset);
				else
					deletions[countMerged++] = current;
			}
			if_fail_fe(deletions.resize(countMerged));
			if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
			for(u2_t iDeletion = deletions.size(); iDeletion; --iDeletion)
				for(u2_t iCharacter = 0; iCharacter < deletions[iDeletion - 1].Count; ++iCharacter)
					if_fail_fe(editor.FileText.remove(deletions[iDeletion - 1].Offset));
			editor.TextSelections.clear();
			if_fail_fe(::fileViewerTextViewsRebuild(editor));
			::fileViewerCursorOffsetSet(editor, cursorOffset);
			rtrn 0;
		}
		editor.TextSelections.clear();
	}
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(editor);
	if(cursorOffset >= editor.FileText.size())
		rtrn 0;
	u2_t count = 1;
	if('\r' == editor.FileText[cursorOffset] && cursorOffset + 1 < editor.FileText.size() && '\n' == editor.FileText[cursorOffset + 1])
		count = 2;
	if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
	for(u2_t iCharacter = 0; iCharacter < count; ++iCharacter)
		if_fail_fe(editor.FileText.remove(cursorOffset));
	editor.TextSelections.clear();
	rtrn ::fileViewerTextViewsRebuild(editor);
}

sttc gpk::err_t fileViewerEditorHandle(SFileViewer & viewer, SFileEditor & editor, cnst SVirtualKeyboardEvent & eventToProcess) {
	if(editor.Cursor.y >= editor.FileLines.size())
		rtrn 0;
	if(VK_EVENT_Release == eventToProcess.Type)
		rtrn ::fileViewerTextInsert(viewer, editor, (sc_t)eventToProcess.ScanCode);
	if(VK_EVENT_Edit != eventToProcess.Type)
		rtrn 0;
	switch(eventToProcess.ScanCode) {
	default: break;
	case VK_SCANCODE_Escape:
		viewer.Dialog->GUI->Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		editor.Cursor = {0, (u2_t)-1};
		break;
	case VK_SCANCODE_Delete		: if_fail_fe(::fileViewerTextDelete(viewer, editor));		break;
	case VK_SCANCODE_Enter		: if_fail_fe(::fileViewerTextInsert(viewer, editor, '\n'));	break;
	case VK_SCANCODE_Backspace	: if_fail_fe(::fileViewerTextBackspace(viewer, editor));	break;
	case VK_SCANCODE_Clear		: if_fail_fe(::fileViewerTextViewsClear(viewer, editor));
		editor.FileText.clear();
		editor.TextSelections.clear();
		editor.Cursor = {};
		if_fail_fe(::fileViewerTextViewsRebuild(editor));
		break;
	case VK_SCANCODE_Left:
		if(editor.Cursor.x)
			--editor.Cursor.x;
		else if(editor.Cursor.y) {
			--editor.Cursor.y;
			editor.Cursor.x = editor.FileLines[editor.Cursor.y].size();
		}
		break;
	case VK_SCANCODE_Right:
		if(editor.Cursor.x < editor.FileLines[editor.Cursor.y].size())
			++editor.Cursor.x;
		else if(editor.Cursor.y + 1 < editor.FileLines.size()) {
			++editor.Cursor.y;
			editor.Cursor.x = 0;
		}
		break;
	case VK_SCANCODE_Up:
		if(editor.Cursor.y) {
			--editor.Cursor.y;
			editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		}
		break;
	case VK_SCANCODE_Down:
		if(editor.Cursor.y + 1 < editor.FileLines.size()) {
			++editor.Cursor.y;
			editor.Cursor.x = min(editor.Cursor.x, editor.FileLines[editor.Cursor.y].size());
		}
		break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorPhysicalInput(SFileViewer & viewer, SFileEditor & editor, vpobj<SEventSystem> frameEvents) {
	for(u2_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
		cnst pobj<SEventSystem> & systemEvent = frameEvents[iEvent];
		if(systemEvent->Type == SYSTEM_EVENT_Text) {
			if_fail_fe(eventExtractAndHandle<EVENT_TEXT>(*systemEvent, [&viewer, &editor](cnst SEventView<EVENT_TEXT> & eventText) {
				if(eventText.Type == EVENT_TEXT_Char && eventText.Data.size() && eventText.Data[0] >= 0x20 && eventText.Data[0] <= 0x7F)
					if_fail_fe(::fileViewerTextInsert(viewer, editor, (sc_t)eventText.Data[0]));
				rtrn 0;
			}));
		}
		else if(systemEvent->Type == SYSTEM_EVENT_Keyboard) {
			if_fail_fe(eventExtractAndHandle<EVENT_KEYBOARD>(*systemEvent, [&viewer, &editor](cnst SEventView<EVENT_KEYBOARD> & eventKeyboard) {
				if(eventKeyboard.Type != EVENT_KEYBOARD_Down || 0 == eventKeyboard.Data.size())
					rtrn 0;
				VK_SCANCODE scanCode = {};
				switch(eventKeyboard.Data[0]) {
				default: rtrn 0;
				case SYSTEM_KEY_Escape		: scanCode = VK_SCANCODE_Escape	; break;
				case SYSTEM_KEY_Enter		: scanCode = VK_SCANCODE_Enter	; break;
				case SYSTEM_KEY_Backspace	: scanCode = VK_SCANCODE_Backspace; break;
				case SYSTEM_KEY_Clear		: scanCode = VK_SCANCODE_Clear	; break;
				case SYSTEM_KEY_Delete		: scanCode = VK_SCANCODE_Delete	; break;
				case SYSTEM_KEY_Left		: scanCode = VK_SCANCODE_Left	; break;
				case SYSTEM_KEY_Right		: scanCode = VK_SCANCODE_Right	; break;
				case SYSTEM_KEY_Up			: scanCode = VK_SCANCODE_Up		; break;
				case SYSTEM_KEY_Down		: scanCode = VK_SCANCODE_Down	; break;
				}
				cnst SVirtualKeyboardEvent editorEvent = {VK_EVENT_Edit, scanCode};
				rtrn ::fileViewerEditorHandle(viewer, editor, editorEvent);
			}));
		}
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorUpdate(SFileViewer & viewer, SFileEditor & editor, cid_t iEditor) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbarY = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, editor.IdViewport, viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsHidden(viewport->IdGUIControl))
		rtrn 0;
	if(gui.Controls.IsAction(editor.IdKeyboard)) {
		viewer.IdEditorCurrent = iEditor;
		gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false == gui.Controls.IsHidden(viewer.VirtualKeyboard.IdRoot));
	}
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	u2_c firstVisible = (u2_t)scrollbarY->ValueCurrent;
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl)
		if(gui.Controls.IsAction(editor.Lines.IdControls[iControl])) {
			u2_c iLine = firstVisible + iControl;
			if(iLine < editor.FileLines.size()) {
				viewer.IdEditorCurrent = iEditor;
			}
		}
	if(gui.Controls.IsAction(editor.IdSave) && editor.FilePath.size())
		if_fail_fef(fileFromMemory(editor.FilePath, editor.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(editor.FilePath));
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorMousePosition(SFileViewer & viewer, SFileEditor & editor, n2u2_t & cursor, u2_t & characterOffset, b8_t & overTextLine) {
	overTextLine = false;
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbarY = {};
	pobj<SDialogSlider> scrollbarX = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, editor.IdViewport, viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbarX));
	SGUI & gui = *viewer.Dialog->GUI;
	u2_c firstVisible = (u2_t)scrollbarY->ValueCurrent;
	u2_c firstColumn = (u2_t)scrollbarX->ValueCurrent;
	u0_c characterWidth = max((u0_t)1, gui.Fonts[gui.SelectedFont]->CharSize.x);
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl) {
		cnst cid_t idControl = editor.Lines.IdControls[iControl];
		if(gui.Controls.IsHidden(idControl) || false == gui.Controls.IsHovered(idControl))
			continue;
		u2_c iLine = firstVisible + iControl;
		if(iLine >= editor.FileLines.size())
			continue;
		cnst s2_t mouseOffsetX = (s2_t)viewer.Dialog->Input->MouseCurrent.Position.x - gui.Controls.Area[idControl].Text.Offset.x;
		u2_c visibleColumn = 0 >= mouseOffsetX ? 0 : (u2_t)(mouseOffsetX / characterWidth);
		u2_c iColumn = min((u2_t)(firstColumn + visibleColumn), editor.FileLines[iLine].size());
		cursor = {iColumn, iLine};
		characterOffset = ::fileViewerLineOffset(editor, iLine) + iColumn;
		overTextLine = true;
		break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorSelectionUpdate(SFileViewer & viewer, SFileEditor & editor, cid_t iEditor) {
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsHidden(editor.IdViewport))
		rtrn 0;
	n2u2_t cursor = {};
	u2_t characterOffset = 0;
	b8_t overTextLine = false;
	if_fail_fe(::fileViewerEditorMousePosition(viewer, editor, cursor, characterOffset, overTextLine));
	if(viewer.Dialog->Input->ButtonDown(0) && overTextLine) {
		viewer.IdEditorCurrent = iEditor;
		editor.Cursor = cursor;
		if(0 == viewer.Dialog->Input->KeyboardCurrent.KeyState[SYSTEM_KEY_Control])
			editor.TextSelections.clear();
		editor.SelectionAnchor = characterOffset;
		err_t iSelection = editor.TextSelections.push_back({characterOffset, 0});
		if_fail_fe(iSelection);
		editor.SelectionActive = (u2_t)iSelection;
	}
	if((u2_t)-1 != editor.SelectionAnchor && (u2_t)-1 != editor.SelectionActive && editor.SelectionActive < editor.TextSelections.size() && overTextLine) {
		editor.Cursor = cursor;
		rangeu2_t & selection = editor.TextSelections[editor.SelectionActive];
		selection.Offset = min(editor.SelectionAnchor, characterOffset);
		selection.Count = max(editor.SelectionAnchor, characterOffset) - selection.Offset;
	}
	if(viewer.Dialog->Input->ButtonUp(0) && (u2_t)-1 != editor.SelectionAnchor) {
		if(editor.SelectionActive < editor.TextSelections.size() && 0 == editor.TextSelections[editor.SelectionActive].Count)
			if_fail_fe(editor.TextSelections.remove(editor.SelectionActive));
		editor.SelectionAnchor = (u2_t)-1;
		editor.SelectionActive = (u2_t)-1;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorInputUpdate(SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	if(viewer.IdEditorCurrent < 0 || (u2_t)viewer.IdEditorCurrent >= viewer.Editors.size())
		rtrn 0;
	SFileEditor & editor = *viewer.Editors[viewer.IdEditorCurrent];
	if(viewer.Dialog->GUI->Controls.IsHidden(editor.IdViewport)) {
		viewer.Dialog->GUI->Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		rtrn 0;
	}
	SGUI & gui = *viewer.Dialog->GUI;
	apod<SVirtualKeyboardEvent> vkEvents = {};
	cnst cid_t idCharacterBegin = viewer.VirtualKeyboard.IdRoot + 1;
	cnst cid_t idCharacterEnd = idCharacterBegin + (cid_t)viewer.VirtualKeyboard.Keys.size();
	for(cid_t idKey = idCharacterBegin; idKey < idCharacterEnd; ++idKey)
		if(gui.Controls.IsAction(idKey))
			if_fail_fe(virtualKeyboardHandleEvent(viewer.VirtualKeyboard, idKey, vkEvents));
	for(u2_t iKey = 0; iKey < viewer.VirtualKeyboard.IdKeys.size(); ++iKey)
		if(gui.Controls.IsAction(viewer.VirtualKeyboard.IdKeys[iKey]))
			if_fail_fe(virtualKeyboardHandleEvent(viewer.VirtualKeyboard, viewer.VirtualKeyboard.IdKeys[iKey], vkEvents));
	for(u2_t iEvent = 0; iEvent < vkEvents.size(); ++iEvent)
		if_fail_fe(::fileViewerEditorHandle(viewer, editor, vkEvents[iEvent]));
	rtrn ::fileViewerEditorPhysicalInput(viewer, editor, frameEvents);
}

sttc gpk::err_t fileViewerTextWindowUpdate(SFileViewer & viewer, SFileEditor & editor) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbarY = {};
	pobj<SDialogSlider> scrollbarX = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, editor.IdViewport, viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbarX));
	SGUI & gui = *viewer.Dialog->GUI;
	if(0 == editor.Lines.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, editor.Lines, {}, true, false));
	cnst s1_t rowHeight = max((s1_t)1, gui.Controls.Placement[editor.Lines.IdControls[0]].Area.Size.y);
	n2s1_c clientSize = gui.Controls.Area[viewport->IdClient].Client.Global.Size;
	cnst s1_t scrollbarWidth = gui.Controls.Placement[scrollbarY->IdGUIControl].Area.Size.x;
	cnst s1_t scrollbarHeight = gui.Controls.Placement[scrollbarX->IdGUIControl].Area.Size.y;
	cnst u0_t characterWidth = max((u0_t)1, gui.Fonts[gui.SelectedFont]->CharSize.x);
	s3_t visibleCount = 1;
	s3_t visibleColumns = 1;
	b8_t scrollX = false;
	b8_t scrollY = false;
	for(u0_t iPass = 0; iPass < 3; ++iPass) {
		visibleCount = max((s3_t)1, (s3_t)(clientSize.y - (scrollX ? scrollbarHeight : 0)) / rowHeight);
		visibleColumns = max((s3_t)1, (s3_t)(clientSize.x - (scrollY ? scrollbarWidth : 0)) / characterWidth);
		scrollY = editor.FileLines.size() > (u2_t)visibleCount;
		scrollX = editor.FileLineWidthMax > (u2_t)visibleColumns;
	}
	b8_t arrange = false;
	while(editor.Lines.IdControls.size() < (u2_t)visibleCount) {
		if_fail_fe(::fileViewerListRowPush(viewer, editor.Lines, {}, true, false));
		arrange = true;
	}
	cnst s3_t maxOffsetY = max((s3_t)0, (s3_t)editor.FileLines.size() - visibleCount);
	b8_c cursorActive = editor.Cursor.y < editor.FileLines.size();
	cnst s3_t maxOffsetX = max((s3_t)0, (s3_t)editor.FileLineWidthMax + one_if(cursorActive) - visibleColumns);
	scrollbarY->ValueLimits = {0, max((s3_t)1, maxOffsetY)};
	scrollbarX->ValueLimits = {0, max((s3_t)1, maxOffsetX)};
	if_fail_fe(sliderSetValue(*scrollbarY, maxOffsetY ? scrollbarY->ValueCurrent : 0));
	if_fail_fe(sliderSetValue(*scrollbarX, maxOffsetX ? scrollbarX->ValueCurrent : 0));
	if(cursorActive) {
		if(editor.Cursor.y < (u2_t)scrollbarY->ValueCurrent) {
			if_fail_fe(sliderSetValue(*scrollbarY, editor.Cursor.y));
		}
		else if(editor.Cursor.y >= (u2_t)(scrollbarY->ValueCurrent + visibleCount)) {
			if_fail_fe(sliderSetValue(*scrollbarY, editor.Cursor.y - visibleCount + 1));
		}
		if(editor.Cursor.x < (u2_t)scrollbarX->ValueCurrent) {
			if_fail_fe(sliderSetValue(*scrollbarX, editor.Cursor.x));
		}
		else if(editor.Cursor.x >= (u2_t)(scrollbarX->ValueCurrent + visibleColumns)) {
			if_fail_fe(sliderSetValue(*scrollbarX, editor.Cursor.x - visibleColumns + 1));
		}
	}
	viewport->Settings.ScrollBarY = 0 != maxOffsetY;
	viewport->Settings.ScrollBarX = 0 != maxOffsetX;
	u2_c firstVisible = (u2_t)(maxOffsetY ? scrollbarY->ValueCurrent : 0);
	u2_c firstColumn = (u2_t)(maxOffsetX ? scrollbarX->ValueCurrent : 0);
	for(u2_t iControl = 0; iControl < editor.Lines.IdControls.size(); ++iControl) {
		cnst cid_t idControl = editor.Lines.IdControls[iControl];
		u2_c iLine = firstVisible + iControl;
		b8_c visible = iControl < (u2_t)visibleCount && iLine < editor.FileLines.size();
		if(visible) {
			vcst_c sourceLine = editor.FileLines[iLine];
			u2_c offset = min(firstColumn, sourceLine.size());
			u2_c count = min((u2_t)visibleColumns, (u2_t)(sourceLine.size() - offset));
			vcst_t visibleText = {};
			if_fail_fe(sourceLine.slice(visibleText, offset, count));
			if(gui.Controls.Text[idControl].Text != visibleText)
				if_fail_fe(controlTextSet(gui, idControl, visibleText));
			apod<STextState> & textState = gui.Controls.TextState[idControl];
			if_fail_fe(textState.resize(visibleText.size()));
			u2_c lineOffset = ::fileViewerLineOffset(editor, iLine);
			for(u2_t iCharacter = 0; iCharacter < textState.size(); ++iCharacter) {
				u2_c characterOffset = lineOffset + offset + iCharacter;
				b8_t selected = false;
				for(u2_t iSelection = 0; iSelection < editor.TextSelections.size(); ++iSelection) {
					rangeu2_c selection = editor.TextSelections[iSelection];
					if(characterOffset >= selection.Offset && (u3_t)characterOffset < (u3_t)selection.Offset + selection.Count) {
						selected = true;
						break;
					}
				}
				textState[iCharacter].SetSelected(idControl, iCharacter, gui.Controls.EventQueue, selected);
				textState[iCharacter].SetCursor(idControl, iCharacter, gui.Controls.EventQueue, iLine == editor.Cursor.y && offset + iCharacter == editor.Cursor.x);
			}
		}
		else
			gui.Controls.TextState[idControl].clear();
		gui.Controls.SetSelected(idControl, visible && iLine == editor.Cursor.y);
		if(gui.Controls.IsHidden(idControl) == visible) {
			gui.Controls.SetHidden(idControl, false == visible);
			arrange = true;
		}
	}
	b8_c cursorVisible = cursorActive
		&& editor.Cursor.y >= firstVisible && editor.Cursor.y < firstVisible + visibleCount
		&& editor.Cursor.x >= firstColumn && editor.Cursor.x < firstColumn + visibleColumns;
	if(cursorVisible) {
		SControlPlacement & cursorPlacement = gui.Controls.Placement[editor.IdCursor];
		n2s1_c cursorOffset = {
			(s1_t)((editor.Cursor.x - firstColumn) * characterWidth),
			(s1_t)((editor.Cursor.y - firstVisible) * rowHeight),
		};
		n2s1_c cursorSize = {(s1_t)characterWidth, rowHeight};
		if(cursorPlacement.Area.Offset.x != cursorOffset.x || cursorPlacement.Area.Offset.y != cursorOffset.y || cursorPlacement.Area.Size.x != cursorSize.x || cursorPlacement.Area.Size.y != cursorSize.y) {
			cursorPlacement.Area.Offset = cursorOffset;
			cursorPlacement.Area.Size = cursorSize;
			if_fail_fe(controlMetricsInvalidate(gui, editor.IdCursor));
		}
	}
	if(gui.Controls.IsHidden(editor.IdCursor) == cursorVisible)
		gui.Controls.SetHidden(editor.IdCursor, false == cursorVisible);
	if(arrange) {
		if_fail_fe(controlListArrange(gui, editor.Lines));
		if_fail_fe(controlMetricsInvalidate(gui, editor.Lines.IdControl));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerViewportLayoutConfigure(SFileViewer & viewer, cid_t idViewport, FILE_VIEWER_LAYOUT layout) {
	if(FILE_VIEWER_LAYOUT_FLOATING == layout)
		rtrn 0;
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, idViewport, viewport));
	viewport->Settings.DisplacementLockX = true;
	viewport->Settings.DisplacementLockY = true;
	viewer.Dialog->GUI->Controls.SetHidden(viewport->IdResizeBottomRight, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerDirectoryCreate(SFileViewer & viewer, vcst_t path, FILE_VIEWER_LAYOUT layout) {
	pobj<SPathList> directory = {};
	if_null_fe(directory.create());
	s1_c cascade = (s1_t)(24 * (viewer.Directories.size() % 8));
	if_fail_fe(::fileViewerViewportCreate(viewer, directory->IdViewport, "FILES / CURRENT DIRECTORY", {{(s1_t)(40 + cascade), (s1_t)(48 + cascade)}, {420, 600}}));
	if_fail_fe(::fileViewerListCreate(viewer, directory->Files, directory->IdViewport));
	if_fail_fe(::fileViewerScrollCreate(viewer, directory->IdViewport, true));
	if_fail_fe(::fileViewerFolderLoad(viewer, *directory, path));
	if_fail_fe(::fileViewerViewportLayoutConfigure(viewer, directory->IdViewport, layout));
	rtrn viewer.Directories.push_back(directory);
}

sttc gpk::err_t fileViewerEditorOpen(SFileViewer & viewer, vcst_t path, FILE_VIEWER_LAYOUT layout) {
	pobj<SFileEditor> editor = {};
	if_null_fe(editor.create());
	s1_c cascade = (s1_t)(24 * (viewer.Editors.size() % 8));
	if_fail_fe(::fileViewerEditorCreate(viewer, *editor, {{(s1_t)(480 + cascade), (s1_t)(72 + cascade)}, {760, 560}}));
	if_fail_fe(::fileViewerTextLoad(viewer, *editor, path));
	if_fail_fe(::fileViewerViewportLayoutConfigure(viewer, editor->IdViewport, layout));
	if_fail_fe(viewer.Editors.push_back(editor));
	viewer.IdEditorCurrent = (cid_t)viewer.Editors.size() - 1;
	vcst_t fileName = {};
	if_fail_fe(pathFilename(path, fileName));
	if_fail_fe(controlListPush(*viewer.Dialog->GUI, viewer.EditorTabs, label(fileName)));
	cnst cid_t idTab = viewer.EditorTabs.IdControls[viewer.EditorTabs.IdControls.size() - 1];
	::fileViewerListRowConfigure(viewer, idTab, true);
	viewer.Dialog->GUI->Controls.SetSelected(idTab, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsAction(viewer.IdMenuFiles))
		if_fail_fe(::fileViewerDirectoryCreate(viewer, viewer.RootPath, layout));
	if(gui.Controls.IsAction(viewer.IdMenuJSON)) {
		b8_c hidden = gui.Controls.IsHidden(viewer.JSONExpressions.IdViewport);
		gui.Controls.SetHidden(viewer.JSONExpressions.IdViewport, false == hidden);
		gui.Controls.SetHidden(viewer.JSONExpressions.Draft.IdViewport, false == hidden);
		if(hidden) {
			if_fail_fe(controlToFront(gui, viewer.JSONExpressions.IdViewport));
			if_fail_fe(controlToFront(gui, viewer.JSONExpressions.Draft.IdViewport));
		}
	}
	gui.Controls.SetSelected(viewer.IdMenuJSON, false == gui.Controls.IsHidden(viewer.JSONExpressions.IdViewport));
	if(gui.Controls.IsAction(viewer.IdMenuExit))
		rtrn APPLICATION_STATE_EXIT;
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size() && iEditor < viewer.EditorTabs.IdControls.size(); ++iEditor) {
		SFileEditor & editor = *viewer.Editors[iEditor];
		cnst cid_t idTab = viewer.EditorTabs.IdControls[iEditor];
		b8_t hidden = gui.Controls.IsHidden(editor.IdViewport);
		if(gui.Controls.IsAction(idTab)) {
			gui.Controls.SetHidden(editor.IdViewport, false == hidden);
			hidden = false == hidden;
			if(false == hidden) {
				viewer.IdEditorCurrent = (cid_t)iEditor;
				if_fail_fe(controlToFront(gui, editor.IdViewport));
			}
			else if(viewer.IdEditorCurrent == (cid_t)iEditor) {
				viewer.IdEditorCurrent = -1;
				gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
			}
		}
		gui.Controls.SetSelected(idTab, false == hidden);
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerSelectionUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	SGUI & gui = *viewer.Dialog->GUI;
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory) {
		SPathList & directory = *viewer.Directories[iDirectory];
		if(gui.Controls.IsHidden(directory.IdViewport))
			continue;
		pobj<SDialogViewport> filesViewport = {};
		pobj<SDialogSlider> filesScrollbar = {};
		if_fail_fe(dialogControlGet(*viewer.Dialog, directory.IdViewport, filesViewport));
		if_fail_fe(dialogControlGet(*viewer.Dialog, filesViewport->IdScrollBarY, filesScrollbar));
		u2_c firstVisible = (u2_t)filesScrollbar->ValueCurrent;
		b8_c hasParent = ::fileViewerFolderHasParent(directory.PathContents);
		for(u2_t iFile = 0; iFile < directory.Files.IdControls.size(); ++iFile) {
			cnst cid_t idControl = directory.Files.IdControls[iFile];
			if(false == gui.Controls.IsAction(idControl))
				continue;
			u2_c iItem = firstVisible + iFile;
			for(u2_t iSelection = 0; iSelection < directory.Files.IdControls.size(); ++iSelection)
				gui.Controls.SetSelected(directory.Files.IdControls[iSelection], iSelection == iFile);
			if(hasParent && 0 == iItem) {
				vcst_t parent = {};
				if_fail_fe(pathDirectory(directory.PathContents.Root, parent));
				rtrn ::fileViewerFolderLoad(viewer, directory, parent);
			}
			u2_c iPath = iItem - one_if(hasParent);
			if(iPath >= directory.PathContents.Name.size())
				rtrn 0;
			string path = {};
			if_fail_fe(pathNameCompose(directory.PathContents.Root, directory.PathContents.Name[iPath], path));
			if(directory.PathContents.Info[iPath].Folder)
				rtrn ::fileViewerFolderLoad(viewer, directory, path);
			rtrn ::fileViewerEditorOpen(viewer, path, layout);
		}
	}
	rtrn 0;
}

gpk::err_t fileViewerSetup(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath) {
	SGUI & gui = *viewer.Dialog->GUI;
	gui.ColorModeDefault = GUI_COLOR_MODE_3D;
	gui.ThemeDefault = ASCII_COLOR_Cyan * 16 + 8;
	viewer.RootPath = rootPath.size() ? rootPath : vcst_t{"."};
	if_fail_fe(::fileViewerMenuCreate(viewer));
	if_fail_fe(virtualKeyboardSetup437(gui, viewer.VirtualKeyboard));
	if_fail_fe(controlSetParent(gui, viewer.VirtualKeyboard.IdRoot, viewer.Dialog->Root));
	gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
	if_fail_fe(::fileViewerJSONExpressionCreate(viewer));
	if_fail_fe(::fileViewerDirectoryCreate(viewer, viewer.RootPath, layout));
	rtrn ::fileViewerLayoutSet(viewer, layout);
}

gpk::err_t fileViewerUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents) {
	if(gpk::err_t result = ::fileViewerMenuUpdate(viewer, layout))
		rtrn result;
	if_fail_fe(::fileViewerSelectionUpdate(viewer, layout));
	if_fail_fe(::fileViewerJSONExpressionUpdate(viewer, frameEvents));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerEditorUpdate(viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
	if(false == viewer.JSONExpressions.Command.Editing)
		if_fail_fe(::fileViewerEditorInputUpdate(viewer, frameEvents));
	if_fail_fe(viewer.Dialog->Update());
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerEditorSelectionUpdate(viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory)
		if_fail_fe(::fileViewerListWindowUpdate(viewer, viewer.Directories[iDirectory]->Files, viewer.Directories[iDirectory]->PathContents, viewer.Directories[iDirectory]->IdViewport));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerTextWindowUpdate(viewer, *viewer.Editors[iEditor]));
	if(false == viewer.Dialog->GUI->Controls.IsHidden(viewer.JSONExpressions.Draft.IdViewport))
		if_fail_fe(::fileViewerTextWindowUpdate(viewer, viewer.JSONExpressions.Draft));
	rtrn 0;
}

gpk::err_t fileViewerDraw(SFileViewer & viewer, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{22, 24, 28}));
	if_fail_fe(guiUpdateMetrics(*viewer.Dialog->GUI, target.metrics().u1(), false));
	rtrn guiDraw(*viewer.Dialog->GUI, target);
}
