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
	::gpk::array_obj<::gpk::pobj<::gpk::SUDPConnection>>			clientsToProcess;
	::gpk::array_pod<byte_t>										receiveBuffer;
	::gpk::array_obj<::gpk::pobj<::gpk::SUDPConnectionMessage>>	cacheSent						= {};
	::gpk::array_obj<::gpk::pobj<::gpk::SUDPConnectionMessage>>	cacheSend						= {};
	while(serverInstance.Listen) {
		::gpk::sleep(1);
		uint32_t														totalClientCount					= serverInstance.Clients.size();
		{
			::gpk::mutex_guard												lock								(serverInstance.Mutex);
			totalClientCount											= serverInstance.Clients.size();
		}
		timeval															wait_time							= {0, 1};
		uint32_t														remainder							= totalClientCount % 64;
		uint32_t														stageCount							= totalClientCount / 64 + one_if(remainder);
		fd_set															sockets								= {};
		for(uint32_t iStage = 0; iStage < stageCount; ++iStage) {
			const uint32_t													offsetClient						= iStage * 64;
			memset(&sockets, -1, sizeof(fd_set));
			sockets.fd_count											= 0;
			uint32_t														stageClientCount					= (iStage == (stageCount - 1) && remainder) ? remainder : 64;
			{
				::gpk::mutex_guard												lock								(serverInstance.Mutex);
				for(uint32_t iClient = 0, actualCount = ::gpk::min(serverInstance.Clients.size(), stageClientCount); iClient < actualCount; ++iClient) {
					::gpk::pobj<::gpk::SUDPConnection>							pclient								= serverInstance.Clients[offsetClient + iClient];
					if(0 == pclient.get_ref() || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
						continue;
					if(pclient->Queue.Send.size())
						gerror_if(errored(::gpk::connectionSendQueue(*pclient, cacheSent, cacheSend)), "??");
					sockets.fd_array[sockets.fd_count]							= pclient->Socket;
					if(sockets.fd_array[sockets.fd_count] != INVALID_SOCKET)
						++sockets.fd_count;
				}
			}
			if(false == serverInstance.Listen)
				return 0;
			{
				clientsToProcess.clear();
				{
					::gpk::mutex_guard												lock								(serverInstance.Mutex);
					break_ginfo_if(0 == serverInstance.Clients.size(), "%s", "No clients to process. Server closed?");
					for(uint32_t sd = 0; sd < sockets.fd_count; ++sd) {
						for(uint32_t iClient = 0, countCli = ::gpk::min(serverInstance.Clients.size(), stageClientCount); iClient < countCli; ++iClient) {
							::gpk::pobj<::gpk::SUDPConnection>							pclient								= serverInstance.Clients[offsetClient + iClient];
							if(0 == pclient.get_ref() || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
								continue;
							if(sockets.fd_array[sd] == pclient->Socket) {
								gpk_necall(clientsToProcess.push_back(pclient), "%s", "Out of memory?");
								break;
							}
						}
					}
				}
				for(uint32_t iClient = 0; iClient < clientsToProcess.size(); ++iClient) {
					if(false == serverInstance.Listen)
						return 0;
					FD_ZERO(sockets.fd_array);
					::gpk::SUDPConnection											& client							= *clientsToProcess[iClient];
					sockets.fd_array[0]											= client.Socket;
					sockets.fd_count											= 1;
					::select(0, &sockets, 0, 0, &wait_time);
					while(sockets.fd_count > 0) {
						sockaddr_in														sa_client							= {};						// Information about the client
						int																sa_length							= (int)sizeof(sockaddr_in);	// Length of client struct
						::gpk::SUDPCommand												command								= {};						// Where to store received data
						int																bytes_received						= 0;
						if errored(bytes_received = ::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
							uint32_t lastError = ::WSAGetLastError();
							if(lastError != WSAEMSGSIZE) {
								warning_printf("%s", "Could not receive datagram.");
								::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);
								if(lastError == WSAENETRESET || lastError == WSAENOTSOCK)
									client.State = ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
								break;
							}
						}
						::gpk::SIPv4													address								= {{}, 9999};
						::gpk::tcpipAddressFromSockaddr(sa_client, address);
						if(client.Address != address) {
							warning_printf("Command received from an invalid address: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
							::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);
							continue;
						}
						e_if(errored(::gpk::connectionHandleCommand(client, command, receiveBuffer)), "Error processing command from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
						if(INVALID_SOCKET != client.Socket)
							::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);
						::select(0, &sockets, 0, 0, &wait_time);
					}
				}
			}
		}
	}
	return 0;
}

