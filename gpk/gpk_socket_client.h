#include "gpk_connection_ip.h"

#ifndef GPK_UDP_CLIENT_H_23627
#define GPK_UDP_CLIENT_H_23627

namespace gpk
{
	struct SSocketClient {
		::gpk::SConnection					Connection;
		::gpk::SSocketConnection			Socket;
		::gpk::SIPv4End						AddressConnect;
		int16_t								AdapterIndex;
		::gpk::apobj<::gpk::SMessage>		CacheSend;	// This is only to speed up message processing.
		::gpk::apobj<::gpk::SMessage>		CacheSent;	// This is only to speed up message processing.
	};

	::gpk::err_t			clientDisconnect		(SSocketClient & client);
	::gpk::err_t			clientConnect			(SSocketClient & client);
	::gpk::err_t			clientUpdate			(SSocketClient & client);
}

#endif // GPK_UDP_CLIENT_H_23627
