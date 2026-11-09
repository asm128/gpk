#include "brt_application_main.h"
#include "gpk_bitmap_file.h"
#include "gpk_tcpip.h"
#include "gpk_process.h"

#include "gpk_parse.h"
#include "gpk_string_compose.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

#if defined(GPK_LINUX)
#	include <errno.h>
#	include <fcntl.h>
#	include <poll.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <wordexp.h>
#endif

using namespace gpk;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::brt::SApplication, "Module Explorer");

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
	::gpk::controlSetParent(gui, app.IdExit, (::gpk::cid_t)-1);
	::gpk::tcpipInitialize();
	u3_t 					port						= 9998;
	u3_t 					adapter						= 0;
	{ // load port from config file
		::gpk::vcst_t				jsonPort					= {};
		const ::gpk::SJSONReader	& jsonReader				= framework.JSONConfig.Reader;
		const s2_t				indexObjectApp				= ::gpk::jsonExpressionResolve("application.brt", jsonReader, 0, app.ProcessFileName);
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

#if defined(GPK_LINUX)
static ::gpk::err_t	closeDescriptor			(int & descriptor) {
	if(0 > descriptor)
		return 0;
	const int result					= ::close(descriptor);
	descriptor						= -1;
	if(0 != result) {
		error_printf("Failed to close descriptor. errno: %i.", errno);
		return -1;
	}
	return 0;
}

static ::gpk::err_t	closeOnExec				(int descriptor) {
	const int flags					= ::fcntl(descriptor, F_GETFD);
	if(0 > flags || 0 > ::fcntl(descriptor, F_SETFD, flags | FD_CLOEXEC)) {
		error_printf("Failed to set close-on-exec for descriptor %i. errno: %i.", descriptor, errno);
		return -1;
	}
	return 0;
}
#endif

#if defined(GPK_WINDOWS)
sttc	::gpk::err_t	createChildProcess
	(	::brt::SProcess		& process
	,	::gpk::vu0_t		environmentBlock
	,	::gpk::vcst_t		appPath
	,	::gpk::vsc_t		commandLine
	,	bool				debugMessageBox			= false
	) {	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	::gpk::vcst_t				szCmdlineApp			= appPath;
	::gpk::vstr_t				szCmdlineFinal			= commandLine;
	bool						bSuccess				= false;
	stxp	bool				isUnicodeEnv			= false;
	stxp	u2_t 			creationFlags			= CREATE_SUSPENDED | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);
	
	if(INVALID_HANDLE_VALUE != process.ProcessInfo.hProcess	){ CloseHandle(process.ProcessInfo.hProcess	); }
	bSuccess				= CreateProcessA(szCmdlineApp	// Create the child process.
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
	if_true_vef(-1, false == bSuccess, "Failed to create process, because... '%s'.", "???");
	if(debugMessageBox) {
		::gpk::string			userMessage				= {};
		if_fail_fe(gpk::append_strings(userMessage, "Attach your debugger to '", szCmdlineApp, "' and press OK to initiate the process' main thread."));
		MessageBoxA(0, userMessage.begin(), "Last chance!", MB_OK | MB_TOPMOST);
	}
	info_printf("Creating process '%.*s' with command line '%.*s'", GPK_VIEW_ARGS(szCmdlineApp), GPK_VIEW_ARGS(szCmdlineFinal));
	ResumeThread(process.ProcessInfo.hThread);
	return 0;
}
#elif defined(GPK_LINUX)
sttc	::gpk::err_t	createChildProcess
	(	::brt::SProcess		& process
	,	::gpk::vu0_t		environmentBlock
	,	::gpk::vcst_t		appPath
	,	::gpk::vsc_t		commandLine
	,	bool				debugMessageBox			= false
	) {
	::gpk::apod<char*> environmentPointers;
	for(::gpk::u2_t offset = 0; offset < environmentBlock.size() && environmentBlock[offset]; ) {
		if_fail_fe(environmentPointers.push_back((char*)&environmentBlock[offset]));
		while(offset < environmentBlock.size() && environmentBlock[offset])
			++offset;
		if_true_fef(offset >= environmentBlock.size(), "Unterminated environment entry at offset %u.", offset);
		++offset;
	}
	if_fail_fe(environmentPointers.push_back(nullptr));

	::wordexp_t words = {};
	const int wordResult = ::wordexp(commandLine.begin(), &words, WRDE_NOCMD);
	if(0 != wordResult) {
		if(WRDE_NOSPACE == wordResult)
			::wordfree(&words);
		error_printf("Failed to parse command line '%.*s'. wordexp: %i.", GPK_VIEW_ARGS(commandLine), wordResult);
		return -1;
	}

	const pid_t child = ::fork();
	if(0 > child) {
		const int error = errno;
		::wordfree(&words);
		error_printf("Failed to create process '%.*s'. errno: %i.", GPK_VIEW_ARGS(appPath), error);
		return -1;
	}
	if(0 == child) {
		if(0 > ::dup2(process.StartInfo.hStdInput , STDIN_FILENO )
		|| 0 > ::dup2(process.StartInfo.hStdOutput, STDOUT_FILENO)
		|| 0 > ::dup2(process.StartInfo.hStdError , STDERR_FILENO))
			::_exit(126);
		char * fallbackArguments[] = {(char*)appPath.begin(), nullptr};
		char ** arguments = words.we_wordc ? words.we_wordv : fallbackArguments;
		::execve(appPath.begin(), arguments, environmentPointers.begin());
		::_exit(127);
	}

	process.ProcessInfo.hProcess = (::gpk::s2_t)child;
	process.ProcessInfo.hThread = -1;
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdInput));
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdOutput));
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdError));
	::wordfree(&words);
	if(debugMessageBox)
		warning_printf("Attach your debugger to process %i ('%.*s').", (int)child, GPK_VIEW_ARGS(appPath));
	info_printf("Creating process '%.*s' with command line '%.*s'", GPK_VIEW_ARGS(appPath), GPK_VIEW_ARGS(commandLine));
	return 0;
}
#endif

