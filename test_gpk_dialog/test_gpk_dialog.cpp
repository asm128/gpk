#include "test_gpk_dialog.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"

#include "gpk_dialog_controls.h"
//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

usns gpk;

stxp	::gpk::n2u1_t			PLAYER_UI_SIZE			= {320, 40};

// Ported from llc_sample_gui_viewport.cpp:13-25, 46-81 and 151-171.
stxp	::gpk::u0_t			DIALOG_THEME_COUNT		= 3;
stxp	::gpk::u0_t			PALETTE_SHADE_COUNT		= 16;

sttc	::gpk::SColorBGRA	dialogThemeColor			(::gpk::u0_t theme) nxpt {
	stxp ::gpk::SColorBGRA colors[DIALOG_THEME_COUNT] =
		{ {248, 208,  72}
		, { 32, 176, 248}
		, {232, 232, 232}
		};
	rtrn colors[theme % DIALOG_THEME_COUNT];
}

sttc	::gpk::SColorBGRA	dialogThemeShade			(::gpk::SColorBGRA color, ::gpk::u0_t shade) nxpt {
	shade = ::gpk::min(shade, (::gpk::u0_t)(PALETTE_SHADE_COUNT - 1));
	rtrn color * ((PALETTE_SHADE_COUNT - (::gpk::f2_t)shade) / PALETTE_SHADE_COUNT);
}

sttc	::gpk::SColorBGRA	dialogThemeTextColor		(::gpk::SColorBGRA background) nxpt {
	cnst ::gpk::u2_t luminance = (::gpk::u2_t)background.r * 299 + (::gpk::u2_t)background.g * 587 + (::gpk::u2_t)background.b * 114;
	rtrn ::dialogThemeShade({255, 255, 255}, luminance > 150000 ? 15 : 1);
}

sttc	::gpk::err_t		dialogThemeResourcesCreate	(::gpk::SDialog & dialog, ::gpk::astatic<::gpk::s1_t, DIALOG_THEME_COUNT> & paletteIds) {
	::gpk::SGUI & gui = *dialog.GUI;
	for(::gpk::u0_t iTheme = 0; iTheme < DIALOG_THEME_COUNT; ++iTheme) {
		paletteIds[iTheme] = dialog.Colors->Viewport[iTheme];
		::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT> & palette = gui.Colors->Palettes[paletteIds[iTheme]];
		cnst ::gpk::SColorBGRA tone = ::dialogThemeColor(iTheme);
		cnst ::gpk::u0_t shade = 8;
		cnst ::gpk::SColorBGRA base = ::dialogThemeShade(tone, shade);
		cnst ::gpk::u0_t borderShade = shade - 5;
		cnst ::gpk::SColorBGRA borderColor = ::dialogThemeShade(tone, borderShade);
		for(::gpk::u2_t iArea = 0; iArea < palette.size(); ++iArea)
			palette[iArea] = ::dialogThemeShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 5, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_BACKGROUND] = ::dialogThemeShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 6, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_CLIENT] = base;
		palette[::gpk::UI_CONTROL_AREA_BORDER_LEFT] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_TOP] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_RIGHT] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM] = borderColor;
		palette[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND] = ::dialogThemeShade(tone, (::gpk::u0_t)::gpk::min((::gpk::u2_t)shade + 2, (::gpk::u2_t)PALETTE_SHADE_COUNT - 1));
		palette[::gpk::UI_CONTROL_AREA_TEXT_FACE] = ::dialogThemeTextColor(base);
	}
	::gpk::SRasterFontManager fonts = {};
	if_fail_fe(::gpk::rasterFontDefaults(fonts));
	gui.Fonts = fonts.Fonts;
	rtrn 0;
}

