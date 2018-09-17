#include "gpk_connection.h"
#include "gpk_chrono.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"

::gpk::error_t												gpk::connectionPushData				(::gpk::SUDPClientQueue & queue, const ::gpk::view_array<const byte_t> & data) {
	ree_if(data.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "%s", "Invalid payload size.");
	{
		::gpk::mutex_guard												lock								(queue.MutexSend);
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					message;
		message->Command.Command									= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
		message->Command.Type										= ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST;
		message->Command.Payload									= 0;
		message->Time												= ::gpk::timeCurrentInMs();
		message->Payload											= data;
		gpk_necall(queue.Send.push_back(message), "%s", "Out of memory?");
	}
	return 0;
}

static				uint64_t								hashFromTime						(uint64_t currentTime)	{ return ::gpk::noise1DBase(currentTime & 0xFFFFFFFF) + (currentTime >> 16); }
static constexpr	const uint32_t							UDP_PAYLOAD_SENT_LIFETIME			= 3000; // milliseconds

::gpk::error_t												gpk::connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageBuffer)				{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& queueToSend						= client.Queue.Send;
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);

	::gpk::mutex_guard												lock								(client.Queue.MutexSend);
	messageBuffer.clear();								
	for(uint32_t iSent = 0; iSent < client.Queue.Sent.size(); ++iSent) 
		if((gpk::timeCurrentInMs() - client.Queue.Sent[iSent]->Time) > ::UDP_PAYLOAD_SENT_LIFETIME) {
			client.Queue.Sent.remove(iSent--);
			warning_printf("%s", "A message sent didn't receive a confirmation on time.");
		}
	for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					 pMessageToSend						= queueToSend[iSend];
		if(0 == pMessageToSend)
			continue;
		::gpk::SUDPConnectionMessage									& messageToSend						= *pMessageToSend;
		int64_t															currentTime							= ::gpk::timeCurrentInMs();
		if(messageToSend.Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD) {
			ce_if((int)sizeof(::gpk::SUDPCommand) != ::sendto(client.Socket, (const char*)&messageToSend.Command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_remote, sa_length), "%s", "Error sending datagram.");	// Send data back
			pMessageToSend->Time										= currentTime;
			gpk_necall(messageBuffer.push_back(pMessageToSend), "%s", "Out of memory?");
			gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "%s", "Out of memory?");
		}
		else {
			::gpk::SUDPPayloadHeader										payloadHeader						= {};
			payloadHeader.Command										= messageToSend.Command;
			payloadHeader.Size											= messageToSend.Payload.size();
			if(messageToSend.Command.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST) {
				ce_if(messageToSend.Payload.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "Maximum allowed payload size is only %u bytes.", ::gpk::UDP_PAYLOAD_SIZE_LIMIT);
				gpk_necall(messageBytes.resize((uint32_t)sizeof(::gpk::SUDPPayloadHeader) + messageToSend.Payload.size()), "%s", "Out of memory?");
				::gpk::view_stream<byte_t>										sendStream							= {messageBytes.begin(), messageBytes.size()};
				payloadHeader.Hash											= ::hashFromTime(currentTime);//(uint64_t)(::gpk::noiseNormal1D(currentTime) * 0xffFFffFFffFFffFFULL);
				gpk_necall(sendStream.write_pod(payloadHeader), "%s", "??");
				gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "%s", "??");
				ce_if((int)sendStream.CursorPosition != ::sendto(client.Socket, sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sa_length), "%s", "Error sending datagram.");	// Send data back
				verbose_printf("%u bytes sent.", sendStream.CursorPosition);
				pMessageToSend->Time										= currentTime;
				gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "%s", "Out of memory?");
				gpk_necall(messageBuffer.push_back(pMessageToSend), "%s", "Out of memory?");
			}
			else { // response
				payloadHeader.Hash											= messageToSend.Time;
				ce_if((int)sizeof(::gpk::SUDPPayloadHeader) != ::sendto(client.Socket, (const char*)&payloadHeader, (int)sizeof(::gpk::SUDPPayloadHeader), 0, (sockaddr*)&sa_remote, sa_length), "%s", "Error sending datagram.");	// Send data back
				gpk_necall(messageBuffer.push_back(pMessageToSend), "%s", "Out of memory?");
			}
		}
	}
	for(uint32_t iSent = 0; iSent < messageBuffer.size(); ++iSent)
		for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend)
			if(queueToSend[iSend] == messageBuffer[iSent]) {
				queueToSend.remove(iSend);
				break;
			}
	return 0;
}

static	::gpk::error_t										handleDISCONNECT					(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{ 
	if(0 != command.Payload || ::gpk::UDP_CONNECTION_STATE_IDLE != client.State) 
		return -1;
	client.LastPing												= gpk::timeCurrentInMs();
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	client.Socket.close();
	info_printf("%s", "Disconnected!");
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
	info_printf("%s", "Connected!");
	return 0; 
}

#if defined(GPK_WINDOWS) 
typedef int socklen_t;
#endif

static	::gpk::error_t										handlePAYLOAD						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client, ::gpk::array_pod<byte_t> & receiveBuffer)		{ 
	if(::gpk::UDP_CONNECTION_STATE_IDLE != client.State)
		return 1;
	::gpk::SUDPPayloadHeader										header								= {};
	sockaddr_in														sa_client							= {};						// Information about the client 
	socklen_t														sa_length							= (int)sizeof(sockaddr_in);	// Length of client struct 
	int																bytes_received						= {};
	if errored(bytes_received = ::recvfrom(client.Socket.Handle, (char*)&header, (int)sizeof(::gpk::SUDPPayloadHeader), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
		rew_if(WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload header.");	
	}
	if(command.Type == ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST) {
		gpk_necall(receiveBuffer.resize(sizeof(::gpk::SUDPPayloadHeader) + header.Size), "%s", "Out of memory?");
		if(errored(bytes_received = ::recvfrom(client.Socket, receiveBuffer.begin(), (int)receiveBuffer.size(), MSG_PEEK, (sockaddr*)&sa_client, &sa_length))) {
			rew_if(WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload data.")
			else 
				warning_printf("%s", "Failed to receive all of the payload data.");
		}
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					messageReceived						= {};
		messageReceived->Command									= header.Command;
		messageReceived->Time										= header.Hash;
		gpk_necall(messageReceived->Payload.resize(header.Size), "%s", "Out of memory?");
		if(header.Size > 0)
			memcpy(messageReceived->Payload.begin(), &receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size);
		client.LastPing												= gpk::timeCurrentInMs();
		{
			::gpk::mutex_guard												lock								(client.Queue.MutexReceive);
			client.Queue.Received.push_back(messageReceived);
		}
		{
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					response							= {};
			response->Command											= header.Command;
			response->Command.Type										= ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE;
			response->Time												= header.Hash;
			::gpk::mutex_guard												lock								(client.Queue.MutexSend);
			client.Queue.Send.push_back(response);
		}
	}
	else { // response
		::gpk::mutex_guard												lock								(client.Queue.MutexSend);
		for(uint32_t iSent = 0; iSent < client.Queue.Sent.size(); ++iSent) {
			uint64_t														hashLocal							= ::hashFromTime(client.Queue.Sent[iSent]->Time);
			if(hashLocal == header.Hash) {
				client.Queue.Sent.remove(iSent);
				client.LastPing												= gpk::timeCurrentInMs();
				break;
			}
		}
	}
	return 0; 
}

::gpk::error_t												gpk::connectionHandleCommand		(::gpk::SUDPConnection& client, ::gpk::SUDPCommand& command, ::gpk::array_pod<byte_t> & receiveBuffer)		{
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