#if defined(GPK_WINDOWS)
static	::gpk::err_t	writeToPipe				(const ::brt::SProcessHandles & handles, ::gpk::vcu0_t chBufToSend)	{	// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data.
	DWORD						dwWritten				= 0;
	bool						bSuccess				= false;
	if_true_ef(false == (bSuccess = WriteFile(handles.ChildStd_IN_Write, chBufToSend.begin(), chBufToSend.size(), &dwWritten, NULL) ? true : false), "%s", "Failed to write to child process' standard input.");
	if_true_vef(-1, false == (CloseHandle(handles.ChildStd_IN_Write) ? true : false), "%s", "Failed to close the pipe handle so the child process stops reading.");
	return bSuccess ? 0 : -1;
}
#elif defined(GPK_LINUX)
static	::gpk::err_t	writeToPipe				(::brt::SProcessHandles & handles, ::gpk::vcu0_t chBufToSend) {
	::gpk::u2_t offset = 0;
	while(offset < chBufToSend.size()) {
		const ssize_t written = ::write(handles.ChildStd_IN_Write, &chBufToSend[offset], chBufToSend.size() - offset);
		if(0 > written) {
			if(EINTR == errno)
				continue;
			error_printf("Failed to write to child process' standard input. errno: %i.", errno);
			return -1;
		}
		offset += (::gpk::u2_t)written;
	}
	return ::closeDescriptor(handles.ChildStd_IN_Write);
}
#endif


