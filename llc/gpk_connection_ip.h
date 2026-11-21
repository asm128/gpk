#include "gpk_connection.h"
#include "gpk_stdsocket.h"
#include "gpk_tcpip.h"

#ifndef GPK_CONNECTION_IP_H_261025
#define GPK_CONNECTION_IP_H_261025

nmsp gpk
{
	struct SSocketConnection {
		auto_socket_close	Socket			= {};
		SIPv4End			Address			= {};
		u2_t				IdConnection	= u2_t(-1);
	};

	err_t	connectionSendQueue		(SConnection & connection, SSocketConnection & client, apobj<SMessage> & messageCacheSent, apobj<SMessage> & messageCacheSend);
	err_t	connectionHandleCommand	(SConnection & connection, SSocketConnection & client, SCommand & command, au0_t & receiveBuffer);
} // nmsp

#endif // GPK_CONNECTION_IP_H_261025
