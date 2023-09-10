#include "gpk_enum.h"
#include "gpk_aobj_pobj.h"

#ifndef GPK_ATMEL
#	include <mutex>
#endif

#ifndef GPK_GTL_COMMAND_H
#define GPK_GTL_COMMAND_H

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (GTL_COMMAND, uint8_t)
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Payload		, 0);	// - Payload 0: Single payload. - Payload 1: Payload combo.
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Connect		, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Disconnect	, 2);
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Upgrade		, 3);

	GDEFINE_ENUM_TYPE (GTL_COMMAND_TYPE, uint8_t)
	GDEFINE_ENUM_VALUE(GTL_COMMAND_TYPE, Request	, 0);
	GDEFINE_ENUM_VALUE(GTL_COMMAND_TYPE, Response	, 1);

	struct SGTLCommand {
		GTL_COMMAND							Command						: 2;
		GTL_COMMAND_TYPE					Type						: 1;
		uint8_t								Packed						: 1;
		uint8_t								Encrypted					: 1;
		uint8_t								Compressed					: 1;
		uint8_t								Multipart					: 1;
		uint8_t								Resend						: 1;
	};

	stacxpr	size_t	GTL_COMMAND_SIZE	= sizeof(SGTLCommand);
	struct SGTLPayloadHeader {
		::gpk::SGTLCommand					Command			;
		uint32_t							IdSender		;
		uint32_t							IdMessage		;
		::gpk::SSerializedViewHeader		CounterHeader	;
	};
#pragma pack(pop)

	struct SGTLMessage {
		::gpk::au8							Payload		;
		uint64_t							Time		;
		::gpk::SGTLCommand					Command		;
		uint8_t								RetryCount	;
	};

	struct SGTLClientQueue {
		::gpk::apobj<::gpk::SGTLMessage>	Received	;
		::gpk::apobj<::gpk::SGTLMessage>	Send		;
		::gpk::apobj<::gpk::SGTLMessage>	Sent		;
#ifndef GPK_ATMEL
		::std::mutex						MutexSend	;
		::std::mutex						MutexReceive;
#endif
	};
} // namespace

#endif // GPK_GTL_COMMAND_H
