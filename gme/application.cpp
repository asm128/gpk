#include "application.h"
#include "gpk_bitmap_file.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

::gpk::error_t												setup					(::gme::SApplication & app)						{ 
	::gpk::SFramework												& framework				= app.Framework;
	::gpk::SDisplay													& mainWindow			= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::gpk::SGUI														& gui					= framework.GUI;
	app.IdExit													= ::gpk::controlCreate(gui);
	::gpk::SControl													& controlExit			= gui.Controls.Controls[app.IdExit];
	controlExit.Area											= {{0, 0}, {64, 20}};
	controlExit.Border											= {10, 10, 10, 10};
	controlExit.Margin											= {1, 1, 1, 1};
	controlExit.Align											= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText												& controlText			= gui.Controls.Text[app.IdExit];
	controlText.Text											= "Exit";
	controlText.Align											= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints										& controlConstraints	= gui.Controls.Constraints[app.IdExit];
	controlConstraints.IndexControlToAttachWidthTo				= app.IdExit;

	::gpk::controlSetParent(gui, app.IdExit, -1);
	return 0; 
}

::gpk::error_t												update					(::gme::SApplication & app, bool exitSignal)	{ 
	::gpk::STimer													timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen							= app.Offscreen;
	}
	::gpk::SFramework												& framework				= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI														& gui					= framework.GUI;
	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::guiProcessInput(gui, *app.Framework.Input);
	}
	if(app.Framework.Input->MouseCurrent.Deltas.z)
		gui.Zoom.ZoomLevel										+= app.Framework.Input->MouseCurrent.Deltas.z * (1.0f / (120 * 4));
 
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		if(gui.Controls.States[iControl].Unused || gui.Controls.States[iControl].Disabled)
			continue;
		if(gui.Controls.States[iControl].Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}

::gpk::error_t												cleanup					(::gme::SApplication & app)						{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	return 0; 
}

::gpk::error_t												draw					(::gme::SApplication & app)						{ 
	::gpk::STimer													timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget>							target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(app.Framework.MainDisplay.Size);
	//::gpk::clearTarget(*target);
	{
		::gme::mutex_guard												lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gme::mutex_guard												lock					(app.LockRender);
		app.Offscreen												= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}