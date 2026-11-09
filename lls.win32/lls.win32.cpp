#include "lls.win32.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "LLS v0.1");

static	::gpk::err_t	loadClientNetworkConfig	(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcst_t & remoteIp, ::gpk::vcst_t & remotePort) {
	::gpk::err_t				appNodeIndex;
	do { if_fail_fe(appNodeIndex = ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.test_udp_client"}, jsonConfig, 0)); } while(0);
	return ::gpk::loadClientConfig(jsonConfig, appNodeIndex, remoteIp, remotePort);
}

static	::gpk::err_t	loadServerNetworkConfig	(const ::gpk::SJSONReader & jsonConfig, ::gpk::vcst_t & port, ::gpk::vcst_t & adapter) {
	::gpk::err_t				appNodeIndex;
	do { if_fail_fe(appNodeIndex = ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.test_udp_server"}, jsonConfig, 0)); } while(0);
	return ::gpk::loadServerConfig(jsonConfig, appNodeIndex, port, adapter);
}

::gpk::err_t			cleanup					(::SApplication & app) {
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	if_fail_w(::lls::llsUpdate(app.Client.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	if_fail_w(::lls::llsUpdate(app.Server.LLSApp, 0, mainWindow.Input, {}, {}));

#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	if_fail_w(::gpk::clientDisconnect(app.Client.Network.SocketClient));
	if_fail_w(::gpk::serverStop(app.Server.Network->SocketServer));
	if_fail_w(::llswin32::processBrokerShutdown(app.ProcessBroker));
	if_fail_w(::gpk::mainWindowDestroy(mainWindow));
	if_fail_w(::lls::llsUpdate(app.Client.LLSApp, 0, mainWindow.Input, mainWindow.EventQueue, {}));
	if_fail_w(::lls::llsUpdate(app.Server.LLSApp, 0, mainWindow.Input, {}, {}));
	if_fail_w(::gpk::tcpipShutdown());
	return 0;
}

static	::gpk::err_t	updateSizeDependentResources(::SApplication & app) {
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	const ::gpk::n2u1_t			newSize					= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer		= {};
	do { if_fail_fe(app.D3DApp.SetWindowSize(newSize)); } while(0);
#else
	do { if_fail_fe(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF)); } while(0);
#endif
	mainWindow.Resized			= false;
	return 0;
}

static	::gpk::err_t	processScreenEvent		(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) {
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		do { if_fail_fe(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.Server.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
#endif
	case ::gpk::EVENT_SCREEN_Resize:
		do { if_fail_fe(::updateSizeDependentResources(app)); } while(0);
		break;
	}
	return 0;
}

static	::gpk::err_t	processGUIEvent			(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & guiEvent) {
	::gpk::err_t				result;
	do { if_fail_fe(result = ::gpk::processGUIEvent(app.Client.Network, *app.Framework.GUI, guiEvent)); } while(0);
	if(result)
		return result;
	return ::gpk::processGUIEvent(*app.Server.Network, *app.Framework.GUI, guiEvent);
}

static	::gpk::err_t	processSystemEvent		(::SApplication & app, const ::gpk::SEventSystem & systemEvent) {
	switch(systemEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(systemEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return ::processScreenEvent(app, screenEvent); });
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(systemEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & guiEvent) { return ::processGUIEvent(app, guiEvent); });
	}
	return 0;
}

::gpk::err_t			setup					(::SApplication & app) {
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size				= {1280, 720};
	do { if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)); } while(0);

	do { if_fail_fe(::gpk::setupGUI(app.Client.Network.UI, *framework.GUI)); } while(0);
	do { if_fail_fe(::gpk::setupGUI(app.Server.Network->UI, *framework.GUI)); } while(0);

	do { if_fail_fe(::gpk::tcpipInitialize()); } while(0);
	do { if_fail_fe(::llswin32::processBrokerSetup(app.ProcessBroker, framework.JSONConfig.Reader)); } while(0);

	if_fail_w(::loadServerNetworkConfig(framework.JSONConfig.Reader, app.Server.Network->Port, app.Server.Network->Adapter));
	if_fail_e(::gpk::serverStart(*app.Server.Network, *framework.GUI));

	if_fail_w(::loadClientNetworkConfig(framework.JSONConfig.Reader, app.Client.Network.RemoteIp, app.Client.Network.RemotePort));
	if_fail_e(::gpk::clientConnect(app.Client.Network, *framework.GUI));
	return 0;
}

::gpk::err_t			update					(::SApplication & app, bool systemRequestedExit) {
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	::gpk::SGUI					& gui					= *framework.GUI;

	bool						systemExit				= false;
	::gpk::TQueueSystemEvent	eventsToProcess			= mainWindow.EventQueue;
	do { if_fail_fe(eventsToProcess.append(gui.Controls.EventQueue)); } while(0);
	do { if_fail_fe(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SEventSystem> & systemEvent) {
		::gpk::err_t				result;
		do { if_fail_fe(result = ::processSystemEvent(app, *systemEvent)); } while(0);
		if(result == 1)
			systemExit				= true;
		return result;
	})); } while(0);

	if_true_vif(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", ::gpk::b82sc(systemExit), ::gpk::b82sc(systemRequestedExit));

	::gpk::SFrameInfo			& frameInfo				= framework.FrameInfo;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::lls::APP_STATE_Quit == ::lls::llsServerUpdate(app.Server.LLSApp, frameInfo.Seconds.LastFrame, mainWindow.Input, {}));
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::lls::APP_STATE_Quit == ::lls::llsClientUpdate(app.Client.LLSApp, frameInfo.Seconds.LastFrame, mainWindow.Input, mainWindow.EventQueue));

	::gpk::pau0_t					payloadCache;
	app.Client.LLSApp.EventsToSend.for_each([&app, &payloadCache](::gpk::pobj<::lls::EventLLS> & eventToSend) {
		payloadCache.create();
		do { if_fail_fe(eventToSend->Save(*payloadCache)); } while(0);
		app.Client.Network.QueueToSend.push_back(payloadCache);
		return 0;
	});
	app.Client.LLSApp.EventsToSend.clear();

	::gpk::err_t				serverResult;
	if_fail_e(serverResult = ::gpk::serverUpdate(*app.Server.Network, gui));
	if_true_vif(::gpk::APPLICATION_STATE_EXIT, serverResult == 1, "%s", "Server requested close. Terminating execution.");
	if_fail_e(::llswin32::processBrokerUpdate(app.ProcessBroker));

	::gpk::err_t				clientResult;
	do { if_fail_fe(clientResult = ::gpk::clientUpdate(app.Client.Network, gui)); } while(0);
	if_true_vif(::gpk::APPLICATION_STATE_EXIT, clientResult > 0, "Client requested close (%i). Terminating execution.", clientResult);

	if_fail_e(app.Server.Network->QueueReceived.for_each([&app](::gpk::TPacketQueue & messages) {
		messages.enumerate([&app](uint32_t & index, ::gpk::pobj<::gpk::SMessage> & message) {
			if(!message)
				return 0;
			::gpk::vcu0_t					input					= message->Payload;
			::gpk::pobj<::lls::EventLLS>	receivedEvent;
			if_fail_fe(receivedEvent->Load(input));
			info_printf("Received '%s' from client %i: %s.", ::gpk::get_enum_namep(receivedEvent->Type), index, ::gpk::get_value_namep(receivedEvent->Type));
			if_fail_fe(app.Server.LLSApp.EventsReceived.push_back(receivedEvent));
			return 0;
		});
		return messages.clear();
	}));

	if_fail_e(app.Client.Network.QueueReceived.for_each([&app](::gpk::pobj<::gpk::SMessage> & message) {
		if(message && message->Payload.size()) {
			::gpk::pobj<::lls::EventLLS>	receivedEvent;
			::gpk::vcu0_t					input					= message->Payload;
			if_fail_e(receivedEvent->Load(input));
			if_fail_fe(app.Client.LLSApp.EventsReceived.push_back(receivedEvent));
		}
		return 0;
	}));

