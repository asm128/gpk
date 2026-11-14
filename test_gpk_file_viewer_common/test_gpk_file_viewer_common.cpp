#include "test_gpk_file_viewer_common.h"

#include "gpk_file.h"
#include "gpk_label.h"
#include "gpk_path.h"

stxp ::gpk::u0_t FILE_VIEWPORT_FILES = 0;
stxp ::gpk::u0_t FILE_VIEWPORT_TEXT  = 1;

sttc ::gpk::err_t fileViewerViewportCreate(SFileViewer & viewer, ::gpk::u0_t index, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(::gpk::viewportCreate(*viewer.Dialog, viewport));
	viewer.Viewports[index] = viewport->IdGUIControl;
	::gpk::SControlTable & controls = viewer.GUI.Controls;
	::gpk::SControlPlacement & root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Text[viewport->IdTitle].Text = title;
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(::gpk::s1_t)(area.Size.x - 8), (::gpk::s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn ::gpk::controlMetricsInvalidate(viewer.GUI, viewport->IdGUIControl);
}

sttc ::gpk::err_t fileViewerViewportAreaSet(SFileViewer & viewer, ::gpk::u0_t index, cnst ::gpk::rect2s1_t & area) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[index], viewport));
	viewer.GUI.Controls.Placement[viewport->IdGUIControl].Area = area;
	viewer.GUI.Controls.Placement[viewport->IdClient].Area.Size = {(::gpk::s1_t)(area.Size.x - 8), (::gpk::s1_t)(area.Size.y - 32)};
	rtrn ::gpk::controlMetricsInvalidate(viewer.GUI, viewport->IdGUIControl);
}

sttc ::gpk::err_t fileViewerLayoutSet(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	if(FILE_VIEWER_LAYOUT_FLOATING == layout) {
		if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_FILES, {{ 40, 48}, {420, 600}}));
		if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_TEXT , {{480, 72}, {760, 560}}));
		rtrn 0;
	}
	cnst ::gpk::n2s1_t windowSize = viewer.Window.Size.s1();
	cnst ::gpk::s1_t margin = 12;
	cnst ::gpk::s1_t gap = 8;
	cnst ::gpk::s1_t leftWidth = ::gpk::min((::gpk::s1_t)360, (::gpk::s1_t)((windowSize.x - margin * 2 - gap) / 3));
	cnst ::gpk::s1_t height = (::gpk::s1_t)(windowSize.y - margin * 2);
	if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_FILES, {{margin, margin}, {leftWidth, height}}));
	if_fail_fe(::fileViewerViewportAreaSet(viewer, FILE_VIEWPORT_TEXT , {{(::gpk::s1_t)(margin + leftWidth + gap), margin}, {(::gpk::s1_t)(windowSize.x - margin * 2 - leftWidth - gap), height}}));
	rtrn 0;
}

