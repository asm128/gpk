#include "test_gpk_dialog.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"

#include "gpk_dialog_controls.h"
//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

usns gpk;

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
		::std::lock_guard				lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	SFramework			& framework					= app.Framework;
	if_true_vi(APPLICATION_STATE_EXIT, APPLICATION_STATE_EXIT == updateFramework(app.Framework));

	SGUI					& gui			= *framework.GUI;
	acid					toProcess		= {};
	if(1 == guiProcessControls(gui, [&app](cid_t iControl) { return one_if(iControl == app.IdExit); }))
		return 1;

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
		::std::lock_guard				lock					(app.Framework.LockGUI);
		guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard				lock					(app.Framework.LockGUI);
		guiDraw(*app.DialogMain.GUI, target->Color.View);
	}
	{
		::std::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
