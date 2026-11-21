// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "lls.server.win32.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "LLS Server v0.1");

usns gpk;

sttc	gpk::err_t	loadNetworkConfig		(cnst SJSONReader & jsonConfig, vcst_t & port, vcst_t & adapter) {
	gpk::err_t				appNodeIndex;
	do { if_fail_fe(appNodeIndex = jsonExpressionResolve("application.test_udp_server", jsonConfig, 0)); } while(0);
	return loadServerConfig(jsonConfig, appNodeIndex, port, adapter);
}

// --- Cleanup application resources.
gpk::err_t			cleanup					(::SApplication & app)											{
	SFramework			& framework				= app.Framework;
	SWindow				& mainWindow			= framework.RootWindow;
	if_fail_w(::lls::llsUpdate(app.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));

#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	if_fail_w(serverStop(app.Server->SocketServer));
	if_fail_w(mainWindowDestroy(mainWindow));
	if_fail_w(::lls::llsUpdate(app.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	if_fail_w(tcpipShutdown());
	return 0;
}

sttc	gpk::err_t	updateSizeDependentResources(::SApplication & app)											{
	SWindow				& mainWindow			= app.Framework.RootWindow;
	n2u1_c			newSize					= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer	= {};
	do { if_fail_fe(app.D3DApp.SetWindowSize(newSize)); } while(0);
#else
	do { if_fail_fe(mainWindow.BackBuffer->resize(newSize, bgra{0, 0, 0, 0}, 0xFFFFFFFF)); } while(0);
#endif
	mainWindow.Resized		= false;

	return 0;
}

sttc	gpk::err_t	processScreenEvent		(::SApplication & app, cnst SEventView<EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		do { if_fail_fe(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
#endif
	case EVENT_SCREEN_Resize: 
		do { if_fail_fe(::updateSizeDependentResources(app)); } while(0);
		break;
	}
	return 0;
}

sttc	gpk::err_t	processSystemEvent		(::SApplication & app, cnst SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case SYSTEM_EVENT_Screen	: return eventExtractAndHandle<EVENT_SCREEN		>(sysEvent, [&app](cnst SEventView<EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); }); 
	case SYSTEM_EVENT_GUI	: return eventExtractAndHandle<EVENT_GUI_CONTROL	>(sysEvent, [&app](cnst SEventView<EVENT_GUI_CONTROL	> & screenEvent) { return processGUIEvent(*app.Server, *app.Framework.GUI, screenEvent); }); 
	}
	return 0;
}

gpk::err_t			setup					(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	SFramework			& framework				= app.Framework;
	SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	do { if_fail_fe(mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)); } while(0);

	do { if_fail_fe(setupGUI(app.Server->UI, *framework.GUI)); } while(0);

	do { if_fail_fe(tcpipInitialize()); } while(0);

	if_fail_w(::loadNetworkConfig(framework.JSONConfig.Reader, app.Server->Port, app.Server->Adapter));
	if_fail_e(serverStart(*app.Server, *framework.GUI));

	return 0;
}

gpk::err_t			update					(::SApplication & app, b8_t systemRequestedExit) {
	SFramework			& framework				= app.Framework;
	SWindow				& mainWindow			= app.Framework.RootWindow;
	SGUI					& gui					= *framework.GUI;

	b8_t						systemExit				= false;

	TQueueSystemEvent	eventsToProcess			= mainWindow.EventQueue;
	do { if_fail_fe(eventsToProcess.append(gui.Controls.EventQueue)); } while(0);
	do { if_fail_fe(eventsToProcess.for_each([&app, &systemExit](cnst pobj<SEventSystem> & sysEvent) { 
		gpk::err_t				result; 
		do { if_fail_fe(result = ::processSystemEvent(app, *sysEvent)); } while(0); 
		if(result == 1) 
			systemExit				= true; 
		return result;
	})); } while(0);

	if_true_vif(APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", (sc_c*)b82st(systemExit), (sc_c*)b82st(systemRequestedExit));

	SFrameInfo			& frameInfo				= framework.FrameInfo;
	{
		STimer				timer					= {};
		if_true_vi(APPLICATION_STATE_EXIT, ::lls::APP_STATE_Quit == ::lls::llsServerUpdate(app.LLSApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue));
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

	int32_t						serverResult;
	if_fail_e(serverResult = serverUpdate(*app.Server, gui));
	if_true_vif(APPLICATION_STATE_EXIT, serverResult == 1, "%s", "User requested close. Terminating execution.");

	if_fail_e(app.Server->QueueReceived.for_each([&app](TPacketQueue & messages) {
		messages.enumerate([&app](uint32_t & index, pobj<SMessage> & message) {
			if(!message)
				return 0;

			vcu0_t						input					= message->Payload;
			gpk::pobj<lls::EventLLS>	newEvent;
			if_fail_fe(newEvent->Load(input));
			info_printf("Received '%s' from client %i: %s.", get_enum_namep(newEvent->Type), index, get_value_namep(newEvent->Type));
			if_fail_fe(app.LLSApp.EventsReceived.push_back(newEvent));
			return 0;
		});
		return messages.clear();
	}));

#if !defined(DISABLE_D3D11)
	if(app.LLSApp.ActiveState >= ::lls::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.LLSApp.Game.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		//do { if_fail_fe(app.D3DApp.CreateDeviceDependentEngineResources(app.D3DApp.DeviceResources->GetD3DDevice(), *app.D1.Game.Engine.Scene->Graphics));
		do { if_fail_fe(app.D3DApp.CreateDeviceResources(*app.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
	}
	do { if_fail_fe(app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FrameMeter.FramesPerSecond)); } while(0);
#endif

	//-----------------------------
	STimer				& timer					= app.Framework.Timer;
	sc_t						buffer	[256]			= {};
	gpk::sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return updateFramework(framework);
}

gpk::err_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	cnst n3f2_t			sunlightPos				= {}; //calcSunPosition();
	//cnst double				sunlightFactor			= .1;//:calcSunlightFactor(app.LLSApp.World.WorldState.DaylightRatioExtra, app.LLSApp.World.WorldState.DaylightOffsetMinutes);
	cnst rgbaf			clearColor				= DARKBLUE * .0625;//interpolate_linear(DARKBLUE * .25, LIGHTBLUE * 1.1, sunlightFactor);

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	do { if_fail_fe(guiDraw(*app.LLSApp.GUI, app.D3DApp.GUIStuff.RenderTarget)); } while(0);

	app.D3DApp.Scene.ConstantBufferScene.Time	= (float)app.Framework.FrameInfo.Seconds.Total;

	cnst SCamera		& cameraSelected		= app.LLSApp.Game.Camera;
	cnst SEngineScene	& engineScene			= *app.LLSApp.Game.Engine.Scene;
	do { if_fail_fe(d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, cameraSelected.NearFar)); } while(0);
#else 
	SFramework			& framework				= app.Framework;
	prtbgra8d32			backBuffer				= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	do { if_fail_fe(::lls::llsDraw(app.LLSApp, *backBuffer, false)); } while(0);
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
