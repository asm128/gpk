#include "gpk_socket_server.h"
#include "gpk_string.h"
#include "gpk_sync.h"
#include "gpk_json.h"
#ifdef GPK_WINDOWS
#	include <Windows.h>
#endif

#ifndef GPK_PROCESS_BROKER_H_23627
#define GPK_PROCESS_BROKER_H_23627


namespace gpk
{
#ifdef GPK_WINDOWS
	struct SProcessHandles {
		HANDLE							ChildStd_IN_Read	= INVALID_HANDLE_VALUE;
		HANDLE							ChildStd_IN_Write	= INVALID_HANDLE_VALUE;
		HANDLE							ChildStd_OUT_Read	= INVALID_HANDLE_VALUE;
		HANDLE							ChildStd_OUT_Write	= INVALID_HANDLE_VALUE;
		HANDLE							ChildStd_ERR_Read	= INVALID_HANDLE_VALUE;
		HANDLE							ChildStd_ERR_Write	= INVALID_HANDLE_VALUE;
	};

	struct SProcess {
		PROCESS_INFORMATION				ProcessInfo			= {};
		STARTUPINFOA					StartInfo			= {sizeof(STARTUPINFOA)};
	};
#endif
	struct SProcessBrokerJob {
		::gpk::pobj<::gpk::SConnection>	Connection;
		::gpk::au0_t					Request;
		::gpk::au0_t					Response;
		::gpk::string					CommandLineApp;
		::gpk::string					CommandLine;
		SProcessHandles					IOHandles;
		SProcess						Process;
		::gpk::refcount_t				Done				= 0;
		::gpk::err_t					Result				= 0;
	};

	struct SProcessBroker {
		::gpk::u1_t						Port				= 9998;
		::gpk::string					CommandLine			;
		::gpk::string					CommandLineApp		;
		::gpk::vcst_t					ProcessParams		;
		::gpk::vcst_t					ProcessFileName		;
		::gpk::vcst_t					ProcessMockPath		;
		::gpk::SSocketServer			Server				;
		::gpk::s1_t						Adapter				;
		::gpk::b8_t						Started				;
		::gpk::apobj<SProcessBrokerJob>	Jobs				;
	};

	::gpk::err_t	processBrokerSetup		(SProcessBroker & broker, cnst ::gpk::SJSONReader & jsonReader);
	::gpk::err_t	processBrokerUpdate		(SProcessBroker & broker);
	::gpk::err_t	processBrokerShutdown	(SProcessBroker & broker);
}

#endif // LLS_PROCESS_BROKER_H_23627
