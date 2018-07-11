#include "gpk_error.h"

#ifndef GPK_TCPIP_H_2874982374
#define GPK_TCPIP_H_2874982374

namespace gpk
{
	struct SIPv4 {
		ubyte_t											IP		[4]							;
		int16_t											Port								;
		int16_t											Adapter								;
	};

	::gpk::error_t									tcpipInitialize						();
	::gpk::error_t									tcpipShutdown						();

	enum TRANSPORT_PROTOCOL		
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP			
		};

	::gpk::error_t									tcpipAddress						(const char_t* szHostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4);
} // namespace

#endif // GPK_TCPIP_H_2874982374
