#include "gpk_udp_server.h"

#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace brt // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SProcessHandles {
		HANDLE				ChildStd_IN_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_IN_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Write		= INVALID_HANDLE_VALUE;
	};

	struct SProcess {
		PROCESS_INFORMATION	ProcessInfo				= {};
		STARTUPINFOA		StartInfo				= {sizeof(STARTUPINFOA)};
	};

	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SUDPServer				Server								= {};
		::gpk::apobj<::gpk::apobj<::gpk::SUDPMessage>>	ReceivedPerClient;
		::gpk::apobj<::gpk::apobj<::gpk::au8>>			ClientResponses;
		::gpk::aobj<::brt::SProcess>					ClientProcesses;
		::gpk::aobj<::brt::SProcessHandles>				ClientIOHandles;

		::gpk::vcc			ProcessFileName						= "";
		::gpk::vcc			ProcessMockPath						= "";
		::gpk::vcc			ProcessParams						= "";
		SECURITY_ATTRIBUTES	DefaultSecurityForPipeHandles		= {sizeof(SECURITY_ATTRIBUTES)};

		::gpk::ac			szCmdlineApp						= "";
		::gpk::ac			szCmdlineFinal						= "";

		::gpk::cid_t		IdExit								= -1;

		::std::mutex		LockGUI;
		::std::mutex		LockRender;

										SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
