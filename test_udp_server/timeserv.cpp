// http://www.gomorgan89.com
#include "application.h"
#include "gpk_endpoint_command.h"
#include "gpk_array.h"
#include "gpk_view_stream.h"
#include "gpk_chrono.h"
#include "gpk_sync.h"

#include <cstdlib>
#if defined(GPK_WINDOWS)
#	include <process.h>
#else
#	error	"Not implemented."
#endif

::gpk::error_t												sendQueue							(::gpk::SUDPConnection & client)				{
	::gpk::array_obj<::gpk::SUDPConnectionMessage>					& queueToSend						= client.Queue.Send;
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);
	for(uint32_t iMessage = 0, countMessages = queueToSend.size(); iMessage < countMessages;  ++iMessage) {
		::gpk::SUDPConnectionMessage									& messageToSend						= queueToSend[iMessage];
		if(messageToSend.Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			ree_if((int)sizeof(::gpk::SEndpointCommand) != sendto(client.Socket, (const char*)&messageToSend.Command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_remote, sa_length), "Error sending datagram.");	// Send data back
		}
		else {
			gpk_necall(messageBytes.resize((uint32_t)sizeof(::gpk::SEndpointCommand) + (uint32_t)sizeof(uint32_t) + messageToSend.Payload.size()), "??");
			::gpk::view_stream<byte_t>										sendStream							= {messageBytes.begin(), messageBytes.size()};
			gpk_necall(sendStream.write_pod(messageToSend.Command), "??");
			gpk_necall(sendStream.write_pod(messageToSend.Payload.size()), "??");
			gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "??");
			ree_if((int)sendStream.CursorPosition != sendto(client.Socket, sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sa_length), "Error sending datagram.");	// Send data back
		}
		client.Queue.Send.remove(0);
		gpk_necall(client.Queue.Sent.push_back(messageToSend), "Out of memory?");
	}
	queueToSend.clear();
	return 0;
}

::gpk::error_t												updateClients						(gpk::SUDPServer& serverInstance)		{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnection>>			clientsToProcess;
	while(serverInstance.Listen) {		
		uint32_t														totalClientCount					= serverInstance.Clients.size();
		{
			::gpk::mutex_guard												lock								(serverInstance.Mutex);
			totalClientCount											= serverInstance.Clients.size();
		}
		timeval															wait_time							= {0, 1000};
		uint32_t														remainder							= totalClientCount % 64;
		uint32_t														stageCount							= totalClientCount / 64 + one_if(remainder);
		fd_set															sockets								= {};
		for(uint32_t iStage = 0; iStage < stageCount; ++iStage) {
			uint32_t														offsetClient						= iStage * 64;
			memset(&sockets, -1, sizeof(fd_set));
			sockets.fd_count											= 0;
			uint32_t														stageClientCount					= (iStage == (stageCount - 1) && remainder) ? remainder : 64;
			{
				::gpk::mutex_guard												lock								(serverInstance.Mutex);
				for(uint32_t iClient = 0; iClient < stageClientCount; ++iClient) {
					::gpk::ptr_obj<::gpk::SUDPConnection>									pclient								= serverInstance.Clients[offsetClient + iClient];
					if(0 == pclient || pclient->Socket == INVALID_SOCKET)
						continue;
					if(pclient->Queue.Send.size()) 
						error_if(errored(::sendQueue(*pclient)), "??");
					sockets.fd_array[sockets.fd_count]							= pclient->Socket;
					if(sockets.fd_array[sockets.fd_count] != INVALID_SOCKET)
						++sockets.fd_count;
				}
			}
			if(false == serverInstance.Listen)
				return 0;
			::select(0, &sockets, 0, 0, &wait_time);
			{
				clientsToProcess.clear();
				for(uint32_t sd = 0; sd < sockets.fd_count; ++sd) {
					for(uint32_t iClient = 0; iClient < stageClientCount; ++iClient) {
				::gpk::mutex_guard												lock								(serverInstance.Mutex);
						::gpk::ptr_obj<::gpk::SUDPConnection>									pclient								= serverInstance.Clients[offsetClient + iClient];
						if(0 == pclient || pclient->Socket == INVALID_SOCKET)
							continue;
						if(sockets.fd_array[sd] == pclient->Socket) {
							gpk_necall(clientsToProcess.push_back(pclient), "Out of memory?");
							break;
						}
					}
				}
				for(uint32_t iClient = 0; iClient < clientsToProcess.size(); ++iClient) {
					if(false == serverInstance.Listen)
						return 0;
					sockaddr_in														sa_client							= {};						// Information about the client 
					int																sa_length							= (int)sizeof(sockaddr_in);	// Length of client struct 
					::gpk::SEndpointCommand											command								= {};						// Where to store received data 
					int																bytes_received						= 0;			
					::gpk::SUDPConnection											& client							= *clientsToProcess[iClient];
					if errored(bytes_received = ::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
						warning_printf("Could not receive datagram.");	
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &sa_length);	
						continue;
					}
					::gpk::SIPv4													address								= {{}, 9999};
					::gpk::tcpipAddressFromSockaddr(sa_client, address);
					if(client.Address != address) {
						warning_printf("Could not receive datagram.");	
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &sa_length);	
						continue;
					}
					switch(command.Type) {
					case ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST:
						switch(command.Command) {
						case ::gpk::ENDPOINT_COMMAND_CONNECT:
							if(command.Payload == 1) {
								info_printf("connected!");
								client.LastPing												= gpk::timeCurrentInMs();
							}
							break;
						case ::gpk::ENDPOINT_COMMAND_PAYLOAD:
							client.LastPing												= gpk::timeCurrentInMs();
							break;
						}
					}
					if(INVALID_SOCKET != client.Socket) 
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_client, &sa_length);	
				}
			}
		}
	}
	return 0;
}