sttc	::gpk::err_t		dialogViewportCreate		(::gme::SApplication & app, ::gpk::u0_t iViewport, ::gpk::vcst_t title, cnst ::gpk::rect2s1_t & area, ::gpk::s1_t palette) {
	if_true_fe(iViewport >= app.Viewports.size());
	for(::gpk::u0_t iState = 0; iState < ::gpk::GUI_CONTROL_PALETTE_COUNT; ++iState) {
		app.DialogMain.Colors->Viewport[iState] = palette;
		app.DialogMain.Colors->ViewportTitle[iState] = palette;
	}
	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	app.Viewports[iViewport] = ::gpk::viewportCreate(app.DialogMain, viewport);
	::gpk::SControlTable & controls = app.DialogMain.GUI->Controls;
	::gpk::SControlPlacement & root = controls.Placement[viewport->IdGUIControl];
	root.Area = area;
	root.Border = {3, 3, 3, 3};
	root.Margin = {1, 1, 1, 1};
	controls.Text[viewport->IdTitle].Text = title;
	controls.Text[viewport->IdTitle].FontSelected = 2;
	controls.Placement[viewport->IdTitle].Border = {2, 2, 2, 2};
	controls.Placement[viewport->IdClient].Area.Size = {(::gpk::s1_t)(area.Size.x - 8), (::gpk::s1_t)(area.Size.y - 32)};
	controls.Placement[viewport->IdClient].Border = {1, 1, 1, 1};
	viewport->SettingsOld = viewport->Settings;
	rtrn ::gpk::controlMetricsInvalidate(*app.DialogMain.GUI, viewport->IdGUIControl);
}

// Ported from llc_sample_gui_viewport.cpp:138-148 and 284-294.
sttc	void				dialogViewportImageUpdate	(::gpk::img8bgra & image, ::gpk::u0_t theme, ::gpk::u2_t frameIndex) nxpt {
	cnst ::gpk::SColorBGRA tone = ::dialogThemeColor(theme);
	cnst ::gpk::bgra background = ::dialogThemeShade(tone, 14);
	cnst ::gpk::bgra grid = ::dialogThemeShade(tone, 9);
	cnst ::gpk::bgra pulse = ::dialogThemeShade(tone, 2);
	for(::gpk::u2_t y = 0; y < image.metrics().y; ++y)
	for(::gpk::u2_t x = 0; x < image.metrics().x; ++x) {
		cnst bool onGrid = 0 == x % 32 || 0 == y % 24;
		cnst bool onPulse = ((x + y + frameIndex) % 160) < 3;
		image[y][x] = onPulse ? pulse : onGrid ? grid : background;
	}
}

sttc	::gpk::err_t		dialogViewportImagesUpdate	(::gme::SApplication & app) {
	::gpk::SGUI & gui = *app.DialogMain.GUI;
	for(::gpk::u0_t iViewport = 0; iViewport < ::gme::DIALOG_VIEWPORT_COUNT; ++iViewport) {
		::gpk::pobj<::gpk::SDialogViewport> viewport = {};
		if_fail_fe(app.DialogMain.Get(app.Viewports[iViewport], viewport));
		cnst ::gpk::cid_t idClient = viewport->IdClient;
		cnst ::gpk::n2u2_t imageSize = gui.Controls.Placement[idClient].Area.Size.u2();
		if(app.PNGImages[iViewport].metrics() != imageSize)
			if_fail_fe(app.PNGImages[iViewport].resize(imageSize));
		::dialogViewportImageUpdate(app.PNGImages[iViewport], iViewport, app.FrameIndex);
		gui.Controls.Images[idClient].Image = app.PNGImages[iViewport].View;
		gui.Controls.Images[idClient].ImageAlign = ::gpk::ALIGN_CENTER;
	}
	++app.FrameIndex;
	rtrn 0;
}

