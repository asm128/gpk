#include "gpk_auto_handler.h"
#include "gpk_safe.h"
#include "gpk_platform.h"
#include "gpk_tcpip.h"

#if defined(GPK_WINDOWS)
#	include <WinSock2.h>
#endif

#ifndef GPK_STDSOCKET_H_9287349823
#define GPK_STDSOCKET_H_9287349823

namespace gpk
{
	struct auto_socket_close : public ::gpk::auto_handler<SOCKET, INVALID_SOCKET>					{ 
		using	TWrapper					::auto_handler; 
		inline								~auto_socket_close			()	noexcept	{ close(); } 
		inline	void						close						()	noexcept	{ safe_closesocket(Handle); } 
	};

	::gpk::error_t									tcpipAddress						(SOCKET socket, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port);
	static inline ::gpk::error_t					tcpipAddress						(SOCKET socket, ::gpk::SIPv4 & addr)																				{ 
		return tcpipAddress(socket, &addr.IP[0], &addr.IP[1], &addr.IP[2], &addr.IP[3], &addr.Port); 
	}
} // namespace

#endif // GPK_STDSOCKET_H_9287349823
