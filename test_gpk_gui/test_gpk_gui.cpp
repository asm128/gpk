#include "test_gpk_gui.h"
#include "gpk_app_impl.h"
#include "gpk_cstring.h"
#include "gpk_framework.h"
#include "gpk_dialog_controls.h"
#include "gpk_label.h"
#include "gpk_runtime.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"
#include "gpk_system_key.h"

stxp ::gpk::u2_t PALETTE_SHADE_COUNT = 16;
stxp ::gpk::u2_t PALETTE_TONE_COLUMNS = 9;
stxp ::gpk::s1_t FONT_BODY = 6; // 8x12
stxp ::gpk::s1_t FONT_TITLE = 7; // 8x16

GDEFINE_ENUM_TYPE(SAMPLE_GUI_INSPECTOR_FIELD, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ID     , 0);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, STATE  , 1);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, PALETTE, 2);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, ALIGN  , 3);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_INSPECTOR_FIELD, TEXT   , 4);

sttc ::gpk::err_t sampleGUIControlCreate(SSampleGUI & app, ::gpk::cid_t & idControl, ::gpk::vcst_t text, cnst ::gpk::rect2s1_t & area, ::gpk::s1_t font = -1, ::gpk::ALIGN align = ::gpk::ALIGN_TOP_LEFT) {
	if_fail_fe(idControl = ::gpk::controlCreate(app.GUI));
	::gpk::SControlPlacement & placement = app.GUI.Controls.Placement[idControl];
	placement.Area = area;
	placement.Align = align;
	placement.Margin = {2, 2, 2, 2};
	placement.Border = {3, 3, 3, 3};
	::gpk::SControlDraw & draw = app.GUI.Controls.Draw[idControl];
	draw.ColorMode = ::gpk::GUI_COLOR_MODE_Default;
	app.GUI.Controls.Text[idControl].Text = text;
	app.GUI.Controls.Text[idControl].FontSelected = font;
	rtrn 0;
}

sttc void sampleGUIStateCardsApply(SSampleGUI & app) nxpt {
	cnst ::gpk::enum_definition<::gpk::GUI_CONTROL_FLAG> & stateDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_FLAG>();
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst ::gpk::GUI_CONTROL_FLAG stateFlag = stateDefinition.Values[iState];
		if(stateFlag == ::gpk::GUI_CONTROL_FLAG_Unused)
			continue;
		::gpk::SControlState & state = app.GUI.Controls.States[::gpk::controlGridControl(app.GridStates, {1, iState})];
		cnst ::gpk::GUI_CONTROL_FLAG updated = (::gpk::GUI_CONTROL_FLAG)(state.Flag & ::gpk::GUI_CONTROL_FLAG_Updated);
		state.Flag = (::gpk::GUI_CONTROL_FLAG)(updated | stateFlag);
	}
}

sttc ::gpk::u2_t sampleGUIPaletteToneCount(cnst SSampleGUI & app) nxpt {
	if(!app.GUI.Colors || !app.GUI.Colors->ControlThemes)
		rtrn 0;
	rtrn app.GUI.Colors->ControlThemes->size() / PALETTE_SHADE_COUNT;
}

sttc ::gpk::err_t sampleGUIPalettePickerApply(SSampleGUI & app) {
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if(0 == toneCount)
		rtrn 0;
	app.PalettePicker.Tone %= toneCount;
	app.PalettePicker.Shade %= PALETTE_SHADE_COUNT;
	app.GUI.ThemeDefault = (::gpk::u1_t)(app.PalettePicker.Tone * PALETTE_SHADE_COUNT + app.PalettePicker.Shade);
	for(::gpk::u2_t iTone = 0; iTone < toneCount; ++iTone) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridTones, iTone);
		app.GUI.Controls.Draw[idControl].ColorTheme = (::gpk::s1_t)(iTone * PALETTE_SHADE_COUNT + app.PalettePicker.Shade + 1);
		app.GUI.Controls.SetSelected(idControl, iTone == app.PalettePicker.Tone);
	}
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.PalettePicker.GridShades, iShade);
		app.GUI.Controls.Draw[idControl].ColorTheme = (::gpk::s1_t)(app.PalettePicker.Tone * PALETTE_SHADE_COUNT + iShade + 1);
		app.GUI.Controls.SetSelected(idControl, iShade == app.PalettePicker.Shade);
	}
	rtrn 0;
}

