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

stxp ::gpk::u2_t PALETTE_SHADE_COUNT = 16;
stxp ::gpk::u2_t PALETTE_TONE_COLUMN_COUNT = 2;
stxp ::gpk::u2_t PALETTE_TONE_COLUMN_SIZE = 8;
stxp ::gpk::s1_t FONT_BODY = 6; // 8x12
stxp ::gpk::s1_t FONT_TITLE = 7; // 8x16

GDEFINE_ENUM_TYPE(SAMPLE_GUI_INSPECTOR_FIELD, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ID     , 0);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, STATE  , 1);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, PALETTE, 2);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ALIGN  , 3);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, TEXT   , 4);

sttc ::gpk::err_t sampleGUIControlCreate(SSampleGUI & app, ::gpk::cid_t & idControl, ::gpk::vcst_t text, cnst ::gpk::rect2s1_t & area, ::gpk::s1_t font = -1, ::gpk::ALIGN align = ::gpk::ALIGN_TOP_LEFT) {
	if_fail_fe(idControl = ::gpk::controlCreate(*app.GUIHandle));
	::gpk::SControlPlacement & placement = app.GUIHandle->Controls.Placement[idControl];
	placement.Area = area;
	placement.Align = align;
	placement.Margin = {2, 2, 2, 2};
	placement.Border = {3, 3, 3, 3};
	::gpk::SControlDraw & draw = app.GUIHandle->Controls.Draw[idControl];
	draw.ColorMode = ::gpk::GUI_COLOR_MODE_Default;
	app.GUIHandle->Controls.Text[idControl].Text = text;
	app.GUIHandle->Controls.Text[idControl].FontSelected = font;
	rtrn 0;
}

sttc ::gpk::err_t sampleGUIGeometryModeApply(SSampleGUI & app) {
	for(::gpk::u2_t iControl = 0; iControl < app.GUIHandle->Controls.Draw.size(); ++iControl)
		app.GUIHandle->Controls.Draw[iControl].UseNewPalettes = app.UseNewPalettes;
	rtrn ::gpk::controlTextSet(*app.GUIHandle, app.ControlGeometry, app.UseNewPalettes ? ::gpk::vcst_t{"Geometry: Areas"} : ::gpk::vcst_t{"Geometry: Indexed"});
}

sttc void sampleGUIStateCardsApply(SSampleGUI & app) nxpt {
	cnst ::gpk::enum_definition<::gpk::GUI_CONTROL_FLAG> & stateDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_FLAG>();
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst ::gpk::GUI_CONTROL_FLAG stateFlag = stateDefinition.Values[iState];
		if(stateFlag == ::gpk::GUI_CONTROL_FLAG_Unused)
			continue;
		::gpk::SControlState & state = app.GUIHandle->Controls.States[::gpk::controlGridControl(app.GridStates, {1, iState})];
		cnst ::gpk::GUI_CONTROL_FLAG updated = (::gpk::GUI_CONTROL_FLAG)(state.Flag & ::gpk::GUI_CONTROL_FLAG_Updated);
		state.Flag = (::gpk::GUI_CONTROL_FLAG)(updated | stateFlag);
	}
}

sttc ::gpk::u2_t sampleGUIPaletteToneCount(cnst SSampleGUI & app) nxpt {
	if(!app.GUIHandle->Colors || !app.GUIHandle->Colors->ControlThemes)
		rtrn 0;
	rtrn app.GUIHandle->Colors->ControlThemes->size() / PALETTE_SHADE_COUNT;
}

sttc ::gpk::u2_t sampleGUIPaletteToneRowCount(cnst ::gpk::u2_t toneCount, cnst ::gpk::u2_t iColumn) nxpt {
	cnst ::gpk::u2_t completeGroupRows = toneCount / PALETTE_SHADE_COUNT * PALETTE_TONE_COLUMN_SIZE;
	cnst ::gpk::u2_t remainingTones = toneCount % PALETTE_SHADE_COUNT;
	rtrn completeGroupRows + (remainingTones + 1 - iColumn) / PALETTE_TONE_COLUMN_COUNT;
}

