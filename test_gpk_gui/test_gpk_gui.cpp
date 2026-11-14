#include "test_gpk_gui.h"
#include "gpk_app_impl.h"
#include "gpk_cstring.h"
#include "gpk_framework.h"
#include "gpk_dialog_controls.h"
#include "gpk_file.h"
#include "gpk_label.h"
#include "gpk_png.h"
#include "gpk_runtime.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"
#include "gpk_system_key.h"

usns gpk;

stxp u2_t PALETTE_SHADE_COUNT = 16;
stxp u2_t PALETTE_TONE_COLUMN_COUNT = 2;
stxp u2_t PALETTE_TONE_COLUMN_SIZE = 8;
stxp s1_t FONT_BODY = 6; // 8x12
stxp s1_t FONT_TITLE = 7; // 8x16

sttc SGUI & getGUI(pobj<SDialog> & dialog) nxpt { rtrn *dialog->GUI; }
sttc cnst SGUI & getGUI(cnst pobj<SDialog> & dialog) nxpt { rtrn *dialog->GUI; }
sttc SControlTable & getControls(pobj<SDialog> & dialog) nxpt { rtrn ::getGUI(dialog).Controls; }
sttc cnst SControlTable & getControls(cnst pobj<SDialog> & dialog) nxpt { rtrn ::getGUI(dialog).Controls; }

GDEFINE_ENUM_TYPE(SAMPLE_GUI_INSPECTOR_FIELD, u0_t);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ID     , 0);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, STATE  , 1);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, PALETTE, 2);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ALIGN  , 3);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, TEXT   , 4);

sttc err_t sampleGUIControlCreate(SSampleGUI & app, cid_t & idControl, vcst_t text, cnst rect2s1_t & area, s1_t font = -1, ALIGN align = ALIGN_TOP_LEFT) {
	if_fail_fe(idControl = controlCreate(::getGUI(app.Dialog)));
	SControlPlacement & placement = ::getControls(app.Dialog).Placement[idControl];
	placement.Area = area;
	placement.Align = align;
	placement.Margin = {2, 2, 2, 2};
	placement.Border = {3, 3, 3, 3};
	SControlDraw & draw = ::getControls(app.Dialog).Draw[idControl];
	draw.ColorMode = GUI_COLOR_MODE_Default;
	::getControls(app.Dialog).Text[idControl].Text = text;
	::getControls(app.Dialog).Text[idControl].FontSelected = font;
	rtrn 0;
}

sttc err_t sampleGUIGeometryModeApply(SSampleGUI & app) {
	for(u2_t iControl = 0; iControl < ::getControls(app.Dialog).Draw.size(); ++iControl)
		::getControls(app.Dialog).Draw[iControl].UseNewPalettes = app.UseNewPalettes;
	rtrn controlTextSet(::getGUI(app.Dialog), app.ControlGeometry, app.UseNewPalettes ? vcst_t{"Areas"} : vcst_t{"Indexed"});
}

sttc void sampleGUIStateCardsApply(SSampleGUI & app) nxpt {
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst GUI_CONTROL_FLAG stateFlag = stateDefinition.Values[iState];
		if(stateFlag == GUI_CONTROL_FLAG_Unused)
			continue;
		SControlState & state = ::getControls(app.Dialog).States[controlGridControl(app.GridStates, {1, iState})];
		cnst GUI_CONTROL_FLAG updated = (GUI_CONTROL_FLAG)(state.Flag & GUI_CONTROL_FLAG_Updated);
		state.Flag = (GUI_CONTROL_FLAG)(updated | stateFlag);
	}
}

sttc u2_t sampleGUIPaletteToneCount(cnst SSampleGUI & app) nxpt {
	if(!::getGUI(app.Dialog).Colors || !::getGUI(app.Dialog).Colors->ControlThemes)
		rtrn 0;
	rtrn ::getGUI(app.Dialog).Colors->ControlThemes->size() / PALETTE_SHADE_COUNT;
}

sttc u2_t sampleGUIPaletteToneRowCount(cnst u2_t toneCount, cnst u2_t iColumn) nxpt {
	cnst u2_t completeGroupRows = toneCount / PALETTE_SHADE_COUNT * PALETTE_TONE_COLUMN_SIZE;
	cnst u2_t remainingTones = toneCount % PALETTE_SHADE_COUNT;
	rtrn completeGroupRows + (remainingTones + 1 - iColumn) / PALETTE_TONE_COLUMN_COUNT;
}

