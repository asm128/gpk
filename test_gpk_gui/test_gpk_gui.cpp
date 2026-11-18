#include "test_gpk_gui.h"
#include "gpk_app_impl.h"
#include "gpk_framework.h"
#include "gpk_gui_theme.h"
#include "gpk_string_compose.h"
#include "gpk_system_key.h"

usns gpk;

sttc err_t sampleGUITextOwn(SSampleGUI & app, vcst_t text, vcst_t & output) {
	pobj<string> owned = {};
	if_null_fe(owned.create());
	if_fail_fe(append_string(*owned, text));
	output = *owned;
	return app.Text.push_back(owned);
}

sttc cid_t sampleGUIControlCreate
	( SGUI & gui
	, cid_t idParent
	, vcst_t text
	, cnst rect2s1_t & area
	, ALIGN align = ALIGN_TOP_LEFT
	, ALIGN textAlign = ALIGN_CENTER
	) {
	cid_t idControl = controlCreate(gui);
	if_fail_fe(idControl);
	if(idParent != CID_INVALID)
		if_fail_fe(controlSetParent(gui, idControl, idParent));
	SControlPlacement & placement = gui.Controls.Placement[idControl];
	placement.Area = area;
	placement.Align = align;
	placement.Margin = {2, 2, 2, 2};
	placement.Border = {1, 1, 1, 1};
	gui.Controls.Text[idControl].Text = text;
	gui.Controls.Text[idControl].Align = textAlign;
	return idControl;
}

sttc cid_t sampleGUIPanelCreate
	( SGUI & gui
	, vcst_t title
	, cnst rect2s1_t & area
	, cid_t & idClient
	, cid_t idParent = CID_INVALID
	) {
	cid_t idRoot = controlCreate(gui);
	if_fail_fe(idRoot);
	if(idParent != CID_INVALID)
		if_fail_fe(controlSetParent(gui, idRoot, idParent));
	SControlPlacement & rootPlacement = gui.Controls.Placement[idRoot];
	rootPlacement.Area = {area.Offset, {area.Size.x, 22}};
	rootPlacement.Margin = {1, 1, 1, 1};
	rootPlacement.Border = {3, 3, 3, 3};
	gui.Controls.Draw[idRoot].FrameOut = true;
	gui.Controls.Text[idRoot].Text = title;
	gui.Controls.Text[idRoot].Align = ALIGN_CENTER;

	cid_t idClose = ::sampleGUIControlCreate(gui, idRoot, "X", {{}, {18, 18}}, ALIGN_TOP_RIGHT);
	if_fail_fe(idClose);
	gui.Controls.Placement[idClose].Margin = {};
	gui.Controls.Placement[idClose].Border = {1, 1, 1, 1};

	idClient = ::sampleGUIControlCreate
		( gui, idRoot, {}
		, {{0, 18}, {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 26)}}
		);
	if_fail_fe(idClient);
	gui.Controls.Placement[idClient].Margin = {};
	gui.Controls.Placement[idClient].Border = {1, 1, 1, 1};
	gui.Controls.States[idClient].Mask &= ~(GUI_CONTROL_FLAG_Hovered | GUI_CONTROL_FLAG_Action);
	gui.Controls.Text[idClient].Text = {};

	cid_t idResize = ::sampleGUIControlCreate(gui, idClient, {}, {{-2, -2}, {16, 16}}, ALIGN_BOTTOM_RIGHT);
	if_fail_fe(idResize);
	gui.Controls.Placement[idResize].Margin = {};
	gui.Controls.Text[idResize].Text = {};
	return idRoot;
}

