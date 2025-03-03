// http://www.gomorgan89.com
#include "gpk_udp_client.h"
#include "gpk_sync.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"

#if defined(GPK_WINDOWS)
#	include <process.h>
typedef	int			socklen_t;
#endif

GPK_USING_TYPEINT();

static	::gpk::error_t	clientConnectAttempt						(::gpk::SUDPClient & client)		{
	client.FirstPing = client.KeyPing							= 0;
	::gpk::SUDPCommand												commandToSend								= {::gpk::ENDPOINT_COMMAND_CONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST,};	/* Data to send */
	sockaddr_in														sa_server									= {};				/* Information about the server */
	socklen_t														sa_length									= sizeof(struct sockaddr_in);
	client.Socket.close();
	gpk_necs(gpk::tcpipAddressToSockaddr(client.AddressConnect, sa_server));
	sa_server.sin_port											= htons(client.AddressConnect.Port);
	ree_if(INVALID_SOCKET == (client.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Failed to create socket for address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	gpk_necs(gpk::tcpipAddressToSockaddr(client.AddressConnect, sa_server));
	sockaddr_in														sa_client									= {};				/* Information about the server */

	::gpk::SIPv4End localAddr;
	::gpk::tcpipAddress(rand() % (1024*32) + (1024*32), 0, ::gpk::TRANSPORT_PROTOCOL_UDP, localAddr);
	::gpk::tcpipAddressToSockaddr(localAddr, sa_client);
	if(-1 == ::bind(client.Socket, (sockaddr*)&sa_client, sizeof(sockaddr_in)))
		sa_client.sin_port	= 0;

	gpk_necall(::sendto(client.Socket.Handle, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed to send to address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	sa_length													= sizeof(struct sockaddr_in);
	sa_server													= {};
	::gpk::SUDPCommand												commandReceived								= {};	/* Data to send */
	int																received_bytes								= recvfrom(client.Socket.Handle, (sc_t *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, &sa_length);
	rew_if(-1 == received_bytes, "Failed to receive connect response from '%u.%u.%u.%u:%u'.", GPK_IPV4_EXPAND(client.AddressConnect));
	::gpk::SIPv4End													temp										= {};
	::gpk::tcpipAddressFromSockaddr(sa_server, temp);
	ree_if(temp.IP != client.AddressConnect.IP, "Invalid server response address! Expected: %u.%u.%u.%u:%u. Received from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(client.AddressConnect), GPK_IPV4_EXPAND(temp));
	client.Address												= temp;
	int																wsae										= WSAGetLastError() ;
	ree_if(received_bytes == -1 && wsae != WSAEMSGSIZE, "Failed to receive connect response from address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	ree_if(commandReceived.Type != ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE, "Invalid server command received! Command received: %s.", ::gpk::get_value_label(commandReceived.Type).begin());
	commandToSend.Packed										= 1;
	gpk_necall(::sendto(client.Socket.Handle, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sizeof(struct sockaddr_in)), "Failed to send to address %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(client.AddressConnect));
	client.LastPing = client.FirstPing							= ::gpk::timeCurrentInUs();
	client.State												= ::gpk::UDP_CONNECTION_STATE_IDLE;
	return 0;
}

static	::gpk::error_t	clientQueueReceive								(::gpk::SUDPClient & client)		{
	::gpk::au8														receiveBuffer;
	while(client.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED && client.Socket != INVALID_SOCKET) {
		sockaddr_in														sa_server									= {};				/* Information about the server */
		int																sa_length									= sizeof(struct sockaddr_in);
		::gpk::SUDPCommand												commandReceived								= {};	/* Data to send */
		int																received_bytes								= recvfrom(client.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr *)&sa_server, &sa_length);
		int																wsae										= WSAGetLastError();
		if(received_bytes == -1 && wsae != WSAEMSGSIZE) {
			if(wsae == WSAEINTR)
				continue;
			if(wsae == WSAENOTSOCK || wsae == WSANOTINITIALISED)
				return 0;
			if(wsae == WSAECONNRESET) {
				client.State = ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
				return 0;
			}
			warning_printf("Failed to receive message! Error: 0x%x.", wsae);
			received_bytes												= recvfrom(client.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);
			continue;
		}	/* Receive time */
		::gpk::SIPv4End													temp										= {};
		::gpk::tcpipAddressFromSockaddr(sa_server, temp);
		if(temp.IP == client.Address.IP && commandReceived.Command == ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			if(temp.Port == client.Address.Port)
				es_if(errored(::gpk::connectionHandleCommand(client, commandReceived, receiveBuffer)));
		}
		else {
			info_printf("Data received fro invalid ip. Local: %u, %u, %u, %u, %u. Remote: %u, %u, %u, %u, %u", GPK_IPV4_EXPAND(temp), GPK_IPV4_EXPAND(client.Address));
		}
		received_bytes												= recvfrom(client.Socket, (sc_t *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);
	}
	return 0;
}

static	void												threadClientConnect							(void* pClient)						{ clientConnectAttempt(*(::gpk::SUDPClient*)pClient); }
static	void												threadUpdateClient							(void* pClient)						{
	::gpk::SUDPClient												& client									= *(::gpk::SUDPClient*)pClient;
	::clientQueueReceive(client);
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
}

::gpk::error_t			gpk::clientUpdate							(::gpk::SUDPClient & client)		{
	ree_if(client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Not connected. Current state: %s.", ::gpk::get_value_label(client.State).begin());
	client.CacheSent.clear(); client.CacheSend.clear();
	return ::gpk::connectionSendQueue(client, client.CacheSent, client.CacheSend);
}

::gpk::error_t			gpk::clientDisconnect						(::gpk::SUDPClient & client)		{
	int																sa_length									= sizeof(struct sockaddr_in);
	sockaddr_in														sa_server									= {AF_INET,};
	::gpk::tcpipAddressToSockaddr(client.Address, sa_server);
	::gpk::SUDPCommand												commandToSend								= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST,};	// Data to send */
	{
		::std::lock_guard												lock										(client.Queue.MutexSend);
		for(uint32_t i = 0; i < 2; ++i)
			sendto(client.Socket, (const sc_t*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length);	// Tranmsit data to get time
		client.Socket.close();
	}
	client.KeyPing												= 0;
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	::gpk::sleep(10);
	return 0;
}

::gpk::error_t			gpk::clientConnect							(::gpk::SUDPClient & client)		{
	uint32_t						attempts									= 0;
	stacxpr	uint32_t												MAX_ATTEMPTS								= 1;
	client.State												= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
	{
		::std::lock_guard												lock										(client.Queue.MutexSend);
		client.Queue.Send.clear();
		client.Queue.Sent.clear();
	}
	{
		::std::lock_guard												lock										(client.Queue.MutexReceive);
		client.Queue.Received.clear();
	}
	do {
		client.Socket.close();
		client.State												= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
		_beginthread(::threadClientConnect, 0, &client);
		for(uint32_t i = 0; i < 1000; ++i) {
			::gpk::sleep(1);
			if(client.State == ::gpk::UDP_CONNECTION_STATE_IDLE)
				break;
		}
	} while(client.State != ::gpk::UDP_CONNECTION_STATE_IDLE && ++attempts < MAX_ATTEMPTS);
	if(client.State == ::gpk::UDP_CONNECTION_STATE_IDLE) {
		info_printf("Client connected in %u attempt%s", attempts + 1, (attempts) ? "s" : "");
		_beginthread(::threadUpdateClient, 0, &client);
	}
	else {
		client.Socket.close();
		client.State											= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
		error_printf("Failed to connect to %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(client.AddressConnect));
		return -1;
	}
	return 0;
}
