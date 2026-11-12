#include "test_gpk_gui.h"
#include "gpk_app_impl.h"
#include "gpk_cstring.h"
#include "gpk_framework.h"
#include "gpk_runtime.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"
#include "gpk_system_key.h"

stxp ::gpk::u0_t SAMPLE_GUI_THEME_COUNT = 3;
stxp ::gpk::u0_t PALETTE_SHADE_COUNT = 16;
stxp ::gpk::u0_t THEME_COUNT = SAMPLE_GUI_THEME_COUNT;
stxp ::gpk::u0_t STATE_COUNT = 8;
stxp ::gpk::u0_t ALIGNMENT_COUNT = 9;

GDEFINE_ENUM_TYPE(SAMPLE_GUI_THEME, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(SAMPLE_GUI_THEME, AZURE, 0, "Palette: Azure");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_THEME, AMBER, 1, "Palette: Amber");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_THEME, MONO , 2, "Palette: Mono");

GDEFINE_ENUM_TYPE(SAMPLE_GUI_STATE, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, NORMAL           , 0                                                                  , "NORMAL");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, HOVERED          , ::gpk::GUI_CONTROL_FLAG_Hovered                                    , "HOVERED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, PRESSED          , ::gpk::GUI_CONTROL_FLAG_Pressed                                    , "PRESSED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, DISABLED         , ::gpk::GUI_CONTROL_FLAG_Disabled                                   , "DISABLED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, SELECTED         , ::gpk::GUI_CONTROL_FLAG_Selected                                   , "SELECTED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, SELECTED_HOVERED , ::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Hovered, "SELECTED + HOVERED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, SELECTED_PRESSED , ::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Pressed, "SELECTED + PRESSED");
GDEFINE_ENUM_VALUED(SAMPLE_GUI_STATE, SELECTED_DISABLED, ::gpk::GUI_CONTROL_FLAG_Selected | ::gpk::GUI_CONTROL_FLAG_Disabled, "SELECTED + DISABLED");
GDEFINE_ENUM_VALUE (SAMPLE_GUI_STATE, FOCUSED          , ::gpk::GUI_CONTROL_FLAG_Focused);
GDEFINE_ENUM_VALUE (SAMPLE_GUI_STATE, ACTION           , ::gpk::GUI_CONTROL_FLAG_Action);

GDEFINE_ENUM_TYPE(SAMPLE_GUI_ALIGNMENT, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, TOP_LEFT     , ::gpk::ALIGN_TOP_LEFT);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, CENTER_TOP   , ::gpk::ALIGN_CENTER_TOP);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, TOP_RIGHT    , ::gpk::ALIGN_TOP_RIGHT);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, CENTER_LEFT  , ::gpk::ALIGN_CENTER_LEFT);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, CENTER       , ::gpk::ALIGN_CENTER);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, CENTER_RIGHT , ::gpk::ALIGN_CENTER_RIGHT);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, BOTTOM_LEFT  , ::gpk::ALIGN_BOTTOM_LEFT);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, CENTER_BOTTOM, ::gpk::ALIGN_CENTER_BOTTOM);
GDEFINE_ENUM_VALUE(SAMPLE_GUI_ALIGNMENT, BOTTOM_RIGHT , ::gpk::ALIGN_BOTTOM_RIGHT);

sttc ::gpk::vcst_t sampleGUIThemeText(::gpk::u0_t theme) {
	rtrn ::gpk::get_value_descv((SAMPLE_GUI_THEME)(theme % SAMPLE_GUI_THEME_COUNT));
}

sttc ::gpk::vcst_t sampleGUIThemeName(::gpk::u0_t theme) {
	rtrn ::gpk::get_value_namev((SAMPLE_GUI_THEME)(theme % SAMPLE_GUI_THEME_COUNT));
}

sttc ::gpk::SColorBGRA sampleGUIThemeColor(::gpk::u0_t theme) nxpt {
	stxp ::gpk::SColorBGRA colors[SAMPLE_GUI_THEME_COUNT] =
		{ {248, 208,  72}
		, { 32, 176, 248}
		, {232, 232, 232}
		};
	rtrn colors[theme % SAMPLE_GUI_THEME_COUNT];
}

