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
		s1_t								AdapterIndex;
		bool								Listen								= false;
		s2_t								Timeout								= 3000000;	// Kill clients timeout
	};

	::gpk::err_t						serverStart							(::gpk::SUDPServer& serverInstance, u1_t port, s1_t adapterIndex = 0);
	::gpk::err_t						serverStop							(::gpk::SUDPServer& serverInstance);
	::gpk::err_t						serverPayloadCollect				(::gpk::SUDPServer & server, ::gpk::aobj<::gpk::apobj<::gpk::SUDPMessage>> & receivedMessages);

} // namespace

#endif // GPK_UDP_SERVER_H_23627
