#include "gpk_ptr.h"
#include "gpk_stdsocket.h"
#include "gpk_enum.h"

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

	GDEFINE_ENUM_TYPE (UDP_CONNECTION_STATE, uint8_t)
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, DISCONNECTED				, 0);
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, HANDSHAKE					, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(UDP_CONNECTION_STATE, IDLE						, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.

#pragma pack(pop)

	struct SUDPMessage {
		::gpk::apod<byte_t>					Payload		;
		uint64_t							Time		;
		::gpk::SUDPCommand					Command		;
		uint8_t								RetryCount	;
		::gpk::apod<uint64_t>				Hashes		;
	};

	struct SUDPClientQueue {
		::gpk::apobj<::gpk::SUDPMessage>	Received	;
		::gpk::apobj<::gpk::SUDPMessage>	Send		;
		::gpk::apobj<::gpk::SUDPMessage>	Sent		;
		::std::mutex						MutexSend	;
		::std::mutex						MutexReceive;
	};

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

	static constexpr	const uint32_t	UDP_PAYLOAD_SIZE_LIMIT		= 1024 * 128;

	::gpk::error_t						connectionPayloadCollect	(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage> & receivedMessages);
	::gpk::error_t						connectionSendQueue			(::gpk::SUDPConnection & client, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSent, ::gpk::apobj<::gpk::SUDPMessage>& messageCacheSend);
	::gpk::error_t						connectionHandleCommand		(::gpk::SUDPConnection & client, ::gpk::SUDPCommand & command, ::gpk::apod<byte_t> & receiveBuffer);
	::gpk::error_t						connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::v1cb & data, bool bEncrypted = true, bool bCompress = true, uint8_t retryCount = 0);

	stainli	::gpk::error_t				connectionPushData			(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::v1cub & data, bool bEncrypted = true, bool bCompress = true, uint8_t retryCount = 0) {
		return connectionPushData(client, queue, ::gpk::v1cb{(const byte_t*)data.begin(), data.size()}, bEncrypted, bCompress, retryCount);
	}

}

#endif // GPK_CONNECTION_H_20347892908347