sttc cid_t sampleGUIGridCreate
	( SGUI & gui
	, cid_t idParent
	, view<vcst_c> labels
	, u2_t columnCount
	, cnst n2s1_t & cellSize
	, cnst n2s1_t & spacing = {}
	, cnst n2s1_t & offset = {}
	, ALIGN textAlign = ALIGN_CENTER
	) {
	if_true_fe(0 == columnCount);
	cid_t idRoot = controlCreate(gui);
	if_fail_fe(idRoot);
	if(idParent != CID_INVALID)
		if_fail_fe(controlSetParent(gui, idRoot, idParent));
	u2_c rowCount = (labels.size() + columnCount - 1) / columnCount;
	SControlPlacement & rootPlacement = gui.Controls.Placement[idRoot];
	rootPlacement.Area.Offset = offset;
	rootPlacement.Area.Size =
		{ (s1_t)(columnCount * cellSize.x + (columnCount ? columnCount - 1 : 0) * spacing.x)
		, (s1_t)(rowCount * cellSize.y + (rowCount ? rowCount - 1 : 0) * spacing.y)
		};
	rootPlacement.Margin = {};
	rootPlacement.Border = {};
	gui.Controls.Draw[idRoot].NoClient = true;
	gui.Controls.States[idRoot].Mask &= ~(GUI_CONTROL_FLAG_Hovered | GUI_CONTROL_FLAG_Action);
	for(u2_t iControl = 0; iControl < labels.size(); ++iControl) {
		n2s1_t cell = {(s1_t)(iControl % columnCount), (s1_t)(iControl / columnCount)};
		cid_t idControl = ::sampleGUIControlCreate
			( gui, idRoot, labels[iControl]
			, { { (s1_t)(cell.x * (cellSize.x + spacing.x)), (s1_t)(cell.y * (cellSize.y + spacing.y)) }
			  , cellSize
			  }
			, ALIGN_TOP_LEFT
			, textAlign
			);
		if_fail_fe(idControl);
	}
	return idRoot;
}

sttc u2_t sampleGUIToneFromCell(cnst n2u2_t & cell, u2_t toneCount) nxpt {
	stxp u2_t columnCount = 2;
	u2_c columnSize = ASCII_COLOR_Count / columnCount;
	u2_c completeGroups = toneCount / ASCII_COLOR_Count;
	u2_c completeGroupRows = completeGroups * columnSize;
	if(cell.y >= completeGroupRows)
		return completeGroups * ASCII_COLOR_Count + (cell.y - completeGroupRows) * columnCount + cell.x;
	u2_c toneGroup = cell.y / columnSize;
	u2_c toneRow = cell.y % columnSize;
	return toneGroup * ASCII_COLOR_Count + toneRow + cell.x * columnSize;
}

sttc u2_t sampleGUIToneRows(u2_t toneCount, u2_t column) nxpt {
	stxp u2_t columnCount = 2;
	u2_c columnSize = ASCII_COLOR_Count / columnCount;
	u2_c completeRows = toneCount / ASCII_COLOR_Count * columnSize;
	u2_c remainder = toneCount % ASCII_COLOR_Count;
	return completeRows + (remainder + columnCount - 1 - column) / columnCount;
}

sttc err_t sampleGUIEditorCreate(SSampleGUI & app, SGUI & gui) {
	pobj<SGUIEditor> editor = {};
	if_null_fe(editor.create());
	if_fail_fe(guiSetupDefaultColors(editor->GUI.Colors));
	if_fail_fe(guiSetupDefaultFonts(editor->GUI));
	editor->GUI.ColorModeDefault = GUI_COLOR_MODE_3D;
	if_fail_fe(editor->Target.resize(n2u1_t{752, 440}));

	if_fail_fe(guiUpdateMetrics(editor->GUI, editor->Target.metrics().u1(), true));

	u2_c editorIndex = app.Workspace.Editors.size();
	cid_t idClient = CID_INVALID;
	cid_t idViewport = ::sampleGUIPanelCreate
		( gui, "GUI"
		, {{(s1_t)(160 + editorIndex * 24), (s1_t)(40 + editorIndex * 24)}, {760, 466}}
		, idClient
		);
	if_fail_fe(idViewport);
	editor->IdRoot = idViewport;
	gui.Controls.Images[idClient].Image = editor->Target.View;
	gui.Controls.Images[idClient].ImageAlign = ALIGN_TOP_LEFT;
	if_fail_fe(::sampleGUIControlCreate(gui, idClient, "NEW CONTROL", {{4, 4}, {112, 24}}));
	if_fail_fe(app.Workspace.Editors.push_back(editor));
	return controlMetricsInvalidate(gui, idViewport);
}

