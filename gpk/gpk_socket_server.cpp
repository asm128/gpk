// http://www.gomorgan89.com
#include "gpk_noise.h"
#include "gpk_connection_ip.h"
#include "gpk_array.h"
#include "gpk_view_stream.h"
#include "gpk_chrono.h"
#include "gpk_sync.h"
#include "gpk_socket_server.h"

#include <cstdlib>
#if defined(GPK_WINDOWS)
#	include <process.h>
#else
#	error	"Not implemented."
#endif

GPK_USING_TYPEINT();

sttc	::gpk::err_t	updateClients			(gpk::SSocketServer & serverInstance)		{
	::gpk::apobj<::gpk::SSocketConnection>	clientsToProcess;
	::gpk::au0_t						receiveBuffer;
	::gpk::apobj<::gpk::SMessage>		cacheSent						= {};
	::gpk::apobj<::gpk::SMessage>		cacheSend						= {};
	while(serverInstance.Listen) {
		::gpk::sleep(1);
		u2_t							totalClientCount				= serverInstance.Sockets.size();
		{
			::std::lock_guard					lock								(serverInstance.Mutex);
			totalClientCount				= serverInstance.Sockets.size();
		}
		timeval								wait_time							= {0, 1};
		u2_t							remainder							= totalClientCount % 64;
		u2_t							stageCount							= ::gpk::round_up(totalClientCount, 64U);
		fd_set								sockets								= {};
		for(u2_t iStage = 0; iStage < stageCount; ++iStage) {
			const u2_t						offsetClient						= iStage * 64;
			memset(&sockets, -1, szof(fd_set));
			sockets.fd_count				= 0;
			u2_t							stageClientCount					= (iStage == (stageCount - 1) && remainder) ? remainder : 64;
			{
				::std::lock_guard					lock								(serverInstance.Mutex);
				for(u2_t iClient = 0, actualCount = ::gpk::min(serverInstance.Sockets.size(), stageClientCount); iClient < actualCount; ++iClient) {
					::gpk::pobj<::gpk::SSocketConnection>	pclient							= serverInstance.Sockets[offsetClient + iClient];
					if(0 == pclient.get_ref() || pclient->IdConnection >= serverInstance.Clients.size() || 0 == serverInstance.Clients[pclient->IdConnection].get_ref()) {
						if(pclient.get_ref()) {
							pclient->IdConnection = ::gpk::u2_t(-1);
							pclient->Socket.close();
						}
						continue;
					}
					::gpk::SConnection					& connection						= *serverInstance.Clients[pclient->IdConnection];
					if(pclient->Socket == INVALID_SOCKET || connection.State == ::gpk::CONNECTION_STATE_DISCONNECTED)
						continue;
					if(connection.Queue.Send.size())
						if_fail_e(::gpk::connectionSendQueue(connection, *pclient, cacheSent, cacheSend));
					sockets.fd_array[sockets.fd_count]		= pclient->Socket;
					if(sockets.fd_array[sockets.fd_count] != INVALID_SOCKET)
						++sockets.fd_count;
				}
			}
			if(false == serverInstance.Listen)
				return 0;
			{
				clientsToProcess.clear();
				{
					::std::lock_guard				lock								(serverInstance.Mutex);
					if_true_bi(0 == serverInstance.Sockets.size());
					for(u2_t sd = 0; sd < sockets.fd_count; ++sd) {
						for(u2_t iClient = 0, countCli = ::gpk::min(serverInstance.Sockets.size(), stageClientCount); iClient < countCli; ++iClient) {
							::gpk::pobj<::gpk::SSocketConnection>		pclient								= serverInstance.Sockets[offsetClient + iClient];
							if(0 == pclient.get_ref() || pclient->IdConnection >= serverInstance.Clients.size() || 0 == serverInstance.Clients[pclient->IdConnection].get_ref() || pclient->Socket == INVALID_SOCKET || serverInstance.Clients[pclient->IdConnection]->State == ::gpk::CONNECTION_STATE_DISCONNECTED)
								continue;
							if(sockets.fd_array[sd] == pclient->Socket) {
								if_fail_fe(clientsToProcess.push_back(pclient));
								break;
							}
						}
					}
				}
				for(u2_t iClient = 0; iClient < clientsToProcess.size(); ++iClient) {
					if(false == serverInstance.Listen)
						return 0;
					FD_ZERO(sockets.fd_array);
					::gpk::SSocketConnection					& clientSocket						= *clientsToProcess[iClient];
					if(clientSocket.IdConnection >= serverInstance.Clients.size() || 0 == serverInstance.Clients[clientSocket.IdConnection].get_ref()) {
						clientSocket.IdConnection = ::gpk::u2_t(-1);
						clientSocket.Socket.close();
						continue;
					}
					::gpk::SConnection						& client							= *serverInstance.Clients[clientSocket.IdConnection];
					sockets.fd_array[0]						= clientSocket.Socket;
					sockets.fd_count						= 1;
					::select(0, &sockets, 0, 0, &wait_time);
					while(sockets.fd_count > 0) {
						sockaddr_in									sa_client							= {};						// Information about the client
						int											sa_length							= (int)sizeof(sockaddr_in);	// Length of client struct
						::gpk::SCommand							command								= {};						// Where to store received data
						int											bytes_received						= 0;
						if (::gpk::failed(bytes_received = ::recvfrom(clientSocket.Socket, (sc_t*)&command, (int)sizeof(::gpk::SCommand), MSG_PEEK, (sockaddr*)&sa_client, &sa_length))) {
							u2_t									lastError							= ::WSAGetLastError();
							if(lastError != WSAEMSGSIZE) {
								warning_printf("%s", "'lastError != WSAEMSGSIZE' - Could not receive datagram.");
								::recvfrom(clientSocket.Socket, (sc_t*)&command, (int)szof(::gpk::SCommand), 0, 0, 0);
								if(lastError == WSAENETRESET || lastError == WSAENOTSOCK)
									client.State = ::gpk::CONNECTION_STATE_DISCONNECTED;
								break;
							}
						}
						::gpk::SIPv4End								address								= clientSocket.Address;
						if(::gpk::TRANSPORT_PROTOCOL_UDP == serverInstance.Protocol)
							::gpk::tcpipAddressFromSockaddr(sa_client, address);
						if(clientSocket.Address != address) {
							warning_printf("Command received from an invalid address: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
							::recvfrom(clientSocket.Socket, (sc_t*)&command, (int)szof(::gpk::SCommand), 0, 0, 0);
							continue;
						}
						if_fail_ef(::gpk::connectionHandleCommand(client, clientSocket, command, receiveBuffer), "Error processing command from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
						if(INVALID_SOCKET != clientSocket.Socket) {
							if(::gpk::TRANSPORT_PROTOCOL_TCP == serverInstance.Protocol && ::gpk::ENDPOINT_COMMAND_PAYLOAD == command.Command)
								::recv(clientSocket.Socket, (sc_t*)receiveBuffer.begin(), (int)receiveBuffer.size(), MSG_WAITALL);
							else
								::recvfrom(clientSocket.Socket, (sc_t*)&command, (int)szof(::gpk::SCommand), 0, 0, 0);
						}
						::select(0, &sockets, 0, 0, &wait_time);
					}
				}
			}
		}
	}
	return 0;
}

static	::gpk::err_t	recycleClient					(::gpk::SSocketServer& serverInstance, ::gpk::pobj<::gpk::SConnection> & pClient, ::gpk::pobj<::gpk::SSocketConnection> & pSocket)		{
	for(u2_t iClient = 0, countClients = serverInstance.Clients.size(); iClient < countClients; ++iClient) {
		pClient										= serverInstance.Clients[iClient];
		pSocket										= serverInstance.Sockets[iClient];
		::gpk::SConnection							& client						= *pClient;
		int64_t											limitTime						= (::gpk::timeCurrentInUs() - client.LastPing);
		info_printf("LimitTime : %lli.", limitTime);
		if(limitTime > serverInstance.Timeout || pSocket->Socket == INVALID_SOCKET || client.State == ::gpk::CONNECTION_STATE_DISCONNECTED) {
			client.State								= ::gpk::CONNECTION_STATE_DISCONNECTED;
			pSocket->Socket.close();
			pSocket->IdConnection					= ::gpk::u2_t(-1);
			client.KeyPing								=
			client.LastPing								=
			client.FirstPing							= 0;
			{
				::std::lock_guard								lockRecv						(client.Queue.MutexReceive);
				client.Queue.Received	.clear();
			}
			{
				::std::lock_guard								lockSend						(client.Queue.MutexSend);
				client.Queue.Send		.clear();
				client.Queue.Sent		.clear();
			}
			return iClient;
		}
		else {
			pClient										= {};
			pSocket										= {};
		}
	}
	return -1;
}

static	::gpk::err_t	serverAcceptClient		(::gpk::SSocketServer& serverInstance, ::gpk::SCommand command, const sockaddr_in & sa_client, SOCKET acceptedSocket = INVALID_SOCKET)		{
	gpk::pobj<gpk::SConnection>		pClient				= {};
	gpk::pobj<gpk::SSocketConnection>	pSocket				= {};
	{ // accept
		::std::lock_guard			lock					(serverInstance.Mutex);
		s2_t						found					= ::recycleClient(serverInstance, pClient, pSocket);
		const u2_t				idConnection			= (-1 == found) ? serverInstance.Clients.size() : (u2_t)found;
		::gpk::tcpipAddressFromSockaddr(sa_client, pSocket->Address);
		pSocket->IdConnection	= idConnection;
		command.Type			= ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE;
		::gpk::pobj<::gpk::SMessage>	connectResponse		= {};
		connectResponse.create(::gpk::SMessage{{}, (uint64_t)::gpk::timeCurrentInUs(), command});
		if_fail_fe(pClient->Queue.Send.push_back(connectResponse));
		pClient->LastPing		=
		pClient->FirstPing		= ::gpk::timeCurrentInUs();
		if(INVALID_SOCKET == acceptedSocket) {
			if_true_vef(-1, INVALID_SOCKET == (pSocket->Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "%s", "Could not create socket.");
		}
		else
			pSocket->Socket.Handle	= acceptedSocket;
		pClient->State			= ::gpk::CONNECTION_STATE_HANDSHAKE;
		if(found == -1) {
			if_fail_fe(serverInstance.Clients.push_back(pClient));
			if_fail_fe(serverInstance.Sockets.push_back(pSocket));
		}
	}
	info_printf("Current client count: %u", serverInstance.Clients.size());
	return 0;
}

static	::gpk::err_t	serverListenTick		(::gpk::SSocketServer& serverInstance, const sockaddr_in & server)		{
	int							client_length			= (int)sizeof(sockaddr_in);		// Length of client struct
	::gpk::SCommand			command					= {};	// Where to store received data
	int							bytes_received			= 0;
	sockaddr_in					sa_client				= {};	// Information about the client
	::gpk::auto_socket_close	acceptedSocket			= {};
	if(::gpk::TRANSPORT_PROTOCOL_TCP == serverInstance.Protocol) {
		if(INVALID_SOCKET == serverInstance.Socket) {
			if_true_vef(-1, INVALID_SOCKET == (serverInstance.Socket.Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), "%s", "Could not create socket.");
			if_fail_fef(::bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "%s", "Could not bind name to socket.");
			if_fail_fef(::listen(serverInstance.Socket, SOMAXCONN), "%s", "Could not listen on socket.");
		}
		if_fail_ve(-1, acceptedSocket.Handle = ::accept(serverInstance.Socket, (sockaddr*)&sa_client, &client_length));
		if_fail_ve(-1, bytes_received = ::recv(acceptedSocket, (sc_t*)&command, (int)sizeof(::gpk::SCommand), 0));
	}
	else {
		serverInstance.Socket.close();
		if_true_vef(-1, INVALID_SOCKET == (serverInstance.Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "%s", "Could not create socket.");
		if_fail_fef(::bind(serverInstance.Socket, (sockaddr *)&server, sizeof(sockaddr_in)), "%s", "Could not bind name to socket.");	/* Bind address to socket */
		if_fail_ve(-1, bytes_received = recvfrom(serverInstance.Socket, (sc_t*)&command, (int)sizeof(::gpk::SCommand), MSG_PEEK, (sockaddr*)&sa_client, &client_length));
	}
	if_true_vif(0, 0 == bytes_received, "%s", "Offline?");

	if_true_vef(-1, command.Type		!= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST, "Invalid message type: 0x%x '%s'", command.Type, ::gpk::get_value_label(command.Type).begin()); 
	if_true_vef(-1, command.Command	!= ::gpk::ENDPOINT_COMMAND_DISCONNECT
		&& command.Command	!= ::gpk::ENDPOINT_COMMAND_CONNECT
		, "Invalid message type: 0x%x '%s'", command.Type, ::gpk::get_value_label(command.Type).begin()); 

	switch(command.Command) {
	case ::gpk::ENDPOINT_COMMAND_CONNECT	: 
		if(::gpk::TRANSPORT_PROTOCOL_TCP == serverInstance.Protocol) {
			const SOCKET			clientSocket			= acceptedSocket.Handle;
			acceptedSocket.Handle	= INVALID_SOCKET;
			if_fail_e(::serverAcceptClient(serverInstance, command, sa_client, clientSocket));
		}
		else
			if_fail_e(::serverAcceptClient(serverInstance, command, sa_client));
		::gpk::sleep(1);
		break;
	case ::gpk::ENDPOINT_COMMAND_DISCONNECT	: {
		if(false == serverInstance.Listen)
			break;

		sockaddr_in					sa_srv				= {};	// Information about the client
		::gpk::tcpipAddressToSockaddr(serverInstance.Address, sa_srv);
		::gpk::SIPv4End				address				= {{}, serverInstance.Address.Port};
		::gpk::tcpipAddressFromSockaddr(sa_client, address);
		if_true_vef(-1, sa_client.sin_port != sa_srv.sin_port
			|| memcmp(&sa_client.sin_addr	, &sa_srv.sin_addr	, sizeof(sa_srv.sin_addr))
			|| memcmp(&sa_client.sin_family	, &sa_srv.sin_family, sizeof(sa_srv.sin_family))
			, "Invalid server address: %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(address)
		);
		serverInstance.Listen	= false;	// Stop server (this breaks the listen loop).
		break;
	}
	}
	return 0;
}

static	void			threadUpdateClients		(void* serverInstance)					{ ::updateClients(*(::gpk::SSocketServer*)serverInstance); }
static	::gpk::err_t	server					(::gpk::SSocketServer& serverInstance)		{
	serverInstance.Listen	= true;
	if_fail_fe(gpk::tcpipAddress(serverInstance.Address.Port, serverInstance.AdapterIndex, serverInstance.Protocol, serverInstance.Address));
	sockaddr_in					server					= {};
	if_fail_fe(gpk::tcpipAddressToSockaddr(serverInstance.Address, server));
#if defined(GPK_WINDOWS)
	_beginthread(::threadUpdateClients, 0, &serverInstance);
#else
#	error "Not implemented."
#endif
	info_printf("Server running on %u.%u.%u.%u:%u", GPK_IPV4_EXPAND(serverInstance.Address));
	while(serverInstance.Listen) {	// Loop and get data from clients
		if_fail_w(::serverListenTick(serverInstance, server));
	}
	serverInstance.Socket.close();
	return 0;
}

static	void			threadServer			(void* pServerInstance)				{
	::gpk::SSocketServer			& serverInstance		= *(::gpk::SSocketServer*)pServerInstance;
	if_fail_e(server(serverInstance))
	else
		info_printf("Server gracefully closed.");

	serverInstance.Listen	= false;
	serverInstance.Socket.close();
	{
		::std::lock_guard			lock					(serverInstance.Mutex);
		serverInstance.Clients.clear();
		serverInstance.Sockets.clear();
	}
}

::gpk::err_t			gpk::serverStart		(::gpk::SSocketServer& serverInstance, u1_t port, s1_t adapterIndex, ::gpk::TRANSPORT_PROTOCOL protocol)		{
	serverInstance.Listen		= true;
	serverInstance.Address.Port	= port;
	serverInstance.AdapterIndex	= adapterIndex;
	serverInstance.Protocol		= protocol;
	_beginthread(::threadServer, 0, &serverInstance);
	return 0;
}

::gpk::err_t			gpk::serverStop			(::gpk::SSocketServer& serverInstance)		{
	serverInstance.Listen	= false;
	if(::gpk::TRANSPORT_PROTOCOL_TCP == serverInstance.Protocol) {
		serverInstance.Socket.close();
		return 0;
	}
	sockaddr_in					sa_srv					= {};							// Information about the client
	int							sa_length				= (int)sizeof(sockaddr_in);		// Length of client struct
	::gpk::SCommand			command					= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST, };							// Where to store received data
	::gpk::tcpipAddressToSockaddr(serverInstance.Address, sa_srv);
	u2_t					attempt					= 0;
	do{
		::sendto(serverInstance.Socket, (const sc_t*)&command, (int)sizeof(::gpk::SCommand), 0, (sockaddr*)&sa_srv, sa_length);
		::gpk::sleep(100);
	} while(10 > ++attempt && serverInstance.Clients.size());

	if_true_if(attempt < 10, "Listening socket closed with %u disconnect attempt%s.", attempt, (attempt > 1) ? "s":"");
	return 0;
}

::gpk::err_t			gpk::serverPayloadCollect(::gpk::SSocketServer & server, ::gpk::aobj<::gpk::apobj<::gpk::SMessage>> & receivedMessages)		{
	::std::lock_guard			lock					(server.Mutex);
	if_fail_fe(receivedMessages.resize(server.Clients.size()));
	for(u2_t iClient = 0, countClients = server.Clients.size(); iClient < countClients; ++iClient) {
		::gpk::pnco<::gpk::SConnection>	client			= server.Clients[iClient];
		if(!client)
			continue;

		if_fail_fe(gpk::connectionPayloadCollect(*client, receivedMessages[iClient]));
	}
	return 0;
}