sttc ::gpk::SColorBGRA sampleGUIShade(::gpk::SColorBGRA color, ::gpk::u0_t shade) nxpt {
	shade = ::gpk::min(shade, (::gpk::u0_t)(PALETTE_SHADE_COUNT - 1));
	rtrn color * ((PALETTE_SHADE_COUNT - (::gpk::f2_t)shade) / PALETTE_SHADE_COUNT);
}

sttc ::gpk::SColorBGRA sampleGUIDesaturate(::gpk::SColorBGRA color) nxpt {
	cnst ::gpk::u0_t gray = (::gpk::u0_t)(((::gpk::u2_t)color.r + color.g + color.b) / 3);
	rtrn
		{ (::gpk::u0_t)((color.b + (::gpk::u1_t)gray * 2) / 3)
		, (::gpk::u0_t)((color.g + (::gpk::u1_t)gray * 2) / 3)
		, (::gpk::u0_t)((color.r + (::gpk::u1_t)gray * 2) / 3)
		};
}

sttc ::gpk::u0_t sampleGUIStateShade(::gpk::u0_t state) nxpt {
	stxp ::gpk::u0_t shades[::gpk::GUI_CONTROL_PALETTE_COUNT] = {8, 10, 5, 11, 4, 7, 2, 7, 1, 13};
	rtrn shades[state % ::gpk::GUI_CONTROL_PALETTE_COUNT];
}

sttc ::gpk::u0_t sampleGUIStateBorderOffset(::gpk::u0_t state) nxpt {
	stxp ::gpk::u0_t offsets[::gpk::GUI_CONTROL_PALETTE_COUNT] = {5, 1, 7, 3, 0, 1, 7, 3, 5, 5};
	rtrn offsets[state % ::gpk::GUI_CONTROL_PALETTE_COUNT];
}

sttc ::gpk::SColorBGRA sampleGUITextColor(::gpk::SColorBGRA background) nxpt {
	cnst ::gpk::u2_t luminance = (::gpk::u2_t)background.r * 299 + (::gpk::u2_t)background.g * 587 + (::gpk::u2_t)background.b * 114;
	rtrn ::sampleGUIShade({255, 255, 255}, luminance > 150000 ? 15 : 1);
}

sttc ::gpk::err_t sampleGUIResourcesCreate(::gpk::SGUI & gui) {
	if_true_fe(0 == gui.Colors.create());
	if_fail_fe(gui.Colors->Palettes.resize(SAMPLE_GUI_THEME_COUNT * ::gpk::GUI_CONTROL_PALETTE_COUNT));
	for(::gpk::u0_t iTheme = 0; iTheme < SAMPLE_GUI_THEME_COUNT; ++iTheme)
	for(::gpk::u0_t iState = 0; iState < ::gpk::GUI_CONTROL_PALETTE_COUNT; ++iState) {
		::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT> & palette = gui.Colors->Palettes[iTheme * ::gpk::GUI_CONTROL_PALETTE_COUNT + iState];
		::gpk::SColorBGRA tone = ::sampleGUIThemeColor(iTheme);
		if(iState == ::gpk::GUI_CONTROL_PALETTE_DISABLED || iState == ::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED)
			tone = ::sampleGUIDesaturate(tone);
		cnst ::gpk::u0_t shade = ::sampleGUIStateShade(iState);
		cnst ::gpk::SColorBGRA base = ::sampleGUIShade(tone, shade);
		cnst ::gpk::u0_t borderOffset = ::sampleGUIStateBorderOffset(iState);
		cnst ::gpk::u0_t borderShade = shade > 7
			? (::gpk::u0_t)(shade > borderOffset ? shade - borderOffset : 0)
			: (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + borderOffset, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1);
		cnst ::gpk::SColorBGRA borderColor = ::sampleGUIShade(tone, borderShade);
		for(::gpk::u2_t iArea = 0; iArea < palette.size(); ++iArea)
			palette[iArea] = ::sampleGUIShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 5, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_BACKGROUND] = ::sampleGUIShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 6, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_CLIENT] = base;
		palette[::gpk::UI_CONTROL_AREA_BORDER_LEFT] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_TOP] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_RIGHT] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND] = ::sampleGUIShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 2, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_TEXT_FACE] = ::sampleGUITextColor(base);
	}
	::gpk::SRasterFontManager fonts = {};
	if_fail_fe(::gpk::rasterFontDefaults(fonts));
	gui.Fonts = fonts.Fonts;
	rtrn 0;
}

