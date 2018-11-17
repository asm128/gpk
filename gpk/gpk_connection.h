#include "gpk_array.h"
#include "gpk_ptr.h"
#include "gpk_stdsocket.h"

#ifndef GPK_CONNECTION_H_20347892908347
#define GPK_CONNECTION_H_20347892908347

namespace gpk
{
#pragma pack(push, 1)
	enum ENDPOINT_COMMAND : uint8_t
		{ ENDPOINT_COMMAND_NOOP				= 0
		, ENDPOINT_COMMAND_CONNECT			= 1	// - Payload 0: Request connect. - Payload 1: Confirm connect
		, ENDPOINT_COMMAND_PAYLOAD			= 2	// - Payload 0: Single payload. - Payload 1: Payload combo.
		, ENDPOINT_COMMAND_DISCONNECT		= 3
		};

	enum ENDPOINT_COMMAND_TYPE : uint8_t
		{ ENDPOINT_COMMAND_TYPE_REQUEST		= 0
		, ENDPOINT_COMMAND_TYPE_RESPONSE	= 1
		};

	struct SUDPCommand {
		ENDPOINT_COMMAND														Command			: 2;
		ENDPOINT_COMMAND_TYPE													Type			: 1;
		uint8_t																	Packed			: 1;
		uint8_t																	Encrypted		: 1;
		uint8_t																	Compressed		: 1;
	};

	struct SUDPPayloadHeader {
		::gpk::SUDPCommand														Command			;
		uint32_t																Size			;
		uint64_t																MessageId		;
	};

	enum UDP_CONNECTION_STATE 
		{ UDP_CONNECTION_STATE_DISCONNECTED		= 0
		, UDP_CONNECTION_STATE_HANDSHAKE
		, UDP_CONNECTION_STATE_IDLE
		};
#pragma pack(pop)

	struct SUDPConnectionMessage {
		::gpk::array_pod<byte_t>												Payload			;
		uint64_t																Time			;
		::gpk::SUDPCommand														Command			;
	};

	struct SUDPClientQueue {
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>			Received		;
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>			Send			;
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>			Sent			;
		::std::mutex															MutexSend		;
		::std::mutex															MutexReceive	;
	};

	struct SUDPConnection {
		::gpk::auto_socket_close												Socket								;
		::gpk::SUDPClientQueue													Queue								= {};
		::gpk::SIPv4															Address								= {};
		uint64_t																LastPing							= 0;
		uint64_t																FirstPing							= 0;
		uint64_t																KeyPing								= 0;
		::gpk::UDP_CONNECTION_STATE												State								= ::gpk::UDP_CONNECTION_STATE_DISCONNECTED;
	};

	static constexpr	const uint32_t										UDP_PAYLOAD_SIZE_LIMIT				= 1024*8;

	::gpk::error_t															connectionSendQueue					(::gpk::SUDPConnection & client, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSent, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>& messageCacheSend);
	::gpk::error_t															connectionHandleCommand				(::gpk::SUDPConnection & client, ::gpk::SUDPCommand & command, ::gpk::array_pod<byte_t> & receiveBuffer);
	::gpk::error_t															connectionPushData					(::gpk::SUDPConnection & client, ::gpk::SUDPClientQueue & queue, const ::gpk::view_array<const byte_t> & data, bool bEncrypted = true, bool bCompress = false);
}

#endif // GPK_CONNECTION_H_20347892908347
