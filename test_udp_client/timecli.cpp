// http://www.gomorgan89.com
#include "gpk_stdsocket.h"
#include "gpk_sync.h"
#include "gpk_log.h"
#include "gpk_endpoint_command.h"

int									runClient							(::gpk::SIPv4 & addressServer, SOCKET sd)	{
	::gpk::SEndpointCommand					commandToSend						= {::gpk::ENDPOINT_COMMAND_CONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST,};	/* Data to send */
	sockaddr_in								server								= {};				/* Information about the server */
	sockaddr_in								client								= {};				/* Information about the client */
	::gpk::SIPv4							addressClient						= {};
	gpk_necall(::gpk::tcpipAddress(0, 0, gpk::TRANSPORT_PROTOCOL_UDP, addressClient), "??");
	gpk_necall(::gpk::tcpipAddressToSockaddr(addressClient, client), "??");
	gpk_necall(::gpk::tcpipAddressToSockaddr(addressServer, server), "??");

	gpk_necall(bind(sd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)), "Failed!");	/* Bind local address to socket */
	int										server_length						= sizeof(struct sockaddr_in);
	gpk_necall(sendto(sd, (const char*)&commandToSend, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&server, server_length), "Failed!");	/* Tranmsit data to get time */
	server_length						= sizeof(struct sockaddr_in);
	server								= {};
	::gpk::SEndpointCommand					commandReceived						= {};	/* Data to send */
	int										received_bytes						= recvfrom(sd, (char *)&commandReceived, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&server, &server_length);
	::gpk::tcpipAddressFromSockaddr(server, addressClient);
	int										wsae								= WSAGetLastError() ;
	ree_if(received_bytes == -1 && wsae != WSAEMSGSIZE, "Failed!");	/* Receive time */
	ree_if(commandReceived.Type != ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE, "Invalid server command received!");
	commandToSend.Payload				= 1;
	server_length						= sizeof(struct sockaddr_in);
	gpk_necall(sendto(sd, (const char*)&commandToSend, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&server, server_length), "Failed!");	/* Tranmsit data to get time */
	return 0;
}

::gpk::error_t						client								()											{
	::gpk::SIPv4							addressServer						= {{}, 9999};
	::gpk::tcpipAddress(9999, 0, gpk::TRANSPORT_PROTOCOL_UDP, addressServer);

	SOCKET									sd									= socket(AF_INET, SOCK_DGRAM, 0);
	ree_if(sd == INVALID_SOCKET, "Could not create socket.\n");
	error_if(errored(runClient(addressServer, sd)), "Client exit with error.");
	safe_closesocket(sd);
	::gpk::sleep(1000);
	return 0;
}