sttc ::gpk::err_t fileViewerListCreate(SFileViewer & viewer, ::gpk::SControlList & list, ::gpk::u0_t viewportIndex) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
	if_fail_fe(::gpk::controlListInitialize(viewer.GUI, list));
	list.Orientation = ::gpk::DIRECTION_DOWN;
	if_fail_fe(::gpk::controlSetParent(viewer.GUI, list.IdControl, viewport->IdClient));
	viewer.GUI.Controls.Constraints[list.IdControl].AttachSizeToControl.x = list.IdControl;
	viewer.GUI.Controls.States[list.IdControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	viewer.GUI.Controls.States[list.IdControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Pressed;
	viewer.GUI.Controls.States[list.IdControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Dragging;
	viewer.GUI.Controls.States[list.IdControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Action;
	rtrn 0;
}

sttc void fileViewerListRowConfigure(SFileViewer & viewer, ::gpk::cid_t idControl, bool interactive, bool buttonPalette = true) {
	viewer.GUI.Controls.Draw[idControl].UseNewPalettes = true;
	viewer.GUI.Controls.Draw[idControl].Palettes = buttonPalette ? viewer.Dialog->Colors->Button : viewer.Dialog->Colors->Control;
	viewer.GUI.Controls.Text[idControl].Align = ::gpk::ALIGN_CENTER_LEFT;
	if(false == interactive) {
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Pressed;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Dragging;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Action;
	}
}

sttc ::gpk::err_t fileViewerListRowPush(SFileViewer & viewer, ::gpk::SControlList & list, ::gpk::vcst_t text, bool interactive, bool buttonPalette = true) {
	if_fail_fe(::gpk::controlListPush(viewer.GUI, list, ::gpk::label(text)));
	cnst ::gpk::cid_t idControl = list.IdControls[list.IdControls.size() - 1];
	::fileViewerListRowConfigure(viewer, idControl, interactive, buttonPalette);
	rtrn 0;
}

sttc ::gpk::err_t fileViewerScrollCreate(SFileViewer & viewer, ::gpk::u0_t viewportIndex, bool vertical) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbar = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
	if_fail_fe(::gpk::sliderCreate(*viewer.Dialog, scrollbar));
	scrollbar->Vertical = vertical;
	scrollbar->FuncValueFormat = [](::gpk::vcst_t & format, ::gpk::s3_t, cnst ::gpk::minmax<::gpk::s3_t> &) { format = ""; rtrn 0; };
	scrollbar->ValueLimits = {0, 1};
	if_fail_fe(::gpk::sliderSetValue(*scrollbar, 0));
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

sttc ::gpk::err_t fileViewerEditorCreate(SFileViewer & viewer) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	if_fail_fe(viewer.IdSave = ::gpk::controlCreateChild(viewer.GUI, viewport->IdGUIControl));
	::gpk::SControlPlacement & placement = viewer.GUI.Controls.Placement[viewer.IdSave];
	placement.Area.Size = {64, viewer.GUI.Controls.Placement[viewport->IdClose].Area.Size.y};
	placement.Align = ::gpk::ALIGN_TOP_RIGHT;
	viewer.GUI.Controls.Constraints[viewer.IdSave].AttachSizeToControl = {-1, -1};
	viewer.GUI.Controls.Constraints[viewer.IdSave].DockToControl.Right = viewport->IdClose;
	viewer.GUI.Controls.Draw[viewer.IdSave].UseNewPalettes = true;
	viewer.GUI.Controls.Draw[viewer.IdSave].Palettes = viewer.Dialog->Colors->Button;
	viewer.GUI.Controls.Text[viewer.IdSave].Text = "Save";
	viewer.GUI.Controls.Text[viewer.IdSave].Align = ::gpk::ALIGN_CENTER;
	viewer.GUI.Controls.SetHidden(viewer.IdSave, true);
	if_fail_fe(viewer.IdKeyboard = ::gpk::controlCreateChild(viewer.GUI, viewport->IdGUIControl));
	::gpk::SControlPlacement & keyboardPlacement = viewer.GUI.Controls.Placement[viewer.IdKeyboard];
	keyboardPlacement.Area.Size = {88, viewer.GUI.Controls.Placement[viewport->IdClose].Area.Size.y};
	keyboardPlacement.Align = ::gpk::ALIGN_TOP_RIGHT;
	viewer.GUI.Controls.Constraints[viewer.IdKeyboard].AttachSizeToControl = {-1, -1};
	viewer.GUI.Controls.Constraints[viewer.IdKeyboard].DockToControl.Right = viewer.IdSave;
	viewer.GUI.Controls.Constraints[viewport->IdTitle].DockToControl.Right = viewer.IdKeyboard;
	viewer.GUI.Controls.Draw[viewer.IdKeyboard].UseNewPalettes = true;
	viewer.GUI.Controls.Draw[viewer.IdKeyboard].Palettes = viewer.Dialog->Colors->Button;
	viewer.GUI.Controls.Text[viewer.IdKeyboard].Text = "Keyboard";
	viewer.GUI.Controls.Text[viewer.IdKeyboard].Align = ::gpk::ALIGN_CENTER;
	viewer.GUI.Controls.SetHidden(viewer.IdKeyboard, true);
	if_fail_fe(viewer.IdCursor = ::gpk::controlCreateChild(viewer.GUI, viewport->IdClient));
	viewer.GUI.Controls.Placement[viewer.IdCursor].Border = {};
	viewer.GUI.Controls.Placement[viewer.IdCursor].Margin = {};
	viewer.GUI.Controls.Draw[viewer.IdCursor].NoBorder = true;
	viewer.GUI.Controls.Draw[viewer.IdCursor].NoClient = true;
	viewer.GUI.Controls.Text[viewer.IdCursor].Text = "|";
	viewer.GUI.Controls.Text[viewer.IdCursor].Align = ::gpk::ALIGN_CENTER_LEFT;
	viewer.GUI.Controls.States[viewer.IdCursor].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	viewer.GUI.Controls.States[viewer.IdCursor].Mask &= ~::gpk::GUI_CONTROL_FLAG_Pressed;
	viewer.GUI.Controls.States[viewer.IdCursor].Mask &= ~::gpk::GUI_CONTROL_FLAG_Dragging;
	viewer.GUI.Controls.States[viewer.IdCursor].Mask &= ~::gpk::GUI_CONTROL_FLAG_Action;
	viewer.GUI.Controls.SetHidden(viewer.IdCursor, true);
	if_fail_fe(::gpk::virtualKeyboardSetup437(viewer.GUI, viewer.VirtualKeyboard));
	if_fail_fe(::gpk::controlSetParent(viewer.GUI, viewer.VirtualKeyboard.IdRoot, viewer.Dialog->Root));
	rtrn 0;
}

sttc ::gpk::err_t fileViewerListWindowUpdate(SFileViewer & viewer, ::gpk::SControlList & list, cnst ::gpk::aobj<::gpk::string> & items, ::gpk::u0_t viewportIndex) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbar = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[viewportIndex], viewport));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	if(items.size() && 0 == list.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, list, {}, true));
	cnst ::gpk::s1_t rowHeight = list.IdControls.size() ? ::gpk::max((::gpk::s1_t)1, viewer.GUI.Controls.Placement[list.IdControls[0]].Area.Size.y) : 1;
	cnst ::gpk::s3_t visibleCount = ::gpk::max((::gpk::s3_t)1, (::gpk::s3_t)viewer.GUI.Controls.Placement[viewport->IdClient].Area.Size.y / rowHeight);
	while(list.IdControls.size() < ::gpk::min(items.size(), (::gpk::u2_t)visibleCount))
		if_fail_fe(::fileViewerListRowPush(viewer, list, {}, true));
	cnst ::gpk::s3_t maxOffset = ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)items.size() - visibleCount);
	scrollbar->ValueLimits = {0, ::gpk::max((::gpk::s3_t)1, maxOffset)};
	if_fail_fe(::gpk::sliderSetValue(*scrollbar, maxOffset ? scrollbar->ValueCurrent : 0));
	viewport->Settings.ScrollBarY = 0 != maxOffset;
	cnst ::gpk::s3_t firstVisible = maxOffset ? scrollbar->ValueCurrent : 0;
	for(::gpk::u2_t iControl = 0; iControl < list.IdControls.size(); ++iControl) {
		cnst ::gpk::cid_t idControl = list.IdControls[iControl];
		cnst ::gpk::u2_t iItem = (::gpk::u2_t)firstVisible + iControl;
		::gpk::vcst_t text = {};
		if(iControl < (::gpk::u2_t)visibleCount && iItem < items.size())
			text = items[iItem];
		if(viewer.GUI.Controls.Text[idControl].Text != text)
			if_fail_fe(::gpk::controlTextSet(viewer.GUI, idControl, text));
	}
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextViewsClear(SFileViewer & viewer) {
	for(::gpk::u2_t iControl = 0; iControl < viewer.Lines.IdControls.size(); ++iControl)
		if_fail_fe(::gpk::controlTextSet(viewer.GUI, viewer.Lines.IdControls[iControl], {}));
	viewer.FileLines.clear();
	viewer.FileLineWidthMax = 0;
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextViewsRebuild(SFileViewer & viewer) {
	viewer.FileLines.clear();
	viewer.FileLineWidthMax = 0;
	::gpk::vcst_t remaining = viewer.FileText;
	do {
		::gpk::vcst_t line = {};
		::gpk::vcst_t next = {};
		cnst ::gpk::err_t splitResult = ::gpk::split('\n', remaining, line, next);
		if(line.size() && '\r' == line[line.size() - 1])
			if_fail_fe(line.slice(line, 0, line.size() - 1));
		if_fail_fe(viewer.FileLines.push_back(line));
		viewer.FileLineWidthMax = ::gpk::max(viewer.FileLineWidthMax, line.size());
		if(0 > splitResult)
			break;
		remaining = next;
	} while(true);
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextLoad(SFileViewer & viewer, ::gpk::vcst_t path) {
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	viewer.FileText.clear();
	if_fail_fef(::gpk::fileToMemory(path, viewer.FileText), "Failed to load '%.*s'.", GPK_VIEW_ARGS(path));
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	viewer.FilePath = path;
	viewer.CursorLine = (::gpk::u2_t)-1;
	viewer.CursorColumn = 0;
	viewer.VirtualKeyboard.Events.clear();
	viewer.GUI.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
	viewer.GUI.Controls.SetHidden(viewer.IdSave, false);
	viewer.GUI.Controls.SetHidden(viewer.IdKeyboard, false);
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	::gpk::pobj<::gpk::SDialogSlider> scrollbar = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbar));
	if_fail_fe(::gpk::sliderSetValue(*scrollbar, 0));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbar));
	if_fail_fe(::gpk::sliderSetValue(*scrollbar, 0));
	viewer.GUI.Controls.Text[viewport->IdTitle].Text = ::gpk::label(path);
	rtrn 0;
}

