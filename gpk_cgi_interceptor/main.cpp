#include "gpk_process.h"			// for ::gpk::getEnvironmentBlock()
#include "gpk_cgi_runtime.h"		// for ::gpk::SCGIRuntimeVlaues and ::gpk::cgiRuntimeValuesLoad()
#include "gpk_json_expression.h"	// for ::gpk::jsonParse()
#include "gpk_storage.h"			// for ::gpk::fileToMemory()

#include <Windows.h>
#include <process.h>

namespace brt
{
	struct SProcessHandles {
		HANDLE												ChildStd_IN_Read		= nullptr;
		HANDLE												ChildStd_IN_Write		= nullptr;
		HANDLE												ChildStd_OUT_Read		= nullptr;
		HANDLE												ChildStd_OUT_Write		= nullptr;
		HANDLE												ChildStd_ERR_Read		= nullptr;
		HANDLE												ChildStd_ERR_Write		= nullptr;
	};

	struct SProcess {
		PROCESS_INFORMATION									ProcessInfo				= {}; 
		STARTUPINFOA										StartInfo				= {sizeof(STARTUPINFOA)};
	};
} // namespace 

static	::gpk::error_t								createChildProcess		
	(	::brt::SProcess					& process
	,	::gpk::view_array<char_t>		environmentBlock
	,	::gpk::view_char				appPath
	,	::gpk::view_char				commandLine
	) {	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	::gpk::view_char										szCmdlineApp			= appPath;
	::gpk::view_char										szCmdlineFinal			= commandLine;
	bool													bSuccess				= false; 
	static constexpr const bool								isUnicodeEnv			= false;
	static constexpr const uint32_t							creationFlags			= CREATE_SUSPENDED | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);

	gpk_safe_closehandle(process.ProcessInfo.hProcess);
	bSuccess											= CreateProcessA(szCmdlineApp.begin()	// Create the child process. 
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
	ree_if(false == bSuccess, "Failed to create process'%s'.", szCmdlineApp.begin());
	::gpk::array_pod<char_t>								userMessage				= {};
	userMessage.resize(2 * szCmdlineApp.size() + 2 * szCmdlineFinal.size() + 1024);
	sprintf_s(userMessage.begin(), userMessage.size(), "Attach your debugger to '%s' and press OK to initiate the process' main thread.", szCmdlineApp.begin());
	MessageBoxA(0, userMessage.begin(), "Last chance!", MB_OK | MB_TOPMOST);
	info_printf("Creating process '%s' with command line '%s'", szCmdlineApp.begin(), szCmdlineFinal.begin());
	ResumeThread(process.ProcessInfo.hThread);
	return 0;
}

static	::gpk::error_t								writeToPipe				(const ::brt::SProcessHandles & handles, ::gpk::view_const_byte chBufToSend)	{	// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data. 
	DWORD													dwWritten				= 0;
	bool													bSuccess				= false;
	e_if(false == (bSuccess = WriteFile(handles.ChildStd_IN_Write, chBufToSend.begin(), chBufToSend.size(), &dwWritten, NULL) ? true : false), "Failed to write to child process' standard input.");
	ree_if(false == (CloseHandle(handles.ChildStd_IN_Write) ? true : false), "%s", "Failed to close the pipe handle so the child process stops reading.");
	return bSuccess ? 0 : -1;
} 