sttc ::gpk::u2_t sampleGUIPaletteToneFromCell(cnst ::gpk::n2u2_t & cell, cnst ::gpk::u2_t toneCount) nxpt {
	cnst ::gpk::u2_t completeGroups = toneCount / PALETTE_SHADE_COUNT;
	cnst ::gpk::u2_t completeGroupRows = completeGroups * PALETTE_TONE_COLUMN_SIZE;
	if(cell.y >= completeGroupRows)
		rtrn completeGroups * PALETTE_SHADE_COUNT + (cell.y - completeGroupRows) * PALETTE_TONE_COLUMN_COUNT + cell.x;
	cnst ::gpk::u2_t toneGroup = cell.y / PALETTE_TONE_COLUMN_SIZE;
	cnst ::gpk::u2_t toneRow = cell.y % PALETTE_TONE_COLUMN_SIZE;
	rtrn toneGroup * PALETTE_SHADE_COUNT + toneRow + cell.x * PALETTE_TONE_COLUMN_SIZE;
}

sttc void sampleGUIPaletteToneSelect(SSampleGUI & app, cnst ::gpk::cid_t idSelected) nxpt {
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(::gpk::u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow});
		app.GUIHandle->Controls.SetSelected(idControl, idControl == idSelected);
	}
}

sttc ::gpk::u2_t sampleGUIPaletteToneSelected(cnst SSampleGUI & app, cnst ::gpk::u2_t toneCount) nxpt {
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(::gpk::u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(app.GUIHandle->Controls.IsSelected(::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow})))
			rtrn ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount);
	rtrn app.GUIHandle->ThemeDefault / PALETTE_SHADE_COUNT % toneCount;
}

sttc void sampleGUIPaletteShadeSelect(SSampleGUI & app, cnst ::gpk::u2_t selected) nxpt {
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		app.GUIHandle->Controls.SetSelected(::gpk::controlGridControl(app.PalettePicker.GridShades, iShade), iShade == selected);
}

sttc ::gpk::u2_t sampleGUIPaletteShadeSelected(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(app.GUIHandle->Controls.IsSelected(::gpk::controlGridControl(app.PalettePicker.GridShades, iShade)))
			rtrn iShade;
	rtrn app.GUIHandle->ThemeDefault % PALETTE_SHADE_COUNT;
}

sttc ::gpk::err_t sampleGUIPalettePickerApply(SSampleGUI & app) {
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if(0 == toneCount)
		rtrn 0;
	cnst ::gpk::u2_t tone = ::sampleGUIPaletteToneSelected(app, toneCount);
	cnst ::gpk::u2_t shade = ::sampleGUIPaletteShadeSelected(app);
	app.GUIHandle->ThemeDefault = (::gpk::u1_t)(tone * PALETTE_SHADE_COUNT + shade);
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(::gpk::u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst ::gpk::u2_t iTone = ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount);
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow});
		app.GUIHandle->Controls.Draw[idControl].ColorTheme = (::gpk::s1_t)(iTone * PALETTE_SHADE_COUNT + iShade + 1);
	}
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridShades, iShade);
		app.GUIHandle->Controls.Draw[idControl].ColorTheme = (::gpk::s1_t)(tone * PALETTE_SHADE_COUNT + iShade + 1);
	}
	rtrn 0;
}

sttc void sampleGUIFontSelect(SSampleGUI & app, cnst ::gpk::u2_t selected) nxpt {
	for(::gpk::u2_t iFont = 0; iFont < app.GUIHandle->Fonts.size(); ++iFont)
		app.GUIHandle->Controls.SetSelected(::gpk::controlGridControl(app.FontPicker.Grid, iFont), iFont == selected);
}

sttc ::gpk::u2_t sampleGUIFontSelected(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iFont = 0; iFont < app.GUIHandle->Fonts.size(); ++iFont)
		if(app.GUIHandle->Controls.IsSelected(::gpk::controlGridControl(app.FontPicker.Grid, iFont)))
			rtrn iFont;
	rtrn app.GUIHandle->SelectedFont % app.GUIHandle->Fonts.size();
}