sttc cid_t sampleGUIPanelBody(cnst SGUI & gui, cid_t idRoot) {
	if(controlInvalid(gui, idRoot) || false == controlInvalid(gui, gui.Controls.States[idRoot].Parent))
		return CID_INVALID;
	cnst vcid & children = gui.Controls.Children[idRoot];
	return children.size() > 1 ? children[1] : CID_INVALID;
}

sttc err_t sampleGUIAction(SSampleGUI & app, SGUI & gui, cid_t idControl) {
	cnst vcst_t text = gui.Controls.Text[idControl].Text;
	if(text == "NEW GUI")
		return ::sampleGUIEditorCreate(app, gui);
	if(text == "NEW CONTROL") {
		cid_t idRoot = idControl;
		while(false == controlInvalid(gui, gui.Controls.States[idRoot].Parent))
			idRoot = gui.Controls.States[idRoot].Parent;
		for(u2_t iEditor = 0; iEditor < app.Workspace.Editors.size(); ++iEditor) {
			SGUIEditor & editor = *app.Workspace.Editors[iEditor];
			if(editor.IdRoot != idRoot)
				continue;
			cid_t idCreated = ::sampleGUIControlCreate(editor.GUI, CID_INVALID, "CONTROL", {{}, {128, 48}}, ALIGN_CENTER);
			if_fail_fe(idCreated);
			return controlMetricsInvalidate(editor.GUI, idCreated);
		}
		return 0;
	}

	cnst cid_t idParent = gui.Controls.States[idControl].Parent;
	cnst s1_t colorTheme = gui.Controls.Draw[idControl].ColorTheme;
	if(colorTheme > 0 && gui.Colors && gui.Colors->ControlThemes) {
		u2_t iTheme = colorTheme - 1;
		if(false == controlInvalid(gui, idParent) && gui.Controls.Identifier[idParent] == "palette_shades")
			iTheme = gui.ThemeDefault / ASCII_COLOR_Count * ASCII_COLOR_Count + iTheme % ASCII_COLOR_Count;
		if(iTheme < gui.Colors->ControlThemes->size()) {
			gui.ThemeDefault = (u1_t)iTheme;
			if(false == controlInvalid(gui, idParent)) {
				cnst cid_t idPaletteClient = gui.Controls.States[idParent].Parent;
				if(false == controlInvalid(gui, idPaletteClient))
					for(cid_t idGrid : gui.Controls.Children[idPaletteClient])
						if(gui.Controls.Identifier[idGrid] == "palette_shades") {
							vcid shadeCells = gui.Controls.Children[idGrid];
							for(u2_t iShade = 0; iShade < shadeCells.size(); ++iShade)
								gui.Controls.Draw[shadeCells[iShade]].ColorTheme = (s1_t)(iTheme / ASCII_COLOR_Count * ASCII_COLOR_Count + iShade + 1);
							if(iTheme % ASCII_COLOR_Count < shadeCells.size())
								if_fail_fe(guiSelect(gui, shadeCells[iTheme % ASCII_COLOR_Count]));
							break;
						}
			}
			return guiSelect(gui, idControl);
		}
	}

	if(text == "X" && false == controlInvalid(gui, idParent)) {
		gui.Controls.SetHidden(idParent, true);
		return 0;
	}

	if(controlInvalid(gui, idParent)) {
		cnst cid_t idBody = ::sampleGUIPanelBody(gui, idControl);
		if(false == controlInvalid(gui, idBody)) {
			gui.Controls.SetHidden(idBody, false == gui.Controls.IsHidden(idBody));
			return controlMetricsInvalidate(gui, idControl);
		}
		return 0;
	}

	if(gui.Controls.Text[idParent].Text.size())
		return 0;
	for(u2_t iRoot = 0; iRoot < gui.Controls.States.size(); ++iRoot) {
		if(false == controlInvalid(gui, gui.Controls.States[iRoot].Parent))
			continue;
		if(gui.Controls.Text[iRoot].Text != text)
			continue;
		cnst cid_t idBody = ::sampleGUIPanelBody(gui, (cid_t)iRoot);
		if(controlInvalid(gui, idBody))
			continue;
		gui.Controls.SetHidden((cid_t)iRoot, false);
		gui.Controls.SetHidden(idBody, false);
		if_fail_fe(controlToFront(gui, (cid_t)iRoot));
		return controlMetricsInvalidate(gui, (cid_t)iRoot);
	}
	return 0;
}

