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

err_t			cleanup		(::gme::SApplication & app)						{ return mainWindowDestroy(app.Framework.RootWindow); }
err_t			setup		(::gme::SApplication & app)						{
	SFramework				& framework					= app.Framework;
	SWindow					& mainWindow				= framework.RootWindow;
	app.Framework.GUI													= app.DialogMain.GUI;
	app.DialogMain.Input												= mainWindow.Input;
	mainWindow.Size														= {800, 600};
	if_true_e(failed(mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	SGUI						& gui						= *framework.GUI;
	gui.ColorModeDefault												= GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ASCII_COLOR_DARKGREEN * 16 + 7;
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
	controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
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


	pobj<SDialogViewport>									viewport					= {};
	app.Viewport														= viewportCreate(app.DialogMain, viewport);
	controlTable.Placement[viewport->IdGUIControl].Area.Offset	= {320, 128};
	controlTable.Placement[viewport->IdGUIControl].Area.Size		= {320, 200};
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
	target->resize(app.Framework.RootWindow.Size, LIGHTGRAY, 0xFFFFFFFFU);
	{
		::gpk::lock_guard				lock					(app.Framework.LockGUI);
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
