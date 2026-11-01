#include "gpk_connection.h"
#include "gpk_chrono.h"
#include "gpk_view_stream.h"
#include "gpk_noise.h"
#include "gpk_encoding.h"
#include "gpk_deflate.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_ASTATIC();
GPK_USING_APOD();

gpk::err_t 		gpk::connectionPushData				(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vcu0_t & data, bool bEncrypt, bool bCompress, u0_t retryCount) {
	if_true_vef(-1, data.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "%s", "Invalid payload size.");

	::gpk::pobj<::gpk::SUDPMessage>	message;
	message->Command.Command	= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
	message->Command.Type		= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST;
	message->Command.Encrypted	= (u0_t)(bEncrypt ? 1 : 0);
	message->Command.Compressed	= (u0_t)(bCompress ? 1 : 0);
	message->Command.Packed		= 0;
	message->Command.Multipart	= 0;
	message->Time				= ::gpk::timeCurrentInUs();
	message->RetryCount			= retryCount;
	if(0 == client.KeyPing) {
		client.KeyPing				= message->Time;
		info_printf("Key ping set: %llu.", client.KeyPing);
	}
	//message->Payload.clear();
	message->Payload					= data;
	{
		::std::lock_guard			lock								(queue.MutexSend);
		if_fail_fe(queue.Send.push_back(message));
	}
	return 0;
}

gpk::err_t 		gpk::connectionPayloadCollect		(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & receivedMessages)	{
	if(0 == client.KeyPing || client.State == ::gpk::UDP_CONNECTION_STATE_HANDSHAKE)
		return 1;

	::std::lock_guard			lockRecv							(client.Queue.MutexReceive);
	for(s2_t iMessage = 0; iMessage < (s2_t)client.Queue.Received.size(); ++iMessage) {
		const ::gpk::pobj<::gpk::SUDPMessage>	& messageReceived		= client.Queue.Received[iMessage];
		const ::gpk::SUDPCommand				udpCommand				= messageReceived->Command;
		if( udpCommand.Type		!= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST 
		 || udpCommand.Command	!= ::gpk::ENDPOINT_COMMAND_PAYLOAD
		) 
			continue;
		if_fail_fe(receivedMessages.push_back(messageReceived));
		if_fail_fe(client.Queue.Received.remove_unordered(iMessage--));
	}
	return 0;
}

stxp	u3_t		hashFromTime						(u3_t currentTime)		noexcept	{ return ::gpk::noise1DBase(currentTime & 0xFFFFFFFF) + (currentTime >> 16); }
stxp	u2_t		UDP_PAYLOAD_SENT_LIFETIME			= 1000000; // microseconds

