#include "test_gpk_file_viewer_common.h"

#include "gpk_file.h"
#include "gpk_framework_module.h"
#include "gpk_label.h"
#include "gpk_path.h"

stxp u0_t FILE_VIEWPORT_FILES = 0;
stxp u0_t FILE_VIEWPORT_TEXT  = 1;
stxp u0_t FILE_VIEWER_BUTTON_KEYBOARD = 0;
stxp u0_t FILE_VIEWER_BUTTON_FILES    = 1;
stxp u0_t FILE_VIEWER_BUTTON_EXIT     = 2;

sttc gpk::err_t fileViewerViewportCreate(SFileViewer & viewer, u0_t index, vcst_t title, cnst rect2s1_t & area) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(viewportCreate(*viewer.Dialog, viewport));
	viewer.Viewports[index] = viewport->IdGUIControl;
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

sttc gpk::err_t fileViewerViewportAreaSet(SFileViewer & viewer, u0_t index, cnst rect2s1_t & area) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[index], viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.Placement[viewport->IdGUIControl].Area = area;
	gui.Controls.Placement[viewport->IdClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	rtrn controlMetricsInvalidate(gui, viewport->IdGUIControl);
}

sttc gpk::err_t fileViewerLayoutSet(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	if(FILE_VIEWER_LAYOUT_FLOATING == layout) {
		if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_FILES, {{ 40, 48}, {420, 600}}));
		if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_TEXT , {{480, 72}, {760, 560}}));
		rtrn 0;
	}
	SGUI & gui = *viewer.Dialog->GUI;
	n2s1_c windowSize = gui.LastSize.s1();
	cnst s1_t margin = 12;
	cnst s1_t gap = 8;
	cnst s1_t top = max(margin, gui.Controls.Placement[(cid_t)viewer.Dialog->Buttons[FILE_VIEWER_BUTTON_FILES]].Area.Size.y);
	cnst s1_t leftWidth = min((s1_t)360, (s1_t)((windowSize.x - margin * 2 - gap) / 3));
	cnst s1_t height = (s1_t)(windowSize.y - top - margin);
	if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_FILES, {{margin, top}, {leftWidth, height}}));
	if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_TEXT , {{(s1_t)(margin + leftWidth + gap), top}, {(s1_t)(windowSize.x - margin * 2 - leftWidth - gap), height}}));
	rtrn 0;
}

sttc gpk::err_t fileViewerListCreate(SFileViewer & viewer, SControlList & list, u0_t viewportIndex) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
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

sttc gpk::err_t fileViewerScrollCreate(SFileViewer & viewer, u0_t viewportIndex, b8_t vertical) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
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