sttc void sampleGUIFontPickerApply(SSampleGUI & app) nxpt {
	if(0 == app.GUI.Fonts.size())
		rtrn;
	app.FontPicker.Selected %= app.GUI.Fonts.size();
	app.GUI.SelectedFont = (::gpk::u1_t)app.FontPicker.Selected;
	for(::gpk::u2_t iFont = 0; iFont < app.GUI.Fonts.size(); ++iFont) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.FontPicker.Grid, iFont);
		::gpk::controlFontSet(app.GUI, idControl, (::gpk::s1_t)iFont);
		app.GUI.Controls.SetSelected(idControl, iFont == app.FontPicker.Selected);
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
	if(app.GUI.Controls.Text[idControl].Text == text)
		rtrn 0;
	if_true_fe(text.size() >= storage.size());
	for(::gpk::u2_t iCharacter = 0; iCharacter < text.size(); ++iCharacter)
		storage.Storage[iCharacter] = text[iCharacter];
	storage.Storage[text.size()] = 0;
	rtrn ::gpk::controlTextSet(app.GUI, idControl, {storage.Storage, text.size()});
}

sttc ::gpk::cid_t sampleGUIHoveredControl(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < app.GUI.Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(app.GUI, (::gpk::cid_t)iControl) && app.GUI.Controls.IsHovered((::gpk::cid_t)iControl))
			rtrn (::gpk::cid_t)iControl;
	rtrn ::gpk::CID_INVALID;
}

sttc ::gpk::cid_t sampleGUIFocusedControl(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < app.GUI.Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(app.GUI, (::gpk::cid_t)iControl) && app.GUI.Controls.IsFocused((::gpk::cid_t)iControl))
			rtrn (::gpk::cid_t)iControl;
	rtrn ::gpk::CID_INVALID;
}

sttc ::gpk::err_t sampleGUIInspectorCreate(SSampleGUI & app, SSampleGUIInspector & inspector, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area) {
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdRoot, "", area, -1));
	app.GUI.Controls.Placement[inspector.IdRoot].Align = ::gpk::ALIGN_BOTTOM_LEFT;
	cnst ::gpk::s1_t clientWidth = (::gpk::s1_t)(area.Size.x - ::gpk::controlNCSpacing(app.GUI.Controls.Placement[inspector.IdRoot]).x);
	if_fail_fe(::sampleGUIControlCreate(app, inspector.IdTitle, title, {{0, 0}, {clientWidth, 30}}, FONT_BODY));
	if_fail_fe(::gpk::controlSetParent(app.GUI, inspector.IdTitle, inspector.IdRoot));
	::gpk::enum_definition<SAMPLE_GUI_INSPECTOR_FIELD> & fields = ::gpk::get_enum<SAMPLE_GUI_INSPECTOR_FIELD>();
	if_fail_fe(::gpk::controlListCreate(app.GUI, inspector.Names, {fields.Names}, inspector.IdRoot, ::gpk::DIRECTION_DOWN));
	sttc cnst ::gpk::vcst_t emptyValues[] = {"NONE", "-", "-", "-", "-"};
	if_fail_fe(::gpk::controlListCreate(app.GUI, inspector.Values, {emptyValues}, inspector.IdRoot, ::gpk::DIRECTION_DOWN));
	app.GUI.Controls.Placement[inspector.Names.IdControl].Area = {{0, 34}, {88, 90}};
	app.GUI.Controls.Placement[inspector.Values.IdControl].Area = {{88, 34}, {(::gpk::s1_t)(clientWidth - 88), 90}};
	for(::gpk::u0_t iControl = 0; iControl < fields.Names.size(); ++iControl) {
		app.GUI.Controls.Text[inspector.Names.IdControls[iControl]].Align = ::gpk::ALIGN_CENTER_RIGHT;
		app.GUI.Controls.Text[inspector.Values.IdControls[iControl]].Align = ::gpk::ALIGN_CENTER_LEFT;
	}
	rtrn 0;
}

sttc ::gpk::err_t sampleGUIInspectorValueSet(SSampleGUI & app, SSampleGUIInspector & inspector, SAMPLE_GUI_INSPECTOR_FIELD field, ::gpk::vcst_t value) {
	rtrn ::sampleGUIControlTextSetOwned(app, inspector.Values.IdControls[field], inspector.TextValues[field], value);
}

