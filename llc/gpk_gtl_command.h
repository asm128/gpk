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
	GDEFINE_ENUM_TYPE (GTL_COMMAND, u0_t)
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Payload		, 0);	// - Payload 0: Single payload. - Payload 1: Payload combo.
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Connect		, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Disconnect	, 2);
	GDEFINE_ENUM_VALUE(GTL_COMMAND, Upgrade		, 3);

	GDEFINE_ENUM_TYPE (GTL_COMMAND_TYPE, u0_t)
	GDEFINE_ENUM_VALUE(GTL_COMMAND_TYPE, Request	, 0);
	GDEFINE_ENUM_VALUE(GTL_COMMAND_TYPE, Response	, 1);

	struct SGTLCommand {
		GTL_COMMAND							Command						: 2;
		GTL_COMMAND_TYPE					Type						: 1;
		u0_t								Packed						: 1;
		u0_t								Encrypted					: 1;
		u0_t								Compressed					: 1;
		u0_t								Multipart					: 1;
		u0_t								Resend						: 1;
	};

	stxp	size_t	GTL_COMMAND_SIZE	= sizeof(SGTLCommand);
	struct SGTLPayloadHeader {
		::gpk::SGTLCommand					Command			;
		u3_t							IdSender		;
		u2_t							IdMessage		;
		::gpk::packedu1_t					CounterHeader	;
	};
#pragma pack(pop)

	struct SGTLMessage {
		::gpk::au0_t							Payload		;
		u3_t							Time		;
		::gpk::SGTLCommand					Command		;
		u0_t								RetryCount	;
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

	GDEFINE_ENUM_TYPE (GTL_STATE, u0_t)
	GDEFINE_ENUM_VALUE(GTL_STATE, Disconnected	, 0);
	GDEFINE_ENUM_VALUE(GTL_STATE, Handshake		, 1);	// - Payload 0: Request connect. - Payload 1: Confirm connect
	GDEFINE_ENUM_VALUE(GTL_STATE, Idle			, 2);	// - Payload 0: Single payload. - Payload 1: Payload combo.

	struct SGTLConnection {
		::gpk::SGTLClientQueue	Queue			= {};
		u3_t				LastPing		= 0;
		u3_t				FirstPing		= 0;
		u3_t				KeyPing			= 0;
		::gpk::GTL_STATE		State			= ::gpk::GTL_STATE_Disconnected;
	};

} // namespace

#endif // GPK_GTL_COMMAND_H
