#include "gpk_connection_ip.h"

#ifndef GPK_HTTP_SERVER_H_23627
#define GPK_HTTP_SERVER_H_23627

nmsp gpk
{
	struct SHTTPServer {
		auto_socket_close			Socket;
		apobj<SConnection>	Connections;
		apobj<SSocketConnection>	Clients;
		mutex						Mutex;
		SIPv4End				Address						= {};
		bool								Listen						= false;
		s2_t								Timeout						= 3000000;	// Kill clients timeout
	};

	err_t						serverStart					(SHTTPServer& serverInstance, u1_t port);
	err_t						serverStop					(SHTTPServer& serverInstance);

} // nmsp

#endif // GPK_UDP_SERVER_H_23627
