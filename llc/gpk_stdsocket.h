#include "gpk_auto_handler.h"
#include "gpk_tcpip.h"

#ifndef GPK_ATMEL		

#if defined(GPK_WINDOWS)
#	include <WinSock2.h>
#else
	#include <sys/types.h>          /* See NOTES */
	#include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#ifndef GPK_STDSOCKET_H_23627
#define GPK_STDSOCKET_H_23627

#if !defined(GPK_WINDOWS)
tydf int SOCKET;
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#endif

nmsp gpk
{
	stin	err_t	safe_closesocket	(SOCKET & handle)	{
		if(handle != INVALID_SOCKET) {
#ifdef GPK_WINDOWS
			closesocket(handle);
#else
			::close(handle);
#endif
			handle = INVALID_SOCKET;
		}
		return 0;
	}
	struct auto_socket_close : public auto_handler<SOCKET, INVALID_SOCKET>					{
		usng		TWrapper						::auto_handler;
		inln										~auto_socket_close					()	nxpt	{ close(); }
		inln		void							close								()	nxpt	{ safe_closesocket(Handle); }
	};

			err_t	tcpipAddressToSockaddr		(u0_t a1, u0_t a2, u0_t a3, u0_t a4, u1_t port, sockaddr_in & sockaddr);
			err_t	tcpipAddressToSockaddr		(u2_t address, u1_t port, sockaddr_in & sockaddr);

			err_t	tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, u0_t* a1, u0_t* a2, u0_t* a3, u0_t* a4, u1_t* port);
			err_t	tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, u2_t & address, u1_t & port);
			err_t	tcpipAddress				(SOCKET socket, u0_t* a1, u0_t* a2, u0_t* a3, u0_t* a4, u1_t* port);

	stin	err_t	tcpipAddressToSockaddr		(const SIPv4End & addr, sockaddr_in & sockaddr)	{ return tcpipAddressToSockaddr		
		( byte_at(addr.IP, 0)
		, byte_at(addr.IP, 1)
		, byte_at(addr.IP, 2)
		, byte_at(addr.IP, 3)
		, addr.Port, sockaddr
		); 
	}
	stin	err_t	tcpipAddressFromSockaddr	(const sockaddr_in& sockaddr, SIPv4End & address)	{ u0_t addr[4] = {}; if_fail_fe(tcpipAddressFromSockaddr	(sockaddr	, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
	stin	err_t	tcpipAddress				(SOCKET socket, SIPv4End & address)					{ u0_t addr[4] = {}; if_fail_fe(tcpipAddress				(socket		, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
} // nmsp


#endif // GPK_STDSOCKET_H_23627

#endif // GPK_ATMEL
