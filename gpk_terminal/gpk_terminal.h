#include "gpk_socket_server.h"
#include "gpk_process_broker.h"

#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_string.h"

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace brt // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SSocketServer				Server								= {};
		::gpk::apobj<::gpk::apobj<::gpk::SMessage>>	ReceivedPerClient;
		::gpk::apobj<::gpk::apobj<::gpk::au0_t>>			ClientResponses;
		::gpk::aobj<::gpk::SProcess>					ClientProcesses;
		::gpk::aobj<::gpk::SProcessHandles>				ClientIOHandles;

		::gpk::vcst_t						ProcessFileName						= "";
		::gpk::vcst_t						ProcessMockPath						= "";
		::gpk::vcst_t						ProcessParams						= "";

		::gpk::string			szCmdlineApp						= "";
		::gpk::string			szCmdlineFinal						= "";

		::gpk::mutex		LockGUI;
		::gpk::mutex		LockRender;

										SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
