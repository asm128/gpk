#include "gpk_socket_server.h"

#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_string.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace brt // I'm gonna use a different namespace in order to test a few things about the macros.
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
			::gpk::s2_t		hProcess				= -1;
			::gpk::s2_t		hThread				= -1;
		} ProcessInfo;
		struct {
			int					hStdError				= -1;
			int					hStdOutput				= -1;
			int					hStdInput				= -1;
			::gpk::u2_t		dwFlags				= {};
		} StartInfo;
		::gpk::u2_t			ExitCode				= {};
#endif
	};

	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SSocketServer				Server								= {};
		::gpk::apobj<::gpk::apobj<::gpk::SMessage>>	ReceivedPerClient;
		::gpk::apobj<::gpk::apobj<::gpk::au0_t>>			ClientResponses;
		::gpk::aobj<::brt::SProcess>					ClientProcesses;
		::gpk::aobj<::brt::SProcessHandles>				ClientIOHandles;

		::gpk::vcst_t						ProcessFileName						= "";
		::gpk::vcst_t						ProcessMockPath						= "";
		::gpk::vcst_t						ProcessParams						= "";
#if defined(GPK_WINDOWS)
		SECURITY_ATTRIBUTES	DefaultSecurityForPipeHandles		= {sizeof(SECURITY_ATTRIBUTES)};
#endif

		::gpk::string			szCmdlineApp						= "";
		::gpk::string			szCmdlineFinal						= "";

		::gpk::cid_t		IdExit								= -1;

		::std::mutex		LockGUI;
		::std::mutex		LockRender;

										SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