static	::gpk::error_t								loadConfig				
(	::gpk::array_pod<char_t>	& szCmdlineApp	
,	::gpk::array_pod<char_t>	& szCmdlineFinal
) {	// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data. 
	::gpk::view_const_string								processFileName			= "";
	::gpk::view_const_string								processMockPath			= "";
	::gpk::view_const_string								processParams			= "";	
	::gpk::view_const_string								fileNameJSONConfig		= "gpk_config.json";	
	::gpk::array_pod<char_t>								jsonData				= {};
	::gpk::SJSONReader										jsonReader				= {};
	if(fileNameJSONConfig.size()) {	// Attempt to load config file.
		ree_if(errored(::gpk::fileToMemory(fileNameJSONConfig, jsonData)), "Failed to load config JSON file! File not found? File name: %s.", fileNameJSONConfig.begin());
		ree_if(::gpk::jsonParse(jsonReader, ::gpk::view_const_string{jsonData.begin(), jsonData.size()}), "Failed to read json! Not a valid json file? File name: %s.", fileNameJSONConfig.begin());
	}
	{ // load port from config file
		const int32_t											indexObjectConfig		= ::gpk::jsonArrayValueGet(*jsonReader.Tree[0], 0);	// Get the first JSON {object} found in the [document]
		const int32_t											indexObjectApp			= ::gpk::jsonExpressionResolve("application.gpk_cgi_interceptor", jsonReader, indexObjectConfig, processFileName);
		gwarn_if(errored(indexObjectApp), "Failed to find application node (%s) in json configuration file: '%s'", "application.gpk_cgi_interceptor", fileNameJSONConfig.begin())
		else {
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.executable_path"			, jsonReader, indexObjectApp, processFileName	)), "Failed to load config from json! Last contents found: %s.", processFileName	.begin()) 
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.command_line_app_name"	, jsonReader, indexObjectApp, processMockPath	)), "Failed to load config from json! Last contents found: %s.", processMockPath	.begin()) 
			gwarn_if(errored(::gpk::jsonExpressionResolve("process.command_line_params"		, jsonReader, indexObjectApp, processParams		)), "Failed to load config from json! Last contents found: %s.", processParams		.begin()) 
		}
	}
	szCmdlineApp										= processFileName;
	if(processParams.size()) {
		szCmdlineFinal.push_back(' ');
		szCmdlineFinal.append(processParams);
	}
	szCmdlineFinal.push_back(0);
	szCmdlineApp.push_back(0);
	return 0;
}

//#include <unistd.h>
#include "gpk_stdsocket.h"

static	::gpk::error_t								handleReadable(HANDLE handle){
	fd_set															sockets								= {};
	memset(&sockets, -1, sizeof(fd_set));
	sockets.fd_count											= 0;
	sockets.fd_array[sockets.fd_count]							= (SOCKET)handle;
	if(sockets.fd_array[sockets.fd_count] != INVALID_SOCKET)
		++sockets.fd_count;
	timeval															wait_time							= {0, 1000};
	select(0, &sockets, 0, 0, &wait_time);
	if(1 == sockets.fd_count && sockets.fd_array[0] == (SOCKET)handle)
		return 1;
	return 0;
}

struct SThreadStateRead { 
	::brt::SProcessHandles								IOHandles			;
	::brt::SProcess										Process				;
	::gpk::array_pod<byte_t>							ReadBytes			;
	::gpk::refcount_t									DoneReading			= 0;
};

static	::gpk::error_t								readFromPipe			(::SThreadStateRead & appState)	{	// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data. 
	const ::brt::SProcess									& process				= appState.Process;
	const ::brt::SProcessHandles							& handles				= appState.IOHandles;
	::gpk::array_pod<byte_t>								& readBytes				= appState.ReadBytes;

	//char													chBuf	[BUFSIZE]		= {}; 
	static	::gpk::array_pod<char_t>						chBuf;
	static constexpr	const uint32_t						BUFSIZE					= 1024 * 1024;
	chBuf.resize(BUFSIZE);
	bool													bSuccess				= FALSE;
	while(true) { 
		uint32_t											dwRead					= 0;
		if(handleReadable(handles.ChildStd_OUT_Read)) {
			bSuccess										= ReadFile(handles.ChildStd_OUT_Read, chBuf.begin(), chBuf.size(), (DWORD*)&dwRead, NULL);
			ree_if(false == bSuccess, "Failed to read from child process' standard output."); 
			if(0 == dwRead) {
				gpk_sync_increment(appState.DoneReading);
				break; 
			}
			readBytes.append(chBuf.begin(), dwRead);
		}
		DWORD												exitCode				= 0;
		GetExitCodeProcess(process.ProcessInfo.hProcess, &exitCode);
		if(STILL_ACTIVE != exitCode && 0 == handleReadable(handles.ChildStd_OUT_Read))
			break; 
		char												bufferFormat	[128]	= {};
		sprintf_s(bufferFormat, "Process output: %%.%us", dwRead);
		info_printf(bufferFormat, chBuf.begin());
		if(0 == readBytes[readBytes.size() - 1])
			break;
	}
	return 0;
}

static	void										threadReadFromPipe		(void* appStaet)	{	// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data. 
	::readFromPipe(*(::SThreadStateRead*)appStaet);
}

static ::gpk::error_t								initHandles				(::brt::SProcessHandles & handles) { 
	SECURITY_ATTRIBUTES										saAttr					= {};
	saAttr.bInheritHandle								= TRUE;		// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.lpSecurityDescriptor							= NULL; 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT."); 
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited."); 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT."); 
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited."); 
	ree_if(false == (bool)CreatePipe			(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0)		, "Stdin CreatePipe: '%s'."				, "Failed to create a pipe for the child process's STDIN.");
	ree_if(false == (bool)SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0)						, "Stdin SetHandleInformation: '%s'."	, "Failed to ensure the write handle to the pipe for STDIN is not inherited."); 
	return 0;	// The remaining open handles are cleaned up when this process terminates. To avoid resource leaks in a larger application, close handles explicitly. 
} 

static	int											cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::SThreadStateRead & appState)										{
	::gpk::array_pod<char_t>								environmentBlock		= {};
	{	// Prepare CGI environment and request content packet to send to the service.
		ree_if(errored(::gpk::environmentBlockFromEnviron(environmentBlock)), "%s", "Failed");
		environmentBlock.append(runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size());
		environmentBlock.push_back(0);
	}
	::gpk::array_pod<char_t>								szCmdlineApp;
	::gpk::array_pod<char_t>								szCmdlineFinal;
	{
		::loadConfig(szCmdlineApp, szCmdlineFinal);
	}
	{	// llamar proceso
		::initHandles(appState.IOHandles);
		appState.Process.StartInfo.hStdError	= appState.IOHandles.ChildStd_ERR_Write;
		appState.Process.StartInfo.hStdOutput	= appState.IOHandles.ChildStd_OUT_Write;
		appState.Process.StartInfo.hStdInput	= appState.IOHandles.ChildStd_IN_Read;
		appState.Process.ProcessInfo.hProcess	= INVALID_HANDLE_VALUE;
		appState.Process.StartInfo.dwFlags		|= STARTF_USESTDHANDLES;
		::gpk::view_const_byte									content_body			= {runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size()};
		if(content_body.size())
			e_if(errored(::writeToPipe(appState.IOHandles, content_body)), "Failed to write request content to process' stdin.");
		gerror_if(errored(::createChildProcess(appState.Process, environmentBlock, szCmdlineApp, szCmdlineFinal)), "Failed to create child process: %s.", szCmdlineApp.begin());	// Create the child process. 
		_beginthread(::threadReadFromPipe, 0, &appState);
		SThreadStateRead										state;
		while(false == gpk_sync_compare_exchange(state.DoneReading, true, true)) {
			DWORD												exitCode				= 0;
			GetExitCodeProcess(appState.Process.ProcessInfo.hProcess, &exitCode);
			if(STILL_ACTIVE != exitCode)
				break; 
			::gpk::sleep(5);
		}
		gpk_safe_closehandle(appState.Process.StartInfo.hStdError	);
		gpk_safe_closehandle(appState.Process.StartInfo.hStdInput	);
		gpk_safe_closehandle(appState.Process.StartInfo.hStdOutput	);
		gpk_safe_closehandle(appState.Process.ProcessInfo.hProcess	);
	}
	return 0;
}

static int											cgiMain					()		{
	::gpk::SCGIRuntimeValues								runtimeValues;
	gpk_necall(::gpk::cgiRuntimeValuesLoad(runtimeValues), "%s", "Failed to load cgi runtime values.");
	::SThreadStateRead										appState;
	if errored(::cgiBootstrap(runtimeValues, appState)) {
		printf("%s\r\n", "Content-Type: text/html"
			"\r\nCache-Control: no-store"
			"\r\n\r\n"
			"<html><head><title>Internal server error</title></head><body>Failed to process request.</body></html>"
			"\r\n"
			"\r\n"
		);
	}
	else {
		printf("%s\r\n", "Content-Type: application/json"
			"\r\nCache-Control: no-store"
		);
		appState.ReadBytes.push_back('\0');
		OutputDebugStringA(appState.ReadBytes.begin());
		printf("%s", appState.ReadBytes.begin());
	}
	return 0;
}

int													main					(int argc, char** argv, char**envv)	{
	(void)argc, (void)argv, (void)envv;
	return ::cgiMain();
}

int WINAPI											WinMain				
	(	_In_		HINSTANCE	hInstance
	,	_In_opt_	HINSTANCE	hPrevInstance
	,	_In_		LPSTR		lpCmdLine
	,	_In_		int			nShowCmd
	) {
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;
	return ::cgiMain();
}