sttc void sampleGUIControlThemeApply(::gpk::SGUI & gui, ::gpk::cid_t idControl, ::gpk::u0_t theme) nxpt {
	if(::gpk::controlInvalid(gui, idControl))
		rtrn;
	::gpk::SControlDraw & draw = gui.Controls.Draw[idControl];
	draw.UseNewPalettes = true;
	for(::gpk::u0_t iState = 0; iState < ::gpk::GUI_CONTROL_PALETTE_COUNT; ++iState)
		draw.Palettes[iState] = (::gpk::s1_t)((theme % SAMPLE_GUI_THEME_COUNT) * ::gpk::GUI_CONTROL_PALETTE_COUNT + iState);
}

sttc void sampleGUIThemeApply(::gpk::SGUI & gui, ::gpk::u0_t theme) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < gui.Controls.Draw.size(); ++iControl)
		::sampleGUIControlThemeApply(gui, (::gpk::cid_t)iControl, theme);
}

sttc ::gpk::vcst_t sampleGUIStateText(::gpk::u0_t state) {
	rtrn ::gpk::get_value_descv(::gpk::get_enum<SAMPLE_GUI_STATE>().get_value_by_index(state < STATE_COUNT ? state : 0));
}

sttc ::gpk::ALIGN	sampleGUIControlAlignment	(::gpk::u0_t alignment) nxpt	{ rtrn (::gpk::ALIGN)::gpk::get_enum<SAMPLE_GUI_ALIGNMENT>().get_value_by_index(alignment % ALIGNMENT_COUNT); }
sttc ::gpk::ALIGN	sampleGUITextAlignment		(::gpk::u0_t alignment) nxpt	{ rtrn (::gpk::ALIGN)::gpk::get_enum<SAMPLE_GUI_ALIGNMENT>().get_value_by_index(ALIGNMENT_COUNT - 1 - alignment % ALIGNMENT_COUNT); }
sttc ::gpk::vcst_t	sampleGUIAlignmentText		(::gpk::u0_t alignment)			{ rtrn ::gpk::get_enum<SAMPLE_GUI_ALIGNMENT>().get_label_by_index(alignment % ALIGNMENT_COUNT); }

sttc ::gpk::GUI_CONTROL_FLAG sampleGUIStateFlags(::gpk::u0_t state) nxpt {
	rtrn (::gpk::GUI_CONTROL_FLAG)::gpk::get_enum<SAMPLE_GUI_STATE>().get_value_by_index(state < STATE_COUNT ? state : 0);
}

sttc ::gpk::GUI_CONTROL_PALETTE sampleGUIControlPalette(bool disabled, cnst ::gpk::SControlState & state) nxpt {
	cnst bool highlighted = state.IsHovered() || state.IsFocused();
	if(state.IsSelected()) {
		if(disabled)          rtrn ::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED;
		if(state.IsPressed()) rtrn ::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED;
		if(highlighted)       rtrn ::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER;
		rtrn ::gpk::GUI_CONTROL_PALETTE_SELECTED;
	}
	if(disabled)          rtrn ::gpk::GUI_CONTROL_PALETTE_DISABLED;
	if(state.IsPressed()) rtrn ::gpk::GUI_CONTROL_PALETTE_PRESSED;
	if(highlighted)       rtrn ::gpk::GUI_CONTROL_PALETTE_HOVER;
	rtrn ::gpk::GUI_CONTROL_PALETTE_NORMAL;
}

sttc ::gpk::err_t sampleGUIControlCreate(SSampleGUI & app, ::gpk::cid_t & idControl, ::gpk::vcst_t text, cnst ::gpk::rect2s1_t & area, ::gpk::s1_t font = -1, ::gpk::ALIGN align = ::gpk::ALIGN_TOP_LEFT) {
	if_fail_fe(idControl = ::gpk::controlCreate(app.GUI));
	::gpk::SControlPlacement & placement = app.GUI.Controls.Placement[idControl];
	placement.Area = area;
	placement.Align = align;
	placement.Margin = {2, 2, 2, 2};
	placement.Border = {3, 3, 3, 3};
	::gpk::SControlDraw & draw = app.GUI.Controls.Draw[idControl];
	draw.UseNewPalettes = true;
	draw.ColorMode = ::gpk::GUI_COLOR_MODE_Default;
	app.GUI.Controls.Text[idControl].Text = text;
	app.GUI.Controls.Text[idControl].FontSelected = font;
	rtrn 0;
}