sttc ::gpk::err_t sampleGUIInspectorUpdate(SSampleGUI & app, SSampleGUIInspector & inspector, ::gpk::cid_t idControl) {
	if(::gpk::controlInvalid(app.GUI, idControl)) {
		if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, "NONE"));
		for(::gpk::u0_t iField = SAMPLE_GUI_INSPECTOR_FIELD_STATE; iField <= SAMPLE_GUI_INSPECTOR_FIELD_TEXT; ++iField)
			if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, (SAMPLE_GUI_INSPECTOR_FIELD)iField, "-"));
		rtrn 0;
	}
	cnst ::gpk::SControlState & state = app.GUI.Controls.States[idControl];
	cnst bool disabled = 0 != ::gpk::controlDisabled(app.GUI, idControl);
	::gpk::string stateText = {};
	if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
	cnst ::gpk::GUI_CONTROL_PALETTE palette = ::gpk::paletteIndexFromState(disabled, state);
	cnst ::gpk::vcst_t paletteName = ::gpk::get_value_namev(palette);
	cnst ::gpk::s1_t paletteSlot = app.GUI.Controls.Draw[idControl].Palettes[palette];
	::gpk::astsc_t<512> text = {};
	::gpk::err_t length = ::gpk::sprintf_s(text.Storage, "#%i", idControl);
	if_fail_fe(length);
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ID, {text.Storage, (::gpk::u2_t)length}));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_STATE, stateText));
	if_fail_fe(length = ::gpk::sprintf_s(text.Storage, "%.*s [%i]", (int)paletteName.size(), paletteName.begin(), paletteSlot));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_PALETTE, {text.Storage, (::gpk::u2_t)length}));
	if_fail_fe(::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_ALIGN, ::gpk::get_value_namev(app.GUI.Controls.Placement[idControl].Align)));
	rtrn ::sampleGUIInspectorValueSet(app, inspector, SAMPLE_GUI_INSPECTOR_FIELD_TEXT, ::gpk::get_value_namev(app.GUI.Controls.Text[idControl].Align));
}

sttc ::gpk::err_t sampleGUIViewportCreate(SSampleGUI & app, ::gpk::cid_t & idViewport, ::gpk::pobj<::gpk::SDialogViewport> & viewport, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area) {
	if(!app.Dialog) {
		if(0 == app.Dialog.create(app.GUIHandle))
			rtrn ::gpk::OS_NO_MEMORY;
		app.Dialog->Input = app.Window.Input;
	}
	if_fail_fe(idViewport = ::gpk::viewportCreate(*app.Dialog, viewport));
	::gpk::SControlTable & controls = app.GUI.Controls;
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
	rtrn ::gpk::controlMetricsInvalidate(app.GUI, viewport->IdGUIControl);
}