gpk::err_t 		payloadQueueOptimize				(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage> & payloadsToSend) {
	if(payloadsToSend.size() < 2)
		return 0;

	{
		::gpk::apobj<::gpk::SUDPMessage>	optimizedPayloads		= {};
		::gpk::apobj<::gpk::SUDPMessage>	payloadsToRemove		= {};
		::gpk::au1_t						payloadSizes			= {};
		::gpk::au0_t						serialized				= {};

		::gpk::pobj<::gpk::SUDPMessage>		optimizedMsg			= {};
		optimizedMsg->Time				= ::gpk::timeCurrentInUs();
		optimizedMsg->Command			= {};
		optimizedMsg->Command.Command	= ::gpk::ENDPOINT_COMMAND_PAYLOAD;
		optimizedMsg->Command.Type		= ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST;
		optimizedMsg->Command.Packed	= 1;
		optimizedMsg->RetryCount		= 1;

		bool						keyPing								= false;
		for(u2_t iPayload = 0; iPayload < payloadsToSend.size(); ++iPayload) {
			::gpk::pobj<::gpk::SUDPMessage>	currentPayloadMsg				= payloadsToSend[iPayload];
			const ::gpk::vcu0_t				currentPayloadView				= currentPayloadMsg->Payload;
			if(0 == currentPayloadMsg.get_ref())
				continue;
			if((currentPayloadMsg->Command.Packed & 1) || currentPayloadMsg->Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE || currentPayloadMsg->Command.Command != ::gpk::ENDPOINT_COMMAND_PAYLOAD)
				continue;
			const bool						sizeEnough						= (serialized.size() + sizeof(u1_t) + currentPayloadMsg->Payload.size()) < ::gpk::UDP_PAYLOAD_SIZE_LIMIT;
			if(false == sizeEnough)
				continue;
			if(0 == currentPayloadView.size())
				continue;
			if(currentPayloadMsg->Time == client.KeyPing)
				keyPing						= true;
			if_fail_fe(payloadSizes.push_back((u1_t)currentPayloadView.size()));
			if_fail_fe(serialized.append(currentPayloadView));
			if_fail_fe(payloadsToRemove.push_back(currentPayloadMsg));
			if_fail_fe(messageCacheSent.push_back(currentPayloadMsg));
		}
		if_true_vif(0, 0 == payloadSizes.size(), "No packets to optimize. Total packets searched: %u.", payloadsToSend.size());
		info_printf("%u packets optimized.", payloadSizes.size());
		const u2_t				sizeRequired						= serialized.size() + sizeof(u1_t) * payloadSizes.size() + sizeof(u1_t);	// payloads + sizes + packet count
		u1_t					packetCount							= (u1_t)payloadSizes.size();
		::gpk::au0_t				& optimizedPayloadBuffer			= optimizedMsg->Payload;
		if_fail_fef(optimizedPayloadBuffer.resize(sizeRequired), "Out of memory? Size required: %u.", sizeRequired);
		optimizedPayloadBuffer.clear();
		optimizedPayloadBuffer.append((const u0_t*)&packetCount, sizeof(u1_t));
		optimizedPayloadBuffer.append((const u0_t*)payloadSizes.begin(), sizeof(u1_t) * payloadSizes.size());
		optimizedPayloadBuffer.append(serialized.begin(), serialized.size());
		if_true_vef(-1, sizeRequired != optimizedPayloadBuffer.size(), "Invalid sizes! Size required: %u. Actual size: %u.", sizeRequired, optimizedPayloadBuffer.size());
		optimizedMsg->Command.Compressed	= 1;
		optimizedMsg->Command.Encrypted		= 1;
		if_fail_fef(payloadsToSend.push_back(optimizedMsg), "Failed! Payload queue size: %u.", payloadsToSend.size());

		if(keyPing)
			client.KeyPing						= optimizedMsg->Time;

		for(s2_t iPayloadTR = 0; iPayloadTR < (s2_t)payloadsToRemove.size(); ++iPayloadTR)	// Done. Remove packed payloads from send queue.
			for(s2_t iPayload = 0; iPayload < (s2_t)payloadsToSend.size(); ++iPayload) {
				if(payloadsToRemove[iPayloadTR] == payloadsToSend[iPayload]) {
					payloadsToSend.remove_unordered(iPayload);
					--iPayload;
				}
			}
	}
	return 0;
}

