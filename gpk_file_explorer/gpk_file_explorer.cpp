#include "gpk_file_explorer.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gfe::SApplication, "GPK File Explorer");

::gpk::err_t cleanup(::gfe::SApplication & app) { return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t setup	(::gfe::SApplication & app) {
	::gpk::SWindow				& window				= app.Framework.RootWindow;
	window.Size											= {1280, 720};
	if_null_fe(window.BackBuffer.create());
	if_fail_fe(window.BackBuffer->resize(window.Size.u2()));
	if_fail_fe(::gpk::mainWindowCreate(window, app.Framework.RuntimeValues.PlatformDetail, window.Input));
	app.DialogMain.Input								= window.Input;

	sttc cnst ::gpk::vcst_t	labelExit				= "EXIT";
	::gpk::SControlPlacement	placement				= {};
	placement.Area										= {{8, 8}, {96, 24}};
	placement.Align									= ::gpk::ALIGN_BOTTOM_RIGHT;
	placement.Border									= {1, 1, 1, 1};
	placement.Margin									= {1, 1, 1, 1};
	if_fail_fe(app.IdExit = ::gpk::guiCreateControlList(*app.Framework.GUI, ::gpk::DIRECTION_RIGHT, {&labelExit, 1}, app.DialogMain.Root, placement, ::gpk::ALIGN_CENTER, {&app.IdExit, 1}));
	return 0;
}
::gpk::err_t draw	(::gfe::SApplication & app) {
	::gpk::pobj<::gpk::rtbgra8d32>	target					= {};
	target.create();
	target->resize(app.Framework.RootWindow.Size, ::gpk::SColorBGRA{}, 0xFFFFFFFF);
	{
		::gpk::lock_guard			lock					(app.LockGUI);
		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::gpk::lock_guard			lock					(app.LockRender);
		app.Offscreen									= target;
	}
	return 0;
}

::gpk::err_t update(::gfe::SApplication & app, gpk::b8_t exitSignal) {
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard			lock					(app.LockRender);
		app.Framework.RootWindow.BackBuffer				= app.Offscreen;
	}
	::gpk::lock_guard			lockGUI					(app.LockGUI);
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui					= *app.Framework.GUI;
	if(::gpk::guiProcessControls(gui, [&app](::gpk::cid_t idControl) { return idControl == app.IdExit ? 1 : 0; }))
		return ::gpk::APPLICATION_STATE_EXIT;

	if_fail_fe(app.DialogMain.Update());
	return 0;
}
