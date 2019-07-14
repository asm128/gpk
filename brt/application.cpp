#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_find.h"
#include "gpk_process.h"

#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::brt::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::brt::SApplication & app)						{
	::gpk::serverStop(app.Server);
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::tcpipShutdown();
	::gpk::sleep(1000);
	return 0;
}

			::gpk::error_t											setup						(::brt::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	mainWindow.Size														= {320, 200};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?????!?!?!?!?");
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
	uint64_t																port						= 9998;
	uint64_t																adapter						= 0;
	{ // load port from config file
		::gpk::view_const_string												jsonPort					= {};
		const ::gpk::SJSONReader												& jsonReader						= framework.JSONConfig.Reader;
		const int32_t															indexObjectApp						= ::gpk::jsonExpressionResolve("application.brt", jsonReader, 0, app.ProcessFileName);
		gwarn_if(errored(indexObjectApp), "Failed to find application node (%s) in json configuration file: '%s'", "application.brt", framework.FileNameJSONConfig.begin())
		else {
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.executable_path"			, jsonReader, indexObjectApp, app.ProcessFileName	)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.command_line_app_name"	, jsonReader, indexObjectApp, app.ProcessMockPath	)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.command_line_params"		, jsonReader, indexObjectApp, app.ProcessParams		)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			gwarn_if(errored(::gpk::jsonExpressionResolve("listen_port"						, jsonReader, indexObjectApp, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			else {
				::gpk::parseIntegerDecimal(jsonPort, &port);
				info_printf("Port to listen on: %u.", (uint32_t)port);
			}
			jsonPort = "";
			gwarn_if(errored(::gpk::jsonExpressionResolve("adapter"	, jsonReader, indexObjectApp, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
			else {
				::gpk::parseIntegerDecimal(jsonPort, &adapter);
				info_printf("Adapter: %u.", (uint32_t)adapter);
			}
		}
	}
	::gpk::array_pod<char_t>		& szCmdlineApp		= app.szCmdlineApp		= app.ProcessFileName;
	::gpk::array_pod<char_t>		& szCmdlineFinal	= app.szCmdlineFinal	= app.ProcessMockPath;
	if(app.ProcessParams.size()) {
		szCmdlineFinal.push_back(' ');
		szCmdlineFinal.append(app.ProcessParams);
	}
	szCmdlineFinal.push_back(0);
	szCmdlineApp.push_back(0);

	gpk_necall(::gpk::serverStart(app.Server, (uint16_t)port, (int16_t)adapter), "Failed to start server on port %u. Port busy?", (uint32_t)port);
	return 0;
}

static	::gpk::error_t		createChildProcess		
	(	::brt::SProcess					& process
	,	::gpk::view_array<char_t>		environmentBlock
	,	::gpk::view_char				appPath
	,	::gpk::view_char				commandLine
	,	bool							debugMessageBox			= true
	) {	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	::gpk::view_char				szCmdlineApp			= appPath;
	::gpk::view_char				szCmdlineFinal			= commandLine;
	bool							bSuccess				= false; 
	static constexpr const bool		isUnicodeEnv			= false;
	static constexpr const uint32_t	creationFlags			= CREATE_SUSPENDED | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);

	if(INVALID_HANDLE_VALUE != process.ProcessInfo.hProcess	){ CloseHandle(process.ProcessInfo.hProcess	); }
	bSuccess					= CreateProcessA(szCmdlineApp.begin()	// Create the child process. 
		, szCmdlineFinal.begin()	// command line 
		, nullptr					// process security attributes 
		, nullptr					// primary thread security attributes 
		, true						// handles are inherited 
		, creationFlags				// creation flags 
		, environmentBlock.begin()	// use parent's environment 
		, NULL						// use parent's current directory 
		, &process.StartInfo		// STARTUPINFO pointer 
		, &process.ProcessInfo
		) ? true : false;  // receives PROCESS_INFORMATION 
	ree_if(false == bSuccess, "Failed to create process, because... '%s'.", "???");
	if(debugMessageBox) {
		::gpk::array_pod<char_t>		userMessage				= {};
		userMessage.resize(2 * szCmdlineApp.size() + 2 * szCmdlineFinal.size() + 1024);
		sprintf_s(userMessage.begin(), userMessage.size(), "Attach your debugger to '%s' and press OK to initiate the process' main thread.", szCmdlineApp.begin());
		MessageBoxA(0, userMessage.begin(), "Last chance!", MB_OK | MB_TOPMOST);
	}
	info_printf("Creating process '%s' with command line '%s'", szCmdlineApp.begin(), szCmdlineFinal.begin());
	ResumeThread(process.ProcessInfo.hThread);
	return 0;
}