sttc u2_t sampleGUIPaletteToneFromCell(cnst n2u2_t & cell, cnst u2_t toneCount) nxpt {
	cnst u2_t completeGroups = toneCount / PALETTE_SHADE_COUNT;
	cnst u2_t completeGroupRows = completeGroups * PALETTE_TONE_COLUMN_SIZE;
	if(cell.y >= completeGroupRows)
		rtrn completeGroups * PALETTE_SHADE_COUNT + (cell.y - completeGroupRows) * PALETTE_TONE_COLUMN_COUNT + cell.x;
	cnst u2_t toneGroup = cell.y / PALETTE_TONE_COLUMN_SIZE;
	cnst u2_t toneRow = cell.y % PALETTE_TONE_COLUMN_SIZE;
	rtrn toneGroup * PALETTE_SHADE_COUNT + toneRow + cell.x * PALETTE_TONE_COLUMN_SIZE;
}

sttc void sampleGUIPaletteToneSelect(SSampleGUI & app, cnst cid_t idSelected) nxpt {
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst cid_t idControl = controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow});
		::getControls(app.Dialog).SetSelected(idControl, idControl == idSelected);
	}
}

sttc u2_t sampleGUIPaletteToneSelected(cnst SSampleGUI & app, cnst u2_t toneCount) nxpt {
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(::getControls(app.Dialog).IsSelected(controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow})))
			rtrn ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount);
	rtrn ::getGUI(app.Dialog).ThemeDefault / PALETTE_SHADE_COUNT % toneCount;
}

sttc void sampleGUIPaletteShadeSelect(SSampleGUI & app, cnst u2_t selected) nxpt {
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		::getControls(app.Dialog).SetSelected(controlGridControl(app.PalettePicker.GridShades, iShade), iShade == selected);
}

sttc u2_t sampleGUIPaletteShadeSelected(cnst SSampleGUI & app) nxpt {
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(::getControls(app.Dialog).IsSelected(controlGridControl(app.PalettePicker.GridShades, iShade)))
			rtrn iShade;
	rtrn ::getGUI(app.Dialog).ThemeDefault % PALETTE_SHADE_COUNT;
}

sttc err_t sampleGUIPalettePickerApply(SSampleGUI & app) {
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if(0 == toneCount)
		rtrn 0;
	cnst u2_t tone = ::sampleGUIPaletteToneSelected(app, toneCount);
	cnst u2_t shade = ::sampleGUIPaletteShadeSelected(app);
	::getGUI(app.Dialog).ThemeDefault = (u1_t)(tone * PALETTE_SHADE_COUNT + shade);
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst u2_t iTone = ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount);
		cnst cid_t idControl = controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow});
		::getControls(app.Dialog).Draw[idControl].ColorTheme = (s1_t)(iTone * PALETTE_SHADE_COUNT + iShade + 1);
	}
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst cid_t idControl = controlGridControl(app.PalettePicker.GridShades, iShade);
		::getControls(app.Dialog).Draw[idControl].ColorTheme = (s1_t)(tone * PALETTE_SHADE_COUNT + iShade + 1);
	}
	rtrn 0;
}

sttc void sampleGUIFontSelect(SSampleGUI & app, cnst u2_t selected) nxpt {
	for(u2_t iFont = 0; iFont < ::getGUI(app.Dialog).Fonts.size(); ++iFont)
		::getControls(app.Dialog).SetSelected(controlGridControl(app.FontPicker.Grid, iFont), iFont == selected);
}

sttc u2_t sampleGUIFontSelected(cnst SSampleGUI & app) nxpt {
	for(u2_t iFont = 0; iFont < ::getGUI(app.Dialog).Fonts.size(); ++iFont)
		if(::getControls(app.Dialog).IsSelected(controlGridControl(app.FontPicker.Grid, iFont)))
			rtrn iFont;
	rtrn ::getGUI(app.Dialog).SelectedFont % ::getGUI(app.Dialog).Fonts.size();
}

sttc void sampleGUIFontPickerApply(SSampleGUI & app) nxpt {
	if(0 == ::getGUI(app.Dialog).Fonts.size())
		rtrn;
	::getGUI(app.Dialog).SelectedFont = (u1_t)::sampleGUIFontSelected(app);
	for(u2_t iFont = 0; iFont < ::getGUI(app.Dialog).Fonts.size(); ++iFont) {
		cnst cid_t idControl = controlGridControl(app.FontPicker.Grid, iFont);
		controlFontSet(::getGUI(app.Dialog), idControl, (s1_t)iFont);
	}
}

sttc err_t sampleGUIStateFlagAppend(string & text, vcst_t flag) {
	if(text.size())
		if_fail_fe(text.push_back('+'));
	rtrn append_string(text, flag);
}

sttc err_t sampleGUIStateDescribe(cnst SControlState & state, b8_t disabled, string & text) {
	text.clear();
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		if(bit_test_masked(state.Flag, state.Mask, stateDefinition.Values[iState]))
			if_fail_fe(::sampleGUIStateFlagAppend(text, stateDefinition.Names[iState]));
	if(disabled && false == state.IsDisabled())
		if_fail_fe(::sampleGUIStateFlagAppend(text, get_value_namev(GUI_CONTROL_FLAG_Disabled)));
	if(0 == text.size())
		if_fail_fe(append_string(text, "-"));
	rtrn text.size();
}

sttc err_t sampleGUIControlTextSetOwned(SSampleGUI & app, cid_t idControl, astsc_t<512> & storage, vcst_t text) {
	if(::getControls(app.Dialog).Text[idControl].Text == text)
		rtrn 0;
	if_true_fe(text.size() >= storage.size());
	for(u2_t iCharacter = 0; iCharacter < text.size(); ++iCharacter)
		storage.Storage[iCharacter] = text[iCharacter];
	storage.Storage[text.size()] = 0;
	rtrn controlTextSet(::getGUI(app.Dialog), idControl, {storage.Storage, text.size()});
}

sttc cid_t sampleGUIHoveredControl(cnst SSampleGUI & app) nxpt {
	for(u2_t iControl = 0; iControl < ::getControls(app.Dialog).States.size(); ++iControl)
		if(false == controlInvalid(::getGUI(app.Dialog), (cid_t)iControl) && ::getControls(app.Dialog).IsHovered((cid_t)iControl))
			rtrn (cid_t)iControl;
	rtrn CID_INVALID;
}

sttc cid_t sampleGUIFocusedControl(cnst SSampleGUI & app) nxpt {
	for(u2_t iControl = 0; iControl < ::getControls(app.Dialog).States.size(); ++iControl)
		if(false == controlInvalid(::getGUI(app.Dialog), (cid_t)iControl) && ::getControls(app.Dialog).IsFocused((cid_t)iControl))
			rtrn (cid_t)iControl;
	rtrn CID_INVALID;
}

sttc vcst_t sampleGUIPaletteName(cnst GUI_CONTROL_PALETTE palette) nxpt {
	cnst SCTTIEnumDeclaration * definition = cttiEnum("GUI_CONTROL_PALETTE");
	if(definition)
		for(u2_t iValue = 0; iValue < definition->Count; ++iValue)
			if(definition->Values[iValue].Value == (s3_t)palette) {
				cnst vcst_t name = definition->Names[iValue];
				cnst u2_t prefixLength = definition->Name.size() + 1;
				rtrn name.size() > prefixLength ? vcst_t{name.begin() + prefixLength, name.size() - prefixLength} : name;
			}
	rtrn "-";
}

sttc err_t sampleGUIInspectorCreate(SSampleGUI & app, SSampleGUIInspector & inspector, vcst_t title, cnst rect2s1_t & area, cid_t idParent) {
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdRoot, "", area, -1));
	if_fail_fe(controlSetParent(::getGUI(app.Dialog), inspector.IdRoot, idParent));
	cnst s1_t clientWidth = (s1_t)(area.Size.x - controlNCSpacing(::getControls(app.Dialog).Placement[inspector.IdRoot]).x);
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdTitle, title, {{0, 0}, {clientWidth, 30}}, FONT_BODY));
	if_fail_fe(controlSetParent(::getGUI(app.Dialog), inspector.IdTitle, inspector.IdRoot));
	enum_definition<SAMPLE_GUI_INSPECTOR_FIELD> & fields = get_enum<SAMPLE_GUI_INSPECTOR_FIELD>();
	sttc cnst vcst_t emptyValues[] = {"NONE", "-", "-", "-", "-"};
	acid idNames = {}, idValues = {};
	if_fail_fe(guiCreateControlList(::getGUI(app.Dialog), {fields.Names}, inspector.IdRoot, {88, 18}, {0, 34}, ALIGN_TOP_LEFT, ALIGN_CENTER_RIGHT, idNames));
	if_fail_fe(guiCreateControlList(::getGUI(app.Dialog), {emptyValues}, inspector.IdRoot, {(u1_t)(clientWidth - 88), 18}, {88, 34}, ALIGN_TOP_LEFT, ALIGN_CENTER_LEFT, idValues));
	rtrn 0;
}