void														threadUpdateClients					(void* serverInstance)				{ updateClients(*(::gpk::SUDPServer*)serverInstance); }

::gpk::error_t												server								(::gpk::SUDPServer& serverInstance)		{
	serverInstance.Listen										= true;
	gpk_necall(::gpk::tcpipAddress(serverInstance.Address.Port, 0, gpk::TRANSPORT_PROTOCOL_UDP, serverInstance.Address), "??");
	sockaddr_in														server								= {};			
	gpk_necall(::gpk::tcpipAddressToSockaddr(serverInstance.Address, server), "??");
#if defined(GPK_WINDOWS)
	_beginthread(::threadUpdateClients, 0, &serverInstance);
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
	while(serverInstance.Listen) {	// Loop and get data from clients 
		serverInstance.Socket.close();
		ree_if(INVALID_SOCKET == (serverInstance.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
		gpk_necall(::bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "Could not bind name to socket.");	/* Bind address to socket */
		sockaddr_in														sa_client							= {};							// Information about the client 
		int																client_length						= (int)sizeof(sockaddr_in);		// Length of client struct 
		::gpk::SEndpointCommand											command								= {};							// Where to store received data 
		int																bytes_received						= 0;			
		if errored(bytes_received = recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SEndpointCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length)) {
			error_printf("Could not receive datagram.");	
			continue;
		}
			 error_if(	command.Type	!= ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST	, "Invalid message type!")
		else error_if(	command.Command	!= ::gpk::ENDPOINT_COMMAND_CONNECT		
					&&	command.Command	!= ::gpk::ENDPOINT_COMMAND_DISCONNECT	, "Invalid message type!")
		else if(command.Command	== ::gpk::ENDPOINT_COMMAND_DISCONNECT && false == serverInstance.Listen)
			break;
		else {
			::gpk::ptr_obj<::gpk::SUDPConnection>						pclient								= {};
			int32_t														found								= -1;
			{
				::gpk::mutex_guard											lock								(serverInstance.Mutex);
				for(uint32_t iClient = 0, countClients = serverInstance.Clients.size(); iClient < countClients; ++iClient) {
					pclient														= serverInstance.Clients[iClient];
					int64_t															limitTime						= (::gpk::timeCurrentInMs() - pclient->LastPing);
					info_printf("LimitTime : %lli.", limitTime);
					if(limitTime > serverInstance.Timeout || pclient->Socket == INVALID_SOCKET) {
						found														= (int32_t)iClient;
						pclient->Socket.close();
						pclient->Queue.Received	.clear();
						pclient->Queue.Send		.clear();
						pclient->Queue.Sent		.clear();
						break;
					}
					else
						pclient														= {};
				}
				::gpk::tcpipAddressFromSockaddr(sa_client, pclient->Address);
				command.Type											= ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE;
				::gpk::SUDPConnectionMessage								connectResponse						= {command, {}, (uint64_t)::gpk::timeCurrentInMs()};
				gpk_necall(pclient->Queue.Send.push_back(connectResponse), "Out of memory?");
				pclient->LastPing										= ::gpk::timeCurrentInMs();
				ce_if(INVALID_SOCKET == (pclient->Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
				if(found == -1) 
					gpk_necall(serverInstance.Clients.push_back(pclient), "Out of memory?");
			}
			info_printf("Current client count: %u", serverInstance.Clients.size());
		}
	}
	serverInstance.Socket.close();
	return 0;
}

void														threadServer								(void* pServerInstance)				{ 
	::gpk::SUDPServer												& serverInstance							= *(::gpk::SUDPServer*)pServerInstance;
	error_if(errored(server(serverInstance)), "Server exiting with error.")
	else  
		info_printf("Server gracefully closed.");
	serverInstance.Listen										= false;
	serverInstance.Socket.close();
} 

::gpk::error_t												serverStart									(::gpk::SUDPServer& serverInstance, uint16_t port)		{
	serverInstance.Listen										= true;
	serverInstance.Address.Port									= port;
	_beginthread(threadServer, 0, &serverInstance);
	return 0;
}

::gpk::error_t												serverStop									(::gpk::SUDPServer& serverInstance)		{
	serverInstance.Listen										= false;
	sockaddr_in														sa_srv										= {};							// Information about the client 
	int																sa_length									= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::SEndpointCommand											command										= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST};							// Where to store received data 
	::gpk::tcpipAddressToSockaddr(serverInstance.Address, sa_srv);
	uint32_t														attempt										= 0;
	do{ 
		sendto(serverInstance.Socket, (const char*)&command, (int)sizeof(::gpk::SEndpointCommand), 0, (sockaddr*)&sa_srv, sa_length);
		::gpk::sleep(100);
	} while(10 > ++attempt && serverInstance.Socket != INVALID_SOCKET);
	info_if(attempt < 10, "Listening socket closed with %u disconnect attempt%s.", attempt, (attempt > 1) ? "s":"");
	return 0;
}