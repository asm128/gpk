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

sttc void fileViewerListRowConfigure(SFileViewer & viewer, ::gpk::cid_t idControl, bool interactive) {
	viewer.GUI.Controls.Draw[idControl].UseNewPalettes = true;
	viewer.GUI.Controls.Draw[idControl].Palettes = interactive ? viewer.Dialog->Colors->Button : viewer.Dialog->Colors->Control;
	viewer.GUI.Controls.Text[idControl].Align = ::gpk::ALIGN_CENTER_LEFT;
	if(false == interactive) {
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Pressed;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Dragging;
		viewer.GUI.Controls.States[idControl].Mask &= ~::gpk::GUI_CONTROL_FLAG_Action;
	}
}

sttc ::gpk::err_t fileViewerListRowPush(SFileViewer & viewer, ::gpk::SControlList & list, ::gpk::vcst_t text, bool interactive) {
	if_fail_fe(::gpk::controlListPush(viewer.GUI, list, ::gpk::label(text)));
	cnst ::gpk::cid_t idControl = list.IdControls[list.IdControls.size() - 1];
	::fileViewerListRowConfigure(viewer, idControl, interactive);
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

sttc ::gpk::err_t fileViewerTextLoad(SFileViewer & viewer, ::gpk::vcst_t path) {
	for(::gpk::u2_t iControl = 0; iControl < viewer.Lines.IdControls.size(); ++iControl)
		if_fail_fe(::gpk::controlTextSet(viewer.GUI, viewer.Lines.IdControls[iControl], {}));
	viewer.FileLines.clear();
	viewer.FileText.clear();
	viewer.FileLineWidthMax = 0;
	if_fail_fef(::gpk::fileToMemory(path, viewer.FileText), "Failed to load '%.*s'.", GPK_VIEW_ARGS(path));
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

sttc ::gpk::err_t fileViewerTextWindowUpdate(SFileViewer & viewer) {
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbarY = {};
	::gpk::pobj<::gpk::SDialogSlider> scrollbarX = {};
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewer.Viewports[FILE_VIEWPORT_TEXT], viewport));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarY, scrollbarY));
	if_fail_fe(::gpk::dialogControlGet(*viewer.Dialog, viewport->IdScrollBarX, scrollbarX));
	if(0 == viewer.Lines.IdControls.size())
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Lines, {}, false));
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
		if_fail_fe(::fileViewerListRowPush(viewer, viewer.Lines, {}, false));
		arrange = true;
	}
	cnst ::gpk::s3_t maxOffsetY = ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)viewer.FileLines.size() - visibleCount);
	cnst ::gpk::s3_t maxOffsetX = ::gpk::max((::gpk::s3_t)0, (::gpk::s3_t)viewer.FileLineWidthMax - visibleColumns);
	scrollbarY->ValueLimits = {0, ::gpk::max((::gpk::s3_t)1, maxOffsetY)};
	scrollbarX->ValueLimits = {0, ::gpk::max((::gpk::s3_t)1, maxOffsetX)};
	if_fail_fe(::gpk::sliderSetValue(*scrollbarY, maxOffsetY ? scrollbarY->ValueCurrent : 0));
	if_fail_fe(::gpk::sliderSetValue(*scrollbarX, maxOffsetX ? scrollbarX->ValueCurrent : 0));
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
		if(viewer.GUI.Controls.IsHidden(idControl) == visible) {
			viewer.GUI.Controls.SetHidden(idControl, false == visible);
			arrange = true;
		}
	}
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