gpk::err_t 		payloadQueueSend					(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage> & payloadsToSend) {
	::gpk::au0_t				messageBytes;
	sockaddr_in					sa_remote							= {};							// Information about the client
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);
	for(u2_t iPayload = 0; iPayload < payloadsToSend.size(); ++iPayload) {
		::gpk::pobj<::gpk::SUDPMessage>	pMessageToSend						= payloadsToSend[iPayload];
		if(0 == pMessageToSend.get_ref())
			continue;
		::gpk::SUDPMessage				& messageToSend						= *pMessageToSend;
		::gpk::SUDPPayloadHeader		payloadHeader						= {};
		payloadHeader.Command				= messageToSend.Command;
		payloadHeader.Size					= messageToSend.Payload.size();
		if(messageToSend.Command.Type == ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE) {
			payloadHeader.MessageId				= messageToSend.Time;
			if_true_cef((int)sizeof(::gpk::SUDPPayloadHeader) != ::sendto(client.Socket, (sc_c*)&payloadHeader, (int)sizeof(::gpk::SUDPPayloadHeader), 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			if_fail_fe(messageCacheSent.push_back(pMessageToSend));
			info_printf("Sent response to message id: %llx", payloadHeader.MessageId);
		}
		else {
			if_true_cef(messageToSend.Payload.size() > ::gpk::UDP_PAYLOAD_SIZE_LIMIT, "Maximum allowed payload size is only %u bytes.", ::gpk::UDP_PAYLOAD_SIZE_LIMIT);
			::gpk::view_stream<u0_t>				sendStream;
			::gpk::au0_t					ardellEncoded;
			payloadHeader.MessageId				= ::hashFromTime(messageToSend.Time);
			messageToSend.Hashes.push_back(payloadHeader.MessageId);
			info_printf("Sending message with id: %llx.", payloadHeader.MessageId);
			::gpk::au0_t					compressed;
			if(0 == payloadHeader.Command.Compressed) {
				if(0 == payloadHeader.Command.Encrypted)
					payloadHeader.Size				= messageToSend.Payload.size();
				else {
					::gpk::ardellEncode(messageToSend.Payload, client.KeyPing & 0xFFFFFFFF, true, ardellEncoded);
					payloadHeader.Size				= ardellEncoded.size();
				}
			}
			else {
				::gpk::arrayDeflate(messageToSend.Payload, compressed);
				if(0 == payloadHeader.Command.Encrypted)
					payloadHeader.Size				= compressed.size();
				else {
					::gpk::ardellEncode(compressed, client.KeyPing & 0xFFFFFFFF, true, ardellEncoded);
					payloadHeader.Size				= ardellEncoded.size();
				}
			}

			if_fail_fe(messageBytes.resize((u2_t)sizeof(::gpk::SUDPPayloadHeader) + payloadHeader.Size));
			sendStream						= {messageBytes.begin(), messageBytes.size()};
			if_fail_fe(sendStream.write_pod(payloadHeader));

			if(0 != payloadHeader.Command.Encrypted) {
				info_printf("Original packet size: %u. Final size: %u.", messageToSend.Payload.size(), ardellEncoded.size());
				if_fail_fe(sendStream.write_pod(ardellEncoded));
			}
			else {
				if(0 == payloadHeader.Command.Compressed) {
					if_fail_fe(sendStream.write_pod(messageToSend.Payload));
				}
				else {
					info_printf("Original packet size: %u. Compressed size: %u.", messageToSend.Payload.size(), compressed.size());
					if_fail_fe(sendStream.write_pod(compressed));
				}
			}

			if_true_cef((int)sendStream.CursorPosition != ::sendto(client.Socket, (const sc_t*)sendStream.begin(), (int)sendStream.CursorPosition, 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			verbose_printf("%u bytes sent.", sendStream.CursorPosition);
			{
				if_fail_fe(client.Queue.Sent.push_back(pMessageToSend));
			}
			if_fail_fe(messageCacheSent.push_back(pMessageToSend));
		}
	}
	return 0;
}

gpk::err_t 		gpk::connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage> & messageCacheSend)				{
	::gpk::apobj<::gpk::SUDPMessage>	& queueToSend						= client.Queue.Send;
	sockaddr_in							sa_remote							= {};							// Information about the client
	::gpk::tcpipAddressToSockaddr(client.Address, sa_remote);

	::std::lock_guard					lock								(client.Queue.MutexSend);
	for(s2_t iSent = 0; iSent < (s2_t)client.Queue.Sent.size(); ++iSent) {
		::gpk::pobj<::gpk::SUDPMessage> messageSent = client.Queue.Sent[iSent];
		if(0 == messageSent.get_ref())
			continue;
		u3_t timeCurr = gpk::timeCurrentInUs();
		const u3_t timeSent = messageSent->Time;
		const u3_t timeDiff = timeCurr - timeSent;
		if(timeDiff > ::UDP_PAYLOAD_SENT_LIFETIME) {
			client.Queue.Sent.remove_unordered(iSent);
			--iSent;
			if(messageSent->RetryCount) {
				--messageSent->RetryCount;
				timeCurr	+= ::UDP_PAYLOAD_SENT_LIFETIME;
				info_printf("A message sent didn't receive a confirmation on time: %llx (%llx). Setting current time: %llx, Retries left: %u.", messageSent->Time, ::hashFromTime(messageSent->Time), timeCurr, (u2_t)messageSent->RetryCount);
				if(messageSent->Time == client.KeyPing)
					messageSent->Time	= client.KeyPing	= timeCurr;
				else
					messageSent->Time	= timeCurr;
				queueToSend.push_back(messageSent);
			}
			else {
				info_printf("A message sent didn't receive a confirmation on time: %llx.", messageSent->Time);
			}
		}
	}

	// Send non-payload commands and
	::gpk::apobj<::gpk::SUDPMessage>					& payloadsToSend						= messageCacheSend;
	payloadsToSend.clear();
	messageCacheSent.clear();
	for(u2_t iSend = 0; iSend < queueToSend.size(); ++iSend) {
		::gpk::pobj<::gpk::SUDPMessage>					 pMessageToSend						= queueToSend[iSend];
		if(0 == pMessageToSend.get_ref())
			continue;
		::gpk::SUDPMessage									& messageToSend						= *pMessageToSend;
//		s3_t															currentTime							= ::gpk::timeCurrentInUs();
		if(messageToSend.Command.Command == ::gpk::ENDPOINT_COMMAND_PAYLOAD)
			payloadsToSend.push_back(pMessageToSend);
		else {
			if_true_cef((int)sizeof(::gpk::SUDPCommand) != ::sendto(client.Socket, (const sc_t*)&messageToSend.Command, (int)sizeof(::gpk::SUDPCommand), 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in)), "%s", "Error sending datagram.");	// Send data back
			//pMessageToSend->Time										= currentTime;
			if_fail_fe(client.Queue.Sent.push_back(pMessageToSend));
			if_fail_fe(messageCacheSent.push_back(pMessageToSend));
		}
	}
	if_fail_fef(::payloadQueueOptimize(client, messageCacheSent, payloadsToSend), "%s", "Failed to optimize payload queue.");
	if_fail_fef(::payloadQueueSend(client, messageCacheSent, payloadsToSend), "%s", "Failed ");
	for(u2_t iSent = 0; iSent < messageCacheSent.size(); ++iSent)
		for(u2_t iSend = 0; iSend < queueToSend.size(); ++iSend)
			if(queueToSend[iSend] == messageCacheSent[iSent]) {
				queueToSend.remove(iSend);
				break;
			}
	return 0;
}

static	gpk::err_t handleRequestDISCONNECT					(::gpk::SUDPCommand & command, ::gpk::SUDPConnection & client)												{
	if_true_vef(-1, 0 != command.Packed || 0 != command.Encrypted || 0 != command.Compressed || ::gpk::UDP_CONNECTION_STATE_IDLE != client.State, "Invalid command or client state: 0x%X '%s'.", client.State, ::gpk::get_value_label(client.State).begin());
	client.FirstPing				=
	client.KeyPing					= 0;
	client.LastPing					= gpk::timeCurrentInUs();
	client.State					= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	client.Socket.close();
	info_printf("%s", "Disconnected!");
	return 0;
}

static	gpk::err_t handleRequestCONNECT						(::gpk::SUDPCommand & command, ::gpk::SUDPConnection & client)												{
	if_true_vef(-1, 1 != command.Packed || ::gpk::UDP_CONNECTION_STATE_HANDSHAKE != client.State, "Invalid client state: 0x%X '%s'.", client.State, ::gpk::get_value_label(client.State).begin());
	client.LastPing					= gpk::timeCurrentInUs();
	{
		::std::lock_guard					lock								(client.Queue.MutexSend);
		for(u2_t iSent = 0, countSent = client.Queue.Sent.size(); iSent < countSent; ++iSent)
			if(::gpk::ENDPOINT_COMMAND_CONNECT == client.Queue.Sent[iSent]->Command.Command) {
				client.Queue.Sent.remove_unordered(iSent);
				break;
			}
	}

	client.State					= ::gpk::UDP_CONNECTION_STATE_IDLE;
	info_printf("%s", "Connected!");
	return 0;
}

#if defined(GPK_WINDOWS)
typedef int socklen_t;
#endif

static	gpk::err_t postConfirmationResponse			(::gpk::SUDPClientQueue & queue, const ::gpk::SUDPCommand command, const u3_t messageId)				{
	::gpk::pobj<::gpk::SUDPMessage>		response							= {};
	response->Command				= command;
	response->Command.Type			= ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE;
	response->Time					= messageId;
	{
		::std::lock_guard					lock								(queue.MutexSend);
		if_fail_fef(queue.Send.push_back(response), "Out of memory? Queue size: %u.", queue.Send.size());
	}
	return 0;
}

static	gpk::err_t handlePAYLOADResponse				(const ::gpk::SUDPPayloadHeader & header, ::gpk::SUDPConnection & client)		{
	// Remove from sent queue.
	::std::lock_guard					lock								(client.Queue.MutexSend);
	info_printf("Received response to message id: %llx", header.MessageId);
	bool								bFound								= false;
	for(u2_t iSent = 0; iSent < client.Queue.Sent.size(); ++iSent) {
		::gpk::SUDPMessage					& messageSent						= *client.Queue.Sent[iSent];
		for(u2_t iHash = 0; iHash < messageSent.Hashes.size(); ++iHash) {
			info_printf("Checking hash for message: %llx.", messageSent.Hashes[iHash]);
			if(::hashFromTime(messageSent.Time) == header.MessageId) {
				info_printf("Removing message from confirmation queue: %llx. ", header.MessageId);
				client.Queue.Sent.remove_unordered(iSent);
				client.LastPing					= ::gpk::timeCurrentInUs();
				bFound							= true;
				break;
			}
			u3_t							hashLocal							= messageSent.Hashes[iHash]; //::hashFromTime(messageSent.Time);
			if(hashLocal == header.MessageId) {
				info_printf("Removing message from confirmation queue: %llx. ", header.MessageId);
				client.Queue.Sent.remove_unordered(iSent);
				client.LastPing					= ::gpk::timeCurrentInUs();
				bFound							= true;
				break;
			}
		}
		if(bFound)
			break;
	}
	if(false == bFound)
		info_printf("Response confirmation for message was not found: %llx.", header.MessageId);

	return 0;
}

stxp	s3_t			advantage							= 1000000;

static	gpk::err_t handlePAYLOADRequest				(const ::gpk::SUDPPayloadHeader & header, ::gpk::SUDPCommand & command, ::gpk::SUDPConnection & client, ::gpk::au0_t & receiveBuffer)		{
	sockaddr_in					sa_client							= {};						// Information about the client
	socklen_t					sa_length							= (socklen_t)sizeof(sockaddr_in);	// Length of client struct
	int							bytes_received						= {};
	if_fail_fe(receiveBuffer.resize(sizeof(::gpk::SUDPPayloadHeader) + header.Size));
	memset(receiveBuffer.begin(), 0, receiveBuffer.size());
	if(::gpk::failed(bytes_received = ::recvfrom(client.Socket, (sc_t*)receiveBuffer.begin(), (int)receiveBuffer.size(), MSG_PEEK, (sockaddr*)&sa_client, &sa_length))) {
#if defined(GPK_WINDOWS)
		if_true_vwf(-1, WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload data.")
		else
			warning_printf("%s", "Failed to receive all of the payload data.");
#else
		error_printf("%s", "Failed to receive all of the payload data.");
		return -1;
#endif
	}
	else
		if_true_vif(0, 0 == bytes_received, "The peer has performed an orderly shutdown: '%s'", "https://linux.die.net/man/3/recvfrom");

	if_true_vef(-1, receiveBuffer.size() != (u2_t)bytes_received, "Packet size received doesn't match with header size. Received size: %u. Expected: %u.", bytes_received, receiveBuffer.size());

	u3_t							estimatedTimeSent					= 0;
	if(client.KeyPing == 0) {
		for(s3_t iTime = 0, countLapse = 60000000; iTime < countLapse; ++iTime)
			if(::hashFromTime(client.FirstPing - advantage + iTime) == header.MessageId) {
				client.KeyPing					= client.FirstPing - advantage + iTime;
				estimatedTimeSent				= client.FirstPing - advantage + iTime;
				info_printf("Key ping detected in %u attempts: %llu. First ping: %llu. Difference: %llu.", iTime, client.KeyPing, client.FirstPing, client.KeyPing - client.FirstPing);
				break;
			}
		if_true_vef(-1, 0 == client.KeyPing, "%s", "Failed to determine encryption key!");
	}
	else { // Why the fuck is this commented???
		//const u3_t					nowInUs							= ::gpk::timeCurrentInUs();
		//const u3_t					startTime						= nowInUs - 3000000;
		//for(u2_t iTime = 0, countLapse = 1000000; iTime < countLapse; ++iTime) {
		//	const u3_t						timeSent						= startTime + iTime;
		//	if(::hashFromTime(timeSent) == header.MessageId) {
		//		info_printf("Payload timestamp found in %u attempts: Latency: %llu.", iTime, nowInUs - timeSent);
		//		estimatedTimeSent				= timeSent;
		//		break;
		//	}
		//}
	}
	::gpk::pobj<::gpk::SUDPMessage>		messageReceived						= {};
	messageReceived->Command		= header.Command;
	messageReceived->Time			= estimatedTimeSent; //header.MessageId;
	if(header.Size > 0) {
		const ::gpk::vcu0_t					viewPayload							= {&receiveBuffer[sizeof(::gpk::SUDPPayloadHeader)], header.Size};
		if(0 != command.Compressed) {
			if(0 != command.Encrypted) {
				::gpk::au0_t				inflated;
				::gpk::au0_t				decoded;
				if_true_vwf(-1, ::gpk::failed(::gpk::ardellDecode(viewPayload, client.KeyPing & 0xFFFFFFFF, true, decoded)), "%s", "Failed to decrypt packet!");
				if_true_vwf(-1, ::gpk::failed(::gpk::arrayInflate(decoded, messageReceived->Payload)), "%s", "Failed to inflate packet!");
			}
			else {
				if_true_vwf(-1, ::gpk::failed(::gpk::arrayInflate(viewPayload, messageReceived->Payload)), "%s", "Failed to inflate packet!");
			}
		}
		else {
			if(0 != command.Encrypted) {
				messageReceived->Payload.clear();
				if_true_vwf(-1, ::gpk::failed(::gpk::ardellDecode(viewPayload, client.KeyPing & 0xFFFFFFFF, true, messageReceived->Payload)), "%s", "Failed to decrypt packet!");
			}
			else {
				messageReceived->Payload.resize(header.Size);
				memcpy(messageReceived->Payload.begin(), viewPayload.begin(), header.Size);
				{
					messageReceived->Payload.resize(header.Size + 1, 0);
					messageReceived->Payload.resize(header.Size);
				}
			}
		}
	}
	info_printf("Packet received. Payload size received: %u. Payload size after unpacking/decrypting: %u.", header.Size, messageReceived->Payload.size());
	client.LastPing					= ::gpk::timeCurrentInUs();
	//if_true_vef(-1, header.Command.Payload > 3, "Unsupported payload format. Payload: %u.", (u2_t)header.Command.Payload);
	if(0 == (header.Command.Packed & 1)) {
		{
			::std::lock_guard					lock								(client.Queue.MutexReceive);
			if_fail_fef(client.Queue.Received.push_back(messageReceived), "Out of memory? Queue size: %u.", client.Queue.Received.size());
		}
		::gpk::pobj<::gpk::SUDPMessage>		response							= {};
		::postConfirmationResponse(client.Queue, header.Command, header.MessageId);
		//info_printf("Sent ack for message id: %llu.", header.MessageId);
	}
	else {
		// 1. Read packet count
		// 2. Read packet sizes (sizeof(u1_t) * packet count)
		// 3. Read payload contents
		::gpk::view_stream<u0_t>			payload								= {messageReceived->Payload.begin(), messageReceived->Payload.size()};
		u1_t							packetCount							= 0;
		if_fail_fef(payload.read_pod(packetCount), "%s", "Invalid data.");
		if(0 == packetCount)
			return 0;
		if_true_vef(-1, (packetCount * sizeof(u1_t)) > (payload.size() - payload.CursorPosition), "%s", "Invalid packet data.");
		::gpk::vcu1_t						sizes								= {(u1_t*)&payload[payload.CursorPosition], packetCount};
		payload.CursorPosition			+= packetCount * sizeof(u1_t);
		::gpk::apobj<::gpk::SUDPMessage>	tempQueue;
		for(u2_t iSizes = 0; iSizes < sizes.size(); ++iSizes) {
			::gpk::pobj<::gpk::SUDPMessage>	messageReceivedInt						= {};
			messageReceivedInt->Command				= header.Command;
			messageReceivedInt->Time				= header.MessageId;
			messageReceivedInt->Command.Packed		= 0;	// Clear payload mode bits
			messageReceivedInt->Command.Encrypted	= 0;
			messageReceivedInt->Command.Compressed	= 0;
			if_fail_fe(messageReceivedInt->Payload.resize(sizes[iSizes]));
			if_fail_fef(payload.read_pod(messageReceivedInt->Payload), "Corrupt payload data. Sizes don't match. Payload size: %u.", payload.size());
			if_fail_fe(tempQueue.push_back(messageReceivedInt));
		}
		{
			::std::lock_guard					lock								(client.Queue.MutexReceive);
			for(u2_t iMessage = 0; iMessage < tempQueue.size(); ++iMessage)
				if_fail_fef(client.Queue.Received.push_back(tempQueue[iMessage]), "Out of memory? Queue size: %u.", client.Queue.Received.size());
		}
		::postConfirmationResponse(client.Queue, header.Command, header.MessageId);
	}
	return 0;
}

static	gpk::err_t handlePAYLOAD						(::gpk::SUDPCommand & command, ::gpk::SUDPConnection & client, ::gpk::au0_t & receiveBuffer)		{
	if(::gpk::UDP_CONNECTION_STATE_IDLE != client.State)
		return 1;
	::gpk::SUDPPayloadHeader			header								= {};
	sockaddr_in							sa_client							= {};						// Information about the client
	socklen_t							sa_length							= (socklen_t)sizeof(sockaddr_in);	// Length of client struct
	int									bytes_received						= {};
	if(::gpk::failed(bytes_received = ::recvfrom(client.Socket.Handle, (sc_t*)&header, (int)sizeof(::gpk::SUDPPayloadHeader), MSG_PEEK, (sockaddr*)&sa_client, &sa_length))) {
#if defined(GPK_WINDOWS)
		if_true_vwf(-1, WSAGetLastError() != WSAEMSGSIZE, "%s", "Could not receive payload header.");
#else
#endif
	}
	switch(command.Type) {
	default										: return -1;
	case ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST	: return ::handlePAYLOADRequest (header, command, client, receiveBuffer); 
	case ::gpk::ENDPOINT_COMMAND_TYPE_RESPONSE	: return ::handlePAYLOADResponse(header, client); 
	}
}

gpk::err_t 		gpk::connectionHandleCommand		(::gpk::SUDPConnection & client, ::gpk::SUDPCommand & command, ::gpk::au0_t & receiveBuffer)		{
	::gpk::vcst_t				labelCommand						= ::gpk::get_value_label(command.Command);
	::gpk::vcst_t				labelType							= ::gpk::get_value_label(command.Type	);
	(void)labelCommand	;
	(void)labelType		;
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
		case ::gpk::ENDPOINT_COMMAND_PAYLOAD	: return ::handlePAYLOAD(command, client, receiveBuffer);
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
