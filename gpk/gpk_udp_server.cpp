// http://www.gomorgan89.com
#include "gpk_noise.h"
#include "gpk_connection.h"
#include "gpk_array.h"
#include "gpk_view_stream.h"
#include "gpk_chrono.h"
#include "gpk_sync.h"
#include "gpk_udp_server.h"

#include <cstdlib>
#if defined(GPK_WINDOWS)
#	include <process.h>
#else
#	error	"Not implemented."
#endif

::gpk::error_t												updateClients						(gpk::SUDPServer& serverInstance)		{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnection>>			clientsToProcess;
	::gpk::array_pod<byte_t>										receiveBuffer;
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	cacheSent						= {};
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	cacheSend						= {};
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
					::gpk::ptr_obj<::gpk::SUDPConnection>							pclient								= serverInstance.Clients[offsetClient + iClient];
					if(0 == pclient || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
						continue;
					if(pclient->Queue.Send.size()) 
						error_if(errored(::gpk::connectionSendQueue(*pclient, cacheSent, cacheSend)), "??");
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
				{
					::gpk::mutex_guard												lock								(serverInstance.Mutex);
					break_info_if(0 == serverInstance.Clients.size(), "%s", "No clients to process. Server closed?");
					for(uint32_t sd = 0; sd < sockets.fd_count; ++sd) {
						for(uint32_t iClient = 0, countCli = ::gpk::min(serverInstance.Clients.size(), stageClientCount); iClient < countCli; ++iClient) {
							::gpk::ptr_obj<::gpk::SUDPConnection>							pclient								= serverInstance.Clients[offsetClient + iClient];
							if(0 == pclient || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
								continue;
							if(sockets.fd_array[sd] == pclient->Socket) {
								gpk_necall(clientsToProcess.push_back(pclient), "Out of memory?");
								break;
							}
						}
					}
				}
				for(uint32_t iClient = 0; iClient < clientsToProcess.size(); ++iClient) {
					if(false == serverInstance.Listen)
						return 0;
					sockaddr_in														sa_client							= {};						// Information about the client 
					int																sa_length							= (int)sizeof(sockaddr_in);	// Length of client struct 
					::gpk::SUDPCommand												command								= {};						// Where to store received data 
					int																bytes_received						= 0;			
					::gpk::SUDPConnection											& client							= *clientsToProcess[iClient];
					if errored(bytes_received = ::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
						if(::WSAGetLastError() != WSAEMSGSIZE) {
							warning_printf("Could not receive datagram.");	
							::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);	
							continue;
						}
					}
					::gpk::SIPv4													address								= {{}, 9999};
					::gpk::tcpipAddressFromSockaddr(sa_client, address);
					if(client.Address != address) {
						warning_printf("Command received from an invalid address: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);	
						continue;
					}
					error_if(errored(::gpk::connectionHandleCommand(client, command, receiveBuffer)), "Error processing command from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
					if(INVALID_SOCKET != client.Socket) 
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);	
				}
			}
		}
	}
	return 0;
}

