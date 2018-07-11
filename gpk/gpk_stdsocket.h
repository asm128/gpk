#include "gpk_auto_handler.h"
#include "gpk_safe.h"
#include "gpk_platform.h"

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

} // namespace

#endif // GPK_STDSOCKET_H_9287349823
