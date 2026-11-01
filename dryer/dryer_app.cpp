#include "dryer_app.h"
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::dry::SApplication, "Dryer v0.1");

::gpk::error_t					cleanup						(::dry::SApplication & app) {
#if defined(GPK_WINDOWS)
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
#else
	app.Framework.MainDisplay.Closed	= true;
#endif
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
	info_printf("%s", "bye");
	return 0;
}

::gpk::error_t					setup						(::dry::SApplication & app) {
	auto								& framework					= app.Framework;
#if defined(GPK_WINDOWS)
	framework.Input.create();
	framework.MainDisplay.Size		= {640, 480};//{720, 576};
	gpk_necall(::gpk::mainWindowCreate(framework.MainDisplay, framework.RuntimeValues.PlatformDetail, framework.Input), "%s", "Failed to initialize framework.");
#else
	gpk_necall(::s3d::mainWindowCreate(framework), "%s", "Failed to initialize framework.");
#endif
	framework.MainDisplayOffscreen->resize(framework.MainDisplay.Size, 0xFFFFFFFFU, 0xFFFFFFFFU);
	gpk_sync_increment(app.ThreadClients);
	app.SerialThread				= //::std::thread(&::dry::updateSerial, app);
		::std::thread([&app]() {
				::dry::updateSerial(app);
			});
	::gpk::sleep(100);
	e_if(errored(::dry::catalogLoad(app.Catalog, "./filaments.json")), "%s", "Failed to load filament catalog.");
	e_if(errored(::dry::dryerSetup(app.Dryer)), "%s", "");
	e_if(errored(::dry::setupGUI(app)), "%s", "");
	return 0;
}

::gpk::error_t					update						(::dry::SApplication & app, bool requestedExit) {
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, requestedExit, "%s", "Exit requested by system.");
#if defined(GPK_WINDOWS)
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");
#else
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::s3d::frameworkUpdate(app.Framework), "%s", "Exit requested by framework update.");
#endif
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

::gpk::error_t					draw						(::dry::SApplication & app) {
	auto								& framework					= app.Framework;
	if(framework.MainDisplay.Repaint) {
		verbose_printf("%s", "Repainting...");
		::gpk::SGUI							& gui						= *framework.GUI;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>
											& guiRenderTarget			= framework.MainDisplayOffscreen;
		gpk_necall(guiRenderTarget->resize(framework.MainDisplay.Size.Cast<uint32_t>(), ::gpk::SColorBGRA{0x10, 0x04, 0x08, 0xFF}, 0xFFFFFFFFU), "Failed to resize render target to display size: [%u x %u]", framework.MainDisplay.Size.x, framework.MainDisplay.Size.y);
		::gpk::guiDraw(gui, guiRenderTarget->Color.View);
	}
	return 0;
}
