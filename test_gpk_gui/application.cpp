#include "application.h"

#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

::gpk::error_t									setup					(::gme::SApplication & app)						{ 
	::gpk::SFramework									& framework				= app.Framework;
	::gpk::SDisplay										& mainWindow			= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	int32_t												controlTestRoot			= ::gpk::controlCreate(app.GUI);
	app.GUI.Controls[controlTestRoot].Area			= {{4, 4}, {320, 240}};
	app.GUI.Controls[controlTestRoot].Border		= {0, 0, 0, 0};
	app.GUI.Controls[controlTestRoot].Margin		= {0, 0, 0, 0};
	app.GUI.Controls[controlTestRoot].Align			= ::gpk::ALIGN_CENTER;
	::gpk::controlSetParent(app.GUI, controlTestRoot, -1);

	for(uint32_t iChild = 0; iChild < 90; ++iChild) {
		int32_t												controlTestChild0		= ::gpk::controlCreate(app.GUI);
		app.GUI.Controls[controlTestChild0].Area		= {{0, 0}, {(int32_t)(320 / 3 / (1 + iChild / 9)), (int32_t)(240 / 3 / (1 + iChild / 9))}}; // {32, 32}};//
		app.GUI.Controls[controlTestChild0].Border		= {1, 1, 1, 1};
		app.GUI.Controls[controlTestChild0].Margin		= {2, 2, 2, 2};
		switch(iChild % 9) {
		case 0: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_TOP_LEFT			; break;
		case 1: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_CENTER_TOP			; break;
		case 2: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_TOP_RIGHT			; break;
		case 3: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_CENTER_LEFT		; break;
		case 4: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_CENTER				; break;
		case 5: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_CENTER_RIGHT		; break;
		case 6: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_BOTTOM_LEFT		; break;
		case 7: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_CENTER_BOTTOM		; break;
		case 8: app.GUI.Controls[controlTestChild0].Align = ::gpk::ALIGN_BOTTOM_RIGHT		; break;
		default:
			app.GUI.Controls[controlTestChild0].Align = {};
		}
		::gpk::controlSetParent(app.GUI, controlTestChild0, iChild / 9);
	}
	return 0; 
}

::gpk::error_t									update					(::gme::SApplication & app, bool exitSignal)	{ 
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gme::mutex_guard									lock					(app.LockRender);
		app.Framework.MainDisplayOffscreen				= app.Offscreen;
	}
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");
	return 0; 
}

::gpk::error_t									cleanup					(::gme::SApplication & app)						{ 
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	return 0; 
}

::gpk::error_t									draw					(::gme::SApplication & app)						{ 
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget>				target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(app.Framework.MainDisplay.Size);
	::gpk::clearTarget(*target);
	::gpk::controlPaintHierarchy(app.GUI, 0, target->Color.View);

	{
		::gme::mutex_guard									lock					(app.LockRender);
		app.Offscreen									= target;
	}
	return 0; 
}