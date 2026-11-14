#include "gpk_process_internal.h"

#ifdef GPK_WINDOWS
// Windows declarations already arrive through GPK's synchronization headers.
nmsp gpk::process_detail {
sttc	gpk::err_t		processHandleClose		(TWindowsHandle & handle) {
	if(0 == handle || INVALID_WINDOWS_HANDLE == handle) {
		handle					= INVALID_WINDOWS_HANDLE;
		rtrn 0;
	}
	TWindowsHandle				closing					= handle;
	handle					= INVALID_WINDOWS_HANDLE;
	rtrn CloseHandle((HANDLE)closing) ? 0 : -1;
}

stct auto_process_handle : auto_handler<TWindowsHandle*, nullptr> {
	explicit auto_process_handle(TWindowsHandle & handle) nxpt { Handle = &handle; }
	~auto_process_handle() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		TWindowsHandle * closing = Handle;
		Handle = 0;
		rtrn processHandleClose(*closing);
	}
};

gpk::err_t			processClose			(SProcessWin32 & process) {
	b8_t					failedToClose				= false;
	TWindowsHandle					stdoutWrite				= process.Startup.HandleStdOutput;
	if(failed(processHandleClose(process.Startup.HandleStdInput)))
		failedToClose			= true;
	if(failed(processHandleClose(process.Startup.HandleStdOutput)))
		failedToClose			= true;
	if(process.Startup.HandleStdError == stdoutWrite)
		process.Startup.HandleStdError = INVALID_WINDOWS_HANDLE;
	else if(failed(processHandleClose(process.Startup.HandleStdError)))
		failedToClose			= true;
	if(failed(processHandleClose(process.StdinWrite)))
		failedToClose			= true;
	if(failed(processHandleClose(process.StdoutRead)))
		failedToClose			= true;
	if(failed(processHandleClose(process.Info.hThread)))
		failedToClose			= true;
	if(failed(processHandleClose(process.Info.hProcess)))
		failedToClose			= true;
	process.Info				= {};
	process.Startup			= {sizeof(STARTUPINFOA)};
	rtrn failedToClose ? ((err_t)-1) : 0;
}

gpk::err_t processRedirect(SProcessWin32 & process) {
	if_true_fe(process.Info.hProcess);
	if_true_fe(process.Info.hThread);
	if_fail_fe(processClose(process));
	SProcessNativeScope<SProcessWin32> rollback{process};
	SECURITY_ATTRIBUTES security = {sizeof(SECURITY_ATTRIBUTES), 0, TRUE};
	HANDLE readEnd = 0, writeEnd = 0;
	if(!CreatePipe(&readEnd, &writeEnd, &security, 0))
		rtrn ((err_t)-1);
	process.StdoutRead = (TWindowsHandle)readEnd;
	process.Startup.HandleStdOutput = process.Startup.HandleStdError = (TWindowsHandle)writeEnd;
	if_true_ve(((err_t)-1), !SetHandleInformation(readEnd, HANDLE_FLAG_INHERIT, 0));
	if_true_ve(((err_t)-1), !CreatePipe(&readEnd, &writeEnd, &security, 0));
	process.Startup.HandleStdInput = (TWindowsHandle)readEnd;
	process.StdinWrite = (TWindowsHandle)writeEnd;
	if_true_ve(((err_t)-1), !SetHandleInformation(writeEnd, HANDLE_FLAG_INHERIT, 0));
	process.Startup.Flags |= STARTF_USESTDHANDLES;
	rollback.Inactive = true;
	rtrn 0;
}

tplT ndsi	T *	data_or_null		(view<T> yourView)	nxpt	{ return yourView.size() ? &yourView[0] : 0; }

gpk::err_t		processCreate
	( SProcessWin32	& process
	, vcst_t		application
	, vcst_t		commandLine
	, vcs0_t		environment
	, u2_t			creationFlags
	) {
	if_true_fe(process.Info.hProcess);
	if_true_fe(process.Info.hThread);
	string					applicationStorage			= application;
	string					commandLineStorage			= commandLine;
	as0_t					environmentStorage			= environment;
	cnst BOOL				inheritHandles				= (process.Startup.Flags & STARTF_USESTDHANDLES) ? TRUE : FALSE;
	STARTUPINFOA			startup						= {};
	::gpk::cpycst(startup, process.Startup);
	PROCESS_INFORMATION		info						= {};
	if_zero_vef(((err_t)-1), CreateProcessA(data_or_null(applicationStorage), data_or_null(commandLineStorage), 0, 0, inheritHandles, creationFlags, data_or_null(environmentStorage), 0, &startup, &info)
		, "\"%.*s\", \"%.*s\"", GPK_VIEW_ARGS(applicationStorage), GPK_VIEW_ARGS(commandLineStorage)
	);
	gpk::cpycst(process.Info, info);
	TWindowsHandle			stdoutWrite					= process.Startup.HandleStdOutput;
	b8_t					failedToClose				= false;
	if(failed(processHandleClose(process.Startup.HandleStdInput)))
		failedToClose			= true;
	if(failed(processHandleClose(process.Startup.HandleStdOutput)))
		failedToClose			= true;
	if(process.Startup.HandleStdError == stdoutWrite)
		process.Startup.HandleStdError = INVALID_WINDOWS_HANDLE;
	else if(failed(processHandleClose(process.Startup.HandleStdError)))
		failedToClose			= true;
	rtrn failedToClose ? ((err_t)-1) : 0;
}

