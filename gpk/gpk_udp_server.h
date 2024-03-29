#include "gpk_connection.h"

#ifndef GPK_UDP_SERVER_H_23627
#define GPK_UDP_SERVER_H_23627

namespace gpk
{
	struct SUDPServer {
		::gpk::auto_socket_close			Socket;
		::gpk::apobj<::gpk::SUDPConnection>	Clients;
		::std::mutex						Mutex;
		::gpk::SIPv4End						Address								= {};
		int16_t								AdapterIndex;
		bool								Listen								= false;
		int32_t								Timeout								= 3000000;	// Kill clients timeout
	};

	::gpk::error_t						serverStart							(::gpk::SUDPServer& serverInstance, uint16_t port, int16_t adapterIndex = 0);
	::gpk::error_t						serverStop							(::gpk::SUDPServer& serverInstance);
	::gpk::error_t						serverPayloadCollect				(::gpk::SUDPServer & server, ::gpk::aobj<::gpk::apobj<::gpk::SUDPMessage>> & receivedMessages);

} // namespace

#endif // GPK_UDP_SERVER_H_23627