::gpk::err_t sampleGUISetup(SSampleGUI & app) {
	app.GUI.SelectedFont = FONT_BODY;
	app.GUI.ColorModeDefault = ::gpk::GUI_COLOR_MODE_3D;
	::gpk::pobj<::gpk::SDialogViewport> viewportPalette = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.PalettePicker.IdViewport, viewportPalette, "PALETTE", {{20, 18}, {300, 104}}));
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	if_true_fe(0 == toneCount);
	app.PalettePicker.Tone = toneCount >= 3 ? toneCount - 3 : 0;
	app.PalettePicker.Shade = 8;
	::gpk::aobj<::gpk::vcst_t> toneTexts = {};
	::gpk::aobj<::gpk::vcst_t> shadeTexts = {};
	if_fail_fe(toneTexts.resize(toneCount));
	if_fail_fe(shadeTexts.resize(PALETTE_SHADE_COUNT));
	if_fail_fe(::gpk::controlGridCreate(app.GUI, app.PalettePicker.GridTones, ::gpk::view<::gpk::vcst_c>{toneTexts}, viewportPalette->IdClient, ::gpk::DIRECTION_RIGHT, PALETTE_TONE_COLUMNS, {16, 16}, {1, 1}, {4, 4}));
	if_fail_fe(::gpk::controlGridCreate(app.GUI, app.PalettePicker.GridShades, ::gpk::view<::gpk::vcst_c>{shadeTexts}, viewportPalette->IdClient, ::gpk::DIRECTION_RIGHT, 8, {16, 16}, {1, 1}, {157, 4}));
	::gpk::pobj<::gpk::SDialogViewport> viewportFonts = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.FontPicker.IdViewport, viewportFonts, "FONTS", {{380, 18}, {340, 264}}));
	app.FontPicker.Selected = app.GUI.SelectedFont;
	if_fail_fe(app.FontPicker.Labels.resize(app.GUI.Fonts.size()));
	for(::gpk::u2_t iFont = 0; iFont < app.GUI.Fonts.size(); ++iFont) {
		cnst ::gpk::n2u0_t charSize = app.GUI.Fonts[iFont]->CharSize;
		if_fail_fe(::gpk::append_strings(app.FontPicker.Labels[iFont], charSize.x, "x", charSize.y));
	}
	if_fail_fe(::gpk::controlGridCreate(app.GUI, app.FontPicker.Grid, ::gpk::view<::gpk::string>{app.FontPicker.Labels}, viewportFonts->IdClient, ::gpk::DIRECTION_RIGHT, 2, {152, 28}, {4, 6}, {1, 0}));
	cnst ::gpk::enum_definition<::gpk::GUI_CONTROL_FLAG> & stateDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_FLAG>();
	::gpk::pobj<::gpk::SDialogViewport> viewportStates = {};
	if_fail_fe(::sampleGUIViewportCreate(app, app.ViewportStates, viewportStates, "CONTROL STATES", {{20, 120}, {360, (::gpk::s1_t)(42 + stateDefinition.Values.size() * 18)}}));

	if_fail_fe(::sampleGUIControlCreate(app, app.ControlMode, "Mode: 3D", {{720, 18}, {184, 42}}, FONT_BODY));
	::gpk::aobj<::gpk::vcst_t> stateCells = {};
	if_fail_fe(stateCells.resize(stateDefinition.Values.size() * 2));
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState)
		stateCells[iState] = stateDefinition.Names[iState];
	if_fail_fe(::gpk::controlGridCreate(app.GUI, app.GridStates, ::gpk::view<::gpk::vcst_t>{stateCells}, viewportStates->IdClient, ::gpk::DIRECTION_DOWN, 2, {168, 18}, {0, 0}, {8, 6}));
	for(::gpk::u2_t iState = 0; iState < stateDefinition.Values.size(); ++iState) {
		cnst ::gpk::cid_t idName = app.GridStates[0].IdControls[iState];
		cnst ::gpk::cid_t idState = app.GridStates[1].IdControls[iState];
		app.GUI.Controls.Text[idName].FontSelected = app.GUI.Controls.Text[idState].FontSelected = FONT_BODY;
		app.GUI.Controls.Text[idName].Align = ::gpk::ALIGN_CENTER_RIGHT;
		app.GUI.Controls.Text[idState].Align = ::gpk::ALIGN_CENTER_LEFT;
		app.GUI.Controls.Draw[idName].ColorMode = app.GUI.Controls.Draw[idState].ColorMode = ::gpk::GUI_COLOR_MODE_Default;
		app.GUI.Controls.Draw[idState].UseNewPalettes = true;
	}
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorLive, "LIVE", {{24, 68}, {286, 134}}));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorHovered, "HOVERED", {{323, 68}, {286, 134}}));
	if_fail_fe(::sampleGUIInspectorCreate(app, app.InspectorFocused, "FOCUSED", {{622, 68}, {286, 134}}));
	app.ControlLive = app.InspectorLive.IdTitle;
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlHelp, "TAB FOCUS | ENTER/SPACE ACTIVATE | ESC/EXIT CLOSE", {{136, 18}, {772, 36}}, FONT_BODY, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlExit, "EXIT", {{24, 18}, {96, 36}}, FONT_BODY, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlAlignmentTitle, "ALIGNMENTS", {{24, 18}, {332, 42}}, FONT_BODY, ::gpk::ALIGN_TOP_RIGHT));
	cnst ::gpk::enum_definition<::gpk::ALIGN> & alignmentDefinition = ::gpk::get_enum<::gpk::ALIGN>();
	::gpk::aobj<::gpk::vcst_t> alignmentPrimitiveNames = {};
	for(::gpk::u2_t iAlignment = 0; iAlignment < alignmentDefinition.Values.size(); ++iAlignment) {
		cnst ::gpk::u0_t alignment = alignmentDefinition.Values[iAlignment];
		if(0 == alignment || 0 == (alignment & (alignment - 1)))
			if_fail_fe(alignmentPrimitiveNames.push_back(alignmentDefinition.Names[iAlignment]));
	}
	if_fail_fe(::gpk::controlGridCreate(app.GUI, app.GridAlignments, ::gpk::view<::gpk::vcst_t>{alignmentPrimitiveNames}, ::gpk::CID_INVALID, ::gpk::DIRECTION_RIGHT, 3, {108, 56}, {0, 0}, {24, 76}, ::gpk::ALIGN_TOP_RIGHT));
	for(::gpk::u2_t iAlignment = 0; iAlignment < alignmentPrimitiveNames.size(); ++iAlignment) {
		cnst ::gpk::cid_t idControl = ::gpk::controlGridControl(app.GridAlignments, iAlignment);
		app.GUI.Controls.Text[idControl].FontSelected = FONT_BODY;
	}
	app.ControlsKeyboard[0] = app.ControlMode;
	app.ControlsKeyboard[1] = app.ControlLive;
	app.ControlsKeyboard[2] = app.ControlExit;

	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorLive, app.ControlLive));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, ::gpk::CID_INVALID));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, ::gpk::CID_INVALID));
	rtrn ::gpk::guiUpdateMetrics(app.GUI, app.Window.Size, false);
}