sttc err_t sampleGUIInspectorValueSet(SSampleGUI & app, SSampleGUIInspector & inspector, SAMPLE_GUI_INSPECTOR_FIELD field, vcst_t value) {
	stxp u0_t fieldCount = SAMPLE_GUI_INSPECTOR_FIELD_TEXT + 1;
	cnst acid & children = ::getControls(app.Dialog).Children[inspector.IdRoot];
	rtrn ::sampleGUIControlTextSetOwned(app, children[1 + fieldCount + field], inspector.TextValues[field], value);
}

sttc err_t sampleGUIInspectorUpdate(SSampleGUI & app, SSampleGUIInspector & inspector, cid_t idControl) {
	SGUI			& gui			= ::getGUI(app.Dialog);
	SControlTable	& controls		= gui.Controls;
	if(controlInvalid(gui, idControl)) {
		if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, "NONE"));
		for(u0_t iField = SAMPLE_GUI_INSPECTOR_FIELD_STATE; iField <= SAMPLE_GUI_INSPECTOR_FIELD_TEXT; ++iField)
			if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, (SAMPLE_GUI_INSPECTOR_FIELD)iField, "-"));
		rtrn 0;
	}
	cnst SControlState & state = controls.States[idControl];
	b8_c			disabled	= 0 != controlDisabled(gui, idControl);
	string			stateText	= {};
	if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
	cnst GUI_CONTROL_PALETTE palette = paletteIndexFromState(disabled, state);
	vcst_c				paletteName = ::sampleGUIPaletteName(palette);
	s1_c				paletteSlot	= controls.Draw[idControl].Palettes[palette];
	string			paletteText	= {};
	if_fail_fe(append_strings(paletteText, paletteName, " [", str(paletteSlot), "]"));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, str(idControl)));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_STATE, stateText));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_PALETTE, paletteText));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ALIGN, get_value_namev(controls.Placement[idControl].Align)));
	rtrn ::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_TEXT, get_value_namev(controls.Text[idControl].Align));
}

