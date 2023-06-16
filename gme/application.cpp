#include "application.h"
#include "gpk_bitmap_file.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::STimer															timer;
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SWindow															& mainWindow				= framework.RootWindow;
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)), "Failed to create main window why?!");
	::gpk::SGUI																& gui						= *framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControlPlacement												& controlExit				= gui.Controls.Placement[app.IdExit];
	controlExit.Area													= {{0, 0}, {64, 20}};
	controlExit.Border													= {10, 10, 10, 10};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
	::gpk::controlSetParent(gui, app.IdExit, -1);
	timer.Frame();
	info_printf("Setup time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::error_t				draw			(::gme::SApplication & app)						{
	//::gpk::STimer															timer;
	app;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::SColorBGRA{}, 0xFFFFFFFF);
	{
		::std::lock_guard				lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::std::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//info_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::error_t				update						(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer					timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::std::lock_guard				lock					(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework				& framework					= app.Framework;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI						& gui						= *framework.GUI;
	::gpk::acid						controlsToProcess			= {};
	if(::gpk::guiProcessControls(gui, [&app](::gpk::cid_t idControl) {
		info_printf("Executed %u.", idControl);
		if(idControl == app.IdExit)
			return 1;
		return 0;
	})) 
		return 1;
	//timer.Frame();
	//info_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