sttc void sampleGUIThemeApply(SSampleGUI & app) nxpt {
	::sampleGUIThemeApply(app.GUI, app.Theme);
	for(::gpk::u0_t iTheme = 0; iTheme < THEME_COUNT; ++iTheme)
		app.GUI.Controls.SetSelected(app.ControlsTheme[iTheme], app.Theme == iTheme);
}

sttc void sampleGUIStateCardsApply(SSampleGUI & app) nxpt {
	for(::gpk::u0_t iState = 0; iState < STATE_COUNT; ++iState) {
		::gpk::SControlState & state = app.GUI.Controls.States[app.ControlsState[iState]];
		cnst ::gpk::GUI_CONTROL_FLAG updated = (::gpk::GUI_CONTROL_FLAG)(state.Flag & ::gpk::GUI_CONTROL_FLAG_Updated);
		state.Flag = (::gpk::GUI_CONTROL_FLAG)(updated | ::sampleGUIStateFlags(iState));
	}
}

sttc ::gpk::err_t sampleGUIStateFlagAppend(::gpk::string & text, ::gpk::vcst_t flag) {
	if(text.size())
		if_fail_fe(text.push_back('+'));
	rtrn ::gpk::append_string(text, flag);
}

sttc ::gpk::err_t sampleGUIStateDescribe(cnst ::gpk::SControlState & state, bool disabled, ::gpk::string & text) {
	text.clear();
	if(state.IsSelected()) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_SELECTED)));
	if(state.IsHovered ()) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_HOVERED )));
	if(state.IsPressed ()) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_PRESSED )));
	if(state.IsFocused ()) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_FOCUSED )));
	if(disabled          ) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_DISABLED)));
	if(state.IsAction  ()) if_fail_fe(::sampleGUIStateFlagAppend(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_ACTION  )));
	if(0 == text.size())
		if_fail_fe(::gpk::append_string(text, ::gpk::get_value_namev(SAMPLE_GUI_STATE_NORMAL)));
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

sttc ::gpk::vcst_t sampleGUIControlName(cnst SSampleGUI & app, ::gpk::cid_t idControl) {

	rtrn app.GUI.Controls.Text[idControl].Text;
}

sttc ::gpk::cid_t sampleGUIHoveredControl(cnst SSampleGUI & app) nxpt {
	for(::gpk::u2_t iControl = 0; iControl < app.GUI.Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(app.GUI, (::gpk::cid_t)iControl) && app.GUI.Controls.IsHovered((::gpk::cid_t)iControl))
			rtrn (::gpk::cid_t)iControl;
	rtrn ::gpk::CID_INVALID;
}

sttc ::gpk::err_t sampleGUILiveTextUpdate(SSampleGUI & app) {
	cnst ::gpk::SControlState & state = app.GUI.Controls.States[app.ControlLive];
	cnst bool disabled = 0 != ::gpk::controlDisabled(app.GUI, app.ControlLive);
	::gpk::string stateText = {};
	if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
	cnst ::gpk::GUI_CONTROL_PALETTE palette = ::sampleGUIControlPalette(disabled, state);
	cnst ::gpk::vcst_t paletteName = ::gpk::get_value_namev(palette);
	cnst ::gpk::s1_t paletteSlot = app.GUI.Controls.Draw[app.ControlLive].Palettes[palette];
	::gpk::astsc_t<512> text = {};
	cnst ::gpk::err_t length = ::gpk::sprintf_s(text.Storage, "LIVE | STATE:%.*s | PALETTE:%.*s [%i] | CLICK TO TOGGLE SELECTED", (int)stateText.size(), stateText.begin(), (int)paletteName.size(), paletteName.begin(), paletteSlot);
	if_fail_fe(length);
	rtrn ::sampleGUIControlTextSetOwned(app, app.ControlLive, app.TextLive, {text.Storage, (::gpk::u2_t)length});
}

