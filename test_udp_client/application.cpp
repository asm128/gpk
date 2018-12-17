#include "application.h"
#include "gpk_bitmap_file.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"
#include "gpk_tcpip.h"
#include "gpk_chrono.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup					(::gme::SApplication & app)							{
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::clientDisconnect(app.Client);
	::gpk::tcpipShutdown();
	return 0;
}
			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
	::gpk::SGUI																& gui						= *framework.GUI;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
	controlExit.Area													= {{0, 0}, {64, 20}};
	controlExit.Border													= {1, 1, 1, 1};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToText.y								= app.IdExit;
	controlConstraints.AttachSizeToText.x								= app.IdExit;
	::gpk::controlSetParent(gui, app.IdExit, -1);
	::gpk::tcpipInitialize();

	app.Client.AddressConnect											= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, app.Client.AddressConnect);
	::gpk::clientConnect(app.Client);
	return 0;
}
			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");

	::gpk::SGUI																& gui						= *framework.GUI;
	::gpk::array_pod<uint32_t>												controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t																idControl					= controlsToProcess[iControl];
		const ::gpk::SControlState												& controlState				= gui.Controls.States[idControl];
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			if(idControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	reterr_error_if(app.Client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Failed to connect to server.")
	else {
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived!");
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 2");
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 3");
		::gpk::connectionPushData(app.Client, app.Client.Queue, "Message arrived! 4");
		::gpk::clientUpdate(app.Client);
		::gpk::sleep(1000);
	}

	::gpk::SUDPClient															& connectTest				= app.ClientTest1;
	connectTest.AddressConnect												= app.Client.AddressConnect;
	::gpk::clientConnect		(connectTest);
	::gpk::connectionPushData	(connectTest, connectTest.Queue, "Connect test!");
	::gpk::clientUpdate			(connectTest);
	::gpk::clientDisconnect		(connectTest);

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t												draw					(::gme::SApplication & app)						{
	::gpk::STimer																timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard															lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard															lock					(app.LockRender);
		app.Offscreen															= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
