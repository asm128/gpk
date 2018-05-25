#include "application.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

::gpk::error_t			setup	(::gme::SApplication & app)						{ 
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SDisplay																& mainWindow								= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	return 0; 
}

::gpk::error_t			update	(::gme::SApplication & app, bool exitSignal)	{ 
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal															, "Exit requested by runtime.");
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");
	//::gpk::SFramework															& framework									= app.Framework;
	//::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	return 0; 
}

::gpk::error_t			cleanup	(::gme::SApplication & app)						{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	return 0; 
}

::gpk::error_t			draw	(::gme::SApplication & app)						{ 
	app;
	return 0; 
}