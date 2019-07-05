#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_find.h"
#include "gpk_process.h"

#include "gpk_parse.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{
	::gpk::serverStop(app.Server);
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::gpk::tcpipShutdown();
	return 0;
}

			::gpk::error_t											setup						(::gme::SApplication & app)						{
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
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
	{ // load port from config file
		::gpk::view_const_string												jsonPort					= {};
		const ::gpk::SJSONReader												& jsonReader						= framework.ReaderJSONConfig;
		const int32_t															indexObjectConfig					= ::gpk::jsonArrayValueGet(*jsonReader.Tree[0], 0);	// Get the first JSON {object} found in the [document]
		gwarn_if(errored(::gpk::jsonExpressionResolve("application.brt.listen_port", jsonReader, indexObjectConfig, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
		else {
			::gpk::parseIntegerDecimal(jsonPort, &port);
			info_printf("Port to listen on: %u.", (uint32_t)port);
		}
	}
	gpk_necall(::gpk::serverStart(app.Server, (uint16_t)port), "Failed to start server on port %u. Port busy?", (uint32_t)port);
	return 0;
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{
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
	receivedPerClient.resize(app.Server.Clients.size());
	{	// pick up messages for later processing
		::gpk::mutex_guard																	lock						(app.Server.Mutex);
		for(uint32_t iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::ptr_obj<::gpk::SUDPConnection>									conn						= app.Server.Clients[iClient];
			::gpk::mutex_guard														lockRecv					(conn->Queue.MutexReceive);
			receivedPerClient[iClient]											= app.Server.Clients[iClient]->Queue.Received;
			app.Server.Clients[iClient]->Queue.Received.clear();
		}
	}

	::gpk::array_obj<::gpk::array_obj<::gpk::array_pod<char_t>>>						& clientResponses		= app.ClientResponses;
	clientResponses.resize(receivedPerClient.size());
	{	// Exectue processes
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
				// llamar proceso
			}
		}
	}
	{	// Read processes output if they're done processing.
		for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			clientResponses[iClient].resize(receivedPerClient[iClient].size());
			for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
				info_printf("Client %i received: %s.", iClient, receivedPerClient[iClient][iMessage]->Payload.begin());	
				// generar respuesta proceso
				clientResponses[iClient][iMessage]		= "\r\n{ \"Respuesta\" : \"bleh\"}";
			}
		}
	}
	for(uint32_t iClient = 0; iClient < clientResponses.size(); ++iClient) {
		for(uint32_t iMessage = 0; iMessage < clientResponses[iClient].size(); ++iMessage) { // contestar 
			::gpk::mutex_guard														lock						(app.Server.Mutex);
			::gpk::ptr_obj<::gpk::SUDPConnection>									conn						= app.Server.Clients[iClient];
			::gpk::connectionPushData(*conn, conn->Queue, clientResponses[iClient][iMessage]);
			receivedPerClient[iClient][iMessage]		= {};
		}
	}
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}


			::gpk::error_t											draw					(::gme::SApplication & app)						{
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


#include <windows.h> 
#include <cstdio> 

struct SProcessHandles {
	HANDLE	ChildStd_IN_Read	= nullptr;
	HANDLE	ChildStd_IN_Write	= nullptr;
	HANDLE	ChildStd_OUT_Read	= nullptr;
	HANDLE	ChildStd_OUT_Write	= nullptr;
	HANDLE	InputFile			= nullptr;
};

static ::SProcessHandles g_h	= {};

#define BUFSIZE 4096 

static	::gpk::error_t		writeToPipe				(::gpk::view_const_byte chBufToSend)	{	// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data. 
	DWORD							dwWritten				= 0;
	bool							bSuccess				= false;
	e_if(false == (bSuccess = WriteFile(g_h.ChildStd_IN_Write, chBufToSend.begin(), chBufToSend.size(), &dwWritten, NULL) ? true : false), "Failed to write to child process' standard input.");
	ree_if(false == (CloseHandle(g_h.ChildStd_IN_Write) ? true : false), "%s", "Failed to close the pipe handle so the child process stops reading.");
	return bSuccess ? 0 : -1;
} 
 
static	::gpk::error_t		readFromPipe			()	{	// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data. 
	uint32_t						dwRead					= 0
		,							dwWritten				= 0
		;
	char							chBuf	[BUFSIZE]		= {}; 
	bool							bSuccess				= FALSE;
	HANDLE							hParentStdOut			= GetStdHandle(STD_OUTPUT_HANDLE);
	for (;;) { 
		bSuccess					= ReadFile(g_h.ChildStd_OUT_Read, chBuf, BUFSIZE, (DWORD*)&dwRead, NULL);
		ree_if(false == bSuccess, "Failed to read from child process' standard output."); 
		if(0 == dwRead) 
			break; 
		char							bufferFormat	[128]	= {};
		sprintf_s(bufferFormat, "Process output: %%.%us", dwRead);
		info_printf(bufferFormat, chBuf);
		bSuccess					= WriteFile(hParentStdOut, chBuf, dwRead, (DWORD*)&dwWritten, NULL);
		ree_if(false == bSuccess, "Failed to write to local bs"); 
	}
	return 0;
} 


struct SProcess {
	PROCESS_INFORMATION			ProcessInfo				= {}; 
	STARTUPINFO					StartInfo				= {sizeof(STARTUPINFO)};
};

static	::gpk::error_t		createChildProcess		(SProcess& process, ::gpk::view_const_string commandLine)	{	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	commandLine;
	char			szCmdline	[]			= "C:\\amd128_test\\x64.Release\\KitsuTBS.exe";
	bool							bSuccess				= false; 
	bSuccess					= CreateProcessA(NULL	// Create the child process. 
		, szCmdline					// command line 
		, nullptr					// process security attributes 
		, nullptr					// primary thread security attributes 
		, true						// handles are inherited 
		, 0							// creation flags 
		, NULL						// use parent's environment 
		, NULL						// use parent's current directory 
		, &process.StartInfo		// STARTUPINFO pointer 
		, &process.ProcessInfo
		) ? true : false;  // receives PROCESS_INFORMATION 
	ree_if(false == bSuccess, "Failed to create process, because... '%s'.", "???");
	return 0;
}

static int					REM_main			(int argc, CHAR *argv[]) { 
	ree_if(argc < 2, "USAGE: \n\t%s [processFile]", argv[0]);
	info_printf("\n->Start of parent execution.\n");
	
	SECURITY_ATTRIBUTES				saAttr				= {sizeof(SECURITY_ATTRIBUTES)}; 
	saAttr.bInheritHandle		= TRUE;		// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.lpSecurityDescriptor = NULL; 
	ree_if(false == (bool)CreatePipe			(&g_h.ChildStd_OUT_Read, &g_h.ChildStd_OUT_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT."); 
	ree_if(false == (bool)SetHandleInformation	(g_h.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0)					, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited."); 
	ree_if(false == (bool)CreatePipe			(&g_h.ChildStd_IN_Read, &g_h.ChildStd_IN_Write, &saAttr, 0)		, "Stdin CreatePipe: '%s'."				, "Failed to create a pipe for the child process's STDIN.");
	ree_if(false == (bool)SetHandleInformation	(g_h.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0)					, "Stdin SetHandleInformation: '%s'."	, "Failed to ensure the write handle to the pipe for STDIN is not inherited."); 

	SProcess						process				= {};
	char							szCmdline	[]			= "C:\\amd128_test\\x64.Release\\KitsuTBS.exe";
	createChildProcess(process, szCmdline);	// Create the child process. 
	ree_if(INVALID_HANDLE_VALUE == (g_h.InputFile = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)), "Failed to get a handle to an input file for the parent: '%s'.", "This example assumes a plain text file and uses string output to verify data flow.");

	writeToPipe("Something to send to the standard input of the child process.");	// Write to the pipe that is the standard input for a child process. Data is written to the pipe's buffers, so it is not necessary to wait until the child process is running before writing data.
	printf("\n->Contents of %s written to child STDIN pipe.\n", argv[1]);
	readFromPipe();		// Read from pipe that is the standard output for child process. 
	info_printf("\n->End of parent execution.\n");	
	// Close handles to the child process and its primary thread. Some applications might keep these handles to monitor the status of the child process, for example. 
	CloseHandle(process.ProcessInfo.hProcess);
	CloseHandle(process.ProcessInfo.hThread);
	return 0;	// The remaining open handles are cleaned up when this process terminates. To avoid resource leaks in a larger application, close handles explicitly. 
} 

// Example 1
// By default, a child process inherits a copy of the environment block of the parent process. The following example demonstrates how to create a new environment block to pass to a child process using CreateProcess.
static int					REM_tmain0							()		{
	char							environmentBlock2Set	[BUFSIZE]	= {};
	int32_t							charsWritten						= 0; 
	DWORD							dwFlags								= 0;
	char							szAppName	[]						= "ex3.exe";
	PROCESS_INFORMATION				pi									= {};
	gpk_necall(charsWritten += 1 + (int32_t)sprintf_s(environmentBlock2Set, "MySetting=A"), "String copy failed: %s.", "MySetting=A");	// Copy environment strings into an environment block. 
	gpk_necall(charsWritten += 1 + (int32_t)sprintf_s(environmentBlock2Set + charsWritten, ::gpk::size(environmentBlock2Set) - charsWritten, "MyVersion=2"), "String copy failed: %s.", "MyVersion=2"); 
	// Create the child process, specifying a new environment block. 
	STARTUPINFO						si									= {sizeof(STARTUPINFO)};
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
	::gpk::view_const_string		szAppName			= "ex3.exe";
	STARTUPINFO						si					= {sizeof(STARTUPINFO)};
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
	si							= {sizeof(STARTUPINFO)};
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
	::REM_main(0, 0);
	::REM_tmain0();
	::REM_tmain1();
	::gpk::array_pod<char_t> environmentBlock;
	ree_if(::gpk::getEnvironmentBlock(environmentBlock), "Failed to get environment block!");
	return 0;
}
