#include "gpk_terminal.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_process_broker.h"

#include "gpk_parse.h"
#include "gpk_string_compose.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

using namespace gpk;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::brt::SApplication, "Module Explorer");

stxp	::gpk::vcst_c		JSON_GUI					= R"json(
{
	"GUI": {
		"Exit": {
			"Script": "{\"Runtime\":{\"Event\":{\"Type\":\"EXIT\"}}}",
			"State": {
				"Parent": -1
			},
			"Constraints": {
				"AttachSizeToText": {"x": false, "y": false}
			},
			"Placement": {
				"Area": {"Offset": {"x": 0, "y": 0}, "Size": {"x": 64, "y": 20}},
				"Margin": {"Left": 1, "Top": 1, "Right": 1, "Bottom": 1},
				"Border": {"Left": 1, "Top": 1, "Right": 1, "Bottom": 1},
				"Align": "BOTTOM_RIGHT"
			},
			"Text": {
				"Text": "Exit",
				"Align": "CENTER"
			}
		}
	}
}
)json";

::gpk::err_t	cleanup	(::brt::SApplication & app)	{
	::gpk::serverStop(app.Server);
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	::gpk::tcpipShutdown();
	::gpk::sleep(1000);
	return 0;
}

::gpk::err_t			setup	(::brt::SApplication & app)	{
	::gpk::SFramework			& framework					= app.Framework;
	::gpk::SWindow				& mainWindow				= framework.RootWindow;
	mainWindow.Size			= {320, 200};
	if_fail_fe(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));
	::gpk::SGUI					& gui						= *framework.GUI;
	::gpk::SJSONReader			jsonGUIReader				= {};
	jsonParse(jsonGUIReader, JSON_GUI);
	guiLoad(gui, jsonGUIReader);
	::gpk::tcpipInitialize();
	u3_t 					port						= 9998;
	u3_t 					adapter						= 0;
	{ // load port from config file
		::gpk::vcst_t				jsonPort					= {};
		cnst ::gpk::SJSONReader	& jsonReader				= framework.JSONConfig.Reader;
		cnst s2_t				indexObjectApp				= ::gpk::jsonExpressionResolve("application.brt", jsonReader, 0, app.ProcessFileName);
		if_true_wf(::gpk::failed(indexObjectApp), "Failed to find application node (%.*s) in json configuration file: '%s'", "application.brt", GPK_VIEW_ARGS(framework.FileNameJSONConfig))
		else {
			if_fail_wf(gpk::jsonExpressionResolve("process.executable_path"			, jsonReader, indexObjectApp, app.ProcessFileName	), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(app.ProcessFileName	));
			if_fail_wf(gpk::jsonExpressionResolve("process.command_line_app_name"	, jsonReader, indexObjectApp, app.ProcessMockPath	), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(app.ProcessMockPath	));
			if_fail_wf(gpk::jsonExpressionResolve("process.command_line_params"		, jsonReader, indexObjectApp, app.ProcessParams		), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(app.ProcessParams	));
			if_fail_wf(gpk::jsonExpressionResolve("listen_port"						, jsonReader, indexObjectApp, jsonPort				), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonPort))
			else {
				::gpk::parseIntegerDecimal(jsonPort, port);
				info_printf("Port to listen on: %u.", (u2_t )port);
			}
			jsonPort = "";
			if_true_wf(::gpk::failed(::gpk::jsonExpressionResolve("adapter", jsonReader, indexObjectApp, jsonPort)), "Failed to load config from json! Last contents found: %.*s.", GPK_VIEW_ARGS(jsonPort))
			else {
				::gpk::parseIntegerDecimal(jsonPort, adapter);
				info_printf("Adapter: %u.", (u2_t )adapter);
			}
		}
	}
	::gpk::string			& szCmdlineApp		= app.szCmdlineApp		= app.ProcessFileName;
	::gpk::string			& szCmdlineFinal	= app.szCmdlineFinal	= app.ProcessMockPath;
	if(app.ProcessParams.size()) {
		szCmdlineFinal.push_back(' ');
		szCmdlineFinal.append(app.ProcessParams);
	}
	szCmdlineFinal.push_back(0);
	szCmdlineApp.push_back(0);

	if_fail_fef(gpk::serverStart(app.Server, (u1_t)port, (s1_t)adapter), "Failed to start server on port %u. Port busy?", (u2_t )port);
	return 0;
}

