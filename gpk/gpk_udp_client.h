#include "gpk_connection.h"

#ifndef GPK_UDP_CLIENT_H_238947239847
#define GPK_UDP_CLIENT_H_238947239847

namespace gpk
{
	struct SUDPClient : public ::gpk::SUDPConnection {
		uint16_t														PortServer			= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	MessageBuffer;

	};
}

#endif // GPK_UDP_CLIENT_H_238947239847