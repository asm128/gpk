#include "gpk_connection_ip.h"

#ifndef GPK_UDP_CLIENT_H_23627
#define GPK_UDP_CLIENT_H_23627

namespace gpk
{
	struct SUDPClient {
		::gpk::SConnection					Connection;
		::gpk::SSocketConnection			Socket;
		::gpk::SIPv4End						AddressConnect;
		int16_t								AdapterIndex;
		::gpk::apobj<::gpk::SMessage>		CacheSend;	// This is only to speed up message processing.
		::gpk::apobj<::gpk::SMessage>		CacheSent;	// This is only to speed up message processing.
	};

	::gpk::err_t			clientDisconnect		(SUDPClient & client);
	::gpk::err_t			clientConnect			(SUDPClient & client);
	::gpk::err_t			clientUpdate			(SUDPClient & client);
}

#endif // GPK_UDP_CLIENT_H_23627
