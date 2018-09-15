// http://www.gomorgan89.com
#include "gpk_stdsocket.h"
#include "gpk_sync.h"
#include "gpk_log.h"
#include "gpk_endpoint_command.h"
#include "application.h"

#if defined(GPK_WINDOWS)
#	include <process.h>
#endif

::gpk::error_t								clientConnectAttempt						(::SUDPClient & client)	{
	::gpk::SEndpointCommand							commandToSend								= {::gpk::ENDPOINT_COMMAND_CONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST,};	/* Data to send */
	sockaddr_in										sa_server									= {};				/* Information about the server */
	int												sa_length									= sizeof(struct sockaddr_in);
	client.Socket.close();
	gpk_necall(::gpk::tcpipAddressToSockaddr(client.AddressServer, sa_server), "??");
	sa_server.sin_port								= htons(client.PortServer);
	ree_if(INVALID_SOCKET == (client.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Failed to create socket!!");
	gpk_necall(sendto(client.Socket.Handle, (const char*)&commandToSend, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed!");	/* Tranmsit data to get time */
	sa_length										= sizeof(struct sockaddr_in);
	sa_server										= {};

	::gpk::SEndpointCommand							commandReceived								= {};	/* Data to send */
	int												received_bytes								= recvfrom(client.Socket.Handle, (char *)&commandReceived, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_server, &sa_length);
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
	gpk_necall(sendto(client.Socket.Handle, (const char*)&commandToSend, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_server, sa_length), "Failed!");	/* Tranmsit data to get time */
	client.State								= UDP_CONNECTION_STATE_IDLE;
	return 0;
}

::gpk::error_t								updateClient								(::SUDPClient & client)		{
	while(client.State != UDP_CONNECTION_STATE_DISCONNECTED && client.Socket != INVALID_SOCKET) {
		sockaddr_in										sa_server									= {};				/* Information about the server */
		int												sa_length									= sizeof(struct sockaddr_in);
		::gpk::SEndpointCommand							commandReceived								= {};	/* Data to send */
		int												received_bytes								= recvfrom(client.Socket.Handle, (char *)&commandReceived, (int)sizeof(::gpk::SEndpointCommand), MSG_PEEK, (sockaddr *)&sa_server, &sa_length);
		int												wsae										= WSAGetLastError();
		if(received_bytes == -1 && wsae != WSAEMSGSIZE) {
			if(wsae == WSAEINTR)
				continue;
			if(wsae == WSAENOTSOCK || wsae == WSANOTINITIALISED)
				return 0;
			warning_printf("Failed to receive message!");
			received_bytes								= recvfrom(client.Socket.Handle, (char *)&commandReceived, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_server, &sa_length);
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
		received_bytes								= recvfrom(client.Socket.Handle, (char *)&commandReceived, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_server, &sa_length);
	}
	return 0;
}

void										threadClientConnect							(void* pClient)				{ clientConnectAttempt(*(SUDPClient*)pClient); }
void										threadUpdateClient							(void* pClient)				{ 
	::SUDPClient									& client									= *(SUDPClient*)pClient;
	updateClient(client); 
	client.State								= UDP_CONNECTION_STATE_DISCONNECTED;
}

::gpk::error_t								clientConnect								(::SUDPClient & client)		{
	uint32_t										attempts									= 0;
	client.State								= UDP_CONNECTION_STATE_HANDSHAKE;
	do {
		client.Socket.close();
		client.State								= UDP_CONNECTION_STATE_HANDSHAKE;
		_beginthread(threadClientConnect, 0, &client);
		//clientConnectAttempt(client);
		::gpk::sleep(1000);
	} while(client.State != UDP_CONNECTION_STATE_IDLE && ++attempts < 10 );
	if(client.State == UDP_CONNECTION_STATE_IDLE) {
		info_printf("Client connected in %u attempt%s", attempts + 1, (attempts) ? "s" : "");
		_beginthread(threadUpdateClient, 0, &client);
	}
	else {
		client.State								= UDP_CONNECTION_STATE_DISCONNECTED;
		error_printf("Failed to connect to %u.%u.%u.%u:%u."
			, (uint32_t)client.AddressServer.IP[0]
			, (uint32_t)client.AddressServer.IP[1]
			, (uint32_t)client.AddressServer.IP[2]
			, (uint32_t)client.AddressServer.IP[3]
			, (uint32_t)client.PortServer
			);
		return -1;
	}
	return 0;
}
