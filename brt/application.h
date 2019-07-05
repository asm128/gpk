#include "gpk_udp_server.h"

#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace brt // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SProcessHandles {
		HANDLE						ChildStd_IN_Read	= nullptr;
		HANDLE						ChildStd_IN_Write	= nullptr;
		HANDLE						ChildStd_OUT_Read	= nullptr;
		HANDLE						ChildStd_OUT_Write	= nullptr;
	};

	struct SProcess {
		PROCESS_INFORMATION			ProcessInfo				= {}; 
		STARTUPINFO					StartInfo				= {sizeof(STARTUPINFO)};
	};

	struct SApplication {
		::gpk::SFramework																	Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>					Offscreen							= {};

		::gpk::SUDPServer																	Server								= {};
		::gpk::array_obj<::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>>	ReceivedPerClient;
		::gpk::array_obj<::gpk::array_obj<::gpk::array_pod<char_t>>>						ClientResponses;
		::gpk::array_obj<::brt::SProcess>													ClientProcesses;
		::gpk::array_obj<::brt::SProcessHandles>											ClientIOHandles;
		::gpk::view_const_string															ProcessFileName						= "./test_cgi_process.exe";
		SECURITY_ATTRIBUTES																	DefaultSecurityForPipeHandles		= {sizeof(SECURITY_ATTRIBUTES)}; 

		int32_t																				IdExit								= -1;

		::std::mutex																		LockGUI;
		::std::mutex																		LockRender;

																							SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
