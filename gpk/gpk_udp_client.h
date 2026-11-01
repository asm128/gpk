#include "gpk_connection.h"

#ifndef GPK_UDP_CLIENT_H_23627
#define GPK_UDP_CLIENT_H_23627

namespace gpk
{
	struct SUDPClient : public ::gpk::SUDPConnection {
		::gpk::SIPv4End						AddressConnect;
		s1_t								AdapterIndex;
		::gpk::apobj<::gpk::SUDPMessage>	CacheSend;	// This is only to speed up message processing.
		::gpk::apobj<::gpk::SUDPMessage>	CacheSent;	// This is only to speed up message processing.
	};

	::gpk::err_t			clientDisconnect		(SUDPClient & client);
	::gpk::err_t			clientConnect			(SUDPClient & client);
	::gpk::err_t			clientUpdate			(SUDPClient & client);
}

#endif // GPK_UDP_CLIENT_H_23627
