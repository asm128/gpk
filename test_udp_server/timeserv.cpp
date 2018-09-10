// http://www.gomorgan89.com
#include "application.h"
#include "gpk_endpoint_command.h"
#include "gpk_array.h"
#include "gpk_view_stream.h"
#include "gpk_chrono.h"

#include <cstdlib>
#if defined(GPK_WINDOWS)
#	include <process.h>
#else
#	error	"Not implemented."
#endif

::gpk::error_t												sendQueue							(SUDPServer& serverInstance, uint32_t iClient)		{
	::SUDPClient													& client							= serverInstance.Clients[iClient];
	::gpk::array_obj<::SUDPClientMessage>							& queueToSend						= client.Queue.Send;
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);
	for(uint32_t iMessage = 0, countMessages = queueToSend.size(); iMessage < countMessages;  ++iMessage) {
		::SUDPClientMessage												& messageToSend = queueToSend[iMessage];
		if(messageToSend.Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			ree_if(sendto(client.Socket, (const char*)&messageToSend.Command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_remote, sa_length) != (int)sizeof(::gpk::SEndpointCommand), "Error sending datagram.");	// Send data back
		}
		else {
			gpk_necall(messageBytes.resize(sizeof(::gpk::SEndpointCommand) + sizeof(uint32_t) + messageToSend.Payload.size()), "??");
			::gpk::view_stream<byte_t>										sendStream							= {messageBytes.begin(), messageBytes.size()};
			gpk_necall(sendStream.write_pod(messageToSend.Command), "??");
			gpk_necall(sendStream.write_pod(messageToSend.Payload.size()), "??");
			gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "??");
		}
	}
	return 0;
}

::gpk::error_t												updateClients						(SUDPServer& serverInstance)		{
	::gpk::array_obj<SUDPClient>									clientsToProcess;
	while(serverInstance.Socket != INVALID_SOCKET) {		
		clientsToProcess.clear();
		const uint32_t													clientCount							= serverInstance.Clients.size();
		fd_set															sockets								= {};
		timeval															wait_time							= {0, 100000};
		for(uint32_t iClient = 0; iClient < clientCount; ++iClient) {
			error_if(errored(sendQueue(serverInstance, iClient)), "??");
			sockets.fd_array[sockets.fd_count++]						= serverInstance.Clients[iClient].Socket;
		}
		select(0, &sockets, 0, 0, &wait_time);
		for(uint32_t sd = 0; sd < sockets.fd_count; ++sd) 
			for(uint32_t iClient = 0; iClient < clientCount; ++iClient) {
				if(sockets.fd_array[sd] == serverInstance.Clients[iClient].Socket) {
					gpk_necall(clientsToProcess.push_back(serverInstance.Clients[iClient]), "Out of memory?");
				}
			}
		for(uint32_t iClient = 0; iClient < clientsToProcess.size(); ++iClient) {
			sockaddr_in														sa_client							= {};						// Information about the client 
			int																client_length						= (int)sizeof(sockaddr_in);	// Length of client struct 
			::gpk::SEndpointCommand											command								= {};						// Where to store received data 
			int																bytes_received						= 0;			
			if errored(bytes_received = recvfrom(clientsToProcess[iClient].Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length)) {
				error_printf("Could not receive datagram.");	
				recvfrom(clientsToProcess[iClient].Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &client_length);	
				continue;
			}
			switch(command.Type) {
			case ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST:
				switch(command.Command) {
				case ::gpk::ENDPOINT_COMMAND_CONNECT:
					if(command.Payload == 1)
						info_printf("connected!");
					break;
				}
			}
			if(INVALID_SOCKET != clientsToProcess[iClient].Socket) {
				recvfrom(clientsToProcess[iClient].Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &client_length);	
			}
		}
	}
	return 0;
}

void														threadUpdateClients					(void* serverInstance)				{ updateClients(*(::SUDPServer*)serverInstance); }

::gpk::error_t												server								(SUDPServer& serverInstance)		{
	ree_if(INVALID_SOCKET == (serverInstance.Socket = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
	gpk_necall(::gpk::tcpipAddress(serverInstance.Address.Port, 0, gpk::TRANSPORT_PROTOCOL_UDP, serverInstance.Address), "??");
	sockaddr_in														server								= {};			
	gpk_necall(::gpk::tcpipAddressToSockaddr(serverInstance.Address, server), "??");
	gpk_necall(bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "Could not bind name to socket.");	/* Bind address to socket */
	
#if defined(GPK_WINDOWS)
	_beginthread(threadUpdateClients, 0, &serverInstance);
#else
#	error "Not implemented."
#endif

	info_printf("Server running on %u.%u.%u.%u:%u"
		, (uint32_t)serverInstance.Address.IP[0]
		, (uint32_t)serverInstance.Address.IP[1]
		, (uint32_t)serverInstance.Address.IP[2]
		, (uint32_t)serverInstance.Address.IP[3]
		, (uint32_t)serverInstance.Address.Port
		);
	while(INVALID_SOCKET != serverInstance.Socket) {	// Loop and get data from clients 
		sockaddr_in														sa_client							= {};							// Information about the client 
		int																client_length						= (int)sizeof(sockaddr_in);		// Length of client struct 
		::gpk::SEndpointCommand											command								= {};							// Where to store received data 
		int																bytes_received						= 0;			
		if errored(bytes_received = recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length)) {
			error_printf("Could not receive datagram.");	
			recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &client_length);	
			continue;
		}
			 error_if(	command.Type	!= ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST	, "Invalid message type!")
		else error_if(	command.Command	!= ::gpk::ENDPOINT_COMMAND_CONNECT		
					&&	command.Command	!= ::gpk::ENDPOINT_COMMAND_DISCONNECT	, "Invalid message type!")
		else {
			SUDPClient														client								= {};
			::gpk::tcpipAddressFromSockaddr(sa_client, client.Address);
			ce_if(INVALID_SOCKET == (client.Socket = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
			//ree_if(sendto(client.Socket, (char *)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr *)&sa_client, client_length) != (int)sizeof(::gpk::SEndpointCommand), "Error sending datagram.");			// Send data back
			command.Type												= ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE;
			gpk_necall(client.Queue.Send		.push_back({command, {}, (uint64_t)::gpk::timeCurrentInMs()}), "Out of memory?");
			gpk_necall(serverInstance.Clients	.push_back(client), "Out of memory?");
		}
		continue_warn_if(errored(bytes_received = recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &client_length)), "Could not receive datagram.");	// Receive bytes from client
	}
	safe_closesocket(serverInstance.Socket);
	return 0;
}
