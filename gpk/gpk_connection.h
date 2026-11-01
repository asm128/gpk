#include "gpk_array_ptr.h"
#include "gpk_stdsocket.h"
#include "gpk_enum.h"

#include <functional>

#if !defined(GPK_ATMEL)
#	include <mutex>
#endif

#ifndef GPK_CONNECTION_H_23627
#define GPK_CONNECTION_H_23627

namespace gpk
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

	struct SUDPCommand {
		ENDPOINT_COMMAND					Command						: 2;
		ENDPOINT_COMMAND_TYPE				Type						: 1;
		u0_t								Packed						: 1;
		u0_t								Encrypted					: 1;
		u0_t								Compressed					: 1;
		u0_t								Multipart					: 1;
		u0_t								Resend						: 1;
	};

	struct SUDPPayloadHeader {
		::gpk::SUDPCommand					Command		;
		u2_t							Size		;
		u3_t							MessageId	;
	};
#pragma pack(pop)

	struct SUDPMessage {
		::gpk::au0_t						Payload		;
		u3_t							Time		;
		::gpk::SUDPCommand					Command		;
		u0_t								RetryCount	;
		::gpk::au3_t						Hashes		;
	};

	struct SUDPClientQueue {
		::gpk::apobj<::gpk::SUDPMessage>	Received	;
		::gpk::apobj<::gpk::SUDPMessage>	Send		;
		::gpk::apobj<::gpk::SUDPMessage>	Sent		;
		::std::mutex						MutexSend	;
		::std::mutex						MutexReceive;
	};

	GDEFINE_ENUM_TYPE (UDP_CONNECTION_STATE, u0_t)
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, DISCONNECTED				, 0);
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, HANDSHAKE					, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, IDLE						, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.

	struct SUDPConnection {
		::gpk::auto_socket_close			Socket						;
		::gpk::SUDPClientQueue				Queue						= {};
		::gpk::SIPv4End						Address						= {};	// Local for servers, remote for clients
		u3_t							LastPing					= 0;
		u3_t							FirstPing					= 0;
		u3_t							KeyPing						= 0;
		::gpk::UDP_CONNECTION_STATE			State						= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	};

	struct STCPConnection {
		::gpk::auto_socket_close			Socket						;
		::gpk::SUDPClientQueue				Queue						= {};
		::gpk::SIPv4End						Address						= {};
		u3_t							LastPing					= 0;
		u3_t							FirstPing					= 0;
		u3_t							KeyPing						= 0;
		::gpk::UDP_CONNECTION_STATE			State						= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	};

	stxp	const u2_t				UDP_PAYLOAD_SIZE_LIMIT		= 1024 * 128;

	gpk::err_t 					connectionPayloadCollect	(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & receivedMessages);
	gpk::err_t 					connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSend);
	gpk::err_t 					connectionHandleCommand		(::gpk::SUDPConnection & client, ::gpk::SUDPCommand & command, ::gpk::au0_t & receiveBuffer);
	gpk::err_t 					connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vcu0_t & data, bool bEncrypted = true, bool bCompress = true, u0_t retryCount = 0);
	stin	gpk::err_t 			connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::vcs0_t & data, bool bEncrypted = true, bool bCompress = true, u0_t retryCount = 0) {
		return ::gpk::connectionPushData(client, queue, *(const ::gpk::vcu0_t*)&data, bEncrypted, bCompress, retryCount);
	}
	stin	gpk::err_t 			connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, ::gpk::view<sc_c> & data, bool bEncrypted = true, bool bCompress = true, u0_t retryCount = 0) {
		return ::gpk::connectionPushData(client, queue, *(const ::gpk::vcu0_t*)&data, bEncrypted, bCompress, retryCount);
	}

	tplt<tpnm _tWithLoad, tpnm _tState> 
	gpk::err_t 					processUDPClientQueueEvents	
		( _tWithLoad						& eventCache
		, u2_t							iClient
		, ::gpk::vpobj<::gpk::SUDPMessage>	clientQueue
		, _tState							& state
		, const ::std::function<::gpk::err_t (u2_t iClient, _tState &, const _tWithLoad &)>	& funcProcessRequest
		) {
		for(u2_t iMessage = 0; iMessage < clientQueue.size(); ++iMessage) {
			const ::gpk::SUDPMessage				& message					= *clientQueue[iMessage];
			switch(message.Command.Command) {
			case ::gpk::ENDPOINT_COMMAND_PAYLOAD: {
				::gpk::vcu0_t							payloadInput				= {(const u0_t*)message.Payload.begin(), message.Payload.size()};
				if_fail_fe(eventCache.Load(payloadInput));
				switch(message.Command.Type) {
				case ::gpk::ENDPOINT_COMMAND_TYPE_REQUEST	: 
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
	stin	gpk::err_t 			processUDPClientQueueEvents	
		( u2_t							iClient
		, ::gpk::vcpobj<::gpk::SUDPMessage>	clientQueue
		, _tState							& state
		, const ::std::function<::gpk::err_t (u2_t iClient, _tState &, const _tWithLoad &)>	& funcProcessRequest
		) {
		_tWithLoad								eventCache					= {};
		return ::gpk::processUDPClientQueueEvents(eventCache, iClient, clientQueue, state, funcProcessRequest);
	}

	tplt<tpnm _tEvent, tpnm _tState> 
	gpk::err_t 					processUDPEvents
		( ::gpk::view<::gpk::apobj<::gpk::SUDPMessage>>	queueReceived
		, _tState										& state
		, const ::std::function<::gpk::err_t (u2_t iClient, _tState &, const _tEvent &)>	& funcProcessRequest
		) {
		_tEvent									eventReceived				= {};
		for(u2_t iClient = 0; iClient < queueReceived.size(); ++iClient) {
			 const ::gpk::apobj<::gpk::SUDPMessage>	& clientQueue				= queueReceived[iClient];
			 if_fail_fe(gpk::processUDPClientQueueEvents(eventReceived, iClient, clientQueue, state, funcProcessRequest));
		}
		return 0;
	}

	typedef	::gpk::apobj<::gpk::SUDPMessage>	TUDPQueue;
	typedef	::gpk::apobj<::gpk::au0_t>			TQueueSend;
}

#endif // GPK_CONNECTION_H_23627

