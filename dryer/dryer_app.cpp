#include "dryer_app.h"
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::dry::SApplication, "Dryer v0.1");

usns gpk;

::gpk::err_t					cleanup						(::dry::SApplication & app) {
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	if(app.SerialThread.joinable()) {
		gpk_sync_decrement(app.ThreadClients);
		{
			::gpk::STimer						timer;
			double								elapsedTime					= 0;
			const double						timeOut						= 10.0;
			while(-1 != gpk_sync_compare_exchange(app.ThreadClients, -1, 0)) {		// Wait until the serial thread is closed.
				if(elapsedTime > timeOut) {
					error_printf("%s", "Thread taking too long to finish. Terminating...");
					break;
				}
				::gpk::sleep(100);
				timer.Frame();
				elapsedTime						+= timer.LastTimeSeconds;
			}
#if defined(GPK_LINUX)
			if(elapsedTime >= timeOut) // unblocks the thread if it's blocked by an io call and can't read the loop close flag
				pthread_cancel((pthread_t)app.SerialThread.native_handle());
#endif
		}
		::gpk::sleep(250);
		app.SerialThread.join();
	}
	info_printf("%s", "bye");
	return 0;
}

::gpk::err_t					setup						(::dry::SApplication & app) {
	auto								& framework					= app.Framework;
	framework.RootWindow.Size		= {640, 480};//{720, 576};
	{ if_fail_fef(::gpk::mainWindowCreate(framework.RootWindow, framework.RuntimeValues.PlatformDetail, framework.RootWindow.Input), "%s", "Failed to initialize framework."); }
	framework.RootWindow.BackBuffer->resize(framework.RootWindow.Size.u2(), 0xFFFFFFFFU, 0xFFFFFFFFU);
#if !defined(GPK_WINDOWS)
	gpk_sync_increment(app.ThreadClients);
	app.SerialThread				=
		::std::thread([&app]() {
				::dry::updateSerial(app);
			});
	::gpk::sleep(100);
#endif
	if_fail_ef(::gpk::failed(::dry::catalogLoad(app.Catalog, "./filaments.json")), "%s", "Failed to load filament catalog.");
	if_fail_e(::gpk::failed(::dry::dryerSetup(app.Dryer)));
	if_fail_e(::gpk::failed(::dry::setupGUI(app)));
	return 0;
}

::gpk::err_t					update						(::dry::SApplication & app, bool requestedExit) {
	if_true_vif(::gpk::APPLICATION_STATE_EXIT, requestedExit, "%s", "Exit requested by system.");
	if_true_vif(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");
#if defined(GPK_LINUX)
	if((time(0) - app.TimeLastSerial) > app.SERIAL_TIMEOUT) {
		pthread_cancel(app.SerialThread.native_handle());
		::gpk::sleep(1000);
		app.SerialThread				=
			::std::thread([&app]() {
				::dry::updateSerial(app);
			});
		::gpk::sleep(500);
	}
#endif

	const bool							running						= ::dry::updateApp(app) ? false : true;
	::gpk::sleep(10);
	return running ? 0 : 1;
}

::gpk::err_t					draw						(::dry::SApplication & app) {
	auto								& framework					= app.Framework;
	if(framework.RootWindow.Repaint) {
		verbose_printf("%s", "Repainting...");
		::gpk::SGUI							& gui						= *framework.GUI;
		::gpk::pobj<::gpk::rt<::gpk::bgra, u2_t >>
											& guiRenderTarget			= framework.RootWindow.BackBuffer;
		{ if_fail_fef(guiRenderTarget->resize(framework.RootWindow.Size.u2(), ::gpk::bgra{0x10, 0x04, 0x08, 0xFF}, 0xFFFFFFFFU), "Failed to resize render target to display size: [%u x %u]", framework.RootWindow.Size.x, framework.RootWindow.Size.y); }
		::gpk::guiDraw(gui, guiRenderTarget->Color.View);
	}
	return 0;
}
