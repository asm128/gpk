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

::gpk::error_t												sendQueue							(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageBuffer)				{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& queueToSend						= client.Queue.Send;
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);

	::gpk::mutex_guard												lock								(client.Queue.MutexSend);
	messageBuffer.clear();								
	for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					 pMessageToSend						= queueToSend[0];
		if(0 == pMessageToSend)
			continue;
		::gpk::SUDPConnectionMessage									& messageToSend						= *pMessageToSend;
		int64_t															currentTime							= ::gpk::timeCurrentInMs();
		if(messageToSend.Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			ce_if((int)sizeof(::gpk::SUDPCommand) != ::sendto(client.Socket, (const char*)&messageToSend.Command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_remote, sa_length), "Error sending datagram.");	// Send data back
			pMessageToSend->Time										= currentTime;
			messageBuffer.push_back(pMessageToSend);
		}
		else {
			if(messageToSend.Command.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST) {
				ce_if(messageToSend.Payload.size() > 60000U, "Maximum allowed payload size is only %u bytes.", 60000U);
				gpk_necall(messageBytes.resize((uint32_t)sizeof(::gpk::SUDPPayloadHeader) + messageToSend.Payload.size()), "Out of memory?");
				::gpk::view_stream<byte_t>										sendStream							= {messageBytes.begin(), messageBytes.size()};
				::gpk::SUDPPayloadHeader										payloadHeader						= {};
				payloadHeader.Command										= messageToSend.Command;
				payloadHeader.Size											= messageToSend.Payload.size();
				payloadHeader.Hash											= (uint64_t)(::gpk::noiseNormal1D(currentTime) * 0xffFFffFFffFFffFFULL);
				gpk_necall(sendStream.write_pod(payloadHeader), "??");
				gpk_necall(sendStream.write_pod(messageToSend.Payload.size()), "??");
				gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "??");
				ce_if((int)sendStream.CursorPosition != ::sendto(client.Socket, sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sa_length), "Error sending datagram.");	// Send data back
				pMessageToSend->Time										= currentTime;
				messageBuffer.push_back(pMessageToSend);
			}
			else { // response
				::gpk::SUDPPayloadHeader										payloadHeader						= {};
				payloadHeader.Command										= messageToSend.Command;
				payloadHeader.Size											= messageToSend.Payload.size();
				payloadHeader.Hash											= messageToSend.Time;
				ce_if((int)sizeof(::gpk::SUDPPayloadHeader) != ::sendto(client.Socket, (const char*)&payloadHeader, (int)sizeof(::gpk::SUDPPayloadHeader), 0, (sockaddr*)&sa_remote, sa_length), "Error sending datagram.");	// Send data back
				messageBuffer.push_back(pMessageToSend);
			}
		}
		gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "Out of memory?");
	}
	for(uint32_t iSent = 0; iSent < messageBuffer.size(); ++iSent)
		for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend)
			if(queueToSend[iSend] == messageBuffer[iSent]) {
				queueToSend.remove(iSend);
				break;
			}
	return 0;
}

static	::gpk::error_t										handleCONNECT						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{ 
	if(1 != command.Payload || ::gpk::UDP_CONNECTION_STATE_HANDSHAKE != client.State) 
		return -1;
	client.LastPing												= gpk::timeCurrentInMs();
	{
		::gpk::mutex_guard												lock								(client.Queue.MutexSend);
		for(uint32_t iSent = 0, countSent = client.Queue.Sent.size(); iSent < countSent; ++iSent) {
			if(::gpk::ENDPOINT_COMMAND_CONNECT == client.Queue.Sent[iSent]->Command.Command) {
				client.Queue.Sent.remove(iSent);
				break;
			}
		}
		client.State												= ::gpk::UDP_CONNECTION_STATE_IDLE;
	}
	info_printf("Connected!");
	return 0; 
}

