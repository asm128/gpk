#include "gpk_connection.h"
#include "gpk_chrono.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"
#include "gpk_encoding.h"

::gpk::error_t												gpk::connectionPushData				(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::view_array<const byte_t> & data) {
	ree_if(data.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "%s", "Invalid payload size.");
	{
		::gpk::mutex_guard												lock								(queue.MutexSend);
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					message;
		message->Command.Command									= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
		message->Command.Type										= ::gpk::ENDPOINT_MESSAGE_TYPE_REQUEST;
		message->Command.Payload									= 0;
		message->Time												= ::gpk::timeCurrentInUs();
		//message->Payload											= data;
		if(0 == client.KeyPing) {
			client.KeyPing												= message->Time;
			info_printf("Key ping set: %llu.", client.KeyPing);
		}
		message->Payload.clear();
		::gpk::ardellEncode(data, client.KeyPing & 0xFFFFFFFF, true, message->Payload);
		gpk_necall(queue.Send.push_back(message), "%s", "Out of memory?");
	}
	return 0;
}

static				uint64_t								hashFromTime						(uint64_t currentTime)	{ return ::gpk::noise1DBase(currentTime & 0xFFFFFFFF) + (currentTime >> 16); }
static constexpr	const uint32_t							UDP_PAYLOAD_SENT_LIFETIME			= 3000000; // microseconds

::gpk::error_t												gpk::connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageBuffer)				{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& queueToSend						= client.Queue.Send;
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client 
	int																sa_length							= (int)sizeof(sockaddr_in);		// Length of client struct 
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);

	::gpk::mutex_guard												lock								(client.Queue.MutexSend);
	messageBuffer.clear();								
	for(uint32_t iSent = 0; iSent < client.Queue.Sent.size(); ++iSent) 
		if((gpk::timeCurrentInUs() - client.Queue.Sent[iSent]->Time) > ::UDP_PAYLOAD_SENT_LIFETIME) {
			client.Queue.Sent.remove(iSent);
			if(iSent)
				--iSent;
			warning_printf("%s", "A message sent didn't receive a confirmation on time.");

		}
	for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					 pMessageToSend						= queueToSend[iSend];
		if(0 == pMessageToSend)
			continue;
		::gpk::SUDPConnectionMessage									& messageToSend						= *pMessageToSend;
		int64_t															currentTime							= ::gpk::timeCurrentInUs();
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
				payloadHeader.MessageId										= ::hashFromTime(messageToSend.Time);//(uint64_t)(::gpk::noiseNormal1D(currentTime) * 0xffFFffFFffFFffFFULL);
				gpk_necall(sendStream.write_pod(payloadHeader), "%s", "??");
				gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "%s", "??");
				ce_if((int)sendStream.CursorPosition != ::sendto(client.Socket, sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sa_length), "%s", "Error sending datagram.");	// Send data back
				verbose_printf("%u bytes sent.", sendStream.CursorPosition);
				//pMessageToSend->Time										= currentTime;
				gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "%s", "Out of memory?");
				gpk_necall(messageBuffer.push_back(pMessageToSend), "%s", "Out of memory?");
			}
			else { // response
				payloadHeader.MessageId										= messageToSend.Time;
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

static	::gpk::error_t										handleRequestDISCONNECT					(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{ 
	ree_if(0 != command.Payload || ::gpk::UDP_CONNECTION_STATE_IDLE != client.State, "Invalid client state.");
	client.FirstPing											= 
	client.KeyPing												= 0;
	client.LastPing												= gpk::timeCurrentInUs();
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	client.Socket.close();
	info_printf("%s", "Disconnected!");
	return 0; 
}

static	::gpk::error_t										handleRequestCONNECT						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{ 
	ree_if(1 != command.Payload || ::gpk::UDP_CONNECTION_STATE_HANDSHAKE != client.State, "Invalid client state.");
	{
		client.LastPing												= gpk::timeCurrentInUs();
		::gpk::mutex_guard												lock								(client.Queue.MutexSend);
		for(uint32_t iSent = 0, countSent = client.Queue.Sent.size(); iSent < countSent; ++iSent) 
			if(::gpk::ENDPOINT_COMMAND_CONNECT == client.Queue.Sent[iSent]->Command.Command) {
				client.Queue.Sent.remove(iSent);
				break;
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
		memset(receiveBuffer.begin(), 0, receiveBuffer.size());
		if(errored(bytes_received = ::recvfrom(client.Socket, receiveBuffer.begin(), (int)receiveBuffer.size(), MSG_PEEK, (sockaddr*)&sa_client, &sa_length))) {
			rew_if(WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload data.")
			else 
				warning_printf("%s", "Failed to receive all of the payload data.");
		}
		ree_if(receiveBuffer.size() != (uint32_t)bytes_received, "Packet size received doesn't match with header size. Received size: %u. Expected: %u.", bytes_received, receiveBuffer.size());
		if(client.KeyPing == 0) {
			for(uint32_t iTime = 0, countLapse = 3000000; iTime < countLapse; ++iTime)
				if(::hashFromTime(client.FirstPing + iTime) == header.MessageId) {
					client.KeyPing												= client.FirstPing + iTime;
					always_printf("Key ping detected in %u attempts: %llu. First ping: %llu. Difference: %llu.", iTime, client.KeyPing, client.FirstPing, client.KeyPing - client.FirstPing);
					break;
				}
			reterr_error_if(0 == client.KeyPing, "Failed to determine encryption key!");
		}
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					messageReceived						= {};
		messageReceived->Command									= header.Command;
		messageReceived->Time										= header.MessageId;
		if(header.Size > 0) {
			messageReceived->Payload.clear();
			::gpk::ardellDecode({&receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size}, client.KeyPing & 0xFFFFFFFF, true, messageReceived->Payload);
		}
		client.LastPing												= ::gpk::timeCurrentInUs();
		{
			::gpk::mutex_guard												lock								(client.Queue.MutexReceive);
			client.Queue.Received.push_back(messageReceived);
		}
		{
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					response							= {};
			response->Command											= header.Command;
			response->Command.Type										= ::gpk::ENDPOINT_MESSAGE_TYPE_RESPONSE;
			response->Time												= header.MessageId;
			::gpk::mutex_guard												lock								(client.Queue.MutexSend);
			client.Queue.Send.push_back(response);
		}
	}
	else { // response
		::gpk::mutex_guard												lock								(client.Queue.MutexSend);
		for(uint32_t iSent = 0; iSent < client.Queue.Sent.size(); ++iSent) {
			uint64_t														hashLocal							= ::hashFromTime(client.Queue.Sent[iSent]->Time);
			if(hashLocal == header.MessageId) {
				client.Queue.Sent.remove(iSent);
				client.LastPing												= ::gpk::timeCurrentInUs();
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
		case ::gpk::ENDPOINT_COMMAND_PAYLOAD	: return ::handlePAYLOAD			(command, client, receiveBuffer); 
		case ::gpk::ENDPOINT_COMMAND_CONNECT	: return ::handleRequestCONNECT		(command, client); 
		case ::gpk::ENDPOINT_COMMAND_DISCONNECT	: return ::handleRequestDISCONNECT	(command, client); 
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