static	::gpk::error_t										recycleClient						(::gpk::SUDPServer& serverInstance, ::gpk::pobj<::gpk::SUDPConnection> & pClient)		{
	for(uint32_t iClient = 0, countClients = serverInstance.Clients.size(); iClient < countClients; ++iClient) {
		pClient														= serverInstance.Clients[iClient];
		::gpk::SUDPConnection											& client							= *pClient;
		int64_t															limitTime							= (::gpk::timeCurrentInUs() - client.LastPing);
		info_printf("LimitTime : %lli.", limitTime);
		if(limitTime > serverInstance.Timeout || client.Socket == INVALID_SOCKET || client.State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {
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
			return iClient;
		}
		else
			pClient														= {};
	}
	return -1;
}

static	::gpk::error_t										serverAcceptClient					(::gpk::SUDPServer& serverInstance, ::gpk::SUDPCommand command, const sockaddr_in & sa_client)		{
	::gpk::pobj<::gpk::SUDPConnection>							pClient								= {};
	{ // accept
		::gpk::mutex_guard												lock								(serverInstance.Mutex);
		int32_t															found								= ::recycleClient(serverInstance, pClient);
		::gpk::tcpipAddressFromSockaddr(sa_client, pClient->Address);
		command.Type												= ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE;
		::gpk::pobj<::gpk::SUDPConnectionMessage>					connectResponse								= {};
		connectResponse.create(::gpk::SUDPConnectionMessage{{}, (uint64_t)::gpk::timeCurrentInUs(), command});
		gpk_necall(pClient->Queue.Send.push_back(connectResponse), "Out of memory?");
		pClient->LastPing											=
		pClient->FirstPing											= ::gpk::timeCurrentInUs();
		ree_if(INVALID_SOCKET == (pClient->Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
		pClient->State												= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
		if(found == -1)
			gpk_necall(serverInstance.Clients.push_back(pClient), "Out of memory?");
	}
	info_printf("Current client count: %u", serverInstance.Clients.size());
	return 0;
}

static	::gpk::error_t										serverListenTick					(::gpk::SUDPServer& serverInstance, const sockaddr_in & server)		{
	serverInstance.Socket.close();
	ree_if(INVALID_SOCKET == (serverInstance.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
	gpk_necall(::bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "Could not bind name to socket.");	/* Bind address to socket */
	int																client_length						= (int)sizeof(sockaddr_in);		// Length of client struct
	::gpk::SUDPCommand												command								= {};							// Where to store received data
	int																bytes_received						= 0;
	sockaddr_in														sa_client							= {};							// Information about the client
	if errored(bytes_received = recvfrom(serverInstance.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length)) {
		warning_printf("Could not receive datagram.");
	}
		 gerror_if(	command.Type	!= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST	, "Invalid message type!")
	else gerror_if(	command.Command	!= ::gpk::ENDPOINT_COMMAND_CONNECT
				&&	command.Command	!= ::gpk::ENDPOINT_COMMAND_DISCONNECT	, "Invalid message type!")
	else if(command.Command	== ::gpk::ENDPOINT_COMMAND_DISCONNECT && false == serverInstance.Listen)
		return 0;
	else {
		::serverAcceptClient(serverInstance, command, sa_client);
		::gpk::sleep(1);
	}
	return 0;
}

static	void												threadUpdateClients					(void* serverInstance)					{ updateClients(*(::gpk::SUDPServer*)serverInstance); }
static	::gpk::error_t										server								(::gpk::SUDPServer& serverInstance)		{
	serverInstance.Listen										= true;
	gpk_necall(::gpk::tcpipAddress(serverInstance.Address.Port, serverInstance.AdapterIndex, gpk::TRANSPORT_PROTOCOL_UDP, serverInstance.Address), "??");
	sockaddr_in														server								= {};
	gpk_necall(::gpk::tcpipAddressToSockaddr(serverInstance.Address, server), "??");
#if defined(GPK_WINDOWS)
	_beginthread(::threadUpdateClients, 0, &serverInstance);
#else
#	error "Not implemented."
#endif
	info_printf("Server running on %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(serverInstance.Address));
	while(serverInstance.Listen) {	// Loop and get data from clients
		::serverListenTick(serverInstance, server);
	}
	serverInstance.Socket.close();
	return 0;
}

void														threadServer						(void* pServerInstance)				{
	::gpk::SUDPServer												& serverInstance					= *(::gpk::SUDPServer*)pServerInstance;
	gerror_if(errored(server(serverInstance)), "Server exiting with error.")
	else
		info_printf("Server gracefully closed.");
	serverInstance.Listen										= false;
	serverInstance.Socket.close();
	{
		::gpk::mutex_guard												lock								(serverInstance.Mutex);
		serverInstance.Clients.clear();
	}
}

::gpk::error_t												gpk::serverStart					(::gpk::SUDPServer& serverInstance, uint16_t port, int16_t adapterIndex)		{
	serverInstance.Listen										= true;
	serverInstance.Address.Port									= port;
	serverInstance.AdapterIndex									= adapterIndex;
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
	ginfo_if(attempt < 10, "Listening socket closed with %u disconnect attempt%s.", attempt, (attempt > 1) ? "s":"");
	return 0;
}