static	::gpk::error_t		writeToPipe				(const ::brt::SProcessHandles & handles, ::gpk::view_const_byte chBufToSend)	{	// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data. 
	DWORD							dwWritten				= 0;
	bool							bSuccess				= false;
	e_if(false == (bSuccess = WriteFile(handles.ChildStd_IN_Write, chBufToSend.begin(), chBufToSend.size(), &dwWritten, NULL) ? true : false), "Failed to write to child process' standard input.");
	ree_if(false == (CloseHandle(handles.ChildStd_IN_Write) ? true : false), "%s", "Failed to close the pipe handle so the child process stops reading.");
	return bSuccess ? 0 : -1;
} 


static	::gpk::error_t		readFromPipe			(const ::brt::SProcess & process, const ::brt::SProcessHandles & handles, ::gpk::array_pod<byte_t> & readBytes)	{	// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data. 
	static	::gpk::array_pod<char_t>	chBuf;
	static constexpr	const uint32_t	BUFSIZE					= 1024 * 1024 * 50;
	chBuf.resize(BUFSIZE);
	bool								bSuccess				= FALSE;
	for (;;) { 
		uint32_t						dwRead					= 0;
		bSuccess					= ReadFile(handles.ChildStd_OUT_Read, chBuf.begin(), chBuf.size(), (DWORD*)&dwRead, NULL);
		ree_if(false == bSuccess, "Failed to read from child process' standard output."); 
		DWORD							exitCode				= 0;
		if(0 == dwRead) 
			break; 
		readBytes.append(chBuf.begin(), dwRead);
		GetExitCodeProcess(process.ProcessInfo.hProcess, &exitCode);
		if(STILL_ACTIVE != exitCode) 
			break; 
		char							bufferFormat	[128]	= {};
		sprintf_s(bufferFormat, "Process output: %%.%us", dwRead);
		info_printf(bufferFormat, chBuf.begin());
		if(0 == readBytes[readBytes.size() - 1])
			break;
	}
	return 0;
}

			static ::gpk::error_t							initHandles						(::brt::SProcessHandles & handles) { 
	SECURITY_ATTRIBUTES				saAttr;
	saAttr.bInheritHandle		= TRUE;		// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.lpSecurityDescriptor = NULL; 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT."); 
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited."); 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT."); 
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited."); 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0)		, "Stdin CreatePipe: '%s'."				, "Failed to create a pipe for the child process's STDIN.");
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0)						, "Stdin SetHandleInformation: '%s'."	, "Failed to ensure the write handle to the pipe for STDIN is not inherited."); 
	return 0;	// The remaining open handles are cleaned up when this process terminates. To avoid resource leaks in a larger application, close handles explicitly. 
} 

		::gpk::error_t											update						(::brt::SApplication & app, bool exitSignal)	{
	::gpk::STimer															timer;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_ginfo_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "Exit requested by framework update.");
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
	::gpk::array_obj<::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>>	& receivedPerClient		= app.ReceivedPerClient;
	{	// pick up messages for later processing
		::gpk::mutex_guard																	lock						(app.Server.Mutex);
		receivedPerClient.resize(app.Server.Clients.size());
		for(uint32_t iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::ptr_obj<::gpk::SUDPConnection>												conn						= app.Server.Clients[iClient];
			::gpk::mutex_guard																	lockRecv					(conn->Queue.MutexReceive);
			receivedPerClient[iClient]														= app.Server.Clients[iClient]->Queue.Received;
			app.Server.Clients[iClient]->Queue.Received.clear();
		}
	}

	app.ClientIOHandles.resize(receivedPerClient.size());
	app.ClientProcesses.resize(receivedPerClient.size());
	{	// Exectue processes
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			::brt::SProcessHandles									& iohandles				= app.ClientIOHandles[iClient];
			::brt::SProcess											& process				= app.ClientProcesses[iClient];
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
				::gpk::view_byte										environmentBlock		= receivedPerClient[iClient][iMessage]->Payload;
				// llamar proceso
				::initHandles(iohandles);
				process.StartInfo.hStdError		= iohandles.ChildStd_ERR_Write;
				process.StartInfo.hStdOutput	= iohandles.ChildStd_OUT_Write;
				process.StartInfo.hStdInput		= iohandles.ChildStd_IN_Read;
				process.ProcessInfo.hProcess	= INVALID_HANDLE_VALUE;
				process.StartInfo.dwFlags		|= STARTF_USESTDHANDLES;
				::gpk::view_const_byte									payload					= receivedPerClient[iClient][iMessage]->Payload;
				::gpk::error_t											contentOffset			= ::gpk::find_sequence_pod(::gpk::view_const_byte{"\0"}, payload);
				ce_if(errored(contentOffset), "Failed to find environment block stop code.");
				if(payload.size() && (payload.size() > (uint32_t)contentOffset + 2))
					e_if(errored(::writeToPipe(app.ClientIOHandles[iClient], {&payload[contentOffset + 2], payload.size() - contentOffset - 2})), "Failed to write request content to process' stdin.");
				gerror_if(errored(::createChildProcess(app.ClientProcesses[iClient], environmentBlock, app.szCmdlineApp, app.szCmdlineFinal)), "Failed to create child process: %s.", app.ProcessFileName.begin());	// Create the child process. 
			}
		}
	}
	Sleep(10);
	::gpk::array_obj<::gpk::array_obj<::gpk::array_pod<char_t>>>						& clientResponses		= app.ClientResponses;
	clientResponses.resize(receivedPerClient.size());
	{	// Read processes output if they're done processing.
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			clientResponses[iClient].resize(receivedPerClient[iClient].size());
			::brt::SProcessHandles									& iohandles				= app.ClientIOHandles[iClient];
			::brt::SProcess											& process				= app.ClientProcesses[iClient];
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
				// generar respuesta proceso
				//clientResponses[iClient][iMessage]		= "\r\n{ \"Respuesta\" : \"bleh\"}";
				clientResponses[iClient][iMessage].clear();
				::readFromPipe(process, iohandles, clientResponses[iClient][iMessage]);
				gpk_safe_closehandle(process.StartInfo.hStdError	);
				gpk_safe_closehandle(process.StartInfo.hStdInput	);
				gpk_safe_closehandle(process.StartInfo.hStdOutput	);
			}
		}
	}
	for(uint32_t iClient = 0; iClient < clientResponses.size(); ++iClient) {
		for(uint32_t iMessage = 0; iMessage < clientResponses[iClient].size(); ++iMessage) { // contestar 
			if(clientResponses[iClient][iMessage].size()) {
				::gpk::mutex_guard														lock						(app.Server.Mutex);
				::gpk::ptr_obj<::gpk::SUDPConnection>									conn						= app.Server.Clients[iClient];
				::gpk::connectionPushData(*conn, conn->Queue, clientResponses[iClient][iMessage], true, true);
				receivedPerClient[iClient][iMessage]		= {};
				::brt::SProcess											& process				= app.ClientProcesses[iClient];
				gpk_safe_closehandle(process.ProcessInfo.hProcess	);
				gpk_safe_closehandle(process.ProcessInfo.hThread	);

			}
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

			::gpk::error_t											draw					(::brt::SApplication & app)						{
	::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	target->resize(app.Framework.MainDisplay.Size, {0xFF, 0x40, 0x7F, 0xFF}, (uint32_t)-1);
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	::gpk::sleep(15);
	return 0;
}

// ---------------------------- Unused garbage follows
// Example 1
// By default, a child process inherits a copy of the environment block of the parent process. The following example demonstrates how to create a new environment block to pass to a child process using CreateProcess.
static int					REM_tmain0							()		{
	static constexpr	const uint32_t	BUFSIZE					= 4096;
	char							environmentBlock2Set	[BUFSIZE]	= {};
	int32_t							charsWritten						= 0; 
	DWORD							dwFlags								= 0;
	char							szAppName	[]						= "ex3.exe";
	PROCESS_INFORMATION				pi									= {};
	gpk_necall(charsWritten += 1 + (int32_t)sprintf_s(environmentBlock2Set, "MySetting=A"), "String copy failed: %s.", "MySetting=A");	// Copy environment strings into an environment block. 
	gpk_necall(charsWritten += 1 + (int32_t)sprintf_s(environmentBlock2Set + charsWritten, ::gpk::size(environmentBlock2Set) - charsWritten, "MyVersion=2"), "String copy failed: %s.", "MyVersion=2"); 
	// Create the child process, specifying a new environment block. 
	STARTUPINFOA						si									= {sizeof(STARTUPINFOA)};
	static constexpr const bool		isUnicodeEnv						= false;
	dwFlags						= dwFlags | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);
	ree_if(FALSE == CreateProcessA(szAppName, NULL, NULL, NULL, TRUE, dwFlags, (void*)environmentBlock2Set, NULL, &si, &pi), "CreateProcess failed (%d)\n", GetLastError());
	WaitForSingleObject(pi.hProcess, INFINITE);
	return TRUE;
}

// Example 2
// Altering the environment variables of a child process during process creation is the only way one process can directly change the environment variables of another process. A process can never directly change the environment variables of another process that is not a child of that process.
// If you want the child process to inherit most of the parent's environment with only a few changes, retrieve the current values using GetEnvironmentVariable, save these values, create an updated block for the child process to inherit, create the child process, and then restore the saved values using SetEnvironmentVariable, as shown in the following example.
#define VARNAME "MyVariable"

static int					REM_tmain1			() {
	static constexpr	const uint32_t	BUFSIZE					= 4096;
	::gpk::view_const_string		szAppName			= "ex3.exe";
	STARTUPINFOA						si					= {sizeof(STARTUPINFOA)};
	PROCESS_INFORMATION				pi					= {};
	BOOL							fExist				= 0
		; 
	// Retrieves the current value of the variable if it exists.
	// Sets the variable to a new value, creates a child process, then uses SetEnvironmentVariable to restore the original value or delete it if it did not exist previously. 
	char							* pszOldVal			= (char*)malloc(BUFSIZE * 4);
	ree_if(nullptr == pszOldVal, "%s", "Out of memory\n");
	DWORD							dwRet				= GetEnvironmentVariableA(VARNAME, pszOldVal, BUFSIZE * 4);
	if(0 == dwRet) {
		DWORD							dwErr				= GetLastError();
		if(ERROR_ENVVAR_NOT_FOUND == dwErr) {
			printf("Environment variable does not exist.\n");
			fExist						= FALSE;
		}
	}
	else if(BUFSIZE < dwRet) {
		ree_if(nullptr == (pszOldVal = (char*)realloc(pszOldVal, dwRet * 4)), "%s", "Out of memory\n");

		dwRet						= GetEnvironmentVariableA(VARNAME, pszOldVal, dwRet);
		ree_if(FALSE == dwRet, "GetEnvironmentVariable failed (%d)\n", GetLastError());
		fExist						= TRUE;
	}
	else 
		fExist						= TRUE;

	ree_if(FALSE == SetEnvironmentVariableA(VARNAME, "Test"), "SetEnvironmentVariable failed (%d)\n", GetLastError());		// Set a value for the child process to inherit. 
	// ---- Create a child process. 
	si							= {sizeof(STARTUPINFOA)};
	static constexpr const bool		isUnicodeEnv			= false;
	const DWORD						dwFlags					= isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0;
	char							bufferFormat	[64]	= {};
	sprintf_s(bufferFormat, "%%.%us", szAppName.size());
	::gpk::array_pod<char_t>		bufferAppName;
	bufferAppName.resize(szAppName.size() * 2);
	sprintf_s(bufferAppName.begin(), bufferAppName.size(), bufferFormat, szAppName.begin());
	ree_if(FALSE == CreateProcessA(bufferAppName.begin(), NULL, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi), "CreateProcess failed (%d)\n", GetLastError()); // inherit parent's environment 
	WaitForSingleObject(pi.hProcess, INFINITE);
	// - Restore the original environment variable. 
	if(fExist) {
		ree_if(FALSE == SetEnvironmentVariableA(VARNAME, pszOldVal), "SetEnvironmentVariable failed (%d)\n", GetLastError()); 
	}
	else 
		SetEnvironmentVariableA(VARNAME, NULL);

	free(pszOldVal);
	return 0;
}

int							test					() {
	::REM_tmain0();
	::REM_tmain1();
	return 0;
}