static	::gpk::error_t										handleDISCONNECT					(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{ 
	if(0 != command.Payload || ::gpk::UDP_CONNECTION_STATE_IDLE != client.State) 
		return -1;
	client.LastPing												= gpk::timeCurrentInMs();
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	client.Socket.close();
	info_printf("Disconnected!");
	return 0; 
}

static	::gpk::error_t										handlePAYLOAD						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client, ::gpk::array_pod<byte_t> & receiveBuffer)		{ 
	if(::gpk::UDP_CONNECTION_STATE_IDLE != client.State)
		return 1;
	::gpk::SUDPPayloadHeader										header							= {};
	sockaddr_in														sa_client						= {};						// Information about the client 
	int																sa_length						= (int)sizeof(sockaddr_in);	// Length of client struct 
	int																bytes_received					= {};
	if errored(bytes_received = ::recvfrom(client.Socket, (char*)&header, (int)sizeof(::gpk::SUDPPayloadHeader), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
		rew_if(WSAGetLastError() != WSAEMSGSIZE, "Could not receive payload header.");	
	}
	if(command.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST) {
		gpk_necall(receiveBuffer.resize(sizeof(::gpk::SUDPPayloadHeader) + header.Size), "Out of memory?");
		rew_if(errored(bytes_received = ::recvfrom(client.Socket, receiveBuffer.begin(), (int)receiveBuffer.size(), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)), "Could not receive payload data.");	
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					messageReceived					= {};
		messageReceived->Command									= header.Command;
		messageReceived->Time										= header.Hash;
		gpk_necall(messageReceived->Payload.resize(header.Size), "Out of memory?");
		if(header.Size > 0)
			memcpy(messageReceived->Payload.begin(), &receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size);
		client.LastPing												= gpk::timeCurrentInMs();
		{
			::gpk::mutex_guard												lock							(client.Queue.MutexReceive);
			client.Queue.Received.push_back(messageReceived);
		}
		{
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					response						= {};
			response->Command											= header.Command;
			response->Command.Type										= ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE;
			response->Time												= header.Hash;
			::gpk::mutex_guard												lock							(client.Queue.MutexSend);
			client.Queue.Send.push_back(response);
		}
	}
	else { // response
		::gpk::mutex_guard												lock							(client.Queue.MutexSend);
		for(uint32_t iSent = 0, countSent = client.Queue.Sent.size(); iSent < countSent; ++iSent) {
			int64_t															hashLocal						=  (uint64_t)(::gpk::noiseNormal1D(client.Queue.Sent[iSent]->Time) * 0xffFFffFFffFFffFFULL);
			if((hashLocal & 0xffFFffFFffFF0000ULL) == (header.Hash & 0xffFFffFFffFF0000ULL)) {
				client.Queue.Sent.remove(iSent);
				client.LastPing												= gpk::timeCurrentInMs();
				break;
			}
		}
	}
	return 0; 
}

::gpk::error_t												handleCommand						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client, ::gpk::array_pod<byte_t> & receiveBuffer)		{
	switch(command.Type) {
	case ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST:
		switch(command.Command) {
		case ::gpk::ENDPOINT_COMMAND_CONNECT	: return ::handleCONNECT	(command, client); 
		case ::gpk::ENDPOINT_COMMAND_PAYLOAD	: return ::handlePAYLOAD	(command, client, receiveBuffer); 
		case ::gpk::ENDPOINT_COMMAND_DISCONNECT	: return ::handleDISCONNECT	(command, client); 
		case ::gpk::ENDPOINT_COMMAND_NOOP		: return 0; 
		default									: 
			error_printf("Invalid command!: %u.", command.Command); 
			break;
		}
		break;
	case ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE:
		switch(command.Command) {
		case ::gpk::ENDPOINT_COMMAND_PAYLOAD	: return ::handlePAYLOAD	(command, client, receiveBuffer); 
		case ::gpk::ENDPOINT_COMMAND_CONNECT	: 
		case ::gpk::ENDPOINT_COMMAND_DISCONNECT	: 
		case ::gpk::ENDPOINT_COMMAND_NOOP		: return 0; 
		default									: 
			error_printf("Invalid command!: %u.", command.Command); 
			break;
		}
		break;
	}
	return -1;
}

::gpk::error_t												updateClients						(gpk::SUDPServer& serverInstance)		{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnection>>			clientsToProcess;
	::gpk::array_pod<byte_t>										receiveBuffer;
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	messageBuffer						= {};
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
						error_if(errored(::sendQueue(*pclient, messageBuffer)), "??");
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
						::gpk::ptr_obj<::gpk::SUDPConnection>							pclient								= serverInstance.Clients[offsetClient + iClient];
						if(0 == pclient || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED)
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
					::gpk::SUDPCommand											command								= {};						// Where to store received data 
					int																bytes_received						= 0;			
					::gpk::SUDPConnection											& client							= *clientsToProcess[iClient];
					if errored(bytes_received = ::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
						warning_printf("Could not receive datagram.");	
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);	
						continue;
					}
					::gpk::SIPv4													address								= {{}, 9999};
					::gpk::tcpipAddressFromSockaddr(sa_client, address);
					if(client.Address != address) {
						warning_printf("Command received from an invalid address: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
						::recvfrom(client.Socket, (char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, 0, 0);	
						continue;
					}
					error_if(errored(::handleCommand(command, client, receiveBuffer)), "Error processing command from: %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(address));
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
					if(limitTime > serverInstance.Timeout || pclient->Socket == INVALID_SOCKET || pclient->State == ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {
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
				::gpk::ptr_obj<::gpk::SUDPConnectionMessage>				connectResponse						= {};
				connectResponse.create(::gpk::SUDPConnectionMessage{{}, (uint64_t)::gpk::timeCurrentInMs(), command});
				gpk_necall(pclient->Queue.Send.push_back(connectResponse), "Out of memory?");
				pclient->LastPing										= ::gpk::timeCurrentInMs();
				ce_if(INVALID_SOCKET == (pclient->Socket.Handle = socket(AF_INET, SOCK_DGRAM, 0)), "Could not create socket.");
				pclient->State											= ::gpk::UDP_CONNECTION_STATE_HANDSHAKE;
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
	{
		::gpk::mutex_guard											lock								(serverInstance.Mutex);
		serverInstance.Clients.clear();
	}
} 

::gpk::error_t												serverStart									(::gpk::SUDPServer& serverInstance, uint16_t port)		{
	serverInstance.Listen										= true;
	serverInstance.Address.Port									= port;
	_beginthread(::threadServer, 0, &serverInstance);
	return 0;
}

::gpk::error_t												serverStop									(::gpk::SUDPServer& serverInstance)		{
	serverInstance.Listen										= false;
	sockaddr_in														sa_srv										= {};							// Information about the client 
	int																sa_length									= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::SUDPCommand											command										= {::gpk::ENDPOINT_COMMAND_DISCONNECT, ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST};							// Where to store received data 
	::gpk::tcpipAddressToSockaddr(serverInstance.Address, sa_srv);
	uint32_t														attempt										= 0;
	do{ 
		::sendto(serverInstance.Socket, (const char*)&command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_srv, sa_length);
		::gpk::sleep(100);
	} while(10 > ++attempt && serverInstance.Clients.size());
	info_if(attempt < 10, "Listening socket closed with %u disconnect attempt%s.", attempt, (attempt > 1) ? "s":"");
	return 0;
}