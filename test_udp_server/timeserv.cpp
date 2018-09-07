/* http://www.gomorgan89.com */
#include "gpk_stdsocket.h"
#include "gpk_log.h"

#include <cstdlib>
#include <ctime>

#define BUFFER_SIZE 2048

int							server								()		{
	int								bytes_received						= 0;					/* Bytes received from client */
	sockaddr_in						server								= {};			/* Information about the server */

	SOCKET							sd									= socket(AF_INET, SOCK_DGRAM, 0);							/* Socket descriptor of server */
	ree_if(sd == INVALID_SOCKET, "Could not create socket.\n");
	::gpk::SIPv4					addressServer						= {{}, 9999};
	gpk_necall(::gpk::tcpipAddress(9999, 0, gpk::TRANSPORT_PROTOCOL_UDP, addressServer), "??");
	gpk_necall(::gpk::tcpipAddressToSockaddr(addressServer, server), "??");
	gpk_necall(bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)), "Could not bind name to socket.");	/* Bind address to socket */
	
	info_printf("Server running on %u.%u.%u.%u:%u"
		, (uint32_t)addressServer.IP[0]
		, (uint32_t)addressServer.IP[1]
		, (uint32_t)addressServer.IP[2]
		, (uint32_t)addressServer.IP[3]
		, (uint32_t)addressServer.Port
		);
	while (1) {	/* Loop and get data from clients */
		sockaddr_in					client					= {};			/* Information about the client */
		int							client_length			= (int)sizeof(sockaddr_in);					/* Length of client struct */
		char						buffer[BUFFER_SIZE]		= {};			/* Where to store received data */
		gpk_necall(bytes_received = recvfrom(sd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_length), "Could not receive datagram.");	/* Receive bytes from client */
		if (strcmp(buffer, "GET TIME\r\n") == 0) {	/* Check for time request */
			time_t							current_time						= time(NULL);	/* Get current time */
			ree_if(sendto(sd, (char *)&current_time, (int)sizeof(current_time), 0, (struct sockaddr *)&client, client_length) != (int)sizeof(current_time), "Error sending datagram.");	/* Send data back */
		}
	}
	safe_closesocket(sd);
	return 0;
}
