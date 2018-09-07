/* http://www.gomorgan89.com */
#include "gpk_stdsocket.h"
#include "gpk_log.h"
#include "gpk_sync.h"

#include <cstdlib>
#include <ctime>
#include "gpk_windows.h"

#define SIZE 500

int									runClient					(::gpk::SIPv4 & addressServer, SOCKET sd)	{
	char									send_buffer[SIZE]			= "GET TIME\r\n";	/* Data to send */
	sockaddr_in								server						= {};				/* Information about the server */
	sockaddr_in								client						= {};				/* Information about the client */
	::gpk::SIPv4							addressClient				= {};
	gpk_necall(::gpk::tcpipAddress(0, 0, gpk::TRANSPORT_PROTOCOL_UDP, addressClient), "??");
	gpk_necall(::gpk::tcpipAddressToSockaddr(addressClient, client), "??");
	gpk_necall(::gpk::tcpipAddressToSockaddr(addressServer, server), "??");

	gpk_necall(bind(sd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)), "Failed!");	/* Bind local address to socket */
	int										server_length						= sizeof(struct sockaddr_in);
	gpk_necall(sendto(sd, send_buffer, (int)strlen(send_buffer) + 1, 0, (struct sockaddr *)&server, server_length), "Failed!");	/* Tranmsit data to get time */
	server_length						= sizeof(struct sockaddr_in);
	server								= {};
	time_t									current_time				= 0;				/* Time received */
	int										received_bytes				= recvfrom(sd, (char *)&current_time, (int)sizeof(current_time), 0, (struct sockaddr *)&server, &server_length);
	int										wsae						= WSAGetLastError() ;
	ree_if(received_bytes == -1 && wsae != WSAEMSGSIZE, "Failed!");	/* Receive time */
	
	ctime_s(send_buffer, SIZE, &current_time);
	info_printf("Current time: %s", send_buffer);	/* Display time */
	return 0;
}

int					client		()		{
	::gpk::SIPv4						addressServer		= {{}, 9999};
	::gpk::tcpipAddress(9999, 0, gpk::TRANSPORT_PROTOCOL_UDP, addressServer);

	SOCKET								sd					= socket(AF_INET, SOCK_DGRAM, 0);
	ree_if(sd == INVALID_SOCKET, "Could not create socket.\n");
	error_if(errored(runClient(addressServer, sd)), "Client exit with error.");
	safe_closesocket(sd);
	::gpk::sleep(1000);
	return 0;
}