sttc void sampleGUIFontPickerApply(SSampleGUI & app) nxpt {
	if(0 == app.GUIHandle->Fonts.size())
		rtrn;
	app.GUIHandle->SelectedFont = (::gpk::u1_t)::sampleGUIFontSelected(app);
	for(::gpk::u2_t iFont = 0; iFont < app.GUIHandle->Fonts.size(); ++iFont) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.FontPicker.Grid, iFont);
		::gpk::controlFontSet(*app.GUIHandle, idControl, (::gpk::s1_t)iFont);
	}
}

sttc ::gpk::err_t sampleGUIStateFlagAppend(::gpk::string & text, ::gpk::vcst_t flag) {
	if(text.size())
		if_fail_fe(text.push_back('+'));
	rtrn ::gpk::append_string(text, flag);
}

sttc ::gpk::err_t sampleGUIStateDescribe(cnst ::gpk::SControlState & state, bool disabled, ::gpk::string & text) {
	text.clear();
	cnst ::gpk::enum_definition<::gpk::GUI_CONTROL_FLAG> & stateDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_FLAG>();
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		if(::gpk::bit_test_masked(state.Flag, state.Mask, stateDefinition.Values[iState]))
			if_fail_fe(::sampleGUIStateFlagAppend(text, stateDefinition.Names[iState]));
	if(disabled && false == state.IsDisabled())
		if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(::gpk::GUI_CONTROL_FLAG_Disabled)));
	if(0 == text.size())
		if_fail_fe(::gpk::append_string(text, "-"));
	rtrn text.size();
}

sttc ::gpk::err_t sampleGUIControlTextSetOwned(SSampleGUI & app, ::gpk::cid_t idControl, ::gpk::astsc_t<512> & storage, ::gpk::vcst_t text) {
	if(app.GUIHandle->Controls.Text[idControl].Text == text)
		rtrn 0;
	if_true_fe(text.size() >= storage.size());
	for(::gpk::u2_t iCharacter = 0; iCharacter < text.size(); ++iCharacter)
		storage.Storage[iCharacter] = text[iCharacter];
	storage.Storage[text.size()] = 0;
	rtrn ::gpk::controlTextSet(*app.GUIHandle, idControl, {storage.Storage, text.size()});
}

sttc ::gpk::cid_t sampleGUIHoveredControl(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < app.GUIHandle->Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(*app.GUIHandle, (::gpk::cid_t)iControl) && app.GUIHandle->Controls.IsHovered((::gpk::cid_t)iControl))
			rtrn (::gpk::cid_t)iControl;
	rtrn ::gpk::CID_INVALID;
}

sttc ::gpk::cid_t sampleGUIFocusedControl(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < app.GUIHandle->Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(*app.GUIHandle, (::gpk::cid_t)iControl) && app.GUIHandle->Controls.IsFocused((::gpk::cid_t)iControl))
			rtrn (::gpk::cid_t)iControl;
	rtrn ::gpk::CID_INVALID;
}