err_t sampleGUISetup(SSampleGUI & app, SGUI & gui) {
	if_fail_fe(guiSetupDefaultColors(gui.Colors));
	if_fail_fe(guiSetupDefaultFonts(gui));
	gui.ColorModeDefault = GUI_COLOR_MODE_3D;

	sttc vcst_c menuLeft[] = {"PALETTE", "FONTS", "CONTROL STATES", "INSPECTORS", "ALIGN"};
	sttc vcst_c menuRight[] = {"SCREENSHOT", "EXIT"};
	if_fail_fe(::sampleGUIMenuCreate(gui, menuLeft, ALIGN_TOP_LEFT));
	if_fail_fe(::sampleGUIMenuCreate(gui, menuRight, ALIGN_TOP_RIGHT));

	cid_t idPaletteClient = CID_INVALID;
	if_fail_fe(::sampleGUIPanelCreate(gui, "PALETTE", {{20, 28}, {300, 220}}, idPaletteClient));
	u2_c toneCount = gui.Colors && gui.Colors->ControlThemes ? gui.Colors->ControlThemes->size() / ASCII_COLOR_Count : 0;
	u2_t toneDefault = gui.ThemeDefault / ASCII_COLOR_Count;
	if(toneDefault >= toneCount)
		toneDefault = 0;
	aobj<vcst_t> shadeLabels = {};
	if_fail_fe(shadeLabels.resize(ASCII_COLOR_Count));
	cid_t idShades = ::sampleGUIGridCreate(gui, idPaletteClient, view<vcst_c>{shadeLabels}, ASCII_COLOR_Count / 2, {16, 16}, {1, 1}, {62, 4});
	if_fail_fe(idShades);
	gui.Controls.Identifier[idShades] = "palette_shades";
	for(u2_t iShade = 0; iShade < gui.Controls.Children[idShades].size(); ++iShade) {
		cid_t idControl = gui.Controls.Children[idShades][iShade];
		gui.Controls.Draw[idControl].ColorTheme = (s1_t)(toneDefault * ASCII_COLOR_Count + iShade + 1);
		gui.Controls.Placement[idControl].Margin = {};
		gui.Controls.Placement[idControl].Border = {};
		gui.Controls.Draw[idControl].NoBorder = true;
	}
	for(u2_t iColumn = 0; iColumn < 2; ++iColumn) {
		u2_c rowCount = ::sampleGUIToneRows(toneCount, iColumn);
		aobj<vcst_t> toneLabels = {};
		if_fail_fe(toneLabels.resize(rowCount * ASCII_COLOR_Count));
		cid_t idTones = ::sampleGUIGridCreate(gui, idPaletteClient, view<vcst_c>{toneLabels}, ASCII_COLOR_Count, {8, 5}, {}, {(s1_t)(16 + iColumn * 132), 40});
		if_fail_fe(idTones);
		for(u2_t iCell = 0; iCell < gui.Controls.Children[idTones].size(); ++iCell) {
			u2_c row = iCell / ASCII_COLOR_Count;
			u2_c shade = iCell % ASCII_COLOR_Count;
			u2_c tone = ::sampleGUIToneFromCell({iColumn, row}, toneCount);
			cid_t idControl = gui.Controls.Children[idTones][iCell];
			gui.Controls.Draw[idControl].ColorTheme = (s1_t)(tone * ASCII_COLOR_Count + shade + 1);
			gui.Controls.Placement[idControl].Margin = {};
			gui.Controls.Placement[idControl].Border = {};
			gui.Controls.Draw[idControl].NoBorder = true;
		}
	}
	if_fail_fe(::sampleGUIControlCreate(gui, idPaletteClient, "Mode: 3D", {{4, 162}, {134, 42}}));
	if_fail_fe(::sampleGUIControlCreate(gui, idPaletteClient, "Geometry: Indexed", {{142, 162}, {146, 42}}));

	cid_t idFontsClient = CID_INVALID;
	if_fail_fe(::sampleGUIPanelCreate(gui, "FONTS", {{332, 28}, {252, 264}}, idFontsClient));
	aobj<vcst_t> fontLabels = {};
	if_fail_fe(fontLabels.resize(gui.Fonts.size()));
	for(u2_t iFont = 0; iFont < gui.Fonts.size(); ++iFont) {
		string labelText = {};
		if_fail_fe(append_strings(labelText, str(gui.Fonts[iFont]->CharSize.x), "x", str(gui.Fonts[iFont]->CharSize.y)));
		if_fail_fe(::sampleGUITextOwn(app, labelText, fontLabels[iFont]));
	}
	cid_t idFonts = ::sampleGUIGridCreate(gui, idFontsClient, view<vcst_c>{fontLabels}, 2, {120, 28}, {}, {4, 4});
	if_fail_fe(idFonts);
	for(u2_t iFont = 0; iFont < gui.Controls.Children[idFonts].size(); ++iFont)
		if_fail_fe(controlFontSet(gui, gui.Controls.Children[idFonts][iFont], (s1_t)iFont));
	if(gui.SelectedFont < gui.Controls.Children[idFonts].size())
		gui.Controls.SetSelected(gui.Controls.Children[idFonts][gui.SelectedFont], true);

	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	cid_t idStatesClient = CID_INVALID;
	if_fail_fe(::sampleGUIPanelCreate(gui, "CONTROL STATES", {{20, 254}, {176, (s1_t)(42 + stateDefinition.Values.size() * 18)}}, idStatesClient));
	aobj<vcst_t> stateLabels = {};
	if_fail_fe(stateLabels.resize(stateDefinition.Values.size() * 2));
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		stateLabels[iState * 2] = stateLabels[iState * 2 + 1] = stateDefinition.Names[iState];
	cid_t idStates = ::sampleGUIGridCreate(gui, idStatesClient, view<vcst_c>{stateLabels}, 2, {76, 18}, {}, {8, 6});
	if_fail_fe(idStates);
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cid_t idName = gui.Controls.Children[idStates][iState * 2];
		cid_t idState = gui.Controls.Children[idStates][iState * 2 + 1];
		gui.Controls.Text[idName].Align = ALIGN_CENTER_RIGHT;
		gui.Controls.Text[idState].Align = ALIGN_CENTER_LEFT;
		if(stateDefinition.Values[iState] < GUI_CONTROL_FLAG_Updated)
			gui.Controls.States[idState].Flag = bit_set(gui.Controls.States[idState].Flag, stateDefinition.Values[iState]);
	}

	cid_t idInspectorsClient = CID_INVALID;
	if_fail_fe(::sampleGUIPanelCreate(gui, "INSPECTORS", {{596, 28}, {300, 456}}, idInspectorsClient));
	sttc vcst_c inspectorTitles[] = {"LIVE", "HOVERED", "FOCUSED"};
	sttc vcst_c inspectorNames[] = {"ID", "", "STATE", "", "PALETTE", "", "ALIGN", "", "TEXT", ""};
	sttc vcst_c inspectorLive[] = {"ID", "#TITLE", "STATE", "Updated", "PALETTE", "NORMAL [0]", "ALIGN", "TOP_LEFT", "TEXT", "CENTER"};
	for(u2_t iInspector = 0; iInspector < size(inspectorTitles); ++iInspector) {
		cid_t idInspectorClient = CID_INVALID;
		if_fail_fe(::sampleGUIPanelCreate(gui, inspectorTitles[iInspector], {{4, (s1_t)(4 + iInspector * 140)}, {286, 134}}, idInspectorClient, idInspectorsClient));
		view<vcst_c> cells = iInspector ? view<vcst_c>{inspectorNames} : view<vcst_c>{inspectorLive};
		cid_t idInspector = ::sampleGUIGridCreate(gui, idInspectorClient, cells, 2, {134, 18}, {}, {4, 4});
		if_fail_fe(idInspector);
		for(u2_t iRow = 0; iRow < 5; ++iRow) {
			gui.Controls.Text[gui.Controls.Children[idInspector][iRow * 2]].Align = ALIGN_CENTER_RIGHT;
			gui.Controls.Text[gui.Controls.Children[idInspector][iRow * 2 + 1]].Align = ALIGN_CENTER_LEFT;
		}
	}

	cid_t idAlignClient = CID_INVALID;
	if_fail_fe(::sampleGUIPanelCreate(gui, "ALIGN", {{908, 28}, {352, 320}}, idAlignClient));
	sttc cnst ALIGN horizontal[] = {ALIGN_LEFT, ALIGN_HCENTER, ALIGN_RIGHT};
	sttc cnst ALIGN vertical[] = {ALIGN_TOP, ALIGN_VCENTER, ALIGN_BOTTOM};
	for(u2_t y = 0; y < 3; ++y)
	for(u2_t x = 0; x < 3; ++x) {
		ALIGN controlAlign = (ALIGN)(vertical[y] | horizontal[x]);
		ALIGN textAlign = (ALIGN)(vertical[2 - y] | horizontal[2 - x]);
		cid_t idControl = ::sampleGUIControlCreate(gui, idAlignClient, get_value_namev(controlAlign), {{(s1_t)(4 + x * 114), (s1_t)(4 + y * 92)}, {110, 54}});
		if_fail_fe(idControl);
		gui.Controls.Text[idControl].Align = textAlign;
	}

	if_fail_fe(::sampleGUIControlCreate(gui, CID_INVALID, "NEW GUI", {{20, 500}, {112, 32}}));
	return guiUpdateMetrics(gui, {1280, 540}, true);
}