void														threadUpdateClients					(void* serverInstance)					{ updateClients(*(::gpk::SUDPServer*)serverInstance); }

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
	info_printf("Server running on %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(serverInstance.Address));
	while(serverInstance.Listen) {	// Loop and get data from clients 
		serverInstance.Socket.close();
		ree_if(INVALID_SOCKET == (serverInstance.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
		gpk_necall(::bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "Could not bind name to socket.");	/* Bind address to socket */
		sockaddr_in														sa_client							= {};							// Information about the client 
		int																client_length						= (int)sizeof(sockaddr_in);		// Length of client struct 
		::gpk::SUDPCommand												command								= {};							// Where to store received data 
		int																bytes_received						= 0;			
		if errored(bytes_received = recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length)) {
			error_printf("Could not receive datagram.");	
			continue;
		}
			 error_if(	command.Type	!= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST	, "Invalid message type!")
		else error_if(	command.Command	!= ::gpk::ENDPOINT_COMMAND_CONNECT		
					&&	command.Command	!= ::gpk::ENDPOINT_COMMAND_DISCONNECT	, "Invalid message type!")
		else if(command.Command	== ::gpk::ENDPOINT_COMMAND_DISCONNECT && false == serverInstance.Listen)
			break;
		else {
			::gpk::ptr_obj<::gpk::SUDPConnection>							pClient								= {};
			int32_t															found								= -1;
			{ // accept
				::gpk::mutex_guard												lock								(serverInstance.Mutex);
				for(uint32_t iClient = 0, countClients = serverInstance.Clients.size(); iClient < countClients; ++iClient) {
					pClient														= serverInstance.Clients[iClient];
					::gpk::SUDPConnection											& client							= *pClient;
					int64_t															limitTime							= (::gpk::timeCurrentInUs() - client.LastPing);
					info_printf("LimitTime : %lli.", limitTime);
					if(limitTime > serverInstance.Timeout || client.Socket == INVALID_SOCKET || client.State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {
						found														= (int32_t)iClient;
						client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
						client.Socket.close();
						client.KeyPing												= 
						client.LastPing												= 
						client.FirstPing											= 0;
						{
							::gpk::mutex_guard												lockRecv							(client.Queue.MutexReceive);
							client.Queue.Received	.clear();
						}
						{
							::gpk::mutex_guard												lockSend							(client.Queue.MutexSend);
							client.Queue.Send		.clear();
							client.Queue.Sent		.clear();
						}
						break;
					}
					else
						pClient														= {};
				}
				::gpk::tcpipAddressFromSockaddr(sa_client, pClient->Address);
				command.Type												= ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE;
				::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					connectResponse								= {};
				connectResponse.create(::gpk::SUDPConnectionMessage{{}, (uint64_t)::gpk::timeCurrentInUs(), command});
				gpk_necall(pClient->Queue.Send.push_back(connectResponse), "Out of memory?");
				pClient->LastPing											= 
				pClient->FirstPing											= ::gpk::timeCurrentInUs();
				ce_if(INVALID_SOCKET == (pClient->Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
				pClient->State												= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
				if(found == -1) 
					gpk_necall(serverInstance.Clients.push_back(pClient), "Out of memory?");
			}
			info_printf("Current client count: %u", serverInstance.Clients.size());
			::gpk::sleep(1);
		}
	}
	serverInstance.Socket.close();
	return 0;
}

void														threadServer						(void* pServerInstance)				{ 
	::gpk::SUDPServer												& serverInstance					= *(::gpk::SUDPServer*)pServerInstance;
	error_if(errored(server(serverInstance)), "Server exiting with error.")
	else  
		info_printf("Server gracefully closed.");
	serverInstance.Listen										= false;
	serverInstance.Socket.close();
	{
		::gpk::mutex_guard												lock								(serverInstance.Mutex);
		serverInstance.Clients.clear();
	}
} 

::gpk::error_t												gpk::serverStart					(::gpk::SUDPServer& serverInstance, uint16_t port)		{
	serverInstance.Listen										= true;
	serverInstance.Address.Port									= port;
	_beginthread(::threadServer, 0, &serverInstance);
	return 0;
}

::gpk::error_t												gpk::serverStop						(::gpk::SUDPServer& serverInstance)		{
	serverInstance.Listen										= false;
	sockaddr_in														sa_srv								= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::SUDPCommand												command								= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST, };							// Where to store received data 
	::gpk::tcpipAddressToSockaddr(serverInstance.Address, sa_srv);
	uint32_t														attempt								= 0;
	do{ 
		::sendto(serverInstance.Socket, (const char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_srv, sa_length);
		::gpk::sleep(100);
	} while(10 > ++attempt && serverInstance.Clients.size());
	info_if(attempt < 10, "Listening socket closed with %u disconnect attempt%s.", attempt, (attempt > 1) ? "s":"");
	return 0;
}
