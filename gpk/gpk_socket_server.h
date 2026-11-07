#include "gpk_connection_ip.h"

#ifndef GPK_SOCKET_SERVER_H_23627
#define GPK_SOCKET_SERVER_H_23627

namespace gpk
{
	struct SSocketServer {
		::gpk::auto_socket_close				Socket;
		::gpk::apobj<::gpk::SConnection>		Clients;
		::gpk::apobj<::gpk::SSocketConnection>	Sockets;
		::std::mutex							Mutex;
		::gpk::SIPv4End							Address								= {};
		s1_t									AdapterIndex						= 0;
		::gpk::TRANSPORT_PROTOCOL				Protocol							= ::gpk::TRANSPORT_PROTOCOL_UDP;
		bool									Listen								= false;
		s2_t									Timeout								= 3000000;	// Kill clients timeout
	};

	::gpk::err_t						serverStart							(::gpk::SSocketServer & serverInstance, u1_t port, s1_t adapterIndex = 0, ::gpk::TRANSPORT_PROTOCOL protocol = ::gpk::TRANSPORT_PROTOCOL_UDP);
	::gpk::err_t						serverStop							(::gpk::SSocketServer & serverInstance);
	::gpk::err_t						serverPayloadCollect				(::gpk::SSocketServer & server, ::gpk::aobj<::gpk::apobj<::gpk::SMessage>> & receivedMessages);
} // namespace

#endif // GPK_SOCKET_SERVER_H_23627