sttc err_t sampleGUIViewportCreate(SSampleGUI & app, cid_t & idViewport, pobj<SDialogViewport> & viewport, vcst_t title, cnst rect2s1_t & area) {
	if_fail_fe(idViewport = viewportCreate(*app.Dialog, viewport));
	if_fail_fe(app.ViewportIds.push_back(idViewport));
	SControlTable & controls = ::getControls(app.Dialog);
	controls.Draw[viewport->IdGUIControl      ].UseNewPalettes = false;
	controls.Draw[viewport->IdClose           ].UseNewPalettes = false;
	controls.Draw[viewport->IdResizeBottomRight].UseNewPalettes = false;
	SControlPlacement & root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Text[viewport->IdTitle].Text = title;
	controls.Text[viewport->IdTitle].FontSelected = FONT_BODY;
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(s1_t)(area.Size.x - 8), (s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn controlMetricsInvalidate(::getGUI(app.Dialog), viewport->IdGUIControl);
}

err_t sampleGUISetup(SSampleGUI & app, pobj<SInput> & input) {
	app.Dialog->Input = input;
	::getGUI(app.Dialog).SelectedFont = FONT_BODY;
	::getGUI(app.Dialog).ColorModeDefault = GUI_COLOR_MODE_3D;
	pobj<SDialogViewport> viewportPalette = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.PalettePicker.IdViewport, viewportPalette, "PALETTE", {{20, 18}, {300, 220}}));
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if_true_fe(0 == toneCount);
	cnst u2_t toneDefault = toneCount >= 3 ? toneCount - 3 : 0;
	cnst u2_t shadeDefault = 8;
	aobj<vcst_t> toneTexts[PALETTE_TONE_COLUMN_COUNT] = {};
	aobj<vcst_t> shadeTexts = {};
	if_fail_fe(shadeTexts.resize(PALETTE_SHADE_COUNT));
	if_fail_fe(controlGridCreate(::getGUI(app.Dialog), app.PalettePicker.GridShades, view<vcst_c>{shadeTexts}, viewportPalette->IdClient, DIRECTION_RIGHT, 8, {16, 16}, {1, 1}, {78, 4}));
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn) {
		if_fail_fe(toneTexts[iColumn].resize(::sampleGUIPaletteToneRowCount(toneCount, iColumn) * PALETTE_SHADE_COUNT));
		if_fail_fe(controlGridCreate(::getGUI(app.Dialog), app.PalettePicker.GridTones[iColumn], view<vcst_c>{toneTexts[iColumn]}, viewportPalette->IdClient, DIRECTION_RIGHT, PALETTE_SHADE_COUNT, {8, 5}, {}, {(s1_t)(16 + iColumn * 132), 40}));
		for(u2_t iColor = 0; iColor < toneTexts[iColumn].size(); ++iColor) {
			cnst cid_t idControl = controlGridControl(app.PalettePicker.GridTones[iColumn], iColor);
			::getControls(app.Dialog).Placement[idControl].Border = {};
			::getControls(app.Dialog).Placement[idControl].Margin = {};
			::getControls(app.Dialog).Draw[idControl].NoBorder = true;
		}
	}
	::sampleGUIPaletteShadeSelect(app, shadeDefault);
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
		if(toneDefault == ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount))
			::sampleGUIPaletteToneSelect(app, controlGridControl(app.PalettePicker.GridTones[iColumn], {shadeDefault, iRow}));
	pobj<SDialogViewport> viewportFonts = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.FontPicker.IdViewport, viewportFonts, "FONTS", {{332, 18}, {252, 264}}));
	if_fail_fe(app.FontPicker.Labels.resize(::getGUI(app.Dialog).Fonts.size()));
	for(u2_t iFont = 0; iFont < ::getGUI(app.Dialog).Fonts.size(); ++iFont) {
		cnst n2u0_t charSize = ::getGUI(app.Dialog).Fonts[iFont]->CharSize;
		if_fail_fe(append_strings(app.FontPicker.Labels[iFont], str(charSize.x), "x", str(charSize.y)));
	}
	if_fail_fe(controlGridCreate(::getGUI(app.Dialog), app.FontPicker.Grid, view<string>{app.FontPicker.Labels}, viewportFonts->IdClient, DIRECTION_RIGHT, 2, {120, 28}, {4, 6}));
	::sampleGUIFontSelect(app, ::getGUI(app.Dialog).SelectedFont);
	cnst enum_definition<GUI_CONTROL_FLAG> & stateDefinition = get_enum<GUI_CONTROL_FLAG>();
	pobj<SDialogViewport> viewportStates = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportStates, viewportStates, "CONTROL STATES", {{20, 242}, {176, (s1_t)(42 + stateDefinition.Values.size() * 18)}}));

	if_fail_fe(::sampleGUIControlCreate(app, app.ControlMode, "Mode: 3D", {{4, 136}, {134, 42}}, FONT_BODY));
	if_fail_fe(controlSetParent(::getGUI(app.Dialog), app.ControlMode, viewportPalette->IdClient));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlGeometry, "Geometry: Indexed", {{142, 136}, {146, 42}}, FONT_BODY));
	if_fail_fe(controlSetParent(::getGUI(app.Dialog), app.ControlGeometry, viewportPalette->IdClient));
	aobj<vcst_t> stateCells = {};
	if_fail_fe(stateCells.resize(stateDefinition.Values.size() * 2));
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		stateCells[iState] = stateDefinition.Names[iState];
		stateCells[stateDefinition.Values.size() + iState] = stateDefinition.Names[iState];
	}
	if_fail_fe(controlGridCreate(::getGUI(app.Dialog), app.GridStates, view<vcst_t>{stateCells}, viewportStates->IdClient, DIRECTION_DOWN, 2, {76, 18}, {0, 0}, {8, 6}));
	for(u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst cid_t idName = app.GridStates[0].IdControls[iState];
		cnst cid_t idState = app.GridStates[1].IdControls[iState];
		::getControls(app.Dialog).Text[idName].FontSelected = ::getControls(app.Dialog).Text[idState].FontSelected = FONT_BODY;
		::getControls(app.Dialog).Text[idName].Align = ALIGN_CENTER_RIGHT;
		::getControls(app.Dialog).Text[idState].Align = ALIGN_CENTER_LEFT;
		::getControls(app.Dialog).Draw[idName].ColorMode = ::getControls(app.Dialog).Draw[idState].ColorMode = GUI_COLOR_MODE_Default;
	}
	pobj<SDialogViewport> viewportInspectors = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportInspectors, viewportInspectors, "INSPECTORS", {{384, 18}, {300, 456}}));
	::getControls(app.Dialog).Placement[viewportInspectors->IdGUIControl].Align = ALIGN_TOP_RIGHT;
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorLive, "LIVE", {{4, 4}, {286, 134}}, viewportInspectors->IdClient));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorHovered, "HOVERED", {{4, 140}, {286, 134}}, viewportInspectors->IdClient));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorFocused, "FOCUSED", {{4, 276}, {286, 134}}, viewportInspectors->IdClient));
	app.ControlLive = app.InspectorLive.IdTitle;
	pobj<SDialogViewport> viewportAlignments = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportAlignments, viewportAlignments, "ALIGN", {{20, 18}, {372, 187}}));
	::getControls(app.Dialog).Placement[viewportAlignments->IdGUIControl].Align = ALIGN_TOP_RIGHT;
	sttc cnst ALIGN horizontal[] = {ALIGN_LEFT, ALIGN_HCENTER, ALIGN_RIGHT};
	sttc cnst ALIGN vertical  [] = {ALIGN_TOP , ALIGN_VCENTER, ALIGN_BOTTOM};
	for(u0_t y = 0; y < 3; ++y)
	for(u0_t x = 0; x < 3; ++x) {
		cnst u0_t iAlignment = (u0_t)(y * 3 + x);
		cnst ALIGN controlAlign = (ALIGN)(vertical[y] | horizontal[x]);
		cnst ALIGN textAlign = (ALIGN)(vertical[2 - y] | horizontal[2 - x]);
		if_fail_fe(::sampleGUIControlCreate(app, app.ControlsAlignment[iAlignment], get_value_namev(controlAlign), {{2, 2}, {114, 42}}, FONT_BODY, controlAlign));
		::getControls(app.Dialog).Text[app.ControlsAlignment[iAlignment]].Align = textAlign;
		if_fail_fe(controlSetParent(::getGUI(app.Dialog), app.ControlsAlignment[iAlignment], viewportAlignments->IdClient));
	}
	aobj<vcst_t> viewportTitles = {};
	if_fail_fe(viewportTitles.resize(app.ViewportIds.size()));
	for(u2_t iViewport = 0; iViewport < app.ViewportIds.size(); ++iViewport) {
		pobj<SDialogViewport> viewport = {};
		if_fail_fe(app.Dialog->Get(app.ViewportIds[iViewport], viewport));
		viewportTitles[iViewport] = ::getControls(app.Dialog).Text[viewport->IdTitle].Text;
	}
	if_fail_fe(controlListCreate(::getGUI(app.Dialog), app.ListViewports, view<vcst_c>{viewportTitles}, CID_INVALID, DIRECTION_RIGHT));
	::getControls(app.Dialog).Placement[app.ListViewports.IdControl].Area.Offset = {0, 0};
	::getControls(app.Dialog).Placement[app.ListViewports.IdControl].Align = ALIGN_TOP_LEFT;
	sttc cnst vcst_t actionText[] = {"SCREENSHOT", "EXIT"};
	if_fail_fe(controlListCreate(::getGUI(app.Dialog), app.ListExit, view<vcst_c>{actionText}, CID_INVALID, DIRECTION_RIGHT));
	::getControls(app.Dialog).Placement[app.ListExit.IdControl].Area.Offset = {0, 0};
	::getControls(app.Dialog).Placement[app.ListExit.IdControl].Align = ALIGN_TOP_RIGHT;
	app.ControlScreenshot = app.ListExit.IdControls[0];
	app.ControlExit = app.ListExit.IdControls[1];
	app.ControlsKeyboard[0] = app.ControlMode;
	app.ControlsKeyboard[1] = app.ControlGeometry;
	app.ControlsKeyboard[2] = app.ControlLive;
	app.ControlsKeyboard[3] = app.ControlExit;

	if_fail_fe(::sampleGUIGeometryModeApply(app));
	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorLive, app.ControlLive));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, CID_INVALID));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, CID_INVALID));
	rtrn 0;
}