sttc ::gpk::err_t sampleGUIInspectorCreate(SSampleGUI & app, SSampleGUIInspector & inspector, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area, ::gpk::cid_t idParent) {
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdRoot, "", area, -1));
	if_fail_fe(::gpk::controlSetParent(*app.GUIHandle, inspector.IdRoot, idParent));
	cnst ::gpk::s1_t clientWidth = (::gpk::s1_t)(area.Size.x - ::gpk::controlNCSpacing(app.GUIHandle->Controls.Placement[inspector.IdRoot]).x);
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdTitle, title, {{0, 0}, {clientWidth, 30}}, FONT_BODY));
	if_fail_fe(::gpk::controlSetParent(*app.GUIHandle, inspector.IdTitle, inspector.IdRoot));
	::gpk::enum_definition<SAMPLE_GUI_INSPECTOR_FIELD> & fields = ::gpk::get_enum<SAMPLE_GUI_INSPECTOR_FIELD>();
	if_fail_fe(::gpk::controlListCreate(*app.GUIHandle, inspector.Names, {fields.Names}, inspector.IdRoot, ::gpk::DIRECTION_DOWN));
	sttc cnst ::gpk::vcst_t emptyValues[] = {"NONE", "-", "-", "-", "-"};
	if_fail_fe(::gpk::controlListCreate(*app.GUIHandle, inspector.Values, {emptyValues}, inspector.IdRoot, ::gpk::DIRECTION_DOWN));
	app.GUIHandle->Controls.Placement[inspector.Names.IdControl].Area = {{0, 34}, {88, 90}};
	app.GUIHandle->Controls.Placement[inspector.Values.IdControl].Area = {{88, 34}, {(::gpk::s1_t)(clientWidth - 88), 90}};
	for(::gpk::u0_t iControl = 0; iControl < fields.Names.size(); ++iControl) {
		app.GUIHandle->Controls.Text[inspector.Names.IdControls[iControl]].Align = ::gpk::ALIGN_CENTER_RIGHT;
		app.GUIHandle->Controls.Text[inspector.Values.IdControls[iControl]].Align = ::gpk::ALIGN_CENTER_LEFT;
	}
	rtrn 0;
}

sttc ::gpk::err_t sampleGUIInspectorValueSet(SSampleGUI & app, SSampleGUIInspector & inspector, SAMPLE_GUI_INSPECTOR_FIELD field, ::gpk::vcst_t value) {
	rtrn ::sampleGUIControlTextSetOwned(app, inspector.Values.IdControls[field], inspector.TextValues[field], value);
}

sttc ::gpk::err_t sampleGUIInspectorUpdate(SSampleGUI & app, SSampleGUIInspector & inspector, ::gpk::cid_t idControl) {
	if(::gpk::controlInvalid(*app.GUIHandle, idControl)) {
		if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, "NONE"));
		for(::gpk::u0_t iField = SAMPLE_GUI_INSPECTOR_FIELD_STATE; iField <= SAMPLE_GUI_INSPECTOR_FIELD_TEXT; ++iField)
			if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, (SAMPLE_GUI_INSPECTOR_FIELD)iField, "-"));
		rtrn 0;
	}
	cnst ::gpk::SControlState & state = app.GUIHandle->Controls.States[idControl];
	cnst bool disabled = 0 != ::gpk::controlDisabled(*app.GUIHandle, idControl);
	::gpk::string stateText = {};
	if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
	cnst ::gpk::GUI_CONTROL_PALETTE palette = ::gpk::paletteIndexFromState(disabled, state);
	cnst ::gpk::vcst_t paletteName = ::gpk::get_value_namev(palette);
	cnst ::gpk::s1_t paletteSlot = app.GUIHandle->Controls.Draw[idControl].Palettes[palette];
	::gpk::astsc_t<512> text = {};
	::gpk::err_t length = ::gpk::sprintf_s(text.Storage, "#%i", idControl);
	if_fail_fe(length);
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, {text.Storage, (::gpk::u2_t)length}));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_STATE, stateText));
	if_fail_fe(length = ::gpk::sprintf_s(text.Storage, "%.*s [%i]", (int)paletteName.size(), paletteName.begin(), paletteSlot));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_PALETTE, {text.Storage, (::gpk::u2_t)length}));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ALIGN, ::gpk::get_value_namev(app.GUIHandle->Controls.Placement[idControl].Align)));
	rtrn ::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_TEXT, ::gpk::get_value_namev(app.GUIHandle->Controls.Text[idControl].Align));
}

