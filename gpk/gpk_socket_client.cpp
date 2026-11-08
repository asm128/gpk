// http://www.gomorgan89.com
#include "gpk_socket_client.h"
#include "gpk_sync.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"

#if defined(GPK_WINDOWS)
#	include <process.h>
tydf	int			socklen_t;
#endif

GPK_USING_TYPEINT();

static	::gpk::err_t	clientConnectAttempt						(::gpk::SSocketClient & client)		{
	client.Connection.FirstPing = client.Connection.KeyPing							= 0;
	::gpk::SCommand												commandToSend								= {::gpk::ENDPOINT_COMMAND_CONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST,};	/* Data to send */
	sockaddr_in														sa_server									= {};				/* Information about the server */
	socklen_t														sa_length									= sizeof(struct sockaddr_in);
	client.Socket.Socket.close();
	if_fail_fe(gpk::tcpipAddressToSockaddr(client.AddressConnect, sa_server));
	sa_server.sin_port											= htons(client.AddressConnect.Port);
	if_true_vef(-1, INVALID_SOCKET == (client.Socket.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Failed to create socket for address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	if_fail_fe(gpk::tcpipAddressToSockaddr(client.AddressConnect, sa_server));
	sockaddr_in														sa_client									= {};				/* Information about the server */

	::gpk::SIPv4End localAddr;
	::gpk::tcpipAddress(rand() % (1024*32) + (1024*32), 0, ::gpk::TRANSPORT_PROTOCOL_UDP, localAddr);
	::gpk::tcpipAddressToSockaddr(localAddr, sa_client);
	if(-1 == ::bind(client.Socket.Socket, (sockaddr*)&sa_client, sizeof(sockaddr_in)))
		sa_client.sin_port	= 0;

	if_fail_fef(::sendto(client.Socket.Socket.Handle, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed to send to address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	sa_length													= sizeof(struct sockaddr_in);
	sa_server													= {};
	::gpk::SCommand												commandReceived								= {};	/* Data to send */
	int																received_bytes								= recvfrom(client.Socket.Socket.Handle, (sc_t *)&commandReceived, (int)sizeof(::gpk::SCommand), 0, (sockaddr *)&sa_server, &sa_length);
	if_true_vwf(-1, -1 == received_bytes, "Failed to receive connect response from '%u.%u.%u.%u:%u'.", GPK_IPV4_EXPAND(client.AddressConnect));
	::gpk::SIPv4End													temp										= {};
	::gpk::tcpipAddressFromSockaddr(sa_server, temp);
	if_true_vef(-1, temp.IP != client.AddressConnect.IP, "Invalid server response address! Expected: %u.%u.%u.%u:%u. Received from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(client.AddressConnect), GPK_IPV4_EXPAND(temp));
	client.Socket.Address												= temp;
	int																wsae										= WSAGetLastError() ;
	if_true_vef(-1, received_bytes == -1 && wsae != WSAEMSGSIZE, "Failed to receive connect response from address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	if_true_vef(-1, commandReceived.Type != ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE, "Invalid server command received! Command received: %s.", ::gpk::get_value_label(commandReceived.Type).begin());
	commandToSend.Packed										= 1;
	if_fail_fef(::sendto(client.Socket.Socket.Handle, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SCommand), 0, (sockaddr *)&sa_server, sizeof(struct sockaddr_in)), "Failed to send to address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	client.Connection.LastPing = client.Connection.FirstPing							= ::gpk::timeCurrentInUs();
	client.Connection.State												= ::gpk::CONNECTION_STATE_IDLE;
	return 0;
}

static	::gpk::err_t	clientQueueReceive								(::gpk::SSocketClient & client)		{
	::gpk::au0_t														receiveBuffer;
	while(client.Connection.State != ::gpk::CONNECTION_STATE_DISCONNECTED && client.Socket.Socket != INVALID_SOCKET) {
		sockaddr_in														sa_server									= {};				/* Information about the server */
		int																sa_length									= sizeof(struct sockaddr_in);
		::gpk::SCommand												commandReceived								= {};	/* Data to send */
		int																received_bytes								= recvfrom(client.Socket.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SCommand), MSG_PEEK, (sockaddr *)&sa_server, &sa_length);
		int																wsae										= WSAGetLastError();
		if(received_bytes == -1 && wsae != WSAEMSGSIZE) {
			if(wsae == WSAEINTR)
				continue;
			if(wsae == WSAENOTSOCK || wsae == WSANOTINITIALISED)
				return 0;
			if(wsae == WSAECONNRESET) {
				client.Connection.State = ::gpk::CONNECTION_STATE_DISCONNECTED;
				return 0;
			}
			warning_printf("Failed to receive message! Error: 0x%x.", wsae);
			received_bytes												= recvfrom(client.Socket.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SCommand), 0, 0, 0);
			continue;
		}	/* Receive time */
		::gpk::SIPv4End													temp										= {};
		::gpk::tcpipAddressFromSockaddr(sa_server, temp);
		if(temp.IP == client.Socket.Address.IP && commandReceived.Command == ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			if(temp.Port == client.Socket.Address.Port)
				if_true_e(::gpk::failed(::gpk::connectionHandleCommand(client.Connection, client.Socket, commandReceived, receiveBuffer)));
		}
		else {
			info_printf("Data received fro invalid ip. Local: %u, %u, %u, %u, %u. Remote: %u, %u, %u, %u, %u", GPK_IPV4_EXPAND(temp), GPK_IPV4_EXPAND(client.Socket.Address));
		}
		received_bytes												= recvfrom(client.Socket.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SCommand), 0, 0, 0);
	}
	return 0;
}

static	void												threadClientConnect							(void* pClient)						{ clientConnectAttempt(*(::gpk::SSocketClient*)pClient); }
static	void												threadUpdateClient							(void* pClient)						{
	::gpk::SSocketClient											& client									= *(::gpk::SSocketClient*)pClient;
	::clientQueueReceive(client);
	client.Connection.State												= ::gpk::CONNECTION_STATE_DISCONNECTED;
}

::gpk::err_t			gpk::clientUpdate							(::gpk::SSocketClient & client)		{
	if_true_vef(-1, client.Connection.State != ::gpk::CONNECTION_STATE_IDLE, "Not connected. Current state: %s.", ::gpk::get_value_label(client.Connection.State).begin());
	client.CacheSent.clear(); client.CacheSend.clear();
	return ::gpk::connectionSendQueue(client.Connection, client.Socket, client.CacheSent, client.CacheSend);
}

::gpk::err_t			gpk::clientDisconnect						(::gpk::SSocketClient & client)		{
	int																sa_length									= sizeof(struct sockaddr_in);
	sockaddr_in														sa_server									= {AF_INET,};
	::gpk::tcpipAddressToSockaddr(client.Socket.Address, sa_server);
	::gpk::SCommand												commandToSend								= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST,};	// Data to send */
	{
		lock_guard												lock										(client.Connection.Queue.MutexSend);
		for(u2_t i = 0; i < 2; ++i)
			sendto(client.Socket.Socket, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SCommand), 0, (sockaddr *)&sa_server, sa_length);	// Tranmsit data to get time
		client.Socket.Socket.close();
	}
	client.Connection.KeyPing												= 0;
	client.Connection.State												= ::gpk::CONNECTION_STATE_DISCONNECTED;
	::gpk::sleep(10);
	return 0;
}

::gpk::err_t			gpk::clientConnect							(::gpk::SSocketClient & client)		{
	u2_t						attempts									= 0;
	stxp	u2_t												MAX_ATTEMPTS								= 1;
	client.Connection.State												= ::gpk::CONNECTION_STATE_HANDSHAKE;
	{
		lock_guard												lock										(client.Connection.Queue.MutexSend);
		client.Connection.Queue.Send.clear();
		client.Connection.Queue.Sent.clear();
	}
	{
		lock_guard												lock										(client.Connection.Queue.MutexReceive);
		client.Connection.Queue.Received.clear();
	}
	do {
		client.Socket.Socket.close();
		client.Connection.State												= ::gpk::CONNECTION_STATE_HANDSHAKE;
		_beginthread(::threadClientConnect, 0, &client);
		for(u2_t i = 0; i < 1000; ++i) {
			::gpk::sleep(1);
			if(client.Connection.State == ::gpk::CONNECTION_STATE_IDLE)
				break;
		}
	} while(client.Connection.State != ::gpk::CONNECTION_STATE_IDLE && ++attempts < MAX_ATTEMPTS);
	if(client.Connection.State == ::gpk::CONNECTION_STATE_IDLE) {
		info_printf("Client connected in %u attempt%s", attempts + 1, (attempts) ? "s" : "");
		_beginthread(::threadUpdateClient, 0, &client);
	}
	else {
		client.Socket.Socket.close();
		client.Connection.State											= ::gpk::CONNECTION_STATE_DISCONNECTED;
		error_printf("Failed to connect to %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(client.AddressConnect));
		return -1;
	}
	return 0;
}