err_t sampleGUIUpdate(SSampleGUI & app, SGUI & gui, b8_t systemRequestedExit) {
	if(systemRequestedExit)
		return APPLICATION_STATE_EXIT;
	if_fail_fe(guiProcessControls(gui, [&](cid_t idControl) { return ::sampleGUIAction(app, gui, idControl); }));
	return 0;
}

err_t sampleGUIDraw(SSampleGUI & app, SGUI & gui, g8bgra target) {
	for(u2_t iEditor = 0; iEditor < app.Workspace.Editors.size(); ++iEditor) {
		SGUIEditor & editor = *app.Workspace.Editors[iEditor];
		if_fail_fe(editor.Target.View.fill(SColorBGRA{8, 10, 12}));
		if_fail_fe(guiDraw(editor.GUI, editor.Target.View));
	}
	if_fail_fe(target.fill(SColorBGRA{16, 14, 12}));
	return guiDraw(gui, target);
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI palettes, states and alignment"

stct SApplication {
	SFramework Framework;
	SSampleGUI Sample;

	SApplication(SRuntimeValues & runtimeValues) : Framework(runtimeValues, {}) {}
};

err_t setup(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 540};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	return ::sampleGUISetup(app.Sample, *app.Framework.GUI);
}

err_t cleanup(SApplication & app) {
	return mainWindowDestroy(app.Framework.RootWindow);
}

err_t update(SApplication & app, b8_t systemRequestedExit) {
	if(systemRequestedExit)
		return APPLICATION_STATE_EXIT;
	err_t result = updateFramework(app.Framework);
	if(result)
		return result;
	return ::sampleGUIUpdate(app.Sample, *app.Framework.GUI, app.Framework.RootWindow.Input->KeyDown(SYSTEM_KEY_Escape));
}

err_t draw(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	if(window.NoDraw)
		return 0;
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	return ::sampleGUIDraw(app.Sample, *app.Framework.GUI, window.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, GPK_SAMPLE_GUI_TITLE)