sttc ::gpk::err_t sampleGUIViewportCreate(SSampleGUI & app, ::gpk::cid_t & idViewport, ::gpk::pobj<::gpk::SDialogViewport> & viewport, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area) {
	if_fail_fe(idViewport = ::gpk::viewportCreate(*app.Dialog, viewport));
	if_fail_fe(app.ViewportIds.push_back(idViewport));
	::gpk::SControlTable & controls = app.GUIHandle->Controls;
	controls.Draw[viewport->IdGUIControl      ].UseNewPalettes = false;
	controls.Draw[viewport->IdClose           ].UseNewPalettes = false;
	controls.Draw[viewport->IdResizeBottomRight].UseNewPalettes = false;
	::gpk::SControlPlacement & root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Text[viewport->IdTitle].Text = title;
	controls.Text[viewport->IdTitle].FontSelected = FONT_BODY;
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(::gpk::s1_t)(area.Size.x - 8), (::gpk::s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn ::gpk::controlMetricsInvalidate(*app.GUIHandle, viewport->IdGUIControl);
}

::gpk::err_t sampleGUISetup(SSampleGUI & app, ::gpk::pobj<::gpk::SInput> & input) {
	if(!app.Dialog)
		if(0 == app.Dialog.create(app.GUIHandle))
			rtrn ::gpk::OS_NO_MEMORY;
	app.Dialog->Input = input;
	app.GUIHandle->SelectedFont = FONT_BODY;
	app.GUIHandle->ColorModeDefault = ::gpk::GUI_COLOR_MODE_3D;
	::gpk::pobj<::gpk::SDialogViewport> viewportPalette = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.PalettePicker.IdViewport, viewportPalette, "PALETTE", {{20, 18}, {300, 220}}));
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if_true_fe(0 == toneCount);
	cnst ::gpk::u2_t toneDefault = toneCount >= 3 ? toneCount - 3 : 0;
	cnst ::gpk::u2_t shadeDefault = 8;
	::gpk::aobj<::gpk::vcst_t> toneTexts[PALETTE_TONE_COLUMN_COUNT] = {};
	::gpk::aobj<::gpk::vcst_t> shadeTexts = {};
	if_fail_fe(shadeTexts.resize(PALETTE_SHADE_COUNT));
	if_fail_fe(::gpk::controlGridCreate(*app.GUIHandle, app.PalettePicker.GridShades, ::gpk::view<::gpk::vcst_c>{shadeTexts}, viewportPalette->IdClient, ::gpk::DIRECTION_RIGHT, 8, {16, 16}, {1, 1}, {78, 4}));
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn) {
		if_fail_fe(toneTexts[iColumn].resize(::sampleGUIPaletteToneRowCount(toneCount, iColumn) * PALETTE_SHADE_COUNT));
		if_fail_fe(::gpk::controlGridCreate(*app.GUIHandle, app.PalettePicker.GridTones[iColumn], ::gpk::view<::gpk::vcst_c>{toneTexts[iColumn]}, viewportPalette->IdClient, ::gpk::DIRECTION_RIGHT, PALETTE_SHADE_COUNT, {8, 5}, {}, {(::gpk::s1_t)(16 + iColumn * 132), 40}));
		for(::gpk::u2_t iColor = 0; iColor < toneTexts[iColumn].size(); ++iColor) {
			cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], iColor);
			app.GUIHandle->Controls.Placement[idControl].Border = {};
			app.GUIHandle->Controls.Placement[idControl].Margin = {};
			app.GUIHandle->Controls.Draw[idControl].NoBorder = true;
		}
	}
	::sampleGUIPaletteShadeSelect(app, shadeDefault);
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(::gpk::u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
		if(toneDefault == ::sampleGUIPaletteToneFromCell({iColumn, iRow}, toneCount))
			::sampleGUIPaletteToneSelect(app, ::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {shadeDefault, iRow}));
	::gpk::pobj<::gpk::SDialogViewport> viewportFonts = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.FontPicker.IdViewport, viewportFonts, "FONTS", {{332, 18}, {252, 264}}));
	if_fail_fe(app.FontPicker.Labels.resize(app.GUIHandle->Fonts.size()));
	for(::gpk::u2_t iFont = 0; iFont < app.GUIHandle->Fonts.size(); ++iFont) {
		cnst ::gpk::n2u0_t charSize = app.GUIHandle->Fonts[iFont]->CharSize;
		if_fail_fe(::gpk::append_strings(app.FontPicker.Labels[iFont], ::gpk::str(charSize.x), "x", ::gpk::str(charSize.y)));
	}
	if_fail_fe(::gpk::controlGridCreate(*app.GUIHandle, app.FontPicker.Grid, ::gpk::view<::gpk::string>{app.FontPicker.Labels}, viewportFonts->IdClient, ::gpk::DIRECTION_RIGHT, 2, {120, 28}, {4, 6}));
	::sampleGUIFontSelect(app, app.GUIHandle->SelectedFont);
	cnst ::gpk::enum_definition<::gpk::GUI_CONTROL_FLAG> & stateDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_FLAG>();
	::gpk::pobj<::gpk::SDialogViewport> viewportStates = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportStates, viewportStates, "CONTROL STATES", {{20, 242}, {176, (::gpk::s1_t)(42 + stateDefinition.Values.size() * 18)}}));

	if_fail_fe(::sampleGUIControlCreate(app, app.ControlMode, "Mode: 3D", {{4, 136}, {134, 42}}, FONT_BODY));
	if_fail_fe(::gpk::controlSetParent(*app.GUIHandle, app.ControlMode, viewportPalette->IdClient));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlGeometry, "Geometry: Indexed", {{142, 136}, {146, 42}}, FONT_BODY));
	if_fail_fe(::gpk::controlSetParent(*app.GUIHandle, app.ControlGeometry, viewportPalette->IdClient));
	::gpk::aobj<::gpk::vcst_t> stateCells = {};
	if_fail_fe(stateCells.resize(stateDefinition.Values.size() * 2));
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		stateCells[iState] = stateDefinition.Names[iState];
		stateCells[stateDefinition.Values.size() + iState] = stateDefinition.Names[iState];
	}
	if_fail_fe(::gpk::controlGridCreate(*app.GUIHandle, app.GridStates, ::gpk::view<::gpk::vcst_t>{stateCells}, viewportStates->IdClient, ::gpk::DIRECTION_DOWN, 2, {76, 18}, {0, 0}, {8, 6}));
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst ::gpk::cid_t idName = app.GridStates[0].IdControls[iState];
		cnst ::gpk::cid_t idState = app.GridStates[1].IdControls[iState];
		app.GUIHandle->Controls.Text[idName].FontSelected = app.GUIHandle->Controls.Text[idState].FontSelected = FONT_BODY;
		app.GUIHandle->Controls.Text[idName].Align = ::gpk::ALIGN_CENTER_RIGHT;
		app.GUIHandle->Controls.Text[idState].Align = ::gpk::ALIGN_CENTER_LEFT;
		app.GUIHandle->Controls.Draw[idName].ColorMode = app.GUIHandle->Controls.Draw[idState].ColorMode = ::gpk::GUI_COLOR_MODE_Default;
		app.GUIHandle->Controls.Draw[idState].UseNewPalettes = true;
	}
	::gpk::pobj<::gpk::SDialogViewport> viewportInspectors = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportInspectors, viewportInspectors, "INSPECTORS", {{384, 18}, {300, 456}}));
	app.GUIHandle->Controls.Placement[viewportInspectors->IdGUIControl].Align = ::gpk::ALIGN_TOP_RIGHT;
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorLive, "LIVE", {{4, 4}, {286, 134}}, viewportInspectors->IdClient));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorHovered, "HOVERED", {{4, 140}, {286, 134}}, viewportInspectors->IdClient));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorFocused, "FOCUSED", {{4, 276}, {286, 134}}, viewportInspectors->IdClient));
	app.ControlLive = app.InspectorLive.IdTitle;
	::gpk::pobj<::gpk::SDialogViewport> viewportAlignments = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportAlignments, viewportAlignments, "ALIGN", {{20, 18}, {372, 187}}));
	app.GUIHandle->Controls.Placement[viewportAlignments->IdGUIControl].Align = ::gpk::ALIGN_TOP_RIGHT;
	sttc cnst ::gpk::ALIGN horizontal[] = {::gpk::ALIGN_LEFT, ::gpk::ALIGN_HCENTER, ::gpk::ALIGN_RIGHT};
	sttc cnst ::gpk::ALIGN vertical  [] = {::gpk::ALIGN_TOP , ::gpk::ALIGN_VCENTER, ::gpk::ALIGN_BOTTOM};
	for(::gpk::u0_t y = 0; y < 3; ++y)
	for(::gpk::u0_t x = 0; x < 3; ++x) {
		cnst ::gpk::u0_t iAlignment = (::gpk::u0_t)(y * 3 + x);
		cnst ::gpk::ALIGN controlAlign = (::gpk::ALIGN)(vertical[y] | horizontal[x]);
		cnst ::gpk::ALIGN textAlign = (::gpk::ALIGN)(vertical[2 - y] | horizontal[2 - x]);
		if_fail_fe(::sampleGUIControlCreate(app, app.ControlsAlignment[iAlignment], ::gpk::get_value_namev(controlAlign), {{2, 2}, {114, 42}}, FONT_BODY, controlAlign));
		app.GUIHandle->Controls.Text[app.ControlsAlignment[iAlignment]].Align = textAlign;
		if_fail_fe(::gpk::controlSetParent(*app.GUIHandle, app.ControlsAlignment[iAlignment], viewportAlignments->IdClient));
	}
	::gpk::aobj<::gpk::vcst_t> viewportTitles = {};
	if_fail_fe(viewportTitles.resize(app.ViewportIds.size()));
	for(::gpk::u2_t iViewport = 0; iViewport < app.ViewportIds.size(); ++iViewport) {
		::gpk::pobj<::gpk::SDialogViewport> viewport = {};
		if_fail_fe(app.Dialog->Get(app.ViewportIds[iViewport], viewport));
		viewportTitles[iViewport] = app.GUIHandle->Controls.Text[viewport->IdTitle].Text;
	}
	if_fail_fe(::gpk::controlListCreate(*app.GUIHandle, app.ListViewports, ::gpk::view<::gpk::vcst_c>{viewportTitles}, ::gpk::CID_INVALID, ::gpk::DIRECTION_RIGHT));
	app.GUIHandle->Controls.Placement[app.ListViewports.IdControl].Area.Offset = {0, 0};
	app.GUIHandle->Controls.Placement[app.ListViewports.IdControl].Align = ::gpk::ALIGN_TOP_LEFT;
	sttc cnst ::gpk::vcst_t actionText[] = {"SCREENSHOT", "EXIT"};
	if_fail_fe(::gpk::controlListCreate(*app.GUIHandle, app.ListExit, ::gpk::view<::gpk::vcst_c>{actionText}, ::gpk::CID_INVALID, ::gpk::DIRECTION_RIGHT));
	app.GUIHandle->Controls.Placement[app.ListExit.IdControl].Area.Offset = {0, 0};
	app.GUIHandle->Controls.Placement[app.ListExit.IdControl].Align = ::gpk::ALIGN_TOP_RIGHT;
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
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, ::gpk::CID_INVALID));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, ::gpk::CID_INVALID));
	rtrn 0;
}