sttc ::gpk::u2_t fileViewerLineOffset(cnst SFileViewer & viewer, ::gpk::u2_t line) {
	::gpk::u2_t offset = 0;
	::gpk::u2_t iLine = 0;
	while(offset < viewer.FileText.size() && iLine < line)
		if('\n' == viewer.FileText[offset++])
			++iLine;
	rtrn offset;
}

sttc ::gpk::u2_t fileViewerCursorOffset(cnst SFileViewer & viewer) {
	if(viewer.CursorLine >= viewer.FileLines.size())
		rtrn viewer.FileText.size();
	cnst ::gpk::vcst_t line = viewer.FileLines[viewer.CursorLine];
	rtrn ::fileViewerLineOffset(viewer, viewer.CursorLine) + ::gpk::min(viewer.CursorColumn, line.size());
}

sttc ::gpk::err_t fileViewerTextInsert(SFileViewer & viewer, ::gpk::sc_t character) {
	cnst ::gpk::u2_t offset = ::fileViewerCursorOffset(viewer);
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	if_fail_fe(viewer.FileText.insert(offset, character));
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	if('\n' == character) {
		++viewer.CursorLine;
		viewer.CursorColumn = 0;
	}
	else
		++viewer.CursorColumn;
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextBackspace(SFileViewer & viewer) {
	if(viewer.CursorLine >= viewer.FileLines.size())
		rtrn 0;
	cnst ::gpk::u2_t cursorOffset = ::fileViewerCursorOffset(viewer);
	if(viewer.CursorColumn) {
		if_fail_fe(::fileViewerTextViewsClear(viewer));
		if_fail_fe(viewer.FileText.remove(cursorOffset - 1));
		--viewer.CursorColumn;
	}
	else if(viewer.CursorLine) {
		cnst ::gpk::vcst_t previousLine = viewer.FileLines[viewer.CursorLine - 1];
		cnst ::gpk::u2_t separatorOffset = ::fileViewerLineOffset(viewer, viewer.CursorLine - 1) + previousLine.size();
		cnst ::gpk::u2_t separatorCount = cursorOffset - separatorOffset;
		viewer.CursorColumn = previousLine.size();
		--viewer.CursorLine;
		if_fail_fe(::fileViewerTextViewsClear(viewer));
		for(::gpk::u2_t iCharacter = 0; iCharacter < separatorCount; ++iCharacter)
			if_fail_fe(viewer.FileText.remove(separatorOffset));
	}
	if_fail_fe(::fileViewerTextViewsRebuild(viewer));
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextDelete(SFileViewer & viewer) {
	if(viewer.CursorLine >= viewer.FileLines.size())
		rtrn 0;
	cnst ::gpk::u2_t cursorOffset = ::fileViewerCursorOffset(viewer);
	if(cursorOffset >= viewer.FileText.size())
		rtrn 0;
	::gpk::u2_t count = 1;
	if('\r' == viewer.FileText[cursorOffset] && cursorOffset + 1 < viewer.FileText.size() && '\n' == viewer.FileText[cursorOffset + 1])
		count = 2;
	if_fail_fe(::fileViewerTextViewsClear(viewer));
	for(::gpk::u2_t iCharacter = 0; iCharacter < count; ++iCharacter)
		if_fail_fe(viewer.FileText.remove(cursorOffset));
	rtrn ::fileViewerTextViewsRebuild(viewer);
}

sttc ::gpk::err_t fileViewerEditorHandle(SFileViewer & viewer, cnst ::gpk::SVirtualKeyboardEvent & eventToProcess) {
	if(viewer.CursorLine >= viewer.FileLines.size())
		rtrn 0;
	if(::gpk::VK_EVENT_RELEASE == eventToProcess.Type)
		rtrn ::fileViewerTextInsert(viewer, (::gpk::sc_t)eventToProcess.ScanCode);
	if(::gpk::VK_EVENT_EDIT != eventToProcess.Type)
		rtrn 0;
	switch(eventToProcess.ScanCode) {
	default: break;
	case ::gpk::VK_SCANCODE_Escape:
		viewer.GUI.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		viewer.CursorLine = (::gpk::u2_t)-1;
		viewer.CursorColumn = 0;
		break;
	case ::gpk::VK_SCANCODE_Enter:
		if_fail_fe(::fileViewerTextInsert(viewer, '\n'));
		break;
	case ::gpk::VK_SCANCODE_Backspace:
		if_fail_fe(::fileViewerTextBackspace(viewer));
		break;
	case ::gpk::VK_SCANCODE_Clear:
		if_fail_fe(::fileViewerTextViewsClear(viewer));
		viewer.FileText.clear();
		viewer.CursorLine = viewer.CursorColumn = 0;
		if_fail_fe(::fileViewerTextViewsRebuild(viewer));
		break;
	case ::gpk::VK_SCANCODE_Delete:
		if_fail_fe(::fileViewerTextDelete(viewer));
		break;
	case ::gpk::VK_SCANCODE_Left:
		if(viewer.CursorColumn)
			--viewer.CursorColumn;
		else if(viewer.CursorLine) {
			--viewer.CursorLine;
			viewer.CursorColumn = viewer.FileLines[viewer.CursorLine].size();
		}
		break;
	case ::gpk::VK_SCANCODE_Right:
		if(viewer.CursorColumn < viewer.FileLines[viewer.CursorLine].size())
			++viewer.CursorColumn;
		else if(viewer.CursorLine + 1 < viewer.FileLines.size()) {
			++viewer.CursorLine;
			viewer.CursorColumn = 0;
		}
		break;
	case ::gpk::VK_SCANCODE_Up:
		if(viewer.CursorLine) {
			--viewer.CursorLine;
			viewer.CursorColumn = ::gpk::min(viewer.CursorColumn, viewer.FileLines[viewer.CursorLine].size());
		}
		break;
	case ::gpk::VK_SCANCODE_Down:
		if(viewer.CursorLine + 1 < viewer.FileLines.size()) {
			++viewer.CursorLine;
			viewer.CursorColumn = ::gpk::min(viewer.CursorColumn, viewer.FileLines[viewer.CursorLine].size());
		}
		break;
	}
	rtrn 0;
}

sttc ::gpk::err_t fileViewerEditorPhysicalInput(SFileViewer & viewer) {
	if(viewer.GUI.Controls.IsHidden(viewer.VirtualKeyboard.IdRoot))
		rtrn 0;
	for(::gpk::u2_t iEvent = 0; iEvent < viewer.Window.EventQueue.size(); ++iEvent) {
		cnst ::gpk::pobj<::gpk::SEventSystem> & systemEvent = viewer.Window.EventQueue[iEvent];
		if(systemEvent->Type == ::gpk::SYSTEM_EVENT_Text) {
			if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT>(*systemEvent, [&viewer](cnst ::gpk::SEventView<::gpk::EVENT_TEXT> & eventText) {
				if(eventText.Type == ::gpk::EVENT_TEXT_Char && eventText.Data.size() && eventText.Data[0] >= 0x20 && eventText.Data[0] <= 0x7F)
					if_fail_fe(viewer.VirtualKeyboard.Events.push_back({::gpk::VK_EVENT_RELEASE, (::gpk::VK_SCANCODE)eventText.Data[0]}));
				rtrn 0;
			}));
		}
		else if(systemEvent->Type == ::gpk::SYSTEM_EVENT_Keyboard) {
			if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD>(*systemEvent, [&viewer](cnst ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & eventKeyboard) {
				if(eventKeyboard.Type != ::gpk::EVENT_KEYBOARD_Down || 0 == eventKeyboard.Data.size())
					rtrn 0;
				::gpk::VK_SCANCODE scanCode = {};
				switch(eventKeyboard.Data[0]) {
				default: rtrn 0;
				case ::gpk::SYSTEM_KEY_Escape	: scanCode = ::gpk::VK_SCANCODE_Escape	; break;
				case ::gpk::SYSTEM_KEY_Enter	: scanCode = ::gpk::VK_SCANCODE_Enter	; break;
				case ::gpk::SYSTEM_KEY_Backspace: scanCode = ::gpk::VK_SCANCODE_Backspace; break;
				case ::gpk::SYSTEM_KEY_Clear	: scanCode = ::gpk::VK_SCANCODE_Clear	; break;
				case ::gpk::SYSTEM_KEY_Delete	: scanCode = ::gpk::VK_SCANCODE_Delete	; break;
				case ::gpk::SYSTEM_KEY_Left		: scanCode = ::gpk::VK_SCANCODE_Left	; break;
				case ::gpk::SYSTEM_KEY_Right	: scanCode = ::gpk::VK_SCANCODE_Right	; break;
				case ::gpk::SYSTEM_KEY_Up		: scanCode = ::gpk::VK_SCANCODE_Up		; break;
				case ::gpk::SYSTEM_KEY_Down		: scanCode = ::gpk::VK_SCANCODE_Down	; break;
				}
				if_fail_fe(viewer.VirtualKeyboard.Events.push_back({::gpk::VK_EVENT_EDIT, scanCode}));
				rtrn 0;
			}));
		}
	}
	rtrn 0;
}

