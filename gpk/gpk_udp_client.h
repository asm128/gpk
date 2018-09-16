#include "gpk_stdsocket.h"
#include "gpk_connection.h"

#ifndef GPK_UDP_CLIENT_H_238947239847
#define GPK_UDP_CLIENT_H_238947239847

namespace gpk
{
	struct SUDPClient {
		::gpk::auto_socket_close				Socket										= {};
		::gpk::SIPv4							AddressServer								= {};
		uint16_t								PortServer									= {};
		::gpk::UDP_CONNECTION_STATE				State										= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
		::gpk::SUDPClientQueue					Queue;
	};
}

#endif // GPK_UDP_CLIENT_H_238947239847