sttc gpk::err_t fileViewerEditorCreate(SFileViewer & viewer) {
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	if_fail_fe(viewer.Editor.IdSave = controlCreate(gui, [&](SControlState &, SControlState &, SControlPlacement & placement, SControlArea &, SControlConstraints & constraints, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {64, gui.Controls.Placement[viewport->IdClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.AttachSizeToControl = {-1, -1};
		constraints.DockToControl.Right = viewport->IdClose;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Save";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, viewer.Editor.IdSave, viewport->IdGUIControl));
	gui.Controls.SetHidden(viewer.Editor.IdSave, true);

	cid_t idKeyboard = {};
	if_fail_fe(idKeyboard = controlCreate(gui, [&](SControlState &, SControlState &, SControlPlacement & placement, SControlArea &, SControlConstraints & constraints, SControlDraw & draw, SControlText & text, SControlImage &) {
		placement.Area.Size = {88, gui.Controls.Placement[viewport->IdClose].Area.Size.y};
		placement.Align = ALIGN_TOP_RIGHT;
		constraints.DockToControl.Right = viewer.Editor.IdSave;
		draw.Palettes = viewer.Dialog->Colors->Button;
		text.Text = "Keyboard";
		text.Align = ALIGN_CENTER;
		rtrn 0;
	}));
	if_fail_fe(controlSetParent(gui, idKeyboard, viewport->IdGUIControl));
	if_fail_fe(viewer.Dialog->Buttons.push_back(idKeyboard));
	gui.Controls.Constraints[viewport->IdTitle].DockToControl.Right = idKeyboard;
	gui.Controls.SetHidden(idKeyboard, true);

	if_fail_fe(viewer.Editor.IdCursor = controlCreate(gui, [](SControlState & state, SControlState &, SControlPlacement & placement, SControlArea &, SControlConstraints &, SControlDraw & draw, SControlText & text, SControlImage &) {
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
	if_fail_fe(controlSetParent(gui, viewer.Editor.IdCursor, viewport->IdClient));
	gui.Controls.SetHidden(viewer.Editor.IdCursor, true);
	if_fail_fe(virtualKeyboardSetup437(gui, viewer.VirtualKeyboard));
	if_fail_fe(controlSetParent(gui, viewer.VirtualKeyboard.IdRoot, viewer.Dialog->Root));
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuCreate(SFileViewer & viewer) {
	SGUI & gui = *viewer.Dialog->GUI;
	SControlList menuViewports = {};
	sttc cnst vcst_t viewportText[] = {"FILES"};
	if_fail_fe(controlListCreate(gui, menuViewports, view<vcst_c>{viewportText}, CID_INVALID, DIRECTION_RIGHT));
	gui.Controls.Placement[menuViewports.IdControl].Area.Offset = {};
	gui.Controls.Placement[menuViewports.IdControl].Align = ALIGN_TOP_LEFT;
	if_fail_fe(viewer.Dialog->Buttons.push_back(menuViewports.IdControls[0]));

	SControlList menuActions = {};
	sttc cnst vcst_t actionText[] = {"EXIT"};
	if_fail_fe(controlListCreate(gui, menuActions, view<vcst_c>{actionText}, CID_INVALID, DIRECTION_RIGHT));
	gui.Controls.Placement[menuActions.IdControl].Area.Offset = {};
	gui.Controls.Placement[menuActions.IdControl].Align = ALIGN_TOP_RIGHT;
	if_fail_fe(viewer.Dialog->Buttons.push_back(menuActions.IdControls[0]));
	rtrn 0;
}

sttc gpk::err_t fileViewerMenuUpdate(SFileViewer & viewer) {
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsAction((cid_t)viewer.Dialog->Buttons[FILE_VIEWER_BUTTON_FILES]))
		gui.Controls.SetHidden(viewer.Viewports[FILE_VIEWPORT_FILES], false);
	if(gui.Controls.IsAction((cid_t)viewer.Dialog->Buttons[FILE_VIEWER_BUTTON_EXIT]))
		rtrn APPLICATION_STATE_EXIT;
	rtrn 0;
}

sttc b8_t fileViewerFolderHasParent(cnst SPathTree & paths) {
	vcst_t parent = {};
	rtrn 0 <= pathDirectory(paths.Root, parent) && parent.size() && parent != paths.Root;
}

sttc gpk::err_t fileViewerFolderLoad(SFileViewer & viewer, vcst_t path) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_FILES], viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	for(u2_t iControl = 0; iControl < viewer.Directory.Files.IdControls.size(); ++iControl)
		if_fail_fe(controlTextSet(gui, viewer.Directory.Files.IdControls[iControl], {}));
	gui.Controls.Text[viewport->IdTitle].Text = {};
	string absolutePath = {};
	if_fail_fe(pathAbsolute(path, absolutePath));
	if_fail_fe(pathList(viewer.Directory.PathContents, absolutePath, [](b8_t folder, vcst_c &) { rtrn folder ? 2 : 0; }));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	gui.Controls.Text[viewport->IdTitle].Text = viewer.Directory.PathContents.Root;
	for(u2_t iControl = 0; iControl < viewer.Directory.Files.IdControls.size(); ++iControl)
		gui.Controls.SetSelected(viewer.Directory.Files.IdControls[iControl], false);
	rtrn 0;
}

sttc gpk::err_t fileViewerListWindowUpdate(SFileViewer & viewer, SControlList & list, cnst SPathTree & items, u0_t viewportIndex) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
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

sttc gpk::err_t fileViewerTextViewsClear(SFileViewer & viewer) {
	for(u2_t iControl = 0; iControl < viewer.Editor.Lines.IdControls.size(); ++iControl)
		if_fail_fe(controlTextSet(*viewer.Dialog->GUI, viewer.Editor.Lines.IdControls[iControl], {}));
	viewer.Editor.FileLines.clear();
	viewer.Editor.FileLineWidthMax = 0;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextViewsRebuild(SFileViewer & viewer) {
	viewer.Editor.FileLines.clear();
	viewer.Editor.FileLineWidthMax = 0;
	vcst_t remaining = viewer.Editor.FileText;
	do {
		vcst_t line = {};
		vcst_t next = {};
		cnst gpk::err_t splitResult = split('\n', remaining, line, next);
		if(line.size() && '\r' == line[line.size() - 1])
			if_fail_fe(line.slice(line, 0, line.size() - 1));
		if_fail_fe(viewer.Editor.FileLines.push_back(line));
		viewer.Editor.FileLineWidthMax = max(viewer.Editor.FileLineWidthMax, line.size());
		if(0 > splitResult)
			break;
		remaining = next;
	} while(true);
	rtrn 0;
}

sttc gpk::err_t fileViewerTextLoad(SFileViewer & viewer, vcst_t path) {
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	viewer.Editor.FileText.clear();
	if_fail_fef(fileToMemory(path, viewer.Editor.FileText), "Failed to load '%.*s'.", GPK_VIEW_ARGS(path));
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	viewer.Editor.Cursor = {0, (u2_t)-1};
	SGUI & gui = *viewer.Dialog->GUI;
	gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
	gui.Controls.SetHidden(viewer.Editor.IdSave, false);
	gui.Controls.SetHidden((cid_t)viewer.Dialog->Buttons[FILE_VIEWER_BUTTON_KEYBOARD], false);
	pobj<SDialogViewport> viewport = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	pobj<SDialogSlider> scrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbar));
	if_fail_fe(sliderSetValue(*scrollbar, 0));
	gui.Controls.Text[viewport->IdTitle].Text = label(path);
	rtrn 0;
}

sttc u2_t fileViewerLineOffset(cnst SFileViewer & viewer, u2_t line) {
	u2_t offset = 0;
	u2_t iLine = 0;
	while(offset < viewer.Editor.FileText.size() && iLine < line)
		if('\n' == viewer.Editor.FileText[offset++])
			++iLine;
	rtrn offset;
}

sttc u2_t fileViewerCursorOffset(cnst SFileViewer & viewer) {
	if(viewer.Editor.Cursor.y >= viewer.Editor.FileLines.size())
		rtrn viewer.Editor.FileText.size();
	cnst vcst_t line = viewer.Editor.FileLines[viewer.Editor.Cursor.y];
	rtrn ::fileViewerLineOffset(viewer, viewer.Editor.Cursor.y) + min(viewer.Editor.Cursor.x, line.size());
}

sttc gpk::err_t fileViewerTextInsert(SFileViewer & viewer, sc_t character) {
	u2_c offset = ::fileViewerCursorOffset(viewer);
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	if_fail_fe(viewer.Editor.FileText.insert(offset, character));
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	if('\n' == character) {
		++viewer.Editor.Cursor.y;
		viewer.Editor.Cursor.x = 0;
	}
	else
		++viewer.Editor.Cursor.x;
	rtrn 0;
}

sttc gpk::err_t fileViewerTextBackspace(SFileViewer & viewer) {
	if(viewer.Editor.Cursor.y >= viewer.Editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(viewer);
	if(viewer.Editor.Cursor.x) {
		if_fail_fe(::fileViewerTextViewsClear(viewer));
		if_fail_fe(viewer.Editor.FileText.remove(cursorOffset - 1));
		--viewer.Editor.Cursor.x;
	}
	else if(viewer.Editor.Cursor.y) {
		cnst vcst_t previousLine = viewer.Editor.FileLines[viewer.Editor.Cursor.y - 1];
		u2_c separatorOffset = ::fileViewerLineOffset(viewer, viewer.Editor.Cursor.y - 1) + previousLine.size();
		u2_c separatorCount = cursorOffset - separatorOffset;
		viewer.Editor.Cursor.x = previousLine.size();
		--viewer.Editor.Cursor.y;
		if_fail_fe(::fileViewerTextViewsClear(viewer));
		for(u2_t iCharacter = 0; iCharacter < separatorCount; ++iCharacter)
			if_fail_fe(viewer.Editor.FileText.remove(separatorOffset));
	}
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	rtrn 0;
}

sttc gpk::err_t fileViewerTextDelete(SFileViewer & viewer) {
	if(viewer.Editor.Cursor.y >= viewer.Editor.FileLines.size())
		rtrn 0;
	u2_c cursorOffset = ::fileViewerCursorOffset(viewer);
	if(cursorOffset >= viewer.Editor.FileText.size())
		rtrn 0;
	u2_t count = 1;
	if('\r' == viewer.Editor.FileText[cursorOffset] && cursorOffset + 1 < viewer.Editor.FileText.size() && '\n' == viewer.Editor.FileText[cursorOffset + 1])
		count = 2;
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	for(u2_t iCharacter = 0; iCharacter < count; ++iCharacter)
		if_fail_fe(viewer.Editor.FileText.remove(cursorOffset));
	rtrn ::fileViewerTextViewsRebuild(viewer);
}

sttc gpk::err_t fileViewerEditorHandle(SFileViewer & viewer, cnst SVirtualKeyboardEvent & eventToProcess) {
	if(viewer.Editor.Cursor.y >= viewer.Editor.FileLines.size())
		rtrn 0;
	if(VK_EVENT_Release == eventToProcess.Type)
		rtrn ::fileViewerTextInsert(viewer, (sc_t)eventToProcess.ScanCode);
	if(VK_EVENT_Edit != eventToProcess.Type)
		rtrn 0;
	switch(eventToProcess.ScanCode) {
	default: break;
	case VK_SCANCODE_Escape:
		viewer.Dialog->GUI->Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		viewer.Editor.Cursor = {0, (u2_t)-1};
		break;
	case VK_SCANCODE_Delete		: if_fail_fe(::fileViewerTextDelete(viewer));		break;
	case VK_SCANCODE_Enter		: if_fail_fe(::fileViewerTextInsert(viewer, '\n'));	break;
	case VK_SCANCODE_Backspace	: if_fail_fe(::fileViewerTextBackspace(viewer));	break;
	case VK_SCANCODE_Clear		: if_fail_fe(::fileViewerTextViewsClear(viewer));
		viewer.Editor.FileText.clear();
		viewer.Editor.Cursor = {};
		if_fail_fe(::fileViewerTextViewsRebuild(viewer));
		break;
	case VK_SCANCODE_Left:
		if(viewer.Editor.Cursor.x)
			--viewer.Editor.Cursor.x;
		else if(viewer.Editor.Cursor.y) {
			--viewer.Editor.Cursor.y;
			viewer.Editor.Cursor.x = viewer.Editor.FileLines[viewer.Editor.Cursor.y].size();
		}
		break;
	case VK_SCANCODE_Right:
		if(viewer.Editor.Cursor.x < viewer.Editor.FileLines[viewer.Editor.Cursor.y].size())
			++viewer.Editor.Cursor.x;
		else if(viewer.Editor.Cursor.y + 1 < viewer.Editor.FileLines.size()) {
			++viewer.Editor.Cursor.y;
			viewer.Editor.Cursor.x = 0;
		}
		break;
	case VK_SCANCODE_Up:
		if(viewer.Editor.Cursor.y) {
			--viewer.Editor.Cursor.y;
			viewer.Editor.Cursor.x = min(viewer.Editor.Cursor.x, viewer.Editor.FileLines[viewer.Editor.Cursor.y].size());
		}
		break;
	case VK_SCANCODE_Down:
		if(viewer.Editor.Cursor.y + 1 < viewer.Editor.FileLines.size()) {
			++viewer.Editor.Cursor.y;
			viewer.Editor.Cursor.x = min(viewer.Editor.Cursor.x, viewer.Editor.FileLines[viewer.Editor.Cursor.y].size());
		}
		break;
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorPhysicalInput(SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	if(viewer.Dialog->GUI->Controls.IsHidden(viewer.VirtualKeyboard.IdRoot))
		rtrn 0;
	for(u2_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
		cnst pobj<SEventSystem> & systemEvent = frameEvents[iEvent];
		if(systemEvent->Type == SYSTEM_EVENT_Text) {
			if_fail_fe(eventExtractAndHandle<EVENT_TEXT>(*systemEvent, [&viewer](cnst SEventView<EVENT_TEXT> & eventText) {
				if(eventText.Type == EVENT_TEXT_Char && eventText.Data.size() && eventText.Data[0] >= 0x20 && eventText.Data[0] <= 0x7F)
					if_fail_fe(::fileViewerTextInsert(viewer, (sc_t)eventText.Data[0]));
				rtrn 0;
			}));
		}
		else if(systemEvent->Type == SYSTEM_EVENT_Keyboard) {
			if_fail_fe(eventExtractAndHandle<EVENT_KEYBOARD>(*systemEvent, [&viewer](cnst SEventView<EVENT_KEYBOARD> & eventKeyboard) {
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
				rtrn ::fileViewerEditorHandle(viewer, editorEvent);
			}));
		}
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerEditorUpdate(SFileViewer & viewer, vpobj<SEventSystem> frameEvents) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbarY = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	SGUI & gui = *viewer.Dialog->GUI;
	if(gui.Controls.IsHidden(viewport->IdGUIControl)) {
		gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		rtrn 0;
	}
	cnst cid_t idKeyboard = (cid_t)viewer.Dialog->Buttons[FILE_VIEWER_BUTTON_KEYBOARD];
	if(gui.Controls.IsAction(idKeyboard))
		gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false == gui.Controls.IsHidden(viewer.VirtualKeyboard.IdRoot));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	u2_c firstVisible = (u2_t)scrollbarY->ValueCurrent;
	for(u2_t iControl = 0; iControl < viewer.Editor.Lines.IdControls.size(); ++iControl)
		if(gui.Controls.IsAction(viewer.Editor.Lines.IdControls[iControl])) {
			u2_c iLine = firstVisible + iControl;
			if(iLine < viewer.Editor.FileLines.size()) {
				viewer.Editor.Cursor = {viewer.Editor.FileLines[iLine].size(), iLine};
				gui.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false);
			}
		}
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
		if_fail_fe(::fileViewerEditorHandle(viewer, vkEvents[iEvent]));
	if_fail_fe(::fileViewerEditorPhysicalInput(viewer, frameEvents));
	cnst vcst_t filePath = gui.Controls.Text[viewport->IdTitle].Text;
	if(gui.Controls.IsAction(viewer.Editor.IdSave) && filePath.size())
		if_fail_fef(fileFromMemory(filePath, viewer.Editor.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(filePath));
	rtrn 0;
}

sttc gpk::err_t fileViewerTextWindowUpdate(SFileViewer & viewer) {
	pobj<SDialogViewport> viewport = {};
	pobj<SDialogSlider> scrollbarY = {};
	pobj<SDialogSlider> scrollbarX = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbarX));
	SGUI & gui = *viewer.Dialog->GUI;
	if(0 == viewer.Editor.Lines.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Editor.Lines, {}, true, false));
	cnst s1_t rowHeight = max((s1_t)1, gui.Controls.Placement[viewer.Editor.Lines.IdControls[0]].Area.Size.y);
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
		scrollY = viewer.Editor.FileLines.size() > (u2_t)visibleCount;
		scrollX = viewer.Editor.FileLineWidthMax > (u2_t)visibleColumns;
	}
	b8_t arrange = false;
	while(viewer.Editor.Lines.IdControls.size() < (u2_t)visibleCount) {
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Editor.Lines, {}, true, false));
		arrange = true;
	}
	cnst s3_t maxOffsetY = max((s3_t)0, (s3_t)viewer.Editor.FileLines.size() - visibleCount);
	b8_c cursorActive = viewer.Editor.Cursor.y < viewer.Editor.FileLines.size();
	cnst s3_t maxOffsetX = max((s3_t)0, (s3_t)viewer.Editor.FileLineWidthMax + one_if(cursorActive) - visibleColumns);
	scrollbarY->ValueLimits = {0, max((s3_t)1, maxOffsetY)};
	scrollbarX->ValueLimits = {0, max((s3_t)1, maxOffsetX)};
	if_fail_fe(sliderSetValue(*scrollbarY, maxOffsetY ? scrollbarY->ValueCurrent : 0));
	if_fail_fe(sliderSetValue(*scrollbarX, maxOffsetX ? scrollbarX->ValueCurrent : 0));
	if(cursorActive) {
		if(viewer.Editor.Cursor.y < (u2_t)scrollbarY->ValueCurrent) {
			if_fail_fe(sliderSetValue(*scrollbarY, viewer.Editor.Cursor.y));
		}
		else if(viewer.Editor.Cursor.y >= (u2_t)(scrollbarY->ValueCurrent + visibleCount)) {
			if_fail_fe(sliderSetValue(*scrollbarY, viewer.Editor.Cursor.y - visibleCount + 1));
		}
		if(viewer.Editor.Cursor.x < (u2_t)scrollbarX->ValueCurrent) {
			if_fail_fe(sliderSetValue(*scrollbarX, viewer.Editor.Cursor.x));
		}
		else if(viewer.Editor.Cursor.x >= (u2_t)(scrollbarX->ValueCurrent + visibleColumns)) {
			if_fail_fe(sliderSetValue(*scrollbarX, viewer.Editor.Cursor.x - visibleColumns + 1));
		}
	}
	viewport->Settings.ScrollBarY = 0 != maxOffsetY;
	viewport->Settings.ScrollBarX = 0 != maxOffsetX;
	u2_c firstVisible = (u2_t)(maxOffsetY ? scrollbarY->ValueCurrent : 0);
	u2_c firstColumn = (u2_t)(maxOffsetX ? scrollbarX->ValueCurrent : 0);
	for(u2_t iControl = 0; iControl < viewer.Editor.Lines.IdControls.size(); ++iControl) {
		cnst cid_t idControl = viewer.Editor.Lines.IdControls[iControl];
		u2_c iLine = firstVisible + iControl;
		b8_c visible = iControl < (u2_t)visibleCount && iLine < viewer.Editor.FileLines.size();
		if(visible) {
			cnst vcst_t sourceLine = viewer.Editor.FileLines[iLine];
			u2_c offset = min(firstColumn, sourceLine.size());
			u2_c count = min((u2_t)visibleColumns, (u2_t)(sourceLine.size() - offset));
			vcst_t visibleText = {};
			if_fail_fe(sourceLine.slice(visibleText, offset, count));
			if(gui.Controls.Text[idControl].Text != visibleText)
				if_fail_fe(controlTextSet(gui, idControl, visibleText));
		}
		gui.Controls.SetSelected(idControl, visible && iLine == viewer.Editor.Cursor.y);
		if(gui.Controls.IsHidden(idControl) == visible) {
			gui.Controls.SetHidden(idControl, false == visible);
			arrange = true;
		}
	}
	b8_c cursorVisible = cursorActive
		&& viewer.Editor.Cursor.y >= firstVisible && viewer.Editor.Cursor.y < firstVisible + visibleCount
		&& viewer.Editor.Cursor.x >= firstColumn && viewer.Editor.Cursor.x < firstColumn + visibleColumns;
	if(cursorVisible) {
		SControlPlacement & cursorPlacement = gui.Controls.Placement[viewer.Editor.IdCursor];
		n2s1_c cursorOffset = {
			(s1_t)((viewer.Editor.Cursor.x - firstColumn) * characterWidth),
			(s1_t)((viewer.Editor.Cursor.y - firstVisible) * rowHeight),
		};
		n2s1_c cursorSize = {(s1_t)characterWidth, rowHeight};
		if(cursorPlacement.Area.Offset.x != cursorOffset.x || cursorPlacement.Area.Offset.y != cursorOffset.y || cursorPlacement.Area.Size.x != cursorSize.x || cursorPlacement.Area.Size.y != cursorSize.y) {
			cursorPlacement.Area.Offset = cursorOffset;
			cursorPlacement.Area.Size = cursorSize;
			if_fail_fe(controlMetricsInvalidate(gui, viewer.Editor.IdCursor));
		}
	}
	if(gui.Controls.IsHidden(viewer.Editor.IdCursor) == cursorVisible)
		gui.Controls.SetHidden(viewer.Editor.IdCursor, false == cursorVisible);
	if(arrange) {
		if_fail_fe(controlListArrange(gui, viewer.Editor.Lines));
		if_fail_fe(controlMetricsInvalidate(gui, viewer.Editor.Lines.IdControl));
	}
	rtrn 0;
}

sttc gpk::err_t fileViewerSelectionUpdate(SFileViewer & viewer) {
	pobj<SDialogViewport> filesViewport = {};
	pobj<SDialogSlider> filesScrollbar = {};
	if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_FILES], filesViewport));
	if_fail_fe(dialogControlGet(*viewer.Dialog, filesViewport->IdScrollBarY, filesScrollbar));
	SGUI & gui = *viewer.Dialog->GUI;
	u2_c firstVisible = (u2_t)filesScrollbar->ValueCurrent;
	b8_c hasParent = ::fileViewerFolderHasParent(viewer.Directory.PathContents);
	for(u2_t iFile = 0; iFile < viewer.Directory.Files.IdControls.size(); ++iFile) {
		cnst cid_t idControl = viewer.Directory.Files.IdControls[iFile];
		if(false == gui.Controls.IsAction(idControl))
			continue;
		u2_c iItem = firstVisible + iFile;
		for(u2_t iSelection = 0; iSelection < viewer.Directory.Files.IdControls.size(); ++iSelection)
			gui.Controls.SetSelected(viewer.Directory.Files.IdControls[iSelection], iSelection == iFile);
		if(hasParent && 0 == iItem) {
			vcst_t parent = {};
			if_fail_fe(pathDirectory(viewer.Directory.PathContents.Root, parent));
			rtrn ::fileViewerFolderLoad(viewer, parent);
		}
		u2_c iPath = iItem - one_if(hasParent);
		if(iPath >= viewer.Directory.PathContents.Name.size())
			rtrn 0;
		string path = {};
		if_fail_fe(pathNameCompose(viewer.Directory.PathContents.Root, viewer.Directory.PathContents.Name[iPath], path));
		if(viewer.Directory.PathContents.Info[iPath].Folder)
			rtrn ::fileViewerFolderLoad(viewer, path);
		pobj<SDialogViewport> viewport = {};
		if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
		gui.Controls.SetHidden(viewport->IdGUIControl, false);
		rtrn ::fileViewerTextLoad(viewer, path);
	}
	rtrn 0;
}

gpk::err_t fileViewerSetup(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vcst_t rootPath) {
	SGUI & gui = *viewer.Dialog->GUI;
	gui.ColorModeDefault = GUI_COLOR_MODE_3D;
	gui.ThemeDefault = ASCII_COLOR_DARKGREY * 16 + ASCII_COLOR_LIGHTGREY;
	if_fail_fe(::fileViewerViewportCreate(viewer, FILE_VIEWPORT_FILES, "FILES / CURRENT DIRECTORY", {{40, 48}, {420, 600}}));
	if_fail_fe(::fileViewerViewportCreate(viewer, FILE_VIEWPORT_TEXT , "SELECT A FILE", {{480, 72}, {760, 560}}));
	if_fail_fe(::fileViewerListCreate(viewer, viewer.Directory.Files, FILE_VIEWPORT_FILES));
	if_fail_fe(::fileViewerListCreate(viewer, viewer.Editor.Lines, FILE_VIEWPORT_TEXT));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_FILES, true));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_TEXT , true));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_TEXT , false));
	if_fail_fe(::fileViewerEditorCreate(viewer));
	if_fail_fe(::fileViewerMenuCreate(viewer));
	if_fail_fe(::fileViewerFolderLoad(viewer, rootPath.size() ? rootPath : vcst_t{"."}));
	if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		for(u0_t iViewport = 0; iViewport < viewer.Viewports.size(); ++iViewport) {
			pobj<SDialogViewport> viewport = {};
			if_fail_fe(dialogControlGet(*viewer.Dialog, viewer.Viewports[iViewport], viewport));
			viewport->Settings.DisplacementLockX = true;
			viewport->Settings.DisplacementLockY = true;
			gui.Controls.SetHidden(viewport->IdResizeBottomRight, true);
		}
	rtrn 0;
}

gpk::err_t fileViewerUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, vpobj<SEventSystem> frameEvents) {
	if(gpk::err_t result = ::fileViewerMenuUpdate(viewer))
		rtrn result;
	if_fail_fe(::fileViewerSelectionUpdate(viewer));
	if_fail_fe(::fileViewerEditorUpdate(viewer, frameEvents));
	if_fail_fe(viewer.Dialog->Update());
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	if_fail_fe(::fileViewerListWindowUpdate(viewer, viewer.Directory.Files, viewer.Directory.PathContents, FILE_VIEWPORT_FILES));
	if_fail_fe(::fileViewerTextWindowUpdate(viewer));
	rtrn 0;
}

gpk::err_t fileViewerDraw(SFileViewer & viewer, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{22, 24, 28}));
	if_fail_fe(guiUpdateMetrics(*viewer.Dialog->GUI, target.metrics().u1(), false));
	rtrn guiDraw(*viewer.Dialog->GUI, target);
}
