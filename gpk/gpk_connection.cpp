#include "gpk_connection.h"
#include "gpk_chrono.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"
#include "gpk_encoding.h"
#include "gpk_deflate.h"

::gpk::error_t												gpk::connectionPushData				(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::view_array<const byte_t> & data, bool bEncrypt, bool bCompress, uint8_t retryCount) {
	ree_if(data.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "%s", "Invalid payload size.");
	{
		::gpk::mutex_guard												lock								(queue.MutexSend);
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					message;
		//message->Command											= {::gpk::ENDPOINT_COMMAND_PAYLOAD, ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST, 0, };
		message->Command.Command									= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
		message->Command.Type										= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST;
		message->Command.Encrypted									= (uint8_t)(bEncrypt ? 1 : 0);
		message->Command.Compressed									= (uint8_t)(bCompress ? 1 : 0);
		message->Command.Packed										= 0;
		message->Command.Multipart									= 0;
		message->Time												= ::gpk::timeCurrentInUs();
		message->RetryCount											= retryCount;

		if(0 == client.KeyPing) {
			client.KeyPing												= message->Time;
			info_printf("Key ping set: %llu.", client.KeyPing);
		}
		message->Payload.clear();
		message->Payload											= data;
		gpk_necall(queue.Send.push_back(message), "%s", "Out of memory?");
	}
	return 0;
}

static				uint64_t								hashFromTime						(uint64_t currentTime)	{ return ::gpk::noise1DBase(currentTime & 0xFFFFFFFF) + (currentTime >> 16); }
static constexpr	const uint32_t							UDP_PAYLOAD_SENT_LIFETIME			= 1000000; // microseconds

::gpk::error_t												payloadQueueOptimize				(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSent, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& payloadsToSend) {
	if(payloadsToSend.size() < 2)
		return 0;
	{
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	optimizedPayloads					= {};
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	payloadsToRemove					= {};
		::gpk::array_pod<uint16_t>										payloadSizes						= {};
		::gpk::array_pod<byte_t>										serialized							= {};

		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					optimizedPayload					= {};
		optimizedPayload->Time										= ::gpk::timeCurrentInUs();
		optimizedPayload->Command									= {};
		optimizedPayload->Command.Command							= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
		optimizedPayload->Command.Type								= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST;
		optimizedPayload->Command.Packed							= 1;
		optimizedPayload->RetryCount								= 1;
		bool															keyPing								= false;
		for(uint32_t iPayload = 0; iPayload < payloadsToSend.size(); ++iPayload) {
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					currentPayload						= payloadsToSend[iPayload];
			if(0 == currentPayload)
				continue;
			if((currentPayload->Command.Packed & 1) || (0 != currentPayload->RetryCount) || currentPayload->Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE || currentPayload->Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD)
				continue;
			const bool														sizeEnough							= (serialized.size() + sizeof(uint16_t) + currentPayload->Payload.size()) < ::gpk::UDP_PAYLOAD_SIZE_LIMIT;
			if(false == sizeEnough)
				continue;
			if(currentPayload->Time == client.KeyPing)
				keyPing														= true;
			gpk_necall(payloadSizes.push_back((uint16_t)currentPayload->Payload.size()), "Out of memory? Payload size: %u.", currentPayload->Payload.size());
			gpk_necall(serialized.append(currentPayload->Payload.begin(), currentPayload->Payload.size()), "Failed to append payload! Payload size: %u.", currentPayload->Payload.size());
			gpk_necall(payloadsToRemove.push_back(currentPayload), "Out of memory? Payload count: %u.", payloadsToRemove.size());
			gpk_necall(messageCacheSent.push_back(currentPayload), "Out of memory? Payload count: %u.", payloadsToRemove.size());
		}
		const uint32_t													sizeRequired						= serialized.size() + sizeof(uint16_t) * payloadSizes.size() + sizeof(uint16_t);	// payloads + sizes + packet count
		uint16_t														packetCount							= (uint16_t)payloadSizes.size();
		gpk_necall(optimizedPayload->Payload.resize(sizeRequired), "Out of memory? Size required: %u.", sizeRequired);
		optimizedPayload->Payload.clear();
		optimizedPayload->Payload.append((const byte_t*)&packetCount, sizeof(uint16_t));
		optimizedPayload->Payload.append((const byte_t*)payloadSizes.begin(), sizeof(uint16_t) * payloadSizes.size());
		optimizedPayload->Payload.append(serialized.begin(), serialized.size());
		ree_if(sizeRequired != optimizedPayload->Payload.size(), "Invalid sizes! Size required: %u. Actual size: %u.", sizeRequired, optimizedPayload->Payload.size());
		gpk_necall(payloadsToSend.push_back(optimizedPayload), "Failed! Payload queue size: %u.", payloadsToSend.size());

		if(keyPing)
			client.KeyPing												= optimizedPayload->Time;

		for(int32_t iPayloadTR = 0; iPayloadTR < (int32_t)payloadsToRemove.size(); ++iPayloadTR)	// Done. Remove packed payloads from send queue.
			for(int32_t iPayload = 0; iPayload < (int32_t)payloadsToSend.size(); ++iPayload) {
				if(payloadsToRemove[iPayloadTR] == payloadsToSend[iPayload]) {
					payloadsToSend.remove_unordered(iPayload);
					--iPayload;
				}
			}
	}
	return 0;
}

