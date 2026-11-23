#include "gpk_aobj_pobj.h"
#include "gpk_string.h"
#include "gpk_sync.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#endif

#ifndef GPK_PROCESS_BROKER_H_23627
#define GPK_PROCESS_BROKER_H_23627

namespace gpk
{
	struct SProcessHandles {
#if defined(GPK_WINDOWS)
		HANDLE				ChildStd_IN_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_IN_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Write		= INVALID_HANDLE_VALUE;
#elif defined(GPK_LINUX)
		int					ChildStd_IN_Read		= -1;
		int					ChildStd_IN_Write		= -1;
		int					ChildStd_OUT_Read		= -1;
		int					ChildStd_OUT_Write		= -1;
		int					ChildStd_ERR_Read		= -1;
		int					ChildStd_ERR_Write		= -1;
#endif
	};

	struct SProcess {
#if defined(GPK_WINDOWS)
		PROCESS_INFORMATION	ProcessInfo				= {};
		STARTUPINFOA		StartInfo				= {sizeof(STARTUPINFOA)};
#elif defined(GPK_LINUX)
		struct {
			s2_t				hProcess				= -1;
			s2_t				hThread				= -1;
		} ProcessInfo;
		struct {
			int					hStdError				= -1;
			int					hStdOutput				= -1;
			int					hStdInput				= -1;
			u2_t				dwFlags				= {};
		} StartInfo;
		u2_t				ExitCode				= {};
#endif
	};

	struct SProcessBrokerJob {
		au0_t					Request;
		au0_t					Response;
		au0_t					ErrorOutput;
		string					CommandLineApp;
		string					CommandLine;
		SProcessHandles			IOHandles;
		SProcess				Process;
		refcount_t				ErrorDone			= 0;
		err_t					ErrorResult			= 0;
		refcount_t				Done				= 0;
		err_t					Result				= 0;
	};

	struct SProcessBroker {
		string					CommandLine;
		string					CommandLineApp;
		apobj<SProcessBrokerJob>	Jobs;
	};

	err_t		processBrokerRun		(SProcessBrokerJob & job);
	err_t		initHandles			(SProcess & process, SProcessHandles & handles);
	err_t		createChildProcess		(SProcess & process, vu0_t environmentBlock, vcst_t appPath, vsc_t commandLine, b8_t debugMessageBox = false);
#if defined(GPK_WINDOWS)
	err_t		writeToPipe			(cnst SProcessHandles & handles, vcu0_t bytesToSend);
	err_t		readFromPipe			(cnst SProcess & process, cnst SProcessHandles & handles, au0_t & readBytes);
#elif defined(GPK_LINUX)
	err_t		writeToPipe			(SProcessHandles & handles, vcu0_t bytesToSend);
	err_t		readFromPipe			(SProcess & process, SProcessHandles & handles, au0_t & readBytes);
#endif
	err_t		closeProcessIO			(SProcess & process);
	err_t		closeProcess			(SProcess & process);
}

#endif // GPK_PROCESS_BROKER_H_23627