sttc ::gpk::err_t fileViewerEditorUpdate(SFileViewer & viewer) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbarY = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	if(viewer.GUI.Controls.IsHidden(viewport->IdGUIControl)) {
		viewer.GUI.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, true);
		rtrn 0;
	}
	if(viewer.GUI.Controls.IsAction(viewer.IdKeyboard))
		viewer.GUI.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false == viewer.GUI.Controls.IsHidden(viewer.VirtualKeyboard.IdRoot));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	cnst ::gpk::u2_t firstVisible = (::gpk::u2_t)scrollbarY->ValueCurrent;
	for(::gpk::u2_t iControl = 0; iControl < viewer.Lines.IdControls.size(); ++iControl)
		if(viewer.GUI.Controls.IsAction(viewer.Lines.IdControls[iControl])) {
			cnst ::gpk::u2_t iLine = firstVisible + iControl;
			if(iLine < viewer.FileLines.size()) {
				viewer.CursorLine = iLine;
				viewer.CursorColumn = viewer.FileLines[iLine].size();
				viewer.GUI.Controls.SetHidden(viewer.VirtualKeyboard.IdRoot, false);
			}
		}
	cnst ::gpk::cid_t idCharacterBegin = viewer.VirtualKeyboard.IdRoot + 1;
	cnst ::gpk::cid_t idCharacterEnd = idCharacterBegin + (::gpk::cid_t)viewer.VirtualKeyboard.Keys.size();
	for(::gpk::cid_t idKey = idCharacterBegin; idKey < idCharacterEnd; ++idKey)
		if(viewer.GUI.Controls.IsAction(idKey))
			if_fail_fe(::gpk::virtualKeyboardHandleEvent(viewer.VirtualKeyboard, idKey, viewer.VirtualKeyboard.Events));
	for(::gpk::u2_t iKey = 0; iKey < viewer.VirtualKeyboard.IdKeys.size(); ++iKey)
		if(viewer.GUI.Controls.IsAction(viewer.VirtualKeyboard.IdKeys[iKey]))
			if_fail_fe(::gpk::virtualKeyboardHandleEvent(viewer.VirtualKeyboard, viewer.VirtualKeyboard.IdKeys[iKey], viewer.VirtualKeyboard.Events));
	if_fail_fe(::fileViewerEditorPhysicalInput(viewer));
	for(::gpk::u2_t iEvent = 0; iEvent < viewer.VirtualKeyboard.Events.size(); ++iEvent)
		if_fail_fe(::fileViewerEditorHandle(viewer, viewer.VirtualKeyboard.Events[iEvent]));
	viewer.VirtualKeyboard.Events.clear();
	if(viewer.GUI.Controls.IsAction(viewer.IdSave) && viewer.FilePath.size())
		if_fail_fef(::gpk::fileFromMemory(viewer.FilePath, viewer.FileText), "Failed to save '%.*s'.", GPK_VIEW_ARGS(viewer.FilePath));
	rtrn 0;
}