::gpk::err_t			update						(::brt::SApplication & app, b8_t exitSignal)	{
	::gpk::STimer				timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard			lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));
	::gpk::apobj<::gpk::apobj<::gpk::SMessage>>	& receivedPerClient		= app.ReceivedPerClient;
	{	// pick up messages for later processing
		::gpk::lock_guard			lock						(app.Server.Mutex);
		receivedPerClient.resize(app.Server.Clients.size());
		for(u2_t  iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::pobj<::gpk::SConnection>	conn						= app.Server.Clients[iClient];
			::gpk::lock_guard					lockRecv					(conn->Queue.MutexReceive);
			*(receivedPerClient[iClient])	= app.Server.Clients[iClient]->Queue.Received;
			app.Server.Clients[iClient]->Queue.Received.clear();
		}
	}

	if_fail_ce(::gpk::resize_obj(receivedPerClient.size(), app.ClientIOHandles, app.ClientProcesses));
	{	// Exectue processes
		for(u2_t  iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			::gpk::SProcessHandles		& iohandles				= app.ClientIOHandles[iClient];
			::gpk::SProcess				& process				= app.ClientProcesses[iClient];
			for(u2_t  iMessage = 0; iMessage < receivedPerClient[iClient]->size(); ++iMessage) {
				::gpk::SMessage			& pClientMessage		= *(*receivedPerClient[iClient])[iMessage];
				::gpk::vu0_t				environmentBlock		= pClientMessage.Payload;
				info_printf("Client %i received: %.*s.", iClient, GPK_VIEW_ARGS(environmentBlock));
				// llamar proceso
				if_fail_ce(::gpk::initHandles(process, iohandles));
				::gpk::vcu0_t				payload					= environmentBlock;
				::gpk::err_t				contentOffset			= payload.find((u0_t)0U);
				if_fail_cef(contentOffset, "%s", "Failed to find environment block stop code.");
				if(payload.size() && (payload.size() > (u2_t)contentOffset + 2))
					if_fail_ef(::gpk::writeToPipe(app.ClientIOHandles[iClient], {&payload[contentOffset + 2], payload.size() - contentOffset - 2}), "%s", "Failed to write request content to process' stdin.");
				if_fail_ef(::gpk::createChildProcess(app.ClientProcesses[iClient], environmentBlock, app.szCmdlineApp, app.szCmdlineFinal), "Failed to create child process: %.*s.", GPK_VIEW_ARGS(app.ProcessFileName));	// Create the child process.
			}
		}
	}
	::gpk::sleep(10);
	::gpk::apobj<::gpk::apobj<::gpk::au0_t>>	& clientsResponses		= app.ClientResponses;
	if_fail_fef(clientsResponses.resize(receivedPerClient.size()), "current:%u,new:%u", clientsResponses.size(), receivedPerClient.size());
	{	// Read processes output if they're done processing.
		for(u2_t  iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			::gpk::pobj<::gpk::apobj<::gpk::au0_t>>	& clientResponses = clientsResponses[iClient];
			if_fail_fe(clientResponses->resize(receivedPerClient[iClient]->size()));
			::gpk::SProcessHandles	& iohandles				= app.ClientIOHandles[iClient];
			::gpk::SProcess			& process				= app.ClientProcesses[iClient];
			for(u2_t  iMessage = 0; iMessage < receivedPerClient[iClient]->size(); ++iMessage) {
				info_printf("Client %i received: %.*s.", iClient, (*(receivedPerClient[iClient]))[iMessage]->Payload.size(), (*(receivedPerClient[iClient]))[iMessage]->Payload.begin());
				// generar respuesta proceso
				//clientResponses[iClient][iMessage]		= "\r\n{ \"Respuesta\" : \"bleh\"}";
				(*clientResponses)[iMessage]->clear();
				if_fail_fef(::gpk::readFromPipe(process, iohandles, *(*clientResponses)[iMessage]), "iClient: %u, iMessage: %u.", iClient, iMessage);
				if_fail_fe(::gpk::closeProcessIO(process));
			}
		}
	}
	for(u2_t  iClient = 0; iClient < clientsResponses.size(); ++iClient) {
		::gpk::pobj<::gpk::apobj<::gpk::au0_t>>	& clientResponses = clientsResponses[iClient];
		for(u2_t  iMessage = 0; iMessage < clientResponses->size(); ++iMessage) { // contestar
			if((*clientResponses)[iMessage]->size()) {
				::gpk::lock_guard					lock						(app.Server.Mutex);
				::gpk::pobj<::gpk::SConnection>	conn						= app.Server.Clients[iClient];
				if_fail_fe(::gpk::connectionPushData(*conn, conn->Queue, *((*clientResponses)[iMessage]), true, true));
				receivedPerClient[iClient][iMessage]	= {};
				::gpk::SProcess						& process				= app.ClientProcesses[iClient];
				if_fail_fe(::gpk::closeProcess(process));

			}
		}
	}

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::err_t				draw					(::brt::SApplication & app)						{
	::gpk::STimer					timer;
	app;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, {0xFF, 0x40, 0x7F, 0xFF}, (u2_t )-1);
	{
		::gpk::lock_guard				lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	::gpk::sleep(15);
	return 0;
}
