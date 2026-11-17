#include "SIDE.h"

#include "gpk_file.h"
#include "gpk_framework_module.h"
#include "SIDE_viewport.h"
#include "gpk_label.h"
#include "gpk_path.h"

sttc gpk::err_t fileViewerViewportScrollGet(SFileViewer & viewer, cid_t idViewport, b8_t vertical, pobj<SDialogSlider> & scrollbar) {
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	u2_c iChild = vertical ? 1 : 2;
	if_true_fef(iChild >= gui.Controls.Children[idClient].size(), "Viewport %i has no %s scrollbar.", idViewport, vertical ? "vertical" : "horizontal");
	rtrn dialogControlGet(*viewer.Dialog, gui.Controls.Children[idClient][iChild], scrollbar);
}

sttc gpk::cid_t fileViewerDirectoryNewFileControl(cnst SGUI & gui, cid_t idViewport) {
	if_true_vef(CID_INVALID, controlInvalid(gui, idViewport), "Invalid directory viewport control: %i.", idViewport);
	if_true_vef(CID_INVALID, gui.Controls.Children[idViewport].size() <= VIEWPORT_CONTROL_COUNT, "Directory viewport %i has no New File control.", idViewport);
	rtrn gui.Controls.Children[idViewport][VIEWPORT_CONTROL_COUNT];
}

sttc gpk::err_t fileViewerViewportCreate(SFileViewer & viewer, cid_t & idViewport, vcst_t title, cnst rect2s1_t & area) {
	SGUI & gui = *viewer.Dialog->GUI;
	SControlTable		& controls = gui.Controls;
	if_fail_fe(idViewport = viewportCreate(gui, viewer.Dialog->Root));
	SControlPlacement	& root = controls.Placement[idViewport];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Draw[idViewport].UseNewPalettes = false;
	controls.Draw[idViewport].FrameOut = true;
	controls.Draw[idViewport].Palettes = viewer.Dialog->Colors->Viewport;
	controls.States[idViewport].Mask &= ~GUI_CONTROL_FLAG_Hovered;

	for(u2_t iControl = 0; iControl < VIEWPORT_CONTROL_TITLE; ++iControl) {
		cid_t idControl = -1;
		if_fail_fe(idControl = viewportControl(gui, idViewport, (VIEWPORT_CONTROL)iControl));
		controls.States[idControl].Mask = viewer.Dialog->DefaultControlMask;
		controls.Draw[idControl].UseNewPalettes = false;
		controls.Draw[idControl].Palettes = viewer.Dialog->Colors->Viewport;
	}
	cid_t idClose = -1;
	cid_t idClient = -1;
	cid_t idTitle = -1;
	if_fail_fe(idClose = viewportControl(gui, idViewport, VIEWPORT_CONTROL_CLOSE));
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	if_fail_fe(idTitle = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TITLE));
	controls.States[idClose].Mask = viewer.Dialog->DefaultControlMask;
	controls.Draw[idClose].UseNewPalettes = false;
	controls.Draw[idClose].Palettes = viewer.Dialog->Colors->ViewportTitle;
	controls.Placement[idClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	controls.Placement[idClient].Margin = {};
	controls.Placement[idClient].Border = {1, 1, 1, 1};
	controls.States[idClient].Mask = viewer.Dialog->DefaultControlMask;
	controls.States[idClient].Mask &= ~GUI_CONTROL_FLAG_Hovered;
	controls.States[idClient].Mask &= ~GUI_CONTROL_FLAG_Action;
	controls.Draw[idClient].Palettes = viewer.Dialog->Colors->Viewport;
	controls.Placement[idTitle].Border = {2, 2, 2, 2};
	controls.States[idTitle].Mask = viewer.Dialog->DefaultControlMask;
	controls.Draw[idTitle].UseNewPalettes = false;
	controls.Draw[idTitle].Palettes = viewer.Dialog->Colors->ViewportTitle;
	controls.Text[idTitle].Text = title;
	rtrn controlMetricsInvalidate(gui, idViewport);
}