sttc ::gpk::err_t fileViewerTextWindowUpdate(SFileViewer & viewer) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbarY = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbarX = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbarX));
	if(0 == viewer.Lines.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Lines, {}, true, false));
	cnst ::gpk::s1_t rowHeight = ::gpk::max((::gpk::s1_t)1, viewer.GUI.Controls.Placement[viewer.Lines.IdControls[0]].Area.Size.y);
	cnst ::gpk::n2s1_t clientSize = viewer.GUI.Controls.Area[viewport->IdClient].Client.Global.Size;
	cnst ::gpk::s1_t scrollbarWidth = viewer.GUI.Controls.Placement[scrollbarY->IdGUIControl].Area.Size.x;
	cnst ::gpk::s1_t scrollbarHeight = viewer.GUI.Controls.Placement[scrollbarX->IdGUIControl].Area.Size.y;
	cnst ::gpk::u0_t characterWidth = ::gpk::max((::gpk::u0_t)1, viewer.GUI.Fonts[viewer.GUI.SelectedFont]->CharSize.x);
	::gpk::s3_t visibleCount = 1;
	::gpk::s3_t visibleColumns = 1;
	bool scrollX = false;
	bool scrollY = false;
	for(::gpk::u0_t iPass = 0; iPass < 3; ++iPass) {
		visibleCount = ::gpk::max((::gpk::s3_t)1, (::gpk::s3_t)(clientSize.y - (scrollX ? scrollbarHeight : 0)) / rowHeight);
		visibleColumns = ::gpk::max((::gpk::s3_t)1, (::gpk::s3_t)(clientSize.x - (scrollY ? scrollbarWidth : 0)) / characterWidth);
		scrollY = viewer.FileLines.size() > (::gpk::u2_t)visibleCount;
		scrollX = viewer.FileLineWidthMax > (::gpk::u2_t)visibleColumns;
	}
	bool arrange = false;
	while(viewer.Lines.IdControls.size() < (::gpk::u2_t)visibleCount) {
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Lines, {}, true, false));
		arrange = true;
	}
	cnst ::gpk::s3_t maxOffsetY = ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)viewer.FileLines.size() - visibleCount);
	cnst bool cursorActive = viewer.CursorLine < viewer.FileLines.size();
	cnst ::gpk::s3_t maxOffsetX = ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)viewer.FileLineWidthMax + ::gpk::one_if(cursorActive) - visibleColumns);
	scrollbarY->ValueLimits = {0, ::gpk::max((::gpk::s3_t)1, maxOffsetY)};
	scrollbarX->ValueLimits = {0, ::gpk::max((::gpk::s3_t)1, maxOffsetX)};
	if_fail_fe(::gpk::sliderSetValue(*scrollbarY, maxOffsetY ? scrollbarY->ValueCurrent : 0));
	if_fail_fe(::gpk::sliderSetValue(*scrollbarX, maxOffsetX ? scrollbarX->ValueCurrent : 0));
	if(cursorActive) {
		if(viewer.CursorLine < (::gpk::u2_t)scrollbarY->ValueCurrent) {
			if_fail_fe(::gpk::sliderSetValue(*scrollbarY, viewer.CursorLine));
		}
		else if(viewer.CursorLine >= (::gpk::u2_t)(scrollbarY->ValueCurrent + visibleCount)) {
			if_fail_fe(::gpk::sliderSetValue(*scrollbarY, viewer.CursorLine - visibleCount + 1));
		}
		if(viewer.CursorColumn < (::gpk::u2_t)scrollbarX->ValueCurrent) {
			if_fail_fe(::gpk::sliderSetValue(*scrollbarX, viewer.CursorColumn));
		}
		else if(viewer.CursorColumn >= (::gpk::u2_t)(scrollbarX->ValueCurrent + visibleColumns)) {
			if_fail_fe(::gpk::sliderSetValue(*scrollbarX, viewer.CursorColumn - visibleColumns + 1));
		}
	}
	viewport->Settings.ScrollBarY = 0 != maxOffsetY;
	viewport->Settings.ScrollBarX = 0 != maxOffsetX;
	cnst ::gpk::u2_t firstVisible = (::gpk::u2_t)(maxOffsetY ? scrollbarY->ValueCurrent : 0);
	cnst ::gpk::u2_t firstColumn = (::gpk::u2_t)(maxOffsetX ? scrollbarX->ValueCurrent : 0);
	for(::gpk::u2_t iControl = 0; iControl < viewer.Lines.IdControls.size(); ++iControl) {
		cnst ::gpk::cid_t idControl = viewer.Lines.IdControls[iControl];
		cnst ::gpk::u2_t iLine = firstVisible + iControl;
		cnst bool visible = iControl < (::gpk::u2_t)visibleCount && iLine < viewer.FileLines.size();
		if(visible) {
			cnst ::gpk::vcst_t sourceLine = viewer.FileLines[iLine];
			cnst ::gpk::u2_t offset = ::gpk::min(firstColumn, sourceLine.size());
			cnst ::gpk::u2_t count = ::gpk::min((::gpk::u2_t)visibleColumns, (::gpk::u2_t)(sourceLine.size() - offset));
			::gpk::vcst_t visibleText = {};
			if_fail_fe(sourceLine.slice(visibleText, offset, count));
			if(viewer.GUI.Controls.Text[idControl].Text != visibleText)
				if_fail_fe(::gpk::controlTextSet(viewer.GUI, idControl, visibleText));
		}
		viewer.GUI.Controls.SetSelected(idControl, visible && iLine == viewer.CursorLine);
		if(viewer.GUI.Controls.IsHidden(idControl) == visible) {
			viewer.GUI.Controls.SetHidden(idControl, false == visible);
			arrange = true;
		}
	}
	cnst bool cursorVisible = cursorActive
		&& viewer.CursorLine >= firstVisible && viewer.CursorLine < firstVisible + visibleCount
		&& viewer.CursorColumn >= firstColumn && viewer.CursorColumn < firstColumn + visibleColumns;
	if(cursorVisible) {
		::gpk::SControlPlacement & cursorPlacement = viewer.GUI.Controls.Placement[viewer.IdCursor];
		cnst ::gpk::n2s1_t cursorOffset = {
			(::gpk::s1_t)((viewer.CursorColumn - firstColumn) * characterWidth),
			(::gpk::s1_t)((viewer.CursorLine - firstVisible) * rowHeight),
		};
		cnst ::gpk::n2s1_t cursorSize = {(::gpk::s1_t)characterWidth, rowHeight};
		if(cursorPlacement.Area.Offset.x != cursorOffset.x || cursorPlacement.Area.Offset.y != cursorOffset.y || cursorPlacement.Area.Size.x != cursorSize.x || cursorPlacement.Area.Size.y != cursorSize.y) {
			cursorPlacement.Area.Offset = cursorOffset;
			cursorPlacement.Area.Size = cursorSize;
			if_fail_fe(::gpk::controlMetricsInvalidate(viewer.GUI, viewer.IdCursor));
		}
	}
	if(viewer.GUI.Controls.IsHidden(viewer.IdCursor) == cursorVisible)
		viewer.GUI.Controls.SetHidden(viewer.IdCursor, false == cursorVisible);
	if(arrange) {
		if_fail_fe(::gpk::controlListArrange(viewer.GUI, viewer.Lines));
		if_fail_fe(::gpk::controlMetricsInvalidate(viewer.GUI, viewer.Lines.IdControl));
	}
	rtrn 0;
}

