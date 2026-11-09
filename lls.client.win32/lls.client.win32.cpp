// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "lls.client.win32.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "LLS Client v0.1");

static	::gpk::err_t	loadNetworkConfig	(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcst_t & remote_ip, ::gpk::vcst_t & remote_port) {
	::gpk::err_t				appNodeIndex;
	do { if_fail_fe(appNodeIndex = ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.test_udp_client"}, jsonConfig, 0)); } while(0);
	return ::gpk::loadClientConfig(jsonConfig, appNodeIndex, remote_ip, remote_port);
}

// --- Cleanup application resources.
::gpk::err_t			cleanup				(::SApplication & app)											{
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= framework.RootWindow;
	if_fail_w(::lls::llsUpdate(app.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	if_fail_w(::gpk::clientDisconnect(app.Client.SocketClient));
	if_fail_w(::gpk::mainWindowDestroy(mainWindow));
	if_fail_w(::lls::llsUpdate(app.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	if_fail_w(::gpk::tcpipShutdown());
	return 0;
}

static	::gpk::err_t	updateSizeDependentResources(::SApplication & app)											{
	::gpk::SWindow				& mainWindow		= app.Framework.RootWindow;
	const ::gpk::n2u1_t			newSize				= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer	= {};
	do { if_fail_fe(app.D3DApp.SetWindowSize(newSize)); } while(0);
#else
	do { if_fail_fe(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF)); } while(0);
#endif
	mainWindow.Resized		= false;
	return 0;
}

static	::gpk::err_t	processScreenEvent	(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		do { if_fail_fe(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		do { if_fail_fe(::updateSizeDependentResources(app)); } while(0);
		break;
	}
	return 0;
}

static	::gpk::err_t	processSystemEvent	(::SApplication & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); });
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return processGUIEvent	(app.Client, *app.Framework.GUI, screenEvent); });
	}
	return 0;
}

::gpk::err_t			setup				(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	do { if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)); } while(0);	// create platform widnow

	::gpk::SGUI					& gui				= *framework.GUI;
	do { if_fail_fe(::gpk::setupGUI(app.Client.UI, gui)); } while(0);

	do { if_fail_fe(::gpk::tcpipInitialize()); } while(0);
	if_fail_w(::loadNetworkConfig(framework.JSONConfig.Reader, app.Client.RemoteIp, app.Client.RemotePort));
	if_fail_e(::gpk::clientConnect(app.Client, *framework.GUI));	// Preemptively try to connect to the gate server. 

	return 0;
}

::gpk::err_t			update				(::SApplication & app, bool systemRequestedExit)					{
	::gpk::SFramework			& framework			= app.Framework;
	::gpk::SWindow				& mainWindow		= app.Framework.RootWindow;
	::gpk::SGUI					& gui				= *framework.GUI;

	bool						systemExit			= false;

	gpk::TQueueSystemEvent		eventsToProcess		= mainWindow.EventQueue;
	do { if_fail_fe(eventsToProcess.append(gui.Controls.EventQueue)); } while(0);
	do { if_fail_fe(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { 
		::gpk::err_t				result; 
		do { if_fail_fe(result = ::processSystemEvent(app, *sysEvent));  } while(0);
		if(result == 1) 
			systemExit				= true; 
		return result;
	})); } while(0);

	if_true_vif(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", ::gpk::b82sc(systemExit), ::gpk::b82sc(systemRequestedExit));

	::gpk::SFrameInfo			& frameInfo			= framework.FrameInfo;
	{	
		::gpk::STimer				timer				= {};
		if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::lls::APP_STATE_Quit == ::lls::llsClientUpdate(app.LLSApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue));
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	::gpk::pau0_t					payloadCache;
	app.LLSApp.EventsToSend.for_each([&app, &payloadCache](::gpk::pobj<::lls::EventLLS> & ev){
		payloadCache.create();
		do { if_fail_fe(ev->Save(*payloadCache)); } while(0);
		app.Client.QueueToSend.push_back(payloadCache);
 		return 0;
	});
	app.LLSApp.EventsToSend.clear();

	int32_t						clientResult;
	do { if_fail_fe(clientResult = ::gpk::clientUpdate(app.Client, gui)); } while(0);
	if_true_vif(::gpk::APPLICATION_STATE_EXIT, clientResult > 0, "User requested close (%i). Terminating execution.", clientResult);

	if_fail_e(app.Client.QueueReceived.for_each([&app](::gpk::pobj<::gpk::SMessage> & udp){ 
		if(udp && udp->Payload.size()) {
			::gpk::pobj<::lls::EventLLS>	eventReceived;
			::gpk::vcu0_t							inputBytes			= udp->Payload;
			if_fail_e(eventReceived->Load(inputBytes)); 
			if_fail_fe(app.LLSApp.EventsReceived.push_back(eventReceived));
		}
		return 0;
	}));

#if !defined(DISABLE_D3D11)
	if(app.LLSApp.ActiveState >= ::lls::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.LLSApp.Game.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		//do { if_fail_fe(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.D1.Game.Engine.Scene->Graphics));
		do { if_fail_fe(app.D3DApp.CreateDeviceResources(*app.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
	}
	app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FrameMeter.FramesPerSecond);
#endif

	//-----------------------------
	::gpk::STimer				& timer					= app.Framework.Timer;
	::gpk::sc_t						buffer	[256]			= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return ::gpk::updateFramework(framework);
}

::gpk::err_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::n3f2_t			sunlightPos				= {}; //::gpk::calcSunPosition();
	//const double				sunlightFactor			= .1;//:::gpk::calcSunlightFactor(app.LLSApp.World.WorldState.DaylightRatioExtra, app.LLSApp.World.WorldState.DaylightOffsetMinutes);
	const ::gpk::rgbaf			clearColor				= ::gpk::DARKBLUE * .0625;//::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, sunlightFactor);

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	do { if_fail_fe(::gpk::guiDraw(*app.LLSApp.GUI, app.D3DApp.GUIStuff.RenderTarget)); } while(0);

	app.D3DApp.Scene.ConstantBufferScene.Time	= (float)app.Framework.FrameInfo.Seconds.Total;

	const ::gpk::SCamera		& cameraSelected		= app.LLSApp.Game.Camera;
	const ::gpk::SEngineScene	& engineScene			= *app.LLSApp.Game.Engine.Scene;
	do { if_fail_fe(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, cameraSelected.NearFar)); } while(0);
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::prtbgra8d32			backBuffer				= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	do { if_fail_fe(::lls::llsDraw(app.LLSApp, *backBuffer, false)); } while(0);
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