::gpk::error_t												payloadQueueSend					(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSent, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& payloadsToSend) {
	::gpk::array_pod<byte_t>										messageBytes;
	sockaddr_in														sa_remote							= {};							// Information about the client
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);
	for(uint32_t iPayload = 0; iPayload < payloadsToSend.size(); ++iPayload) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					 pMessageToSend						= payloadsToSend[iPayload];
		if(0 == pMessageToSend)
			continue;
		::gpk::SUDPConnectionMessage									& messageToSend						= *pMessageToSend;
		::gpk::SUDPPayloadHeader										payloadHeader						= {};
		payloadHeader.Command										= messageToSend.Command;
		payloadHeader.Size											= messageToSend.Payload.size();
		if(messageToSend.Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE) {
			payloadHeader.MessageId										= messageToSend.Time;
			ce_if((int)sizeof(::gpk::SUDPPayloadHeader) != ::sendto(client.Socket, (const char*)&payloadHeader, (int)sizeof(::gpk::SUDPPayloadHeader), 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			gpk_necall(messageCacheSent.push_back(pMessageToSend), "%s", "Out of memory?");
		}
		else {
			ce_if(messageToSend.Payload.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "Maximum allowed payload size is only %u bytes.", ::gpk::UDP_PAYLOAD_SIZE_LIMIT);
			::gpk::view_stream<byte_t>										sendStream;
			::gpk::array_pod<byte_t>										ardellEncoded;
			payloadHeader.MessageId										= ::hashFromTime(messageToSend.Time);
			::gpk::array_pod<byte_t>										compressed;
			ree_if(errored(compressed.resize(messageToSend.Payload.size() * 2)), "Wtf: %s.", "Out of memory?");
			if(0 == payloadHeader.Command.Encrypted) {
				if(0 == payloadHeader.Command.Compressed)
					payloadHeader.Size											= messageToSend.Payload.size();
				else {
					::gpk::arrayDeflate(messageToSend.Payload, compressed);
					payloadHeader.Size											= compressed.size();
				}
			}
			else  {
				::gpk::ardellEncode(messageToSend.Payload, client.KeyPing & 0xFFFFFFFF, true, ardellEncoded);
				if(0 == payloadHeader.Command.Compressed)
					payloadHeader.Size											= ardellEncoded.size();
				else {
					::gpk::arrayDeflate(ardellEncoded, compressed);
					payloadHeader.Size											= compressed.size();
				}
			}

			gpk_necall(messageBytes.resize((uint32_t)sizeof(::gpk::SUDPPayloadHeader) + payloadHeader.Size), "%s", "Out of memory?");
			sendStream							= {messageBytes.begin(), messageBytes.size()};
			gpk_necall(sendStream.write_pod(payloadHeader), "%s", "??");

			if(0 != payloadHeader.Command.Compressed) {
				info_printf("Original packet size: %u. Compressed size: %u.", messageToSend.Payload.size(), compressed.size());
				gpk_necall(sendStream.write_pod(compressed.begin(), compressed.size()), "%s", "??");
			}
			else {
				if(0 == (payloadHeader.Command.Encrypted & 1))
					gpk_necall(sendStream.write_pod(messageToSend.Payload.begin(), messageToSend.Payload.size()), "%s", "??");
				else
					gpk_necall(sendStream.write_pod(ardellEncoded.begin(), ardellEncoded.size()), "%s", "??");
			}

			ce_if((int)sendStream.CursorPosition != ::sendto(client.Socket, sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			verbose_printf("%u bytes sent.", sendStream.CursorPosition);
			{
				gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "%s", "Out of memory?");
			}
			gpk_necall(messageCacheSent.push_back(pMessageToSend), "%s", "Out of memory?");
		}
	}
	return 0;
}

::gpk::error_t												gpk::connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSent, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSend)				{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& queueToSend						= client.Queue.Send;
	sockaddr_in														sa_remote							= {};							// Information about the client
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);

	::gpk::mutex_guard												lock								(client.Queue.MutexSend);
	for(int32_t iSent = 0; iSent < (int32_t)client.Queue.Sent.size(); ++iSent) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage> messageSent = client.Queue.Sent[iSent];
		if(0 == messageSent)
			continue;
		const uint64_t timeCurr = gpk::timeCurrentInUs();
		const uint64_t timeSent = messageSent->Time;
		const uint64_t timeDiff = timeCurr - timeSent;
		if(timeDiff > ::UDP_PAYLOAD_SENT_LIFETIME) {
			client.Queue.Sent.remove(iSent);
			--iSent;
			if(messageSent->RetryCount) {
				--messageSent->RetryCount;
				info_printf("A message sent didn't receive a confirmation on time. Retries left: %u.", (uint32_t)messageSent->RetryCount);
				if(messageSent->Time == client.KeyPing)
					messageSent->Time	= client.KeyPing	= timeCurr;
				else
					messageSent->Time	= timeCurr;
				queueToSend.push_back(messageSent);
			}
			else
				info_printf("%s", "A message sent didn't receive a confirmation on time.");
		}
	}

	// Send non-payload commands and
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	& payloadsToSend						= messageCacheSend;
	payloadsToSend.clear();
	messageCacheSent.clear();
	for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend) {
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					 pMessageToSend						= queueToSend[iSend];
		if(0 == pMessageToSend)
			continue;
		::gpk::SUDPConnectionMessage									& messageToSend						= *pMessageToSend;
//		int64_t															currentTime							= ::gpk::timeCurrentInUs();
		if(messageToSend.Command.Command == ::gpk::ENDPOINT_COMMAND_PAYLOAD)
			payloadsToSend.push_back(pMessageToSend);
		else {
			ce_if((int)sizeof(::gpk::SUDPCommand) != ::sendto(client.Socket, (const char*)&messageToSend.Command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			//pMessageToSend->Time										= currentTime;
			gpk_necall(client.Queue.Sent.push_back(pMessageToSend), "%s", "Out of memory?");
			gpk_necall(messageCacheSent.push_back(pMessageToSend), "%s", "Out of memory?");
		}
	}
	gpk_necall(::payloadQueueOptimize(client, messageCacheSent, payloadsToSend), "Failed to optimize payload queue.");
	gpk_necall(::payloadQueueSend(client, messageCacheSent, payloadsToSend), "Failed ");
	for(uint32_t iSent = 0; iSent < messageCacheSent.size(); ++iSent)
		for(uint32_t iSend = 0; iSend < queueToSend.size(); ++iSend)
			if(queueToSend[iSend] == messageCacheSent[iSent]) {
				queueToSend.remove(iSend);
				break;
			}
	return 0;
}

static	::gpk::error_t										handleRequestDISCONNECT					(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{
	ree_if(0 != command.Packed || 0 != command.Encrypted || 0 != command.Compressed || ::gpk::UDP_CONNECTION_STATE_IDLE != client.State, "Invalid command or client state.");
	client.FirstPing											=
	client.KeyPing												= 0;
	client.LastPing												= gpk::timeCurrentInUs();
	client.State												= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	client.Socket.close();
	info_printf("%s", "Disconnected!");
	return 0;
}

static	::gpk::error_t										handleRequestCONNECT						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client)												{
	ree_if(1 != command.Packed || ::gpk::UDP_CONNECTION_STATE_HANDSHAKE != client.State, "Invalid client state.");
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

static	::gpk::error_t										postConfirmationResponse			(::gpk::SUDPClientQueue & queue, const ::gpk::SUDPCommand command, const uint64_t messageId)				{
	::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					response							= {};
	response->Command											= command;
	response->Command.Type										= ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE;
	response->Time												= messageId;
	{
		::gpk::mutex_guard												lock								(queue.MutexSend);
		gpk_necall(queue.Send.push_back(response), "Out of memory? Queue size: %u.", queue.Send.size());
	}
	return 0;
}

static	::gpk::error_t										handlePAYLOAD						(::gpk::SUDPCommand& command, ::gpk::SUDPConnection& client, ::gpk::array_pod<byte_t> & receiveBuffer)		{
	if(::gpk::UDP_CONNECTION_STATE_IDLE != client.State)
		return 1;
	::gpk::SUDPPayloadHeader										header								= {};
	sockaddr_in														sa_client							= {};						// Information about the client
	socklen_t														sa_length							= (socklen_t)sizeof(sockaddr_in);	// Length of client struct
	int																bytes_received						= {};
	if errored(bytes_received = ::recvfrom(client.Socket.Handle, (char*)&header, (int)sizeof(::gpk::SUDPPayloadHeader), MSG_PEEK, (sockaddr*)&sa_client, &sa_length)) {
		rew_if(WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload header.");
	}
	uint64_t estimatedTimeSent = 0;
	if(command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST) {
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
					estimatedTimeSent											= client.FirstPing + iTime;
					info_printf("Key ping detected in %u attempts: %llu. First ping: %llu. Difference: %llu.", iTime, client.KeyPing, client.FirstPing, client.KeyPing - client.FirstPing);
					break;
				}
			reterr_error_if(0 == client.KeyPing, "Failed to determine encryption key!");
		}
		else {
			//const uint64_t																nowInUs							= ::gpk::timeCurrentInUs();
			//const uint64_t																startTime						= nowInUs - 3000000;
			//for(uint32_t iTime = 0, countLapse = 1000000; iTime < countLapse; ++iTime) {
			//	const uint64_t timeSent	 = startTime + iTime;
			//	if(::hashFromTime(timeSent) == header.MessageId) {
			//		info_printf("Payload timestamp found in %u attempts: Latency: %llu.", iTime, nowInUs - timeSent);
			//		estimatedTimeSent											= timeSent;
			//		break;
			//	}
			//}
		}
		::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					messageReceived						= {};
		messageReceived->Command									= header.Command;
		messageReceived->Time										= estimatedTimeSent; //header.MessageId;
		if(header.Size > 0) {
			if(0 != command.Compressed) {
				if(0 != command.Encrypted) {
					::gpk::array_pod<byte_t>										inflated;
					inflated.resize(65535);
					::gpk::arrayInflate({&receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size}, inflated);
					messageReceived->Payload.clear();
					::gpk::ardellDecode(inflated, client.KeyPing & 0xFFFFFFFF, true, messageReceived->Payload);
				}
				else {
					messageReceived->Payload.resize(65535);
					::gpk::arrayInflate({&receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size}, messageReceived->Payload);
				}
			}
			else {
				if(0 != command.Encrypted) {
					messageReceived->Payload.clear();
					::gpk::ardellDecode({&receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size}, client.KeyPing & 0xFFFFFFFF, true, messageReceived->Payload);
				}
				else {
					messageReceived->Payload.resize(header.Size);
					memcpy(messageReceived->Payload.begin(), &receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size);
				}
			}
		}
		client.LastPing												= ::gpk::timeCurrentInUs();
		//ree_if(header.Command.Payload > 3, "Unsupported payload format. Payload: %u.", (uint32_t)header.Command.Payload);
		if(0 == (header.Command.Packed & 1)) {
			{
				::gpk::mutex_guard												lock								(client.Queue.MutexReceive);
				gpk_necall(client.Queue.Received.push_back(messageReceived), "Out of memory? Queue size: %u.", client.Queue.Received.size());
			}
			::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					response							= {};
			::postConfirmationResponse(client.Queue, header.Command, header.MessageId);
			//info_printf("Sent ack for message id: %llu.", header.MessageId);
		}
		else {
			// 1. Read packet count
			// 2. Read packet sizes (sizeof(uint16_t) * packet count)
			// 3. Read payload contents
			::gpk::view_stream<byte_t>										payload								= {messageReceived->Payload.begin(), messageReceived->Payload.size()};
			uint16_t														packetCount							= 0;
			gpk_necall(payload.read_pod(packetCount), "Invalid data.");
			if(0 == packetCount)
				return 0;
			ree_if((packetCount * sizeof(uint16_t)) > (payload.size() - payload.CursorPosition), "Invalid packet data.");
			::gpk::view_array<const uint16_t>								sizes								= {(uint16_t*)&payload[payload.CursorPosition], packetCount};
			payload.CursorPosition										+= packetCount * sizeof(uint16_t);
			::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	tempQueue;
			for(uint32_t iSizes = 0; iSizes < sizes.size(); ++iSizes) {
				::gpk::ptr_obj<::gpk::SUDPConnectionMessage>					messageReceivedInt						= {};
				messageReceivedInt->Command									= header.Command;
				messageReceivedInt->Time									= header.MessageId;
				messageReceivedInt->Command.Packed							= 0;	// Clear payload mode bits
				messageReceivedInt->Command.Encrypted						= 0;
				messageReceivedInt->Command.Compressed						= 0;
				gpk_necall(messageReceivedInt->Payload.resize(sizes[iSizes]), "%s", "Out of memory?");
				gpk_necall(payload.read_pod(messageReceivedInt->Payload.begin(), messageReceivedInt->Payload.size()), "Corrupt payload data. Sizes don't match. Payload size: %u.", payload.size());
				gpk_necall(tempQueue.push_back(messageReceivedInt), "%s", "Out of memory?");
			}
			{
				::gpk::mutex_guard												lock								(client.Queue.MutexReceive);
				for(uint32_t iMessage = 0; iMessage < tempQueue.size(); ++iMessage)
					gpk_necall(client.Queue.Received.push_back(tempQueue[iMessage]), "Out of memory? Queue size: %u.", client.Queue.Received.size());
			}
			::postConfirmationResponse(client.Queue, header.Command, header.MessageId);
		}
	}
	else { // Payload response. Remove from sent queue.
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
	::gpk::label													labelCommand						= ::gpk::get_value_label(command.Command);
	::gpk::label													labelType							= ::gpk::get_value_label(command.Type	);
	info_printf("Command: %s. Type: %s."
		"\nCompressed   : %i."
		"\nEncrypted    : %i."
		"\nMultipart    : %i."
		"\nPacked       : %i."
		, labelCommand.begin(), labelType.begin()
		, command.Compressed ? 1 : 0
		, command.Encrypted	 ? 1 : 0
		, command.Multipart	 ? 1 : 0
		, command.Packed	 ? 1 : 0
		);
	switch(command.Type) {
	case ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST:
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
	case ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE:
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