err_t sampleGUIUpdate(SSampleGUI & app, gpk::b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	if(::getControls(app.Dialog).IsAction(app.ControlScreenshot))
		app.ScreenshotRequested = true;
	if(::getControls(app.Dialog).IsAction(app.ControlExit))
		rtrn APPLICATION_STATE_EXIT;
	if(::getControls(app.Dialog).IsAction(app.ControlMode))
		::getGUI(app.Dialog).ColorModeDefault = ::getGUI(app.Dialog).ColorModeDefault == GUI_COLOR_MODE_3D ? GUI_COLOR_MODE_Flat : GUI_COLOR_MODE_3D;
	if(::getControls(app.Dialog).IsAction(app.ControlGeometry))
		app.UseNewPalettes = !app.UseNewPalettes;
	for(u2_t iViewport = 0; iViewport < app.ViewportIds.size(); ++iViewport)
		if(::getControls(app.Dialog).IsAction(app.ListViewports.IdControls[iViewport])) {
			pobj<SDialogViewport> viewport = {};
			if_fail_fe(app.Dialog->Get(app.ViewportIds[iViewport], viewport));
			::getControls(app.Dialog).SetHidden(viewport->IdGUIControl, false);
		}
	cnst u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(::getControls(app.Dialog).IsAction(controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow})))
			::sampleGUIPaletteToneSelect(app, controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow}));
	for(u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(::getControls(app.Dialog).IsAction(controlGridControl(app.PalettePicker.GridShades, iShade)))
			::sampleGUIPaletteShadeSelect(app, iShade);
	for(u2_t iFont = 0; iFont < ::getGUI(app.Dialog).Fonts.size(); ++iFont)
		if(::getControls(app.Dialog).IsAction(controlGridControl(app.FontPicker.Grid, iFont)))
			::sampleGUIFontSelect(app, iFont);
	if(::getControls(app.Dialog).IsAction(app.ControlLive))
		::getControls(app.Dialog).SetSelected(app.ControlLive, false == ::getControls(app.Dialog).IsSelected(app.ControlLive));
	cnst cid_t idHovered = ::sampleGUIHoveredControl(app);
	cnst cid_t idFocused = ::sampleGUIFocusedControl(app);
	if_fail_fe(controlTextSet(::getGUI(app.Dialog), app.ControlMode, ::getGUI(app.Dialog).ColorModeDefault == GUI_COLOR_MODE_3D ? vcst_t{"Mode: 3D"} : vcst_t{"Mode: Flat"}));
	if_fail_fe(::sampleGUIGeometryModeApply(app));
	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorLive, app.ControlLive));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, idHovered));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, idFocused));
	rtrn app.Dialog->Update();
}

