#include "brt_application_main.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"

#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

using namespace gpk;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::brt::SApplication, "Module Explorer");

::gpk::err_t	cleanup	(::brt::SApplication & app)	{
	app.ProcessBroker.Jobs.clear();
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
	app.IdExit				= ::gpk::controlCreate(gui);
	::gpk::SControlPlacement	& controlExit				= gui.Controls.Placement[app.IdExit];
	controlExit.Area		= {{0, 0}, {64, 20}};
	controlExit.Border		= {1, 1, 1, 1};
	controlExit.Margin		= {1, 1, 1, 1};
	controlExit.Align		= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText			& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text		= "Exit";
	controlText.Align		= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints	& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToText.y	= app.IdExit;
	controlConstraints.AttachSizeToText.x	= app.IdExit;
	::gpk::controlSetParent(gui.Controls, app.IdExit, (::gpk::cid_t)-1);
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
	::gpk::SProcessBroker	& processBroker			= app.ProcessBroker;
	processBroker.CommandLineApp					= app.ProcessFileName;
	processBroker.CommandLine						= app.ProcessMockPath;
	if(app.ProcessParams.size()) {
		processBroker.CommandLine.push_back(' ');
		processBroker.CommandLine.append(app.ProcessParams);
	}
	processBroker.CommandLine.push_back(0);
	processBroker.CommandLineApp.push_back(0);

	if_fail_fef(gpk::serverStart(app.Server, (u1_t)port, (s1_t)adapter), "Failed to start server on port %u. Port busy?", (u2_t )port);
	return 0;
}

sttc ::gpk::err_t	processClientMessage
	( ::gpk::SProcessBroker			& broker
	, ::gpk::SSocketServer			& server
	, ::gpk::pobj<::gpk::SConnection>	& connection
	, cnst ::gpk::SMessage			& message
	) {
	::gpk::pobj<::gpk::SProcessBrokerJob>	job;
	job.create();
	job->Request								= message.Payload;
	job->CommandLineApp						= broker.CommandLineApp;
	job->CommandLine							= broker.CommandLine;
	if_fail_fe(broker.Jobs.push_back(job));
	job->Result								= ::gpk::processBrokerRun(*job);
	::gpk::err_t				responseResult				= 0;
	if(::gpk::failed(job->Result) && 0 == job->Response.size()) {
		stxp ::gpk::vcst_c	failureResponse				= "BRT process broker failed to execute the configured process.";
		responseResult							= job->Response.append(failureResponse.bytes());
	}
	if(job->ErrorOutput.size())
		warning_printf("Process error: %.*s", (int)job->ErrorOutput.size(), (cnst sc_t*)job->ErrorOutput.begin());
	::gpk::err_t				sendResult					= 0;
	if(connection && job->Response.size()) {
		::gpk::lock_guard			lock						(server.Mutex);
		sendResult								= ::gpk::connectionPushData(*connection, connection->Queue, job->Response, true, true);
	}
	cnst ::gpk::err_t		removeResult					= broker.Jobs.remove_unordered(broker.Jobs.size() - 1);
	if_fail_fe(responseResult);
	if_fail_fe(sendResult);
	if_fail_fe(removeResult);
	return 0;
}

::gpk::err_t			update						(::brt::SApplication & app, b8_t exitSignal)	{
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard			lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework					= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));
	::gpk::SGUI					& gui						= *framework.GUI;
	::gpk::controlHandle(gui.Controls, [&app](::gpk::cid_t iControl) {
			info_printf("Executed %u.", iControl);
			if(iControl == (::gpk::cid_t)app.IdExit)
				return 1;
			return 0;
		});
	::gpk::apobj<::gpk::apobj<::gpk::SMessage>>	& receivedPerClient		= app.ReceivedPerClient;
	{
		::gpk::lock_guard			lock						(app.Server.Mutex);
		if_fail_fe(receivedPerClient.resize(app.Server.Clients.size()));
		for(u2_t iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::pobj<::gpk::SConnection>	connection				= app.Server.Clients[iClient];
			if(!connection)
				continue;
			::gpk::lock_guard			lockReceive				(connection->Queue.MutexReceive);
			*receivedPerClient[iClient]					= connection->Queue.Received;
			connection->Queue.Received.clear();
		}
	}

	for(u2_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
		::gpk::pobj<::gpk::SConnection>	connection;
		{
			::gpk::lock_guard			lock						(app.Server.Mutex);
			if(iClient < app.Server.Clients.size())
				connection							= app.Server.Clients[iClient];
		}
		if(!connection || !receivedPerClient[iClient])
			continue;
		::gpk::apobj<::gpk::SMessage>	& messages					= *receivedPerClient[iClient];
		for(u2_t iMessage = 0; iMessage < messages.size(); ++iMessage) {
			::gpk::pobj<::gpk::SMessage>	& message					= messages[iMessage];
			if(!message)
				continue;
			if_fail_fe(::processClientMessage(app.ProcessBroker, app.Server, connection, *message));
			message									= {};
		}
	}
	return 0;
}

::gpk::err_t				draw					(::brt::SApplication & app)						{
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
	::gpk::sleep(15);
	return 0;
}