sttc ::gpk::err_t fileViewerSelectionUpdate(SFileViewer & viewer) {
	for(::gpk::u2_t iFile = 0; iFile < viewer.Files.IdControls.size(); ++iFile) {
		cnst ::gpk::cid_t idControl = viewer.Files.IdControls[iFile];
		if(false == viewer.GUI.Controls.IsAction(idControl))
			continue;
		for(::gpk::u2_t iSelection = 0; iSelection < viewer.Files.IdControls.size(); ++iSelection)
			viewer.GUI.Controls.SetSelected(viewer.Files.IdControls[iSelection], iSelection == iFile);
		::gpk::pobj<::gpk::SDialogViewport> viewport = {};
		if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
		viewer.GUI.Controls.SetHidden(viewport->IdGUIControl, false);
		rtrn ::fileViewerTextLoad(viewer, viewer.GUI.Controls.Text[idControl].Text);
	}
	rtrn 0;
}

::gpk::err_t fileViewerSetup(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout, ::gpk::vcst_t rootPath) {
	if(0 == viewer.Dialog.create(viewer.GUIHandle))
		rtrn ::gpk::OS_NO_MEMORY;
	viewer.Dialog->Input = viewer.Window.Input;
	viewer.GUI.ColorModeDefault = ::gpk::GUI_COLOR_MODE_3D;
	viewer.GUI.ThemeDefault = ::gpk::ASCII_COLOR_DARKGREY * 16 + ::gpk::ASCII_COLOR_LIGHTGREY;
	if_fail_fe(::fileViewerViewportCreate(viewer, FILE_VIEWPORT_FILES, "FILES / CURRENT DIRECTORY", {{40, 48}, {420, 600}}));
	if_fail_fe(::fileViewerViewportCreate(viewer, FILE_VIEWPORT_TEXT , "SELECT A FILE", {{480, 72}, {760, 560}}));
	if_fail_fe(::fileViewerListCreate(viewer, viewer.Files, FILE_VIEWPORT_FILES));
	if_fail_fe(::fileViewerListCreate(viewer, viewer.Lines, FILE_VIEWPORT_TEXT));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_FILES, true));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_TEXT , true));
	if_fail_fe(::fileViewerScrollCreate(viewer, FILE_VIEWPORT_TEXT , false));
	if_fail_fe(::fileViewerEditorCreate(viewer));
	if_fail_fe(::gpk::pathList(rootPath, viewer.PathContents, [](::gpk::b8_t folder, ::gpk::vcst_c &) { rtrn folder ? 1 : 0; }, {}));
	if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		for(::gpk::u0_t iViewport = 0; iViewport < viewer.Viewports.size(); ++iViewport) {
			::gpk::pobj<::gpk::SDialogViewport> viewport = {};
			if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[iViewport], viewport));
			viewport->Settings.DisplacementLockX = true;
			viewport->Settings.DisplacementLockY = true;
			viewer.GUI.Controls.SetHidden(viewport->IdResizeBottomRight, true);
		}
	rtrn ::gpk::guiUpdateMetrics(viewer.GUI, viewer.Window.Size, false);
}

::gpk::err_t fileViewerUpdate(SFileViewer & viewer, FILE_VIEWER_LAYOUT layout) {
	if_fail_fe(::fileViewerSelectionUpdate(viewer));
	if_fail_fe(::fileViewerEditorUpdate(viewer));
	if_fail_fe(viewer.Dialog->Update());
	if(FILE_VIEWER_LAYOUT_SPLIT == layout)
		if_fail_fe(::fileViewerLayoutSet(viewer, layout));
	if_fail_fe(::fileViewerListWindowUpdate(viewer, viewer.Files, viewer.PathContents.Files, FILE_VIEWPORT_FILES));
	if_fail_fe(::fileViewerTextWindowUpdate(viewer));
	rtrn ::gpk::guiUpdateMetrics(viewer.GUI, viewer.Window.Size, false);
}

::gpk::err_t fileViewerDraw(SFileViewer & viewer, ::gpk::g8bgra target) {
	if_fail_fe(target.fill(::gpk::SColorBGRA{22, 24, 28}));
	rtrn ::gpk::guiDraw(viewer.GUI, target);
}
