#include "gpk_file_explorer.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gfe::SApplication, "GPK File Explorer");

::gpk::err_t cleanup(::gfe::SApplication & app) { return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t setup	(::gfe::SApplication & ) { return 0; }
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

::gpk::err_t update(::gfe::SApplication & app, bool exitSignal) {
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
