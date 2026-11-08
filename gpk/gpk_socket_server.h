#include "gpk_connection_ip.h"

#ifndef GPK_SOCKET_SERVER_H_23627
#define GPK_SOCKET_SERVER_H_23627

nmsp gpk
{
	struct SSocketServer {
		auto_socket_close			Socket			= {};
		apobj<SConnection>			Clients			= {};
		apobj<SSocketConnection>	Sockets			= {};
		mutex						Mutex			= {};
		SIPv4End					Address			= {};
		s1_t						AdapterIndex	= {};
		TRANSPORT_PROTOCOL			Protocol		= TRANSPORT_PROTOCOL_UDP;
		bool						Listen			= {};
		s2_t						Timeout			= 3000000;	// Kill clients timeout
	};

	err_t						serverStart							(SSocketServer & serverInstance, u1_t port, s1_t adapterIndex = 0, TRANSPORT_PROTOCOL protocol = TRANSPORT_PROTOCOL_UDP);
	err_t						serverStop							(SSocketServer & serverInstance);
	err_t						serverPayloadCollect				(SSocketServer & server, aobj<apobj<SMessage>> & receivedMessages);
} // nmsp

#endif // GPK_SOCKET_SERVER_H_23627