::gpk::err_t sampleGUIUpdate(SSampleGUI & app, bool systemRequestedExit) {
	if(systemRequestedExit)
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUIHandle->Controls.IsAction(app.ControlScreenshot))
		app.ScreenshotRequested = true;
	if(app.GUIHandle->Controls.IsAction(app.ControlExit))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUIHandle->Controls.IsAction(app.ControlMode))
		app.GUIHandle->ColorModeDefault = app.GUIHandle->ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::GUI_COLOR_MODE_Flat : ::gpk::GUI_COLOR_MODE_3D;
	if(app.GUIHandle->Controls.IsAction(app.ControlGeometry))
		app.UseNewPalettes = !app.UseNewPalettes;
	for(::gpk::u2_t iViewport = 0; iViewport < app.ViewportIds.size(); ++iViewport)
		if(app.GUIHandle->Controls.IsAction(app.ListViewports.IdControls[iViewport])) {
			::gpk::pobj<::gpk::SDialogViewport> viewport = {};
			if_fail_fe(app.Dialog->Get(app.ViewportIds[iViewport], viewport));
			app.GUIHandle->Controls.SetHidden(viewport->IdGUIControl, false);
		}
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(::gpk::u2_t iColumn = 0; iColumn < PALETTE_TONE_COLUMN_COUNT; ++iColumn)
	for(::gpk::u2_t iRow = 0; iRow < ::sampleGUIPaletteToneRowCount(toneCount, iColumn); ++iRow)
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(app.GUIHandle->Controls.IsAction(::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow})))
			::sampleGUIPaletteToneSelect(app, ::gpk::controlGridControl(app.PalettePicker.GridTones[iColumn], {iShade, iRow}));
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(app.GUIHandle->Controls.IsAction(::gpk::controlGridControl(app.PalettePicker.GridShades, iShade)))
			::sampleGUIPaletteShadeSelect(app, iShade);
	for(::gpk::u2_t iFont = 0; iFont < app.GUIHandle->Fonts.size(); ++iFont)
		if(app.GUIHandle->Controls.IsAction(::gpk::controlGridControl(app.FontPicker.Grid, iFont)))
			::sampleGUIFontSelect(app, iFont);
	if(app.GUIHandle->Controls.IsAction(app.ControlLive))
		app.GUIHandle->Controls.SetSelected(app.ControlLive, false == app.GUIHandle->Controls.IsSelected(app.ControlLive));
	cnst ::gpk::cid_t idHovered = ::sampleGUIHoveredControl(app);
	cnst ::gpk::cid_t idFocused = ::sampleGUIFocusedControl(app);
	if_fail_fe(::gpk::controlTextSet(*app.GUIHandle, app.ControlMode, app.GUIHandle->ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::vcst_t{"Mode: 3D"} : ::gpk::vcst_t{"Mode: Flat"}));
	if_fail_fe(::sampleGUIGeometryModeApply(app));
	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorLive, app.ControlLive));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, idHovered));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, idFocused));
	rtrn app.Dialog->Update();
}

