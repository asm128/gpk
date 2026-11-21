#include "gpk_process_broker.h"

#include "gpk_string_compose.h"

#if defined(GPK_LINUX)
#	include <errno.h>
#	include <fcntl.h>
#	include <poll.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <wordexp.h>
#endif

using namespace gpk;

#if defined(GPK_LINUX)
sttc ::gpk::err_t	closeDescriptor			(int & descriptor) {
	if(0 > descriptor)
		return 0;
	cnst int result					= ::close(descriptor);
	descriptor						= -1;
	if(0 != result) {
		error_printf("Failed to close descriptor. errno: %i.", errno);
		return -1;
	}
	return 0;
}

sttc ::gpk::err_t	closeOnExec				(int descriptor) {
	cnst int flags					= ::fcntl(descriptor, F_GETFD);
	if(0 > flags || 0 > ::fcntl(descriptor, F_SETFD, flags | FD_CLOEXEC)) {
		error_printf("Failed to set close-on-exec for descriptor %i. errno: %i.", descriptor, errno);
		return -1;
	}
	return 0;
}
#endif

::gpk::err_t	gpk::createChildProcess
	( SProcess			& process
	, vu0_t				environmentBlock
	, vcst_t			appPath
	, vsc_t			commandLine
	, b8_t				debugMessageBox
	) {
#if defined(GPK_WINDOWS)
	::gpk::vcst_t				szCmdlineApp			= appPath;
	::gpk::vstr_t				szCmdlineFinal			= commandLine;
	b8_t						bSuccess				= false;
	stxp	b8_t				isUnicodeEnv			= false;
	stxp	u2_t 				creationFlags			= CREATE_SUSPENDED | (isUnicodeEnv ? CREATE_UNICODE_ENVIRONMENT : 0);

	if(INVALID_HANDLE_VALUE != process.ProcessInfo.hProcess	){ CloseHandle(process.ProcessInfo.hProcess	); }
	bSuccess				= CreateProcessA(szCmdlineApp
		, szCmdlineFinal.begin()
		, nullptr
		, nullptr
		, true
		, creationFlags
		, environmentBlock.begin()
		, NULL
		, &process.StartInfo
		, &process.ProcessInfo
		) ? true : false;
	if_true_vef(-1, false == bSuccess, "Failed to create process, because... '%s'.", "???");
	if(debugMessageBox) {
		::gpk::string			userMessage				= {};
		if_fail_fe(gpk::append_strings(userMessage, "Attach your debugger to '", szCmdlineApp, "' and press OK to initiate the process' main thread."));
		MessageBoxA(0, userMessage.begin(), "Last chance!", MB_OK | MB_TOPMOST);
	}
	info_printf("Creating process '%.*s' with command line '%.*s'", GPK_VIEW_ARGS(szCmdlineApp), GPK_VIEW_ARGS(szCmdlineFinal));
	ResumeThread(process.ProcessInfo.hThread);
	return 0;
#elif defined(GPK_LINUX)
	::gpk::apod<sc_t*> environmentPointers;
	for(::gpk::u2_t offset = 0; offset < environmentBlock.size() && environmentBlock[offset]; ) {
		if_fail_fe(environmentPointers.push_back((sc_t*)&environmentBlock[offset]));
		while(offset < environmentBlock.size() && environmentBlock[offset])
			++offset;
		if_true_fef(offset >= environmentBlock.size(), "Unterminated environment entry at offset %u.", offset);
		++offset;
	}
	if_fail_fe(environmentPointers.push_back(nullptr));

	::wordexp_t words = {};
	cnst int wordResult = ::wordexp(commandLine.begin(), &words, WRDE_NOCMD);
	if(0 != wordResult) {
		if(WRDE_NOSPACE == wordResult)
			::wordfree(&words);
		error_printf("Failed to parse command line '%.*s'. wordexp: %i.", GPK_VIEW_ARGS(commandLine), wordResult);
		return -1;
	}

	cnst pid_t child = ::fork();
	if(0 > child) {
		cnst int error = errno;
		::wordfree(&words);
		error_printf("Failed to create process '%.*s'. errno: %i.", GPK_VIEW_ARGS(appPath), error);
		return -1;
	}
	if(0 == child) {
		if(0 > ::dup2(process.StartInfo.hStdInput , STDIN_FILENO )
		|| 0 > ::dup2(process.StartInfo.hStdOutput, STDOUT_FILENO)
		|| 0 > ::dup2(process.StartInfo.hStdError , STDERR_FILENO))
			::_exit(126);
		sc_t * fallbackArguments[] = {(sc_t*)appPath.begin(), nullptr};
		sc_t ** arguments = words.we_wordc ? words.we_wordv : fallbackArguments;
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
#endif
}

#if defined(GPK_WINDOWS)
::gpk::err_t	gpk::writeToPipe			(cnst SProcessHandles & handles, vcu0_t chBufToSend) {
	DWORD						dwWritten				= 0;
	b8_t						bSuccess				= false;
	if_true_ef(false == (bSuccess = WriteFile(handles.ChildStd_IN_Write, chBufToSend.begin(), chBufToSend.size(), &dwWritten, NULL) ? true : false), "%s", "Failed to write to child process' standard input.");
	if_true_vef(-1, false == (CloseHandle(handles.ChildStd_IN_Write) ? true : false), "%s", "Failed to close the pipe handle so the child process stops reading.");
	return bSuccess ? 0 : -1;
}
#elif defined(GPK_LINUX)
::gpk::err_t	gpk::writeToPipe			(SProcessHandles & handles, vcu0_t chBufToSend) {
	::gpk::u2_t offset = 0;
	while(offset < chBufToSend.size()) {
		cnst ssize_t written = ::write(handles.ChildStd_IN_Write, &chBufToSend[offset], chBufToSend.size() - offset);
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
::gpk::err_t	gpk::readFromPipe			(cnst SProcess & process, cnst SProcessHandles & handles, au0_t & readBytes) {
	sttc	::gpk::au0_t		chBuf;
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
::gpk::err_t	gpk::readFromPipe			(SProcess & process, SProcessHandles & handles, au0_t & readBytes) {
	sttc ::gpk::au0_t chBuf;
	stxp ::gpk::u2_t BUFSIZE = 1024 * 1024;
	if_fail_fe(chBuf.resize(BUFSIZE));
	::pollfd descriptors[2] = {
		{handles.ChildStd_OUT_Read, POLLIN | POLLHUP, 0},
		{handles.ChildStd_ERR_Read, POLLIN | POLLHUP, 0},
	};
	::gpk::u0_t descriptorsOpen = 2;
	while(descriptorsOpen) {
		cnst int pollResult = ::poll(descriptors, 2, -1);
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
			cnst ssize_t bytesRead = ::read(descriptor.fd, chBuf.begin(), chBuf.size());
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
				if(0 == index)
					if_fail_fe(::closeDescriptor(handles.ChildStd_OUT_Read));
				else
					if_fail_fe(::closeDescriptor(handles.ChildStd_ERR_Read));
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

::gpk::err_t	gpk::initHandles			(SProcess & process, SProcessHandles & handles) {
#if defined(GPK_WINDOWS)
	SECURITY_ATTRIBUTES			saAttr;
	saAttr.bInheritHandle	= TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if_true_vef(-1, false == (b8_t)CreatePipe			(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT.");
	if_true_vef(-1, false == (b8_t)SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited.");
	if_true_vef(-1, false == (b8_t)CreatePipe			(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0)	, "StdoutRd CreatePipe: '%s'."			, "Failed to create a pipe for the child process's STDOUT.");
	if_true_vef(-1, false == (b8_t)SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0)						, "Stdout SetHandleInformation: '%s'."	, "Failed to ensure the read handle to the pipe for STDOUT is not inherited.");
	if_true_vef(-1, false == (b8_t)CreatePipe			(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0)		, "Stdin CreatePipe: '%s'."				, "Failed to create a pipe for the child process's STDIN.");
	if_true_vef(-1, false == (b8_t)SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0)						, "Stdin SetHandleInformation: '%s'."	, "Failed to ensure the write handle to the pipe for STDIN is not inherited.");
	process.StartInfo.hStdError		= handles.ChildStd_ERR_Write;
	process.StartInfo.hStdOutput	= handles.ChildStd_OUT_Write;
	process.StartInfo.hStdInput		= handles.ChildStd_IN_Read;
	process.ProcessInfo.hProcess	= INVALID_HANDLE_VALUE;
	process.StartInfo.dwFlags		|= STARTF_USESTDHANDLES;
#elif defined(GPK_LINUX)
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
	process.StartInfo.hStdError		= handles.ChildStd_ERR_Write;
	process.StartInfo.hStdOutput	= handles.ChildStd_OUT_Write;
	process.StartInfo.hStdInput		= handles.ChildStd_IN_Read;
	process.ProcessInfo.hProcess	= -1;
#endif
	return 0;
}

::gpk::err_t	gpk::closeProcessIO			(SProcess & process) {
#if defined(GPK_WINDOWS)
	safe_closehandle(process.StartInfo.hStdError	);
	safe_closehandle(process.StartInfo.hStdInput	);
	safe_closehandle(process.StartInfo.hStdOutput	);
#elif defined(GPK_LINUX)
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdError	));
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdInput	));
	if_fail_fe(::closeDescriptor(process.StartInfo.hStdOutput	));
#endif
	return 0;
}

::gpk::err_t	gpk::closeProcess			(SProcess & process) {
#if defined(GPK_WINDOWS)
	safe_closehandle(process.ProcessInfo.hProcess	);
	safe_closehandle(process.ProcessInfo.hThread	);
#elif defined(GPK_LINUX)
	process.ProcessInfo.hProcess = -1;
	process.ProcessInfo.hThread = -1;
#endif
	return 0;
}