sttc gpk::err_t fileViewerViewportAreaSet(SFileViewer & viewer, cid_t idViewport, cnst rect2s1_t & area) {
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	gui.Controls.Placement[idViewport].Area = area;
	gui.Controls.Placement[idClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	rtrn controlMetricsInvalidate(gui, idViewport);
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

sttc gpk::err_t fileViewerListCreate(SFileViewer & viewer, cid_t & idList, cid_t idViewport) {
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	if_fail_fe(idList = createScreenLayout(gui, idClient));
	rtrn 0;
}

sttc gpk::err_t fileViewerListRowPush(SFileViewer & viewer, cid_t idList, vcst_t text, b8_t interactive, b8_t buttonPalette = true, DIRECTION orientation = DIRECTION_DOWN) {
	SGUI & gui = *viewer.Dialog->GUI;
	cnst acid & idControls = gui.Controls.Children[idList];
	cnst cid_t idPrevious = idControls.size() ? idControls[idControls.size() - 1] : CID_INVALID;
	cnst n2u0_t fontCharSize = gui.Fonts[gui.SelectedFont]->CharSize;
	SControlPlacement placement = {};
	placement.Area.Size.y = fontCharSize.y + 6;
	placement.Border = {1, 1, 1, 1};
	placement.Margin = {4, 2, 4, 2};
	vcst_c rowText = label(text);
	cid_t idControl = -1;
	if_fail_fe(idControl = guiCreateControlList(gui, orientation, view<vcst_c>{&rowText, 1}, idList, placement, ALIGN_CENTER_LEFT, {}));
	if(CID_INVALID != idPrevious) {
		if(DIRECTION_DOWN == orientation)
			gui.Controls.Constraints[idControl].DockToControl.Bottom = idPrevious;
		else if(DIRECTION_RIGHT == orientation)
			gui.Controls.Constraints[idControl].DockToControl.Right = idPrevious;
	}
	gui.Controls.Draw[idControl].Palettes = buttonPalette ? viewer.Dialog->Colors->Button : viewer.Dialog->Colors->Control;
	if(false == interactive) {
		gui.Controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Hovered;
		gui.Controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Pressed;
		gui.Controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Dragging;
		gui.Controls.States[idControl].Mask &= ~GUI_CONTROL_FLAG_Action;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerScrollCreate(SFileViewer & viewer, cid_t idViewport, b8_t vertical) {
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(sliderCreate(*viewer.Dialog, scrollbar));
	scrollbar->Vertical = vertical;
	scrollbar->FuncValueFormat = [](vcst_t & format, s3_t, cnst minmax<s3_t> &) { format = ""; rtrn 0; };
	scrollbar->ValueLimits = {0, 1};
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	if_fail_fe(controlSetParent(gui, scrollbar->IdGUIControl, idClient));
	SControlPlacement & placement = gui.Controls.Placement[scrollbar->IdGUIControl];
	SControlConstraints & constraints = gui.Controls.Constraints[scrollbar->IdGUIControl];
	placement.Area.Offset = {};
	if(vertical) {
		placement.Area.Size.x = 12;
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.AttachSizeToControl = {-1, scrollbar->IdGUIControl};
	}
	else {
		placement.Area.Size.y = 12;
		placement.Align = ALIGN_BOTTOM_LEFT;
		constraints.AttachSizeToControl.x = scrollbar->IdGUIControl;
	}
	constraints.DockToControl = {-1, -1, -1, -1};
	gui.Controls.SetHidden(scrollbar->IdGUIControl, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerDirectoryControlsCreate(SFileViewer & viewer, SPathList & directory) {
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClose = -1;
	cid_t idTitle = -1;
	if_fail_fe(idClose = viewportControl(gui, directory.IdViewport, VIEWPORT_CONTROL_CLOSE));
	if_fail_fe(idTitle = viewportControl(gui, directory.IdViewport, VIEWPORT_CONTROL_TITLE));
	cid_t idNewFile = -1;
	if_fail_fe(idNewFile = controlCreate(gui, [&](SControlState &, STextState &, vcst_t &, SControlConstraints & constraints, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {88, gui.Controls.Placement[idClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.AttachSizeToControl = {-1, -1};
		constraints.DockToControl.Right = idClose;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "New File";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, idNewFile, directory.IdViewport));
	gui.Controls.Constraints[idTitle].DockToControl.Right = idNewFile;
	rtrn controlMetricsInvalidate(gui, directory.IdViewport);
}

sttc gpk::err_t fileViewerEditorCreate(SFileViewer & viewer, SFileEditor & editor, cnst rect2s1_t & area) {
	if_fail_fe(::fileViewerViewportCreate(viewer, editor.IdViewport, "SELECT A FILE", area));
	if_fail_fe(::fileViewerListCreate(viewer, editor.Lines, editor.IdViewport));
	if_fail_fe(::fileViewerScrollCreate(viewer, editor.IdViewport, true));
	if_fail_fe(::fileViewerScrollCreate(viewer, editor.IdViewport, false));
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClose = -1;
	cid_t idClient = -1;
	cid_t idTitle = -1;
	if_fail_fe(idClose = viewportControl(gui, editor.IdViewport, VIEWPORT_CONTROL_CLOSE));
	if_fail_fe(idClient = viewportControl(gui, editor.IdViewport, VIEWPORT_CONTROL_TARGET));
	if_fail_fe(idTitle = viewportControl(gui, editor.IdViewport, VIEWPORT_CONTROL_TITLE));
	if_fail_fe(editor.IdSave = controlCreate(gui, [&](SControlState &, STextState &, vcst_t &, SControlConstraints & constraints, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {64, gui.Controls.Placement[idClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.AttachSizeToControl = {-1, -1};
		constraints.DockToControl.Right = idClose;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Save";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, editor.IdSave, editor.IdViewport));
	gui.Controls.SetHidden(editor.IdSave, true);

	if_fail_fe(editor.IdKeyboard = controlCreate(gui, [&](SControlState &, STextState &, vcst_t &, SControlConstraints & constraints, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {88, gui.Controls.Placement[idClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.DockToControl.Right = editor.IdSave;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Keyboard";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, editor.IdKeyboard, editor.IdViewport));
	gui.Controls.Constraints[idTitle].DockToControl.Right = editor.IdKeyboard;
	gui.Controls.SetHidden(editor.IdKeyboard, true);

	if_fail_fe(editor.IdCursor = controlCreate(gui, [](SControlState & state, STextState &, vcst_t &, SControlConstraints &, SControlPlacement & placement, SControlArea &, SControlDraw & draw, SControlText & text, SControlImage &) {
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
	if_fail_fe(controlSetParent(gui, editor.IdCursor, idClient));
	gui.Controls.SetHidden(editor.IdCursor, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuCreate(SFileViewer & viewer) {
	SGUI & gui = *viewer.Dialog->GUI;
	SControlPlacement placement = {};
	placement.Area.Size.y = gui.Fonts[gui.SelectedFont]->CharSize.y + 6;
	placement.Border = {1, 1, 1, 1};
	placement.Margin = {1, 1, 1, 1};
	placement.Align = ALIGN_TOP_LEFT;
	vcst_c viewportText = "FILES";
	if_fail_fe(viewer.IdMenuFiles = guiCreateControlList(gui, DIRECTION_RIGHT, view<vcst_c>{&viewportText, 1}, viewer.Dialog->Root, placement, ALIGN_CENTER_LEFT, {}));
	if_fail_fe(viewer.Dialog->Buttons.push_back(viewer.IdMenuFiles));
	if_fail_fe(viewer.EditorTabs = createScreenLayout(gui, viewer.Dialog->Root));
	gui.Controls.Placement[viewer.EditorTabs].Align = ALIGN_TOP_LEFT;
	gui.Controls.Placement[viewer.EditorTabs].Area.Size.y = placement.Area.Size.y;
	gui.Controls.Constraints[viewer.EditorTabs].AttachSizeToControl.y = CID_INVALID;
	gui.Controls.Constraints[viewer.EditorTabs].DockToControl.Right = viewer.IdMenuFiles;

	placement.Align = ALIGN_TOP_RIGHT;
	vcst_c actionText = "EXIT";
	if_fail_fe(viewer.IdMenuExit = guiCreateControlList(gui, DIRECTION_RIGHT, view<vcst_c>{&actionText, 1}, viewer.Dialog->Root, placement, ALIGN_CENTER_LEFT, {}));
	if_fail_fe(viewer.Dialog->Buttons.push_back(viewer.IdMenuExit));
	rtrn 0;
}

sttc b8_t fileViewerFolderHasParent(cnst SPathTree & paths) {
	vcst_t parent = {};
	rtrn 0 <= pathDirectory(paths.Root, parent) && parent.size() && parent != paths.Root;
}

sttc gpk::err_t fileViewerFolderLoad(SFileViewer & viewer, SPathList & directory, vcst_t path) {
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(::fileViewerViewportScrollGet(viewer, directory.IdViewport, true, scrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idTitle = -1;
	if_fail_fe(idTitle = viewportControl(gui, directory.IdViewport, VIEWPORT_CONTROL_TITLE));
	cnst acid & idFiles = gui.Controls.Children[directory.Files];
	for(u2_t iControl = 0; iControl < idFiles.size(); ++iControl)
		if_fail_fe(controlTextSet(gui, idFiles[iControl], {}));
	gui.Controls.Text[idTitle].Text = {};
	string absolutePath = {};
	if_fail_fe(pathAbsolute(path, absolutePath));
	if_fail_fe(pathList(directory.PathContents, absolutePath, [](b8_t folder, vcst_c &) { rtrn folder ? 2 : 0; }));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	gui.Controls.Text[idTitle].Text = directory.PathContents.Root;
	for(u2_t iControl = 0; iControl < idFiles.size(); ++iControl)
		gui.Controls.SetSelected(idFiles[iControl], false);
	rtrn 0;
}

sttc gpk::err_t fileViewerListWindowUpdate(SFileViewer & viewer, cid_t idList, cnst SPathTree & items, cid_t idViewport) {
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(::fileViewerViewportScrollGet(viewer, idViewport, true, scrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	cnst acid & idControls = gui.Controls.Children[idList];
	b8_c hasParent = ::fileViewerFolderHasParent(items);
	u2_c itemCount = items.Name.size() + one_if(hasParent);
	if(itemCount && 0 == idControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, idList, {}, true));
	cnst s1_t rowHeight = gui.Controls.Children[idList].size() ? max((s1_t)1, gui.Controls.Placement[gui.Controls.Children[idList][0]].Area.Size.y) : 1;
	cnst s3_t visibleCount = max((s3_t)1, (s3_t)gui.Controls.Placement[idClient].Area.Size.y / rowHeight);
	while(gui.Controls.Children[idList].size() < min(itemCount, (u2_t)visibleCount))
		if_fail_fe(::fileViewerListRowPush(viewer, idList, {}, true));
	cnst s3_t maxOffset = max((s3_t)0, (s3_t)itemCount - visibleCount);
	scrollbar->ValueLimits = {0, max((s3_t)1, maxOffset)};
	if_fail_fe(sliderSetValue(*scrollbar, maxOffset ? scrollbar->ValueCurrent : 0));
	cnst b8_t scrollY = 0 != maxOffset;
	if(gui.Controls.IsHidden(scrollbar->IdGUIControl) == scrollY) {
		gui.Controls.SetHidden(scrollbar->IdGUIControl, false == scrollY);
		if_fail_fe(controlMetricsInvalidate(gui, scrollbar->IdGUIControl));
	}
	cnst s3_t firstVisible = maxOffset ? scrollbar->ValueCurrent : 0;
	for(u2_t iControl = 0; iControl < gui.Controls.Children[idList].size(); ++iControl) {
		cnst cid_t idControl = gui.Controls.Children[idList][iControl];
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
	SGUI & gui = *viewer.Dialog->GUI;
	cnst acid & idLines = gui.Controls.Children[editor.Lines];
	for(u2_t iControl = 0; iControl < idLines.size(); ++iControl)
		if_fail_fe(controlTextSet(gui, idLines[iControl], {}));
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
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, true, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, false, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	cid_t idTitle = -1;
	if_fail_fe(idTitle = viewportControl(gui, editor.IdViewport, VIEWPORT_CONTROL_TITLE));
	gui.Controls.Text[idTitle].Text = label(path);
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
	pobj<SDialogSlider> scrollbarY = {};
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsHidden(editor.IdViewport))
		rtrn 0;
	if(gui.Controls.IsAction(editor.IdKeyboard)) {
		viewer.IdEditorCurrent = iEditor;
		gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false == gui.Controls.IsHidden(viewer.VirtualKeyboard.IdRoot));
	}
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, true, scrollbarY));
	u2_c firstVisible = (u2_t)scrollbarY->ValueCurrent;
	cnst acid & idLines = gui.Controls.Children[editor.Lines];
	for(u2_t iControl = 0; iControl < idLines.size(); ++iControl)
		if(gui.Controls.IsAction(idLines[iControl])) {
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
	pobj<SDialogSlider> scrollbarY = {};
	pobj<SDialogSlider> scrollbarX = {};
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, true, scrollbarY));
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, false, scrollbarX));
	SGUI & gui = *viewer.Dialog->GUI;
	u2_c firstVisible = (u2_t)scrollbarY->ValueCurrent;
	u2_c firstColumn = (u2_t)scrollbarX->ValueCurrent;
	u0_c characterWidth = max((u0_t)1, gui.Fonts[gui.SelectedFont]->CharSize.x);
	cnst acid & idLines = gui.Controls.Children[editor.Lines];
	for(u2_t iControl = 0; iControl < idLines.size(); ++iControl) {
		cnst cid_t idControl = idLines[iControl];
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
	pobj<SDialogSlider> scrollbarY = {};
	pobj<SDialogSlider> scrollbarX = {};
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, true, scrollbarY));
	if_fail_fe(::fileViewerViewportScrollGet(viewer, editor.IdViewport, false, scrollbarX));
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idClient = -1;
	if_fail_fe(idClient = viewportControl(gui, editor.IdViewport, VIEWPORT_CONTROL_TARGET));
	if(0 == gui.Controls.Children[editor.Lines].size())
		if_fail_fe(::fileViewerListRowPush(viewer, editor.Lines, {}, true, false));
	cnst s1_t rowHeight = max((s1_t)1, gui.Controls.Placement[gui.Controls.Children[editor.Lines][0]].Area.Size.y);
	n2s1_c clientSize = gui.Controls.Area[idClient].Client.Global.Size;
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
	while(gui.Controls.Children[editor.Lines].size() < (u2_t)visibleCount) {
		if_fail_fe(::fileViewerListRowPush(viewer, editor.Lines, {}, true, false));
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
	cnst b8_t showScrollY = 0 != maxOffsetY;
	cnst b8_t showScrollX = 0 != maxOffsetX;
	b8_t scrollMetricsChanged = false;
	if(gui.Controls.IsHidden(scrollbarY->IdGUIControl) == showScrollY) {
		gui.Controls.SetHidden(scrollbarY->IdGUIControl, false == showScrollY);
		scrollMetricsChanged = true;
	}
	if(gui.Controls.IsHidden(scrollbarX->IdGUIControl) == showScrollX) {
		gui.Controls.SetHidden(scrollbarX->IdGUIControl, false == showScrollX);
		scrollMetricsChanged = true;
	}
	cid_t & idDockRight = gui.Controls.Constraints[scrollbarX->IdGUIControl].DockToControl.Right;
	cnst cid_t idDockRightNew = showScrollY ? scrollbarY->IdGUIControl : (cid_t)-1;
	if(idDockRight != idDockRightNew) {
		idDockRight = idDockRightNew;
		scrollMetricsChanged = true;
	}
	if(scrollMetricsChanged) {
		if_fail_fe(controlMetricsInvalidate(gui, scrollbarX->IdGUIControl));
		if_fail_fe(controlMetricsInvalidate(gui, scrollbarY->IdGUIControl));
	}
	u2_c firstVisible = (u2_t)(maxOffsetY ? scrollbarY->ValueCurrent : 0);
	u2_c firstColumn = (u2_t)(maxOffsetX ? scrollbarX->ValueCurrent : 0);
	cnst acid & idLines = gui.Controls.Children[editor.Lines];
	for(u2_t iControl = 0; iControl < idLines.size(); ++iControl) {
		cnst cid_t idControl = idLines[iControl];
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
			STextState & textState = gui.Controls.TextState[idControl];
			SControlText & controlText = gui.Controls.Text[idControl];
			controlText.Selection = {};
			u2_c lineOffset = ::fileViewerLineOffset(editor, iLine);
			b8_t selected = false;
			u2_c visibleBegin = lineOffset + offset;
			u2_c visibleEnd = visibleBegin + visibleText.size();
			for(u2_t iSelection = 0; iSelection < editor.TextSelections.size(); ++iSelection) {
				rangeu2_c selection = editor.TextSelections[iSelection];
				u2_c selectionBegin = max(visibleBegin, selection.Offset);
				u2_c selectionEnd = min(visibleEnd, selection.Offset + selection.Count);
				if(selectionBegin < selectionEnd) {
					controlText.Selection = {(u1_t)(selectionBegin - visibleBegin), (u1_t)(selectionEnd - visibleBegin)};
					selected = true;
					break;
				}
			}
			textState.SetSelected(idControl, controlText.Selection.Min, gui.Controls.EventQueue, selected);
			textState.SetCursor(idControl, controlText.Selection.Min, gui.Controls.EventQueue, false);
		}
		else {
			gui.Controls.Text[idControl].Selection = {};
			gui.Controls.TextState[idControl].SetSelected(idControl, 0, gui.Controls.EventQueue, false);
			gui.Controls.TextState[idControl].SetCursor(idControl, 0, gui.Controls.EventQueue, false);
		}
		gui.Controls.SetSelected(idControl, visible && iLine == editor.Cursor.y);
		if(gui.Controls.IsHidden(idControl) == visible)
			gui.Controls.SetHidden(idControl, false == visible);
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
	rtrn 0;
}

sttc gpk::err_t fileViewerViewportLayoutConfigure(SFileViewer & viewer, cid_t idViewport, FILE_VIEWER_LAYOUT layout) {
	if(FILE_VIEWER_LAYOUT_FLOATING == layout)
		rtrn 0;
	SGUI & gui = *viewer.Dialog->GUI;
	cid_t idTitle = -1;
	if_fail_fe(idTitle = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TITLE));
	gui.Controls.States[idTitle].Mask &= ~GUI_CONTROL_FLAG_Dragging;
	for(u2_t iResize = 0; iResize < VIEWPORT_CONTROL_TITLE; ++iResize) {
		cid_t idResize = -1;
		if_fail_fe(idResize = viewportControl(gui, idViewport, (VIEWPORT_CONTROL)iResize));
		gui.Controls.States[idResize].Mask &= ~GUI_CONTROL_FLAG_Dragging;
		gui.Controls.SetHidden(idResize, true);
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerDirectoryCreate(SFileViewer & viewer, vcst_t path, FILE_VIEWER_LAYOUT layout) {
	pobj<SPathList> directory = {};
	if_null_fe(directory.create());
	s1_c cascade = (s1_t)(24 * (viewer.Directories.size() % 8));
	if_fail_fe(::fileViewerViewportCreate(viewer, directory->IdViewport, "FILES / CURRENT DIRECTORY", {{(s1_t)(40 + cascade), (s1_t)(48 + cascade)}, {420, 600}}));
	if_fail_fe(::fileViewerDirectoryControlsCreate(viewer, *directory));
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
	SGUI & gui = *viewer.Dialog->GUI;
	if_fail_fe(::fileViewerListRowPush(viewer, viewer.EditorTabs, fileName, true, true, DIRECTION_RIGHT));
	cnst acid & idTabs = gui.Controls.Children[viewer.EditorTabs];
	cnst cid_t idTab = idTabs[idTabs.size() - 1];
	gui.Controls.SetSelected(idTab, true);
	rtrn 0;
}

sttc gpk::err_t fileViewerFileCreate(SFileViewer & viewer, SPathList & directory, FILE_VIEWER_LAYOUT layout) {
	string filePath = {};
	sc_t fileName[64] = {};
	for(u2_t iFile = 0; iFile < 0xFFFFU; ++iFile) {
		cnst err_t fileNameLength = iFile
			? ::gpk::sprintf_s(fileName, "new_file_%u.txt", iFile)
			: ::gpk::sprintf_s(fileName, "new_file.txt")
			;
		if_fail_fe(fileNameLength);
		filePath.clear();
		if_fail_fe(pathNameCompose(directory.PathContents.Root, vcst_t{fileName, (u2_t)fileNameLength}, filePath));
		s3_t modified = 0;
		if(0 == fileLastWriteTime(filePath, modified))
			continue;
		if_fail_fe(fileFromMemory(filePath, vcu0_t{}));
		if_fail_fe(::fileViewerFolderLoad(viewer, directory, directory.PathContents.Root));
		rtrn ::fileViewerEditorOpen(viewer, filePath, layout);
	}
	if_true_fef(true, "No available New File name in '%.*s'.", GPK_VIEW_ARGS(directory.PathContents.Root));
}

sttc gpk::err_t fileViewerMenuUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsAction(viewer.IdMenuFiles))
		if_fail_fe(::fileViewerDirectoryCreate(viewer, viewer.RootPath, layout));
	if(gui.Controls.IsAction(viewer.IdMenuExit))
		rtrn APPLICATION_STATE_EXIT;
	cnst acid & idTabs = gui.Controls.Children[viewer.EditorTabs];
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size() && iEditor < idTabs.size(); ++iEditor) {
		SFileEditor & editor = *viewer.Editors[iEditor];
		cnst cid_t idTab = idTabs[iEditor];
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
		cid_t idNewFile = -1;
		if_fail_fe(idNewFile = ::fileViewerDirectoryNewFileControl(gui, directory.IdViewport));
		if(gui.Controls.IsAction(idNewFile))
			rtrn ::fileViewerFileCreate(viewer, directory, layout);
		pobj<SDialogSlider> filesScrollbar = {};
		if_fail_fe(::fileViewerViewportScrollGet(viewer, directory.IdViewport, true, filesScrollbar));
		u2_c firstVisible = (u2_t)filesScrollbar->ValueCurrent;
		b8_c hasParent = ::fileViewerFolderHasParent(directory.PathContents);
		cnst acid & idFiles = gui.Controls.Children[directory.Files];
		for(u2_t iFile = 0; iFile < idFiles.size(); ++iFile) {
			cnst cid_t idControl = idFiles[iFile];
			if(false == gui.Controls.IsAction(idControl))
				continue;
			u2_c iItem = firstVisible + iFile;
			for(u2_t iSelection = 0; iSelection < idFiles.size(); ++iSelection)
				gui.Controls.SetSelected(idFiles[iSelection], iSelection == iFile);
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
	gui.ThemeDefault = ASCII_COLOR_CYAN * 16 + 8;
	viewer.RootPath = rootPath.size() ? rootPath : vcst_t{"."};
	if_fail_fe(::fileViewerMenuCreate(viewer));
	if_fail_fe(virtualKeyboardSetup437(gui, viewer.VirtualKeyboard));
	if_fail_fe(controlSetParent(gui, viewer.VirtualKeyboard.IdRoot, viewer.Dialog->Root));
	gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
	if_fail_fe(::fileViewerDirectoryCreate(viewer, viewer.RootPath, layout));
	rtrn ::fileViewerLayoutSet(viewer, layout);
}

gpk::err_t fileViewerUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents) {
	if(gpk::err_t result = ::fileViewerMenuUpdate(viewer, layout))
		rtrn result;
	if_fail_fe(::fileViewerSelectionUpdate(viewer, layout));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerEditorUpdate(viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
	if_fail_fe(::fileViewerEditorInputUpdate(viewer, frameEvents));
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory)
		if_fail_fe(viewportUpdate(*viewer.Dialog->GUI, *viewer.Dialog->Input, viewer.Directories[iDirectory]->IdViewport));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(viewportUpdate(*viewer.Dialog->GUI, *viewer.Dialog->Input, viewer.Editors[iEditor]->IdViewport));
	if_fail_fe(viewer.Dialog->Update());
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerEditorSelectionUpdate(viewer, *viewer.Editors[iEditor], (cid_t)iEditor));
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	for(u2_t iDirectory = 0; iDirectory < viewer.Directories.size(); ++iDirectory)
		if_fail_fe(::fileViewerListWindowUpdate(viewer, viewer.Directories[iDirectory]->Files, viewer.Directories[iDirectory]->PathContents, viewer.Directories[iDirectory]->IdViewport));
	for(u2_t iEditor = 0; iEditor < viewer.Editors.size(); ++iEditor)
		if_fail_fe(::fileViewerTextWindowUpdate(viewer, *viewer.Editors[iEditor]));
	rtrn 0;
}

gpk::err_t fileViewerDraw(SFileViewer & viewer, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{22, 24, 28}));
	if_fail_fe(guiUpdateMetrics(*viewer.Dialog->GUI, target.metrics().u1(), false));
	rtrn guiDraw(*viewer.Dialog->GUI, target);
}