err_t sampleGUIDraw(SSampleGUI & app, g8bgra target) {
	if_fail_fe(target.fill(SColorBGRA{16, 14, 12}));
	if_fail_fe(guiDraw(::getGUI(app.Dialog), target));
	if(app.ScreenshotRequested) {
		app.ScreenshotRequested = false;
		au0_t pngBytes = {};
		if_fail_fe(pngFileWrite(target, pngBytes));
		if_fail_fe(fileFromMemory("test_gpk_gui.png", pngBytes));
	}
	rtrn 0;
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI palettes, states and alignment"

stct SApplication {
	SFramework	Framework;
	SSampleGUI			Sample;

	SApplication(SRuntimeValues & runtimeValues)
		: Framework(runtimeValues, {}), Sample(Framework.GUI) {}
};

err_t setup(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 540};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	rtrn ::sampleGUISetup(app.Sample, window.Input);
}

err_t cleanup(SApplication & app) {
	rtrn mainWindowDestroy(app.Framework.RootWindow);
}

err_t update(SApplication & app, gpk::b8_t systemRequestedExit) {
	if(systemRequestedExit)
		rtrn APPLICATION_STATE_EXIT;
	cnst err_t result = updateFramework(app.Framework);
	if(result)
		rtrn result;
	cnst SWindow & window = app.Framework.RootWindow;
	cnst err_t resultSample = ::sampleGUIUpdate(app.Sample, window.Input->KeyDown(SYSTEM_KEY_Escape));
	sleep(1);
	rtrn resultSample;
}

err_t draw(SApplication & app) {
	SWindow & window = app.Framework.RootWindow;
	if(window.NoDraw)
		rtrn 0;
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	rtrn ::sampleGUIDraw(app.Sample, window.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, GPK_SAMPLE_GUI_TITLE)