#if !defined(DISABLE_D3D11)
	if((app.Server.LLSApp.ActiveState >= ::lls::APP_STATE_Welcome && app.D3DApp.Scene.IndexBuffer.size() < app.Server.LLSApp.Game.Engine.Scene->Graphics->Meshes.size()) || !app.D3DApp.GUIStuff.IndexBuffer)
		do { if_fail_fe(app.D3DApp.CreateDeviceResources(*app.Server.LLSApp.Game.Engine.Scene->Graphics)); } while(0);
	do { if_fail_fe(app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FrameMeter.FramesPerSecond)); } while(0);
#endif

	::gpk::STimer				& timer					= framework.Timer;
	::gpk::sc_t					buffer[256]				= {};
	sprintf_s(buffer, "LLS [%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	SetWindowTextA(mainWindow.PlatformDetail.WindowHandle, buffer);
	return ::gpk::updateFramework(framework);
}

::gpk::err_t			draw					(::SApplication & app) {
	const ::gpk::n3f2_t			sunlightPos			= {};
	const ::gpk::rgbaf			clearColor				= ::gpk::DARKBLUE * .0625;

#if !defined(DISABLE_D3D11)
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	do { if_fail_fe(::gpk::guiDraw(*app.Framework.GUI, app.D3DApp.GUIStuff.RenderTarget)); } while(0);
	do { if_fail_fe(::lls::llsClientUIDraw(app.Client.LLSApp, app.D3DApp.GUIStuff.RenderTarget)); } while(0);
	app.D3DApp.Scene.ConstantBufferScene.Time = (float)app.Framework.FrameInfo.Seconds.Total;

	const ::gpk::SCamera		& cameraSelected		= app.Server.LLSApp.Game.Camera;
	const ::gpk::SEngineScene	& engineScene			= *app.Server.LLSApp.Game.Engine.Scene;
	do { if_fail_fe(::gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, cameraSelected.NearFar)); } while(0);
#else
	::gpk::prtbgra8d32			backBuffer				= app.Framework.RootWindow.BackBuffer;
	backBuffer->resize(backBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	const ::gpk::SCamera		& cameraSelected		= app.Server.LLSApp.Game.Camera;
	do { if_fail_fe(::lls::llsGameDraw(app.Server.LLSApp.Game, *backBuffer, cameraSelected.Position, cameraSelected.Target, cameraSelected.Up, cameraSelected.NearFar)); } while(0);
	do { if_fail_fe(::gpk::guiDraw(*app.Framework.GUI, backBuffer->Color.View)); } while(0);
	do { if_fail_fe(::lls::llsClientUIDraw(app.Client.LLSApp, backBuffer->Color.View)); } while(0);
#endif
	return 0;
}
