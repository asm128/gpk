#include "gpk_typeint.h"

#ifndef GPK_ENDPOINT_COMMAND_H_0923409238
#define GPK_ENDPOINT_COMMAND_H_0923409238

namespace gpk
{
#pragma pack(push, 1)
	enum ENDPOINT_COMMAND : uint8_t
		{ ENDPOINT_COMMAND_NOOP				= 0
		, ENDPOINT_COMMAND_DISCONNECT		= 1
		, ENDPOINT_COMMAND_CONNECT			= 2
		, ENDPOINT_COMMAND_PING				= 3
		, ENDPOINT_COMMAND_TIME				= 4
		, ENDPOINT_COMMAND_PAYLOAD			= 5
		, ENDPOINT_COMMAND_RESERVED_0		= 6
		, ENDPOINT_COMMAND_INVALID			= 7
		};

	enum ENDPOINT_MESSAGE_TYPE : uint8_t
		{ ENDPOINT_MESSAGE_TYPE_REQUEST		= 0
		, ENDPOINT_MESSAGE_TYPE_RESPONSE
		};

	struct SEndpointCommand {
		ENDPOINT_COMMAND					Command			: 3;
		uint8_t								PayloadBytes	: 4;
		ENDPOINT_MESSAGE_TYPE				Type			: 1;
	};
	//constexpr const size_t etlaskn = sizeof(SEndpointCommand);
#pragma pack(pop)
} // namespace


#endif // GPK_ENDPOINT_COMMAND_H_0923409238