gpk::err_t			processWrite			(SProcessWin32 & process, vcu0_t input) {
	if(0 == process.StdinWrite || INVALID_WINDOWS_HANDLE == process.StdinWrite)
		rtrn input.size() ? ((err_t)-1) : 0;
	auto_process_handle scope{process.StdinWrite};
	u2_t			offset						= 0;
	while(offset < input.size()) {
		DWORD				written						= 0;
		if_true_ve(((err_t)-1), FALSE == WriteFile((HANDLE)process.StdinWrite, input.begin() + offset, input.size() - offset, &written, 0) || 0 == written);
		offset					+= written;
	}
	rtrn scope.Close();
}

gpk::err_t			processRead			(SProcessWin32 & process, au0_t & output) {
	if(0 == process.StdoutRead || INVALID_WINDOWS_HANDLE == process.StdoutRead)
		rtrn ((err_t)-1);
	auto_process_handle scope{process.StdoutRead};
	u0_t					buffer[4096]					= {};
	while(true) {
		DWORD					readCount						= 0;
		if(FALSE == ReadFile((HANDLE)process.StdoutRead, buffer, sizeof(buffer), &readCount, 0)) {
			cnst DWORD				lastError						= GetLastError();
			rtrn ERROR_BROKEN_PIPE == lastError ? 0 : ((err_t)-1);
		}
		if(0 == readCount)
			break;
		if_fail_ve(((err_t)-1), output.append(buffer, readCount));
	}
	rtrn scope.Close();
}

gpk::err_t			processWait			(SProcessWin32 & process, u2_t & exitCode) {
	if(0 == process.Info.hProcess || INVALID_WINDOWS_HANDLE == process.Info.hProcess)
		rtrn ((err_t)-1);
	if(WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)process.Info.hProcess, INFINITE))
		rtrn ((err_t)-1);
	DWORD					processExitCode		= 0;
	if(FALSE == GetExitCodeProcess((HANDLE)process.Info.hProcess, &processExitCode))
		rtrn ((err_t)-1);
	exitCode			= processExitCode;
	rtrn 0;
}

stct SProcessInput {
	SProcessWin32	& Process;
	vcu0_t			Input;
	gpk::err_t		Result		= ((err_t)-1);
};

// Native thread entry point; the operation's err_t stays in the shared context.
sttc DWORD WINAPI threadInputWrite(void * parameter) {
	SProcessInput		& context = *(SProcessInput*)parameter;
	context.Result = processWrite(context.Process, context.Input);
	rtrn 0;
}

stct auto_process_writer : auto_handler<HANDLE, nullptr> {
	err_t Result = ((err_t)-1);
	~auto_process_writer() { if_fail_e(Close()); }
	err_t Close() {
		if(!Handle) rtrn 0;
		// Cancellation can race the writer entering WriteFile; retry until it has exited.
		for(;;) {
			cnst DWORD waited = WaitForSingleObject(Handle, failed(Result) ? 10 : INFINITE);
			if(WAIT_OBJECT_0 == waited)
				break;
			if(WAIT_TIMEOUT != waited)
				Result = ((err_t)-1);
			if(FALSE == CancelSynchronousIo(Handle) && ERROR_NOT_FOUND != GetLastError())
				Result = ((err_t)-1);
		}
		cnst HANDLE closing = Handle;
		Handle = 0;
		rtrn CloseHandle(closing) ? 0 : ((err_t)-1);
	}
};

gpk::err_t processCommunicate(SProcessWin32 & process, vcu0_t input, au0_t & output) {
	if(0 == input.size()) {
		if_fail_fe(processWrite(process, input));
		rtrn processRead(process, output);
	}
	SProcessInput context = {process, input};
	auto_process_writer writer;
	writer.Handle = CreateThread(0, 0, threadInputWrite, &context, 0, 0);
	if_null_fef(writer.Handle, "Cannot start process input writer: %lu.", GetLastError());
	writer.Result = processRead(process, output);
	cnst err_t closeResult = writer.Close();
	rtrn failed(writer.Result) ? writer.Result : failed(context.Result) ? context.Result : closeResult;
}

}
#endif