sttc ::gpk::err_t sampleGUIInspectorTextUpdate(SSampleGUI & app, ::gpk::cid_t idHovered) {
	cnst ::gpk::vcst_t themeName = ::sampleGUIThemeName(app.Theme);
	cnst ::gpk::vcst_t modeName = app.GUI.ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::vcst_t{"3D"} : ::gpk::vcst_t{"FLAT"};
	::gpk::astsc_t<512> text = {};
	::gpk::err_t length = 0;
	if(::gpk::controlInvalid(app.GUI, idHovered))
		length = ::gpk::sprintf_s(text.Storage, "GUI | THEME:%.*s | MODE:%.*s | HOVER:NONE", (int)themeName.size(), themeName.begin(), (int)modeName.size(), modeName.begin());
	else {
		cnst ::gpk::SControlState & state = app.GUI.Controls.States[idHovered];
		cnst bool disabled = 0 != ::gpk::controlDisabled(app.GUI, idHovered);
		::gpk::string stateText = {};
		if_fail_fe(::sampleGUIStateDescribe(state, disabled, stateText));
		cnst ::gpk::GUI_CONTROL_PALETTE palette = ::sampleGUIControlPalette(disabled, state);
		cnst ::gpk::vcst_t paletteName = ::gpk::get_value_namev(palette);
		cnst ::gpk::vcst_t controlName = ::sampleGUIControlName(app, idHovered);
		cnst ::gpk::vcst_t controlAlignName = ::gpk::get_value_namev(app.GUI.Controls.Placement[idHovered].Align);
		cnst ::gpk::vcst_t textAlignName = ::gpk::get_value_namev(app.GUI.Controls.Text[idHovered].Align);
		cnst ::gpk::s1_t paletteSlot = app.GUI.Controls.Draw[idHovered].Palettes[palette];
		length = ::gpk::sprintf_s(text.Storage, "GUI | THEME:%.*s | MODE:%.*s | HOVER #%i %.*s | STATE:%.*s | PALETTE:%.*s [%i] | ALIGN:%.*s / TEXT:%.*s", (int)themeName.size(), themeName.begin(), (int)modeName.size(), modeName.begin(), idHovered, (int)controlName.size(), controlName.begin(), (int)stateText.size(), stateText.begin(), (int)paletteName.size(), paletteName.begin(), paletteSlot, (int)controlAlignName.size(), controlAlignName.begin(), (int)textAlignName.size(), textAlignName.begin());
	}
	if_fail_fe(length);
	rtrn ::sampleGUIControlTextSetOwned(app, app.ControlInspector, app.TextInspector, {text.Storage, (::gpk::u2_t)length});
}