#if defined(GPK_WINDOWS)
static	::gpk::err_t	readFromPipe			(const ::brt::SProcess & process, const ::brt::SProcessHandles & handles, ::gpk::au0_t & readBytes)	{	// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data.
	static	::gpk::au0_t		chBuf;
	stxp	u2_t 			BUFSIZE					= 1024 * 1024 * 50;
	chBuf.resize(BUFSIZE);
	for (;;) {
		u2_t 					dwRead					= 0;
		if_zero_fe(ReadFile(handles.ChildStd_OUT_Read, chBuf.begin(), chBuf.size(), (DWORD*)&dwRead, NULL));
		if(0 == dwRead)
			break;
		if_fail_fe(readBytes.append(chBuf.begin(), dwRead));
		DWORD						exitCode				= 0;
		if_zero_fe(GetExitCodeProcess(process.ProcessInfo.hProcess, &exitCode));
		if_true_bif(STILL_ACTIVE != exitCode, "exitCode: %lu", exitCode);
		info_printf("Process output: %.*s", chBuf.size(), chBuf.begin());
		if(0 == readBytes[readBytes.size() - 1])
			break;
	}
	return 0;
}
#elif defined(GPK_LINUX)
static	::gpk::err_t	readFromPipe			(::brt::SProcess & process, ::brt::SProcessHandles & handles, ::gpk::au0_t & readBytes) {
	static ::gpk::au0_t chBuf;
	stxp ::gpk::u2_t BUFSIZE = 1024 * 1024;
	if_fail_fe(chBuf.resize(BUFSIZE));
	::pollfd descriptors[2] = {
		{handles.ChildStd_OUT_Read, POLLIN | POLLHUP, 0},
		{handles.ChildStd_ERR_Read, POLLIN | POLLHUP, 0},
	};
	::gpk::u0_t descriptorsOpen = 2;
	while(descriptorsOpen) {
		const int pollResult = ::poll(descriptors, 2, -1);
		if(0 > pollResult) {
			if(EINTR == errno)
				continue;
			error_printf("Failed waiting for child process output. errno: %i.", errno);
			return -1;
		}
		for(::gpk::u0_t index = 0; index < 2; ++index) {
			::pollfd & descriptor = descriptors[index];
			if(0 > descriptor.fd || 0 == (descriptor.revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL)))
				continue;
			const ssize_t bytesRead = ::read(descriptor.fd, chBuf.begin(), chBuf.size());
			if(0 < bytesRead) {
				if(0 == index) {
					if_fail_fe(readBytes.append(chBuf.begin(), (::gpk::u2_t)bytesRead));
					info_printf("Process output: %.*s", (int)bytesRead, chBuf.begin());
				}
				else
					warning_printf("Process error: %.*s", (int)bytesRead, chBuf.begin());
				continue;
			}
			if(0 > bytesRead && EINTR == errno)
				continue;
			if(0 > bytesRead && EAGAIN != errno && EWOULDBLOCK != errno) {
				error_printf("Failed reading child process output. errno: %i.", errno);
				return -1;
			}
			if(0 == bytesRead || descriptor.revents & (POLLHUP | POLLERR | POLLNVAL)) {
				if(0 == index) {
					if_fail_fe(::closeDescriptor(handles.ChildStd_OUT_Read));
				}
				else {
					if_fail_fe(::closeDescriptor(handles.ChildStd_ERR_Read));
				}
				descriptor.fd = -1;
				--descriptorsOpen;
			}
		}
	}

	int status = 0;
	pid_t waitResult = 0;
	do
		waitResult = ::waitpid((pid_t)process.ProcessInfo.hProcess, &status, 0);
	while(0 > waitResult && EINTR == errno);
	if(0 > waitResult) {
		error_printf("Failed waiting for child process %i. errno: %i.", process.ProcessInfo.hProcess, errno);
		return -1;
	}
	process.ExitCode = WIFEXITED(status) ? (::gpk::u2_t)WEXITSTATUS(status) : (::gpk::u2_t)(128 + WTERMSIG(status));
	process.ProcessInfo.hProcess = -1;
	if(0 != process.ExitCode)
		warning_printf("exitCode: %u", process.ExitCode);
	return 0;
}
#endif