::gpk::err_t sampleGUIUpdate(SSampleGUI & app, bool systemRequestedExit) {
	if(systemRequestedExit || app.Window.Closed || app.Window.Input->KeyDown(::gpk::SYSTEM_KEY_Escape))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUI.Controls.IsAction(app.ControlExit))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUI.Controls.IsAction(app.ControlMode))
		app.GUI.ColorModeDefault = app.GUI.ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::GUI_COLOR_MODE_Flat : ::gpk::GUI_COLOR_MODE_3D;
	cnst ::gpk::u2_t toneCount = ::sampleGUIPaletteToneCount(app);
	for(::gpk::u2_t iTone = 0; iTone < toneCount; ++iTone)
		if(app.GUI.Controls.IsAction(::gpk::controlGridControl(app.PalettePicker.GridTones, iTone)))
			app.PalettePicker.Tone = iTone;
	for(::gpk::u2_t iShade = 0; iShade < PALETTE_SHADE_COUNT; ++iShade)
		if(app.GUI.Controls.IsAction(::gpk::controlGridControl(app.PalettePicker.GridShades, iShade)))
			app.PalettePicker.Shade = iShade;
	bool fontChanged = false;
	for(::gpk::u2_t iFont = 0; iFont < app.GUI.Fonts.size(); ++iFont)
		if(app.GUI.Controls.IsAction(::gpk::controlGridControl(app.FontPicker.Grid, iFont))) {
			fontChanged = app.FontPicker.Selected != iFont;
			app.FontPicker.Selected = iFont;
		}
	if(app.GUI.Controls.IsAction(app.ControlLive))
		app.GUI.Controls.SetSelected(app.ControlLive, false == app.GUI.Controls.IsSelected(app.ControlLive));
	cnst ::gpk::cid_t idHovered = ::sampleGUIHoveredControl(app);
	cnst ::gpk::cid_t idFocused = ::sampleGUIFocusedControl(app);
	if_fail_fe(::gpk::controlTextSet(app.GUI, app.ControlMode, app.GUI.ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::vcst_t{"Mode: 3D"} : ::gpk::vcst_t{"Mode: Flat"}));
	if_fail_fe(::sampleGUIPalettePickerApply(app));
	::sampleGUIFontPickerApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorLive, app.ControlLive));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorHovered, idHovered));
	if_fail_fe(::sampleGUIInspectorUpdate(app, app.InspectorFocused, idFocused));
	if_fail_fe(app.Dialog->Update());
	rtrn ::gpk::guiUpdateMetrics(app.GUI, app.Window.Size, fontChanged);
}

::gpk::err_t sampleGUIDraw(SSampleGUI & app, ::gpk::g8bgra target) {
	if_fail_fe(target.fill(::gpk::SColorBGRA{16, 14, 12}));
	rtrn ::gpk::guiDraw(app.GUI, target);
}

#define GPK_SAMPLE_GUI_TITLE "LLC GUI palettes, states and alignment"

stct SApplication {
	::gpk::SFramework	Framework;
	SSampleGUI			Sample;

	SApplication(::gpk::SRuntimeValues & runtimeValues)
		: Framework(runtimeValues, {}), Sample(Framework.RootWindow, Framework.GUI) {}
};

::gpk::err_t setup(SApplication & app) {
	::gpk::SWindow & window = app.Framework.RootWindow;
	window.Size = {1280, 540};
	if_null_ve(::gpk::OS_NO_MEMORY, window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(::gpk::mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	rtrn ::sampleGUISetup(app.Sample);
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
	cnst ::gpk::err_t resultSample = ::sampleGUIUpdate(app.Sample, false);
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
