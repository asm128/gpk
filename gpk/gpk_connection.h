#include "gpk_ptr.h"
#include "gpk_stdsocket.h"
#include "gpk_enum.h"

#include <functional>

#if !defined(GPK_ATMEL)
#	include <mutex>
#endif

#ifndef GPK_CONNECTION_H_20347892908347
#define GPK_CONNECTION_H_20347892908347

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (ENDPOINT_COMMAND, uint8_t)
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, NOOP		, 0);
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, CONNECT	, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, PAYLOAD	, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND, DISCONNECT	, 3);

	GDEFINE_ENUM_TYPE (ENDPOINT_COMMAND_TYPE, uint8_t)
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND_TYPE, REQUEST	, 0);
	GDEFINE_ENUM_VALUE(ENDPOINT_COMMAND_TYPE, RESPONSE	, 1);

	struct SUDPCommand {
		ENDPOINT_COMMAND					Command						: 2;
		ENDPOINT_COMMAND_TYPE				Type						: 1;
		uint8_t								Packed						: 1;
		uint8_t								Encrypted					: 1;
		uint8_t								Compressed					: 1;
		uint8_t								Multipart					: 1;
		//uint8_t								Retry						: 1;
	};

	struct SUDPPayloadHeader {
		::gpk::SUDPCommand					Command		;
		uint32_t							Size		;
		uint64_t							MessageId	;
	};
#pragma pack(pop)

	struct SUDPMessage {
		::gpk::au8							Payload		;
		uint64_t							Time		;
		::gpk::SUDPCommand					Command		;
		uint8_t								RetryCount	;
		::gpk::au64							Hashes		;
	};

	struct SUDPClientQueue {
		::gpk::apobj<::gpk::SUDPMessage>	Received	;
		::gpk::apobj<::gpk::SUDPMessage>	Send		;
		::gpk::apobj<::gpk::SUDPMessage>	Sent		;
		::std::mutex						MutexSend	;
		::std::mutex						MutexReceive;
	};

	GDEFINE_ENUM_TYPE (UDP_CONNECTION_STATE, uint8_t)
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, DISCONNECTED				, 0);
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, HANDSHAKE					, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, IDLE						, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.

	struct SUDPConnection {
		::gpk::auto_socket_close			Socket						;
		::gpk::SUDPClientQueue				Queue						= {};
		::gpk::SIPv4						Address						= {};	// Local for servers, remote for clients
		uint64_t							LastPing					= 0;
		uint64_t							FirstPing					= 0;
		uint64_t							KeyPing						= 0;
		::gpk::UDP_CONNECTION_STATE			State						= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	};

	struct STCPConnection {
		::gpk::auto_socket_close			Socket						;
		::gpk::SUDPClientQueue				Queue						= {};
		::gpk::SIPv4						Address						= {};
		uint64_t							LastPing					= 0;
		uint64_t							FirstPing					= 0;
		uint64_t							KeyPing						= 0;
		::gpk::UDP_CONNECTION_STATE			State						= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	};

	stacxpr	const uint32_t				UDP_PAYLOAD_SIZE_LIMIT		= 1024 * 128;

	::gpk::error_t						connectionPayloadCollect	(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & receivedMessages);
	::gpk::error_t						connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSend);
	::gpk::error_t						connectionHandleCommand		(::gpk::SUDPConnection & client, ::gpk::SUDPCommand & command, ::gpk::au8 & receiveBuffer);
	::gpk::error_t						connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vcu8 & data, bool bEncrypted = true, bool bCompress = true, uint8_t retryCount = 0);
	stainli	::gpk::error_t				connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vci8 & data, bool bEncrypted = true, bool bCompress = true, uint8_t retryCount = 0) {
		return ::gpk::connectionPushData(client, queue, *(const ::gpk::vcu8*)&data, bEncrypted, bCompress, retryCount);
	}
	stainli	::gpk::error_t				connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vcc & data, bool bEncrypted = true, bool bCompress = true, uint8_t retryCount = 0) {
		return ::gpk::connectionPushData(client, queue, *(const ::gpk::vcu8*)&data, bEncrypted, bCompress, retryCount);
	}
	template<typename _tWithLoad, typename _tState> 
	::gpk::error_t						processUDPClientQueueEvents	
		( _tWithLoad						& eventCache
		, uint32_t							iClient
		, ::gpk::vcpobj<::gpk::SUDPMessage>	clientQueue
		, _tState							& state
		, const ::std::function<::gpk::error_t (uint32_t iClient, _tState &, const _tWithLoad &)>	& funcProcessRequest
		) {
		for(uint32_t iMessage = 0; iMessage < clientQueue.size(); ++iMessage) {
			const ::gpk::SUDPMessage				& message					= *clientQueue[iMessage];
			switch(message.Command.Command) {
			case ::gpk::ENDPOINT_COMMAND_PAYLOAD: {
				::gpk::vcu8								payloadInput				= {(const uint8_t*)message.Payload.begin(), message.Payload.size()};
				gpk_necs(eventCache.Load(payloadInput));
				switch(message.Command.Type) {
				case ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST	: 
					gpk_necs(funcProcessRequest(iClient, state, eventCache)); 
					break;
				}
				break;
			}
			}
		}
		return 0;
	}

	template<typename _tWithLoad, typename _tState> 
	stainli	::gpk::error_t				processUDPClientQueueEvents	
		( uint32_t							iClient
		, ::gpk::vcpobj<::gpk::SUDPMessage>	clientQueue
		, _tState							& state
		, const ::std::function<::gpk::error_t (uint32_t iClient, _tState &, const _tWithLoad &)>	& funcProcessRequest
		) {
		_tWithLoad								eventCache					= {};
		return ::gpk::processUDPClientQueueEvents(eventCache, iClient, clientQueue, state, funcProcessRequest);
	}

	template<typename _tEvent, typename _tState> 
	::gpk::error_t						processUDPEvents
		( ::gpk::view<const ::gpk::apobj<::gpk::SUDPMessage>>	queueReceived
		, _tState												& state
		, const ::std::function<::gpk::error_t (uint32_t iClient, _tState &, const _tEvent &)>	& funcProcessRequest
		) {
		_tEvent									eventReceived				= {};
		for(uint32_t iClient = 0; iClient < queueReceived.size(); ++iClient) {
			 const ::gpk::apobj<::gpk::SUDPMessage>	& clientQueue				= queueReceived[iClient];
			 gpk_necs(::gpk::processUDPClientQueueEvents(eventReceived, iClient, clientQueue, state, funcProcessRequest));
		}
		return 0;
	}

}

#endif // GPK_CONNECTION_H_20347892908347