#if defined(GPK_WINDOWS)
static ::gpk::err_t	initHandles						(::brt::SProcessHandles & handles) {
	SECURITY_ATTRIBUTES			saAttr;
	saAttr.bInheritHandle	= TRUE;		// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.lpSecurityDescriptor = NULL;
	if_true_vef(-1, false == (bool)CreatePipe			(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT.");
	if_true_vef(-1, false == (bool)SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited.");
	if_true_vef(-1, false == (bool)CreatePipe			(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT.");
	if_true_vef(-1, false == (bool)SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited.");
	if_true_vef(-1, false == (bool)CreatePipe			(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0)		, "Stdin CreatePipe: '%s'."				, "Failed to create a pipe for the child process's STDIN.");
	if_true_vef(-1, false == (bool)SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0)						, "Stdin SetHandleInformation: '%s'."	, "Failed to ensure the write handle to the pipe for STDIN is not inherited.");
	return 0;	// The remaining open handles are cleaned up when this process terminates. To avoid resource leaks in a larger application, close handles explicitly.
}
#elif defined(GPK_LINUX)
static ::gpk::err_t	initHandles						(::brt::SProcessHandles & handles) {
	int output[2] = {-1, -1};
	int error [2] = {-1, -1};
	int input [2] = {-1, -1};
	if(0 != ::pipe(output) || 0 != ::pipe(error) || 0 != ::pipe(input)) {
		error_printf("Failed to create child process pipes. errno: %i.", errno);
		if(0 <= output[0]) ::close(output[0]);
		if(0 <= output[1]) ::close(output[1]);
		if(0 <= error [0]) ::close(error [0]);
		if(0 <= error [1]) ::close(error [1]);
		if(0 <= input [0]) ::close(input [0]);
		if(0 <= input [1]) ::close(input [1]);
		return -1;
	}
	handles.ChildStd_OUT_Read = output[0];
	handles.ChildStd_OUT_Write = output[1];
	handles.ChildStd_ERR_Read = error[0];
	handles.ChildStd_ERR_Write = error[1];
	handles.ChildStd_IN_Read = input[0];
	handles.ChildStd_IN_Write = input[1];
	if_fail_fe(::closeOnExec(handles.ChildStd_OUT_Read));
	if_fail_fe(::closeOnExec(handles.ChildStd_OUT_Write));
	if_fail_fe(::closeOnExec(handles.ChildStd_ERR_Read));
	if_fail_fe(::closeOnExec(handles.ChildStd_ERR_Write));
	if_fail_fe(::closeOnExec(handles.ChildStd_IN_Read));
	if_fail_fe(::closeOnExec(handles.ChildStd_IN_Write));
	return 0;
}
#endif

::gpk::err_t			update						(::brt::SApplication & app, bool exitSignal)	{
	::gpk::STimer				timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard			lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer	= app.Offscreen;
	}
	::gpk::SFramework			& framework					= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));
	::gpk::SGUI					& gui						= *framework.GUI;
	::gpk::acid					controlsToProcess			= {};
	::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) {
			info_printf("Executed %u.", iControl);
			if(iControl == (::gpk::cid_t)app.IdExit)
				return 1;
			return 0;
		});
	::gpk::apobj<::gpk::apobj<::gpk::SMessage>>	& receivedPerClient		= app.ReceivedPerClient;
	{	// pick up messages for later processing
		::std::lock_guard			lock						(app.Server.Mutex);
		receivedPerClient.resize(app.Server.Clients.size());
		for(u2_t  iClient = 0; iClient < app.Server.Clients.size(); ++iClient) {
			::gpk::pobj<::gpk::SConnection>	conn						= app.Server.Clients[iClient];
			::gpk::lock_guard					lockRecv					(conn->Queue.MutexReceive);
			*(receivedPerClient[iClient])	= app.Server.Clients[iClient]->Queue.Received;
			app.Server.Clients[iClient]->Queue.Received.clear();
		}
	}

	app.ClientIOHandles.resize(receivedPerClient.size());
	app.ClientProcesses.resize(receivedPerClient.size());
	{	// Exectue processes
		for(u2_t  iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			::brt::SProcessHandles		& iohandles				= app.ClientIOHandles[iClient];
			::brt::SProcess				& process				= app.ClientProcesses[iClient];
			for(u2_t  iMessage = 0; iMessage < receivedPerClient[iClient]->size(); ++iMessage) {
				::gpk::SMessage			& pClientMessage		= *(*receivedPerClient[iClient])[iMessage];
				::gpk::vu0_t				environmentBlock		= pClientMessage.Payload;
				info_printf("Client %i received: %.*s.", iClient, GPK_VIEW_ARGS(environmentBlock));
				// llamar proceso
				if_fail_ce(::initHandles(iohandles));
				process.StartInfo.hStdError		= iohandles.ChildStd_ERR_Write;
				process.StartInfo.hStdOutput	= iohandles.ChildStd_OUT_Write;
				process.StartInfo.hStdInput		= iohandles.ChildStd_IN_Read;
#if defined(GPK_WINDOWS)
				process.ProcessInfo.hProcess	= INVALID_HANDLE_VALUE;
				process.StartInfo.dwFlags		|= STARTF_USESTDHANDLES;
#elif defined(GPK_LINUX)
				process.ProcessInfo.hProcess	= -1;
#endif
				::gpk::vcu0_t				payload					= environmentBlock;
				::gpk::err_t				contentOffset			= payload.find((u0_t)0U);
				if_fail_cef(contentOffset, "%s", "Failed to find environment block stop code.");
				if(payload.size() && (payload.size() > (u2_t)contentOffset + 2))
					if_fail_ef(::writeToPipe(app.ClientIOHandles[iClient], {&payload[contentOffset + 2], payload.size() - contentOffset - 2}), "%s", "Failed to write request content to process' stdin.");
				if_fail_ef(::createChildProcess(app.ClientProcesses[iClient], environmentBlock, app.szCmdlineApp, app.szCmdlineFinal), "Failed to create child process: %.*s.", GPK_VIEW_ARGS(app.ProcessFileName));	// Create the child process.
			}
		}
	}
#if defined(GPK_WINDOWS)
	Sleep(10);
#elif defined(GPK_LINUX)
	::gpk::sleep(10);
#endif
	::gpk::apobj<::gpk::apobj<::gpk::au0_t>>	& clientsResponses		= app.ClientResponses;
	if_fail_fef(clientsResponses.resize(receivedPerClient.size()), "current:%u,new:%u", clientsResponses.size(), receivedPerClient.size());
	{	// Read processes output if they're done processing.
		for(u2_t  iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
			::gpk::pobj<::gpk::apobj<::gpk::au0_t>>	& clientResponses = clientsResponses[iClient];
			if_fail_fe(clientResponses->resize(receivedPerClient[iClient]->size()));
			::brt::SProcessHandles	& iohandles				= app.ClientIOHandles[iClient];
			::brt::SProcess			& process				= app.ClientProcesses[iClient];
			for(u2_t  iMessage = 0; iMessage < receivedPerClient[iClient]->size(); ++iMessage) {
				info_printf("Client %i received: %.*s.", iClient, (*(receivedPerClient[iClient]))[iMessage]->Payload.size(), (*(receivedPerClient[iClient]))[iMessage]->Payload.begin());
				// generar respuesta proceso
				//clientResponses[iClient][iMessage]		= "\r\n{ \"Respuesta\" : \"bleh\"}";
				(*clientResponses)[iMessage]->clear();
				if_fail_fef(::readFromPipe(process, iohandles, *(*clientResponses)[iMessage]), "iClient: %u, iMessage: %u.", iClient, iMessage);
#if defined(GPK_WINDOWS)
				safe_closehandle(process.StartInfo.hStdError	);
				safe_closehandle(process.StartInfo.hStdInput	);
				safe_closehandle(process.StartInfo.hStdOutput	);
#elif defined(GPK_LINUX)
				::closeDescriptor(process.StartInfo.hStdError	);
				::closeDescriptor(process.StartInfo.hStdInput	);
				::closeDescriptor(process.StartInfo.hStdOutput	);
#endif
			}
		}
	}
	for(u2_t  iClient = 0; iClient < clientsResponses.size(); ++iClient) {
		::gpk::pobj<::gpk::apobj<::gpk::au0_t>>	& clientResponses = clientsResponses[iClient];
		for(u2_t  iMessage = 0; iMessage < clientResponses->size(); ++iMessage) { // contestar
			if((*clientResponses)[iMessage]->size()) {
				::std::lock_guard					lock						(app.Server.Mutex);
				::gpk::pobj<::gpk::SConnection>	conn						= app.Server.Clients[iClient];
				if_fail_fe(::gpk::connectionPushData(*conn, conn->Queue, *((*clientResponses)[iMessage]), true, true));
				receivedPerClient[iClient][iMessage]	= {};
#if defined(GPK_WINDOWS)
				::brt::SProcess						& process				= app.ClientProcesses[iClient];
				safe_closehandle(process.ProcessInfo.hProcess	);
				safe_closehandle(process.ProcessInfo.hThread	);
#endif

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
		::std::lock_guard				lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(*app.Framework.GUI, 0, target->Color.View);
	}
	{
		::std::lock_guard				lock					(app.LockRender);
		app.Offscreen				= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	::gpk::sleep(15);
	return 0;
}
//
//// ---------------------------- Unused garbage follows
//// Example 1
//// By default, a child process inherits a copy of the environment block of the parent process. The following example demonstrates how to create a new environment block to pass to a child process using CreateProcess.
//static int					REM_tmain0							()		{
//	stxp	u2_t 	BUFSIZE					= 4096;
//	char							environmentBlock2Set	[BUFSIZE]	= {};
//	s2_t							charsWritten						= 0;
//	DWORD							dwFlags								= 0;
//	stxp	::gpk::vcst_c			szAppName	[]						= "ex3.exe");
//	PROCESS_INFORMATION				pi									= {};
//	if_fail_fef(charsWritten += 1 + (s2_t)sprintf_s(environmentBlock2Set, "MySetting=A"), "String copy failed: %s.", "MySetting=A");	// Copy environment strings into an environment block.
//	if_fail_fef(charsWritten += 1 + (s2_t)sprintf_s(environmentBlock2Set + charsWritten, ::gpk::size(environmentBlock2Set) - charsWritten, "MyVersion=2"), "String copy failed: %s.", "MyVersion=2");
//	// Create the child process, specifying a new environment block.
//	STARTUPINFOA						si									= {sizeof(STARTUPINFOA)};
//	stxp	bool		isUnicodeEnv						= false;
//	dwFlags						= dwFlags | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);
//	if_true_vef(-1, FALSE == CreateProcessA(szAppName, NULL, NULL, NULL, TRUE, dwFlags, (void*)environmentBlock2Set, NULL, &si, &pi), "CreateProcess failed (%d)\n", GetLastError());
//	WaitForSingleObject(pi.hProcess, INFINITE);
//	return TRUE;
//}
//
//// Example 2
//// Altering the environment variables of a child process during process creation is the only way one process can directly change the environment variables of another process. A process can never directly change the environment variables of another process that is not a child of that process.
//// If you want the child process to inherit most of the parent's environment with only a few changes, retrieve the current values using GetEnvironmentVariable, save these values, create an updated block for the child process to inherit, create the child process, and then restore the saved values using SetEnvironmentVariable, as shown in the following example.
//#define VARNAME "MyVariable"
//
//static int					REM_tmain1			() {
//	stxp	const u2_t 			BUFSIZE					= 4096;
//	stxp	::gpk::vcst_c			szAppName			= "ex3.exe");
//	STARTUPINFOA					si					= {sizeof(STARTUPINFOA)};
//	PROCESS_INFORMATION				pi					= {};
//	BOOL							fExist				= 0
//		;
//	// Retrieves the current value of the variable if it exists.
//	// Sets the variable to a new value, creates a child process, then uses SetEnvironmentVariable to restore the original value or delete it if it did not exist previously.
//	char							* pszOldVal			= (char*)malloc(BUFSIZE * 4);
//	if_true_vef(-1, nullptr == pszOldVal, "%s", "Out of memory\n");
//	DWORD							dwRet				= GetEnvironmentVariableA(VARNAME, pszOldVal, BUFSIZE * 4);
//	if(0 == dwRet) {
//		DWORD							dwErr				= GetLastError();
//		if(ERROR_ENVVAR_NOT_FOUND == dwErr) {
//			printf("Environment variable does not exist.\n");
//			fExist						= FALSE;
//		}
//	}
//	else if(BUFSIZE < dwRet) {
//		if_true_vef(-1, nullptr == (pszOldVal = (char*)realloc(pszOldVal, dwRet * 4)), "%s", "Out of memory\n");
//
//		dwRet						= GetEnvironmentVariableA(VARNAME, pszOldVal, dwRet);
//		if_true_vef(-1, FALSE == dwRet, "GetEnvironmentVariable failed (%d)\n", GetLastError());
//		fExist						= TRUE;
//	}
//	else
//		fExist						= TRUE;
//
//	if_true_vef(-1, FALSE == SetEnvironmentVariableA(VARNAME, "Test"), "SetEnvironmentVariable failed (%d)\n", GetLastError());		// Set a value for the child process to inherit.
//	// ---- Create a child process.
//	si							= {sizeof(STARTUPINFOA)};
//	stxp	bool		isUnicodeEnv			= false;
//	const DWORD						dwFlags					= isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0;
//	::gpk::string				bufferAppName			= GPK_VIEW_ARGS(szAppName);
//	if_true_vef(-1, FALSE == CreateProcessA(bufferAppName.begin(), NULL, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi), "CreateProcess failed (%d)\n", GetLastError()); // inherit parent's environment
//	WaitForSingleObject(pi.hProcess, INFINITE);
//	// - Restore the original environment variable.
//	if(fExist) {
//		if_true_vef(-1, FALSE == SetEnvironmentVariableA(VARNAME, pszOldVal), "SetEnvironmentVariable failed (%d)\n", GetLastError());
//	}
//	else
//		SetEnvironmentVariableA(VARNAME, NULL);
//
//	free(pszOldVal);
//	return 0;
//}
//
//int							test					() {
//	::REM_tmain0();
//	::REM_tmain1();
//	return 0;
//}
