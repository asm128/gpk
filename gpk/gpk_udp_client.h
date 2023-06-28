#include "gpk_connection.h"

#ifndef GPK_UDP_CLIENT_H_23627
#define GPK_UDP_CLIENT_H_23627

namespace gpk
{
	struct SUDPClient : public ::gpk::SUDPConnection {
		::gpk::SIPv4									AddressConnect;
		int16_t											AdapterIndex;
		::gpk::apobj<::gpk::SUDPMessage>		CacheSend;	// This is only to speed up message processing.
		::gpk::apobj<::gpk::SUDPMessage>		CacheSent;	// This is only to speed up message processing.
	};

	::gpk::error_t									clientDisconnect			(::gpk::SUDPClient & client);
	::gpk::error_t									clientConnect				(::gpk::SUDPClient & client);
	::gpk::error_t									clientUpdate				(::gpk::SUDPClient & client);
}

#endif // GPK_UDP_CLIENT_H_23627