::gpk::err_t sampleGUIDraw(SSampleGUI & app, ::gpk::g8bgra target) {
	if_fail_fe(target.fill(::gpk::SColorBGRA{16, 14, 12}));
	if_fail_fe(::gpk::guiDraw(*app.GUIHandle, target));
	if(app.ScreenshotRequested) {
		app.ScreenshotRequested = false;
		::gpk::au0_t pngBytes = {};
		if_fail_fe(::gpk::pngFileWrite(target, pngBytes));
		if_fail_fe(::gpk::fileFromMemory("test_gpk_gui.png", pngBytes));
	}
	rtrn 0;
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI palettes, states and alignment"

stct SApplication {
	::gpk::SFramework	Framework;
	SSampleGUI			Sample;

	SApplication(::gpk::SRuntimeValues & runtimeValues)
		: Framework(runtimeValues, {}), Sample(Framework.GUI) {}
};

::gpk::err_t setup(SApplication & app) {
	::gpk::SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 540};
	if_null_ve(::gpk::OS_NO_MEMORY, window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(::gpk::mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	rtrn ::sampleGUISetup(app.Sample, window.Input);
}

::gpk::err_t cleanup(SApplication & app) {
	rtrn ::gpk::mainWindowDestroy(app.Framework.RootWindow);
}

::gpk::err_t update(SApplication & app, bool systemRequestedExit) {
	if(systemRequestedExit)
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	cnst ::gpk::err_t result = ::gpk::updateFramework(app.Framework);
	if(result)
		rtrn result;
	cnst ::gpk::SWindow & window = app.Framework.RootWindow;
	cnst ::gpk::err_t resultSample = ::sampleGUIUpdate(app.Sample, window.Input->KeyDown(::gpk::SYSTEM_KEY_Escape));
	::gpk::sleep(1);
	rtrn resultSample;
}

::gpk::err_t draw(SApplication & app) {
	::gpk::SWindow & window = app.Framework.RootWindow;
	if(window.NoDraw)
		rtrn 0;
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	rtrn ::sampleGUIDraw(app.Sample, window.BackBuffer->Color.View);
}

GPK_DEFINE_APPLICATION_ENTRY_POINT(SApplication, GPK_SAMPLE_GUI_TITLE)