err_t			cleanup		(::gme::SApplication & app)						{ return mainWindowDestroy(app.Framework.RootWindow); }
err_t			setup		(::gme::SApplication & app)						{
	SFramework				& framework					= app.Framework;
	SWindow					& mainWindow				= framework.RootWindow;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	mainWindow.Size														= {1280, 720};
	if_true_e(failed(mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	SGUI						& gui						= *framework.GUI;
	gui.ColorModeDefault												= GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ASCII_COLOR_DARKGREY * 16 + ASCII_COLOR_LIGHTGREY;
	::gpk::ALIGN					playerAlign				= ::gpk::ALIGN_BOTTOM_LEFT;
	::gpk::n2s1_t				playerPosition				= {};
	::gpk::guiCreateControlList<::gme::UI_PROFILE>(gui, app.DialogMain.Root, PLAYER_UI_SIZE, playerPosition, playerAlign, ::gpk::ALIGN_CENTER, app.ControlsDialogHome);
	if_fail_fe(::gpk::inputBoxCreate(app.InputBox, *app.DialogMain.GUI, app.DialogMain.Root));
	gui.Controls.SetHidden(app.InputBox.IdRoot, true);
	gui.Controls.Placement[app.InputBox.IdRoot].Align				= playerAlign;
	gui.Controls.Placement[app.InputBox.IdRoot].Area.Offset.x	= 240;
	if_fail_fe(::gpk::controlFontSet(gui, app.ControlsDialogHome[::gme::UI_PROFILE_Name], 10));
	if_fail_fe(::gpk::controlTextSet(gui, app.ControlsDialogHome[::gme::UI_PROFILE_Name], app.PlayerName));
	app.IdExit															= controlCreate(gui);
	SControlPlacement					& controlExit				= gui.Controls.Placement[app.IdExit];
	controlExit.Area													= {{}, {64, 20}};
	controlExit.Border													= {10, 10, 10, 10};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ALIGN_BOTTOM_RIGHT;
	SControlText				& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ALIGN_CENTER;
	SControlConstraints		& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToControl								= {-1,-1};//{app.IdExit, -1};
	controlSetParent(gui, app.IdExit, -1);

	pobj<SDialogTuner<uint8_t>>								tuner						= {};
	app.NumericTuner													= tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min												= 100;
	tuner->ValueLimits.Max												= 200;
	SControlTable		    & controlTable = gui.Controls;
	controlTable.Placement[tuner->IdGUIControl].Area.Offset	= {128, 192};
	controlTable.Placement[tuner->IdGUIControl].Area.Size.x	= 128;
	controlTable.Placement[tuner->IdGUIControl].Area.Size.y	=  20;
	tuner->SetValue(0);

	pobj<SDialogSlider>									slider						= {};
	app.Slider															= sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min												= 0;
	slider->ValueLimits.Max												= 255;
	controlTable.Placement[slider->IdGUIControl].Area.Offset	= {128, 128};
	controlTable.Placement[slider->IdGUIControl].Area.Size.x	= 128;
	controlTable.Placement[slider->IdGUIControl].Area.Size.y	= 8;

	pobj<SDialogCheckBox>									checkbox					= {};
	app.CheckBox														= checkBoxCreate(app.DialogMain, checkbox);
	controlTable.Placement[checkbox->IdGUIControl].Area.Offset	= {128, 256};


	::gpk::astatic<::gpk::s1_t, DIALOG_THEME_COUNT>	viewportPalettes		= {};
	if_fail_fe(::dialogThemeResourcesCreate(app.DialogMain, viewportPalettes));
	gui.Controls.Draw[app.IdExit].UseNewPalettes = true;
	for(::gpk::u0_t iState = 0; iState < ::gpk::GUI_CONTROL_PALETTE_COUNT; ++iState)
		gui.Controls.Draw[app.IdExit].Palettes[iState] = viewportPalettes[2];
	if_fail_fe(::dialogViewportCreate(app, 0, ::gpk::get_enum_namev<::gpk::GUI_CONTROL_PALETTE>(), {{ 80,  92}, {420, 250}}, viewportPalettes[0]));
	if_fail_fe(::dialogViewportCreate(app, 1, "AMBER CPU SURFACE", {{408, 184}, {430, 280}}, viewportPalettes[1]));
	if_fail_fe(::dialogViewportCreate(app, 2, "MONO CPU SURFACE" , {{754, 104}, {360, 230}}, viewportPalettes[2]));
	::gpk::pobj<::gpk::SDialogViewport>	viewportEnum		= {};
	if_fail_fe(app.DialogMain.Get(app.Viewports[0], viewportEnum));
	::gpk::enum_definition<::gpk::GUI_CONTROL_PALETTE> & enumDefinition = ::gpk::get_enum<::gpk::GUI_CONTROL_PALETTE>();
	if_fail_fe(::gpk::controlListCreate(gui, app.ListEnumNames, enumDefinition.Names, viewportEnum->IdClient, ::gpk::DIRECTION_DOWN));
	if_fail_fe(::gpk::controlListCreate(gui, app.ListEnumValues, enumDefinition.Values, viewportEnum->IdClient, ::gpk::DIRECTION_DOWN));
	gui.Controls.Constraints[app.ListEnumNames.IdControl].AttachSizeToControl = {-1, app.ListEnumNames.IdControl};
	gui.Controls.Constraints[app.ListEnumValues.IdControl].AttachSizeToControl = {-1, app.ListEnumValues.IdControl};
	gui.Controls.Constraints[app.ListEnumNames.IdControl].DockToControl.Right = app.ListEnumValues.IdControl;
	gui.Controls.Placement[app.ListEnumValues.IdControl].Area.Size.x = 64;
	::gpk::SControlList * enumLists[] = {&app.ListEnumNames, &app.ListEnumValues};
	for(::gpk::u0_t iList = 0; iList < ::gpk::size(enumLists); ++iList)
	for(::gpk::u2_t iControl = 0; iControl <= enumLists[iList]->IdControls.size(); ++iControl) {
		cnst ::gpk::cid_t idControl = iControl ? enumLists[iList]->IdControls[iControl - 1] : enumLists[iList]->IdControl;
		gui.Controls.Draw[idControl].UseNewPalettes = true;
		for(::gpk::u0_t iState = 0; iState < ::gpk::GUI_CONTROL_PALETTE_COUNT; ++iState)
			gui.Controls.Draw[idControl].Palettes[iState] = viewportPalettes[0];
	}
	if_fail_fe(::gpk::controlMetricsInvalidate(gui, viewportEnum->IdGUIControl));
	if_fail_fe(app.PNGImages.resize(::gme::DIALOG_VIEWPORT_COUNT));
	return 0;
}

err_t			update		(::gme::SApplication & app, bool exitSignal)	{
	//STimer															timer;
	if_true_vi(APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard				lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	SFramework			& framework					= app.Framework;
	if_true_vi(APPLICATION_STATE_EXIT, APPLICATION_STATE_EXIT == updateFramework(app.Framework));

	SGUI					& gui			= *framework.GUI;
	if_fail_fe(::gpk::controlTextSet(gui, app.ControlsDialogHome[::gme::UI_PROFILE_Name], app.PlayerName));
	::gpk::acid			controlsToProcess	= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	if(s2_t result = app.InputBox.Update(gui, app.InputBox.VirtualKeyboard, framework.RootWindow.EventQueue, controlsToProcess)) {
		if(result == INT_MAX) {
			::gpk::vcsc_t		trimmed					= {app.InputBox.Text.begin(), ::gpk::min(app.InputBox.Text.size(), 16U)};
			::gpk::trim(trimmed);
			if(trimmed.size())
				app.PlayerName							= ::gpk::label(trimmed);
			app.InputBox.Edit(gui, false);
		}
	}
	else {
		if(1 == ::gpk::guiProcessControls(gui, controlsToProcess, [&app, &gui](u2_t iControl) {
			if(iControl == (u2_t)app.IdExit)
				return 1;
			if(iControl == (u2_t)app.ControlsDialogHome[::gme::UI_PROFILE_Name]) {
				app.InputBox.SetText(gui, gui.Controls.Text[app.ControlsDialogHome[::gme::UI_PROFILE_Name]].Text);
				app.InputBox.Edit(gui, true);
			}
			return 0;
		}))
			return 1;
	}

	app.DialogMain.Update();
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

err_t			draw		(::gme::SApplication & app)							{
	//STimer															timer;
	pobj<rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::SColorBGRA{16, 14, 12}, 0xFFFFFFFFU);
	{
		::gpk::lock_guard				lock					(app.Framework.LockGUI);
		if_fail_fe(::dialogViewportImagesUpdate(app));
		guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::gpk::lock_guard				lock					(app.Framework.LockGUI);
		guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::gpk::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
