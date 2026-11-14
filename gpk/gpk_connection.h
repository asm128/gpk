#include "gpk_array_ptr.h"
#include "gpk_enum.h"
#include "gpk_mutex.h"

#ifndef GPK_CONNECTION_H_23627
#define GPK_CONNECTION_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ENDPOINT_COMMAND, u0_t)
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, NOOP		, 0);
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, CONNECT	, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, PAYLOAD	, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, DISCONNECT	, 3);

	GDEFINE_ENUM_TYPE (ENDPOINT_COMMAND_TYPE, u0_t)
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND_TYPE, REQUEST	, 0);
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND_TYPE, RESPONSE	, 1);

	struct SCommand {
		ENDPOINT_COMMAND		Command						: 2;
		ENDPOINT_COMMAND_TYPE	Type						: 1;
		u0_t					Packed						: 1;
		u0_t					Encrypted					: 1;
		u0_t					Compressed					: 1;
		u0_t					Multipart					: 1;
		u0_t					Resend						: 1;
	};

	struct SPayloadHeader {
		SCommand			Command		;
		u2_t				Size		;
		u3_t				MessageId	;
	};
#pragma pack(pop)

	struct SMessage {
		au0_t				Payload		;
		u3_t				Time		;
		SCommand			Command		;
		u0_t				RetryCount	;
		au3_t				Hashes		;
	};

	struct SClientQueue {
		apobj<SMessage>		Received	;
		apobj<SMessage>		Send		;
		apobj<SMessage>		Sent		;
		mutex				MutexSend	;
		mutex				MutexReceive;
	};

	GDEFINE_ENUM_TYPE (CONNECTION_STATE, u0_t)
	GDEFINE_ENUM_VALUE(CONNECTION_STATE, DISCONNECTED				, 0);
	GDEFINE_ENUM_VALUE(CONNECTION_STATE, HANDSHAKE					, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(CONNECTION_STATE, IDLE						, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.

	struct SConnection {
		SClientQueue		Queue		= {};
		u3_t				LastPing	= 0;
		u3_t				FirstPing	= 0;
		u3_t				KeyPing		= 0;
		CONNECTION_STATE	State		= CONNECTION_STATE_DISCONNECTED;
	};

	tydf FError<au0_t &>		FConnectionReceive;
	tydf FError<const vcu0_t &>	FConnectionSend;

	stxp	const u2_t				UDP_PAYLOAD_SIZE_LIMIT		= 1024 * 128;

	err_t						connectionPayloadCollect	(SConnection & client, apobj<SMessage> & receivedMessages);
	err_t						connectionSendQueue			(SConnection & client, apobj<SMessage>& messageCacheSent, apobj<SMessage>& messageCacheSend, const FConnectionSend & send);
	err_t						connectionHandleCommand		(SConnection & client, SCommand & command, const FConnectionReceive & receive, au0_t & receiveBuffer);
	err_t						connectionPushData			(SConnection & client, SClientQueue & queue, vcu0_t data, bool bEncrypted = true, bool bCompress = true, u0_t retryCount = 0);
	stin	err_t				connectionPushData			(SConnection & client, SClientQueue & queue, vcst_t data, bool bEncrypted = true, bool bCompress = true, u0_t retryCount = 0) {
		return connectionPushData(client, queue, *(const vcu0_t*)&data, bEncrypted, bCompress, retryCount);
	}

	tplt<tpnm _tWithLoad, tpnm _tState> 
	err_t						processUDPClientQueueEvents	
		( _tWithLoad		& eventCache
		, u2_t				iClient
		, vpobj<SMessage>	clientQueue
		, _tState			& state
		, const FError<u2_t, _tState &, const _tWithLoad &>	& funcProcessRequest
		) {
		for(u2_t iMessage = 0; iMessage < clientQueue.size(); ++iMessage) {
			const SMessage				& message					= *clientQueue[iMessage];
			switch(message.Command.Command) {
			case ENDPOINT_COMMAND_PAYLOAD: {
				vcu0_t							payloadInput			= message.Payload;
				if_fail_fe(eventCache.Load(payloadInput));
				switch(message.Command.Type) {
				case ENDPOINT_COMMAND_TYPE_REQUEST	: 
					if_fail_fe(funcProcessRequest(iClient, state, eventCache)); 
					break;
				}
				break;
			}
			}
		}
		return 0;
	}

	tplt<tpnm _tWithLoad, tpnm _tState> 
	stin	err_t				processUDPClientQueueEvents	
		( u2_t							iClient
		, vcpobj<SMessage>	clientQueue
		, _tState							& state
		, const ::std::function<err_t (u2_t iClient, _tState &, const _tWithLoad &)>	& funcProcessRequest
		) {
		_tWithLoad								eventCache					= {};
		return processUDPClientQueueEvents(eventCache, iClient, clientQueue, state, funcProcessRequest);
	}

	tplt<tpnm _tEvent, tpnm _tState> 
	err_t						processEvents
		( view<apobj<SMessage>>	queueReceived
		, _tState										& state
		, const ::std::function<err_t (u2_t iClient, _tState &, const _tEvent &)>	& funcProcessRequest
		) {
		_tEvent									eventReceived				= {};
		for(u2_t iClient = 0; iClient < queueReceived.size(); ++iClient) {
			 const apobj<SMessage>	& clientQueue				= queueReceived[iClient];
			 if_fail_fe(gpk::processUDPClientQueueEvents(eventReceived, iClient, clientQueue, state, funcProcessRequest));
		}
		return 0;
	}

	tydf apobj<SMessage>	TPacketQueue;
	tydf apobj<au0_t>	TPacketQueueSend;
}

#endif // GPK_CONNECTION_H_23627