::gpk::err_t sampleGUISetup(SSampleGUI & app) {
	app.GUI.SelectedFont = 2;
	app.GUI.ColorModeDefault = ::gpk::GUI_COLOR_MODE_3D;
	if_fail_fe(::sampleGUIResourcesCreate(app.GUI));

	if_fail_fe(::sampleGUIControlCreate(app, app.ControlTitle, "LLC GUI CONTROL PALETTES AND STATES", {{24, 18}, {650, 42}}, 3));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlMode, "Mode: 3D", {{742, 18}, {184, 42}}, 2));
	for(::gpk::u0_t iTheme = 0; iTheme < THEME_COUNT; ++iTheme) {
		if_fail_fe(::sampleGUIControlCreate(app, app.ControlsTheme[iTheme], ::sampleGUIThemeText(iTheme), {{(::gpk::s1_t)(24 + 226 * iTheme), 76}, {206, 40}}, 2));
	}

	for(::gpk::u0_t iState = 0; iState < STATE_COUNT; ++iState) {
		cnst ::gpk::s1_t x = (::gpk::s1_t)(24 + 226 * (iState % 4));
		cnst ::gpk::s1_t y = (::gpk::s1_t)(140 + 88 * (iState / 4));
		if_fail_fe(::sampleGUIControlCreate(app, app.ControlsState[iState], ::sampleGUIStateText(iState), {{x, y}, {206, 68}}, 2));
	}
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlLive, "LIVE", {{24, 138}, {884, 64}}, 2, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlInspector, "GUI | HOVER:NONE", {{24, 68}, {884, 56}}, 1, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlHelp, "TAB FOCUS | ENTER/SPACE ACTIVATE | ESC/EXIT CLOSE | PALETTES SHARE ONE 16-SHADE TONE", {{136, 18}, {772, 36}}, 1, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlExit, "EXIT", {{24, 18}, {96, 36}}, 2, ::gpk::ALIGN_BOTTOM_LEFT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlAlignmentTitle, "CONTROL ALIGNMENT / OPPOSITE TEXT ALIGNMENT", {{24, 18}, {332, 42}}, 1, ::gpk::ALIGN_TOP_RIGHT));
	if_fail_fe(::sampleGUIControlCreate(app, app.ControlAlignmentRoot, "", {{24, 76}, {332, 446}}, -1, ::gpk::ALIGN_TOP_RIGHT));
	for(::gpk::u0_t iAlignment = 0; iAlignment < ALIGNMENT_COUNT; ++iAlignment) {
		if_fail_fe(::sampleGUIControlCreate(app, app.ControlsAlignment[iAlignment], ::sampleGUIAlignmentText(iAlignment), {{0, 0}, {108, 58}}, 1, ::sampleGUIControlAlignment(iAlignment)));
		app.GUI.Controls.Text[app.ControlsAlignment[iAlignment]].Align = ::sampleGUITextAlignment(iAlignment);
		if_fail_fe(::gpk::controlSetParent(app.GUI, app.ControlsAlignment[iAlignment], app.ControlAlignmentRoot));
	}
	app.ControlsKeyboard[0] = app.ControlMode;
	for(::gpk::u0_t iTheme = 0; iTheme < THEME_COUNT; ++iTheme) {
		app.ControlsKeyboard[1 + iTheme] = app.ControlsTheme[iTheme];
	}
	app.ControlsKeyboard[4] = app.ControlLive;
	app.ControlsKeyboard[5] = app.ControlExit;

	::sampleGUIThemeApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUILiveTextUpdate(app));
	if_fail_fe(::sampleGUIInspectorTextUpdate(app, ::gpk::CID_INVALID));
	rtrn ::gpk::guiUpdateMetrics(app.GUI, app.Window.Size, false);
}

::gpk::err_t sampleGUIUpdate(SSampleGUI & app, bool systemRequestedExit) {
	if(systemRequestedExit || app.Window.Closed || app.Window.Input->KeyDown(::gpk::SYSTEM_KEY_Escape))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUI.Controls.IsAction(app.ControlExit))
		rtrn ::gpk::APPLICATION_STATE_EXIT;
	if(app.GUI.Controls.IsAction(app.ControlMode))
		app.GUI.ColorModeDefault = app.GUI.ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::GUI_COLOR_MODE_Flat : ::gpk::GUI_COLOR_MODE_3D;
	for(::gpk::u0_t iTheme = 0; iTheme < THEME_COUNT; ++iTheme)
		if(app.GUI.Controls.IsAction(app.ControlsTheme[iTheme]))
			app.Theme = iTheme;
	if(app.GUI.Controls.IsAction(app.ControlLive))
		app.GUI.Controls.SetSelected(app.ControlLive, false == app.GUI.Controls.IsSelected(app.ControlLive));
	cnst ::gpk::cid_t idHovered = ::sampleGUIHoveredControl(app);
	if_fail_fe(::gpk::controlTextSet(app.GUI, app.ControlMode, app.GUI.ColorModeDefault == ::gpk::GUI_COLOR_MODE_3D ? ::gpk::vcst_t{"Mode: 3D"} : ::gpk::vcst_t{"Mode: Flat"}));
	::sampleGUIThemeApply(app);
	::sampleGUIStateCardsApply(app);
	if_fail_fe(::sampleGUILiveTextUpdate(app));
	if_fail_fe(::sampleGUIInspectorTextUpdate(app, idHovered));
	rtrn ::gpk::guiUpdateMetrics(app.GUI, app.Window.Size, false);
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
		: Framework(runtimeValues, {}), Sample(Framework.RootWindow, *Framework.GUI) {
		if_fail_e(::gpk::rasterFontDefaults(Sample.Engine.Scene->Graphics->Fonts));
	}
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
