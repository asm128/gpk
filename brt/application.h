#include "gpk_udp_server.h"

#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace brt // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SProcessHandles {
		HANDLE						ChildStd_IN_Read		= INVALID_HANDLE_VALUE;
		HANDLE						ChildStd_IN_Write		= INVALID_HANDLE_VALUE;
		HANDLE						ChildStd_OUT_Read		= INVALID_HANDLE_VALUE;
		HANDLE						ChildStd_OUT_Write		= INVALID_HANDLE_VALUE;
		HANDLE						ChildStd_ERR_Read		= INVALID_HANDLE_VALUE;
		HANDLE						ChildStd_ERR_Write		= INVALID_HANDLE_VALUE;
	};

	struct SProcess {
		PROCESS_INFORMATION			ProcessInfo				= {};
		STARTUPINFOA				StartInfo				= {sizeof(STARTUPINFOA)};
	};

	struct SApplication {
		::gpk::SFramework																	Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>					Offscreen							= {};

		::gpk::SUDPServer																	Server								= {};
		::gpk::array_obj<::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>>	ReceivedPerClient;
		::gpk::array_obj<::gpk::array_obj<::gpk::array_pod<char_t>>>						ClientResponses;
		::gpk::array_obj<::brt::SProcess>													ClientProcesses;
		::gpk::array_obj<::brt::SProcessHandles>											ClientIOHandles;
		::gpk::view_const_char																ProcessFileName						= "";
		::gpk::view_const_char																ProcessMockPath						= "";
		::gpk::view_const_char																ProcessParams						= "";
		SECURITY_ATTRIBUTES																	DefaultSecurityForPipeHandles		= {sizeof(SECURITY_ATTRIBUTES)};

		::gpk::array_pod<char_t>															szCmdlineApp						= "";
		::gpk::array_pod<char_t>															szCmdlineFinal						= "";


		int32_t																				IdExit								= -1;

		::std::mutex																		LockGUI;
		::std::mutex																		LockRender;

																							SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
