// http://www.gomorgan89.com
#include "gpk_udp_client.h"
#include "gpk_sync.h"
#include "gpk_log.h"

#if defined(GPK_WINDOWS)
#	include <process.h>
#endif

static	::gpk::error_t						clientConnectAttempt						(::gpk::SUDPClient & client)		{
	::gpk::SUDPCommand								commandToSend								= {::gpk::ENDPOINT_COMMAND_CONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST,};	/* Data to send */
	sockaddr_in										sa_server									= {};				/* Information about the server */
	int												sa_length									= sizeof(struct sockaddr_in);
	client.Socket.close();
	gpk_necall(::gpk::tcpipAddressToSockaddr(client.AddressServer, sa_server), "??");
	sa_server.sin_port								= htons(client.PortServer);
	ree_if(INVALID_SOCKET == (client.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Failed to create socket!!");
	gpk_necall(sendto(client.Socket.Handle, (const char*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed!");	/* Tranmsit data to get time */
	sa_length										= sizeof(struct sockaddr_in);
	sa_server										= {};

	::gpk::SUDPCommand								commandReceived								= {};	/* Data to send */
	int												received_bytes								= recvfrom(client.Socket.Handle, (char *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, &sa_length);
	rew_if(-1 == received_bytes, "Failed to receive connect response!");
	::gpk::SIPv4									temp										= {};
	::gpk::tcpipAddressFromSockaddr(sa_server, temp);
	ree_if(*(uint32_t*)temp.IP != *(uint32_t*)client.AddressServer.IP, "Invalid server response address!");
	client.AddressServer						= temp;
	int												wsae										= WSAGetLastError() ;
	ree_if(received_bytes == -1 && wsae != WSAEMSGSIZE, "Failed!");	/* Receive time */
	ree_if(commandReceived.Type != ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE, "Invalid server command received!");
	commandToSend.Payload						= 1;
	sa_length									= sizeof(struct sockaddr_in);
	gpk_necall(sendto(client.Socket.Handle, (const char*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed!");	/* Tranmsit data to get time */
	client.State								= ::gpk::UDP_CONNECTION_STATE_IDLE;
	return 0;
}

static	::gpk::error_t						updateClient								(::gpk::SUDPClient & client)		{
	while(client.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED && client.Socket != INVALID_SOCKET) {
		sockaddr_in										sa_server									= {};				/* Information about the server */
		int												sa_length									= sizeof(struct sockaddr_in);
		::gpk::SUDPCommand								commandReceived								= {};	/* Data to send */
		int												received_bytes								= recvfrom(client.Socket, (char *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr *)&sa_server, &sa_length);
		int												wsae										= WSAGetLastError();
		if(received_bytes == -1 && wsae != WSAEMSGSIZE) {
			if(wsae == WSAEINTR)
				continue;
			if(wsae == WSAENOTSOCK || wsae == WSANOTINITIALISED)
				return 0;
			warning_printf("Failed to receive message!");
			received_bytes								= recvfrom(client.Socket, (char *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, &sa_length);
			continue;
		}	/* Receive time */
		::gpk::SIPv4									temp										= {};
		::gpk::tcpipAddressFromSockaddr(sa_server, temp);
		if(*(uint32_t*)temp.IP == *(uint32_t*)client.AddressServer.IP && commandReceived.Command == ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			if(temp.Port == client.AddressServer.Port) {
					 if(commandReceived.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE) {}
				else if(commandReceived.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST) {}
			}
		}
		received_bytes								= recvfrom(client.Socket, (char *)&commandReceived, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, &sa_length);
	}
	return 0;
}

void										threadClientConnect							(void* pClient)						{ clientConnectAttempt(*(::gpk::SUDPClient*)pClient); }
void										threadUpdateClient							(void* pClient)						{ 
	::gpk::SUDPClient								& client									= *(::gpk::SUDPClient*)pClient;
	::updateClient(client); 
	client.State								= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
}

::gpk::error_t								clientDisconnect							(::gpk::SUDPClient & client)		{
	int												sa_length									= sizeof(struct sockaddr_in);
	sockaddr_in										sa_server									= {AF_INET,};
	client.State								= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	::gpk::tcpipAddressToSockaddr(client.AddressServer, sa_server);
	::gpk::SUDPCommand								commandToSend								= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST,};	// Data to send */
	{
		::gpk::mutex_guard								lock										(client.Queue.MutexSend);
		sendto(client.Socket, (const char*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length);	// Tranmsit data to get time 
		client.Socket.close();
	}
	return 0;
}

::gpk::error_t								clientConnect								(::gpk::SUDPClient & client)		{
	uint32_t										attempts									= 0;
	client.State								= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
	do {
		client.Socket.close();
		client.State								= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
		_beginthread(threadClientConnect, 0, &client);
		//clientConnectAttempt(client);
		::gpk::sleep(200);
	} while(client.State != ::gpk::UDP_CONNECTION_STATE_IDLE && ++attempts < 10);
	if(client.State == ::gpk::UDP_CONNECTION_STATE_IDLE) {
		info_printf("Client connected in %u attempt%s", attempts + 1, (attempts) ? "s" : "");
		_beginthread(threadUpdateClient, 0, &client);
	}
	else {
		client.Socket.close();
		client.State								= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
		error_printf("Failed to connect to %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(client.AddressServer));
		return -1;
	}
	return 0;
}


::gpk::error_t								clientUpdate								(::gpk::SUDPClient & client)		{
	ree_if(client.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "Not connected.");
	::gpk::mutex_guard								lock										(client.Queue.MutexSend);
	int												sa_length									= sizeof(struct sockaddr_in);
	sockaddr_in										sa_server									= {AF_INET,};
	::gpk::tcpipAddressToSockaddr(client.AddressServer, sa_server);
	while(client.Queue.Send.size()) {
		client.Queue.Send[0];
		::gpk::SUDPCommand								commandToSend								= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST,};	// Data to send */
		ce_if(sizeof(::gpk::SUDPCommand) != sendto(client.Socket, (const char*)&commandToSend, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr *)&sa_server, sa_length), "");	// Tranmsit data to get time 
	}
	return 0;
}
