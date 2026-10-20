#include "gpk_tcpip.h"
#include "gpk_parse.h"
#include "gpk_json.h"
#include "gpk_cstring.h"
#ifdef GPK_WINDOWS
#	include "gpk_windows.h"
#endif
#ifndef GPK_ESP8266
#	include "gpk_stdsocket.h"
#endif

GPK_USING_TYPEINT();

::gpk::error_t			gpk::tcpipAddress		(::gpk::vcst_t strIP, ::gpk::u2_t & address, ::gpk::u1_t & port) {
	::gpk::u2_t					iOffset					= ::gpk::tcpipAddress(strIP, address);
	return (iOffset < strIP.size())
		? iOffset + ::gpk::parseIntegerDecimal({&strIP[iOffset], strIP.size() - iOffset}, port)
		: iOffset
		;
}

::gpk::error_t			jsonTcpipAddress		(gpk::vcst_t strIP, ::gpk::u2_t & ipv4) {
	::gpk::SJSONReader			reader					= {};
	if_fail_fef(gpk::jsonParse(reader, strIP), "Failed to parse ip from string: '%.*s'.", (int)strIP.size(), strIP.begin());
	ipv4					= 0;
	{ int32_t iToken; gpk_necs(iToken = ::gpk::jsonArrayValueGet(reader, 0, 0)); ipv4 |= ::gpk::byte_to<::gpk::u2_t>((u0_t)reader[iToken]->Token->Value, 0); }
	{ int32_t iToken; gpk_necs(iToken = ::gpk::jsonArrayValueGet(reader, 0, 1)); ipv4 |= ::gpk::byte_to<::gpk::u2_t>((u0_t)reader[iToken]->Token->Value, 1); }
	{ int32_t iToken; gpk_necs(iToken = ::gpk::jsonArrayValueGet(reader, 0, 2)); ipv4 |= ::gpk::byte_to<::gpk::u2_t>((u0_t)reader[iToken]->Token->Value, 2); }
	{ int32_t iToken; gpk_necs(iToken = ::gpk::jsonArrayValueGet(reader, 0, 3)); ipv4 |= ::gpk::byte_to<::gpk::u2_t>((u0_t)reader[iToken]->Token->Value, 3); }
	return 0;
}

::gpk::error_t			gpk::tcpipAddress		(gpk::vcst_t strIP, ::gpk::u2_t & ipv4)	{
	ipv4					= 0;
	::gpk::ltrim(strIP);
	if(0 == strIP.size()) 
		return 0;

	::gpk::u2_t					iOffset					= 0;
	::gpk::u2_t					iEnd					= 0;
	for(u0_t iVal = 0; iVal < 4 && iOffset < strIP.size(); ++iVal) {
		while(iEnd < strIP.size()) {
			char curChar = strIP[iEnd];
			if(0 == iVal && curChar == '[') 
				return jsonTcpipAddress(strIP, ipv4);

			if( curChar == ','
			 || curChar == '.'
			 || curChar == ':'
			 || curChar == '\0'
			 || (iEnd - iOffset) > 3	// 3 digit max
			)
				break;
			++iEnd;
		}
		u0_t			valueRead	= 0;
		gpk_necs(gpk::parseIntegerDecimal({&strIP[iOffset], iEnd - iOffset}, valueRead));

		ipv4	|= ::gpk::byte_to<::gpk::u2_t>(valueRead, iVal);

		iOffset		= iEnd + 1;
		iEnd		= iOffset;
	}
	return iOffset;
}

::gpk::error_t			gpk::tcpipAddress		(::gpk::vcst_t strIP, ::gpk::vcst_t strPort, ::gpk::SIPv4End & ipv4) {
	if(0 == strPort.size()) 
		return ::gpk::tcpipAddress(strIP, ipv4.IP, ipv4.Port);

	u2_c				iOffset					= ::gpk::tcpipAddress(strIP, ipv4.IP);
	return iOffset + ::gpk::parseIntegerDecimal(strPort, ipv4.Port);
}

#ifndef GPK_ATMEL
#ifndef GPK_ESP8266
#if defined(GPK_WINDOWS)
#	include <WS2tcpip.h>
#elif defined(GPK_ESP8266)
//#	include <lwip/sockets.h>
//#	include <lwip/netdb.h>
#	include <lwip/inet.h>
#else
#	include <netdb.h>
#	include <arpa/inet.h>
#endif

::gpk::error_t			gpk::tcpipInitialize						()							{
#if defined(GPK_WINDOWS)
	::WSADATA										w											= {};
	if_true_fef(::WSAStartup(0x0202, &w) != 0, "Could not open Windows sockets: 0x%X '%s'", WSAGetLastError(), ::gpk::getWindowsErrorAsString(WSAGetLastError()).begin());
#endif
	info_printf("%s", "Network subsystem initialized.");
	return 0;
}

::gpk::error_t			gpk::tcpipShutdown							()							{
#if defined(GPK_WINDOWS)
	if_true_fef(::WSACleanup() != 0, "Could not shut down Windows sockets: 0x%X '%s'", WSAGetLastError(), ::gpk::getWindowsErrorAsString(WSAGetLastError()).begin());		// Open windows connection
#endif
	info_printf("%s", "Network subsystem shut down.");
	return 0;
}

::gpk::error_t			gpk::tcpipAddressFromSockaddr		(const sockaddr_in & sockaddr_ipv4, ::gpk::u2_t & address, ::gpk::u1_t & port) {
	port 	= (::gpk::u1_t)ntohs(sockaddr_ipv4.sin_port);
#ifndef GPK_WINDOWS
	address	= sockaddr_ipv4.sin_addr.s_addr;
#else
	address	= ::gpk::SIPv4
		{ sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b1 
		, sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b2
		, sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b3
		, sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b4
		};
#endif
	return 0;
}

//::gpk::error_t			gpk::tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr_ipv4, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, ::gpk::u1_t * port)	{
//#if defined(GPK_WINDOWS)
//	gpk_safe_assign(a1, (u0_t)sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b1);
//	gpk_safe_assign(a2, (u0_t)sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b2);
//	gpk_safe_assign(a3, (u0_t)sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b3);
//	gpk_safe_assign(a4, (u0_t)sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b4);
//#else
//	gpk_safe_assign(a1, ::gpk::byte_at(sockaddr_ipv4.sin_addr.s_addr, 0));
//	gpk_safe_assign(a2, ::gpk::byte_at(sockaddr_ipv4.sin_addr.s_addr, 1));
//	gpk_safe_assign(a3, ::gpk::byte_at(sockaddr_ipv4.sin_addr.s_addr, 2));
//	gpk_safe_assign(a4, ::gpk::byte_at(sockaddr_ipv4.sin_addr.s_addr, 3));
//#endif
//	gpk_safe_assign(port, (::gpk::u1_t)ntohs(sockaddr_ipv4.sin_port));
//	return 0;
//}
::gpk::error_t			gpk::tcpipAddressToSockaddr		(::gpk::u2_t address, ::gpk::u1_t port, sockaddr_in & sockaddr_ipv4) {
	sockaddr_ipv4				= {};
	sockaddr_ipv4.sin_family	= AF_INET;
	sockaddr_ipv4.sin_port		= port ? htons(port) : (::gpk::u1_t)0;
#ifndef GPK_WINDOWS
	sockaddr_ipv4.sin_addr.s_addr				=  address;
#else
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b1		= ::gpk::byte_at(address, 0);
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b2		= ::gpk::byte_at(address, 1);
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b3		= ::gpk::byte_at(address, 2);
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b4		= ::gpk::byte_at(address, 3);
#endif
	return 0;
}

::gpk::error_t			gpk::tcpipAddressToSockaddr		(u0_t a1, u0_t a2, u0_t a3, u0_t a4, ::gpk::u1_t port, sockaddr_in & sockaddr_ipv4)	{
	sockaddr_ipv4				= {};
	sockaddr_ipv4.sin_family	= AF_INET;
	sockaddr_ipv4.sin_port		= port ? htons(port) : (::gpk::u1_t)0;
#ifndef GPK_WINDOWS
	sockaddr_ipv4.sin_addr.s_addr				=  ::gpk::SIPv4{a1, a2, a3, a4};
#else
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b1		= a1;
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b2		= a2;
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b3		= a3;
	sockaddr_ipv4.sin_addr.S_un.S_un_b.s_b4		= a4;
#endif
	return 0;
}

::gpk::error_t			gpk::tcpipAddress	(SOCKET socket, ::gpk::u2_t & ip, ::gpk::u1_t & port) {
	sockaddr_in					sockaddr_ipv4		= {};
	socklen_t					len					= szof(sockaddr_in);
	if_true_fef(getsockname(socket, (sockaddr*)&sockaddr_ipv4, &len) != 0, "%s", "getpeername failed.");
	return ::gpk::tcpipAddressFromSockaddr(sockaddr_ipv4, ip, port);
}

::gpk::error_t			gpk::tcpipAddress	(::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4)										{
	char						host_name[257]								= {};
#if defined(GPK_ESP8266)
	if_fail_fe(::gpk::sprintf_s(host_name, "%s", wifi_station_get_hostname()));
#else	
	gethostname(host_name, 256);
#endif
	return ::gpk::tcpipAddress(host_name, portRequested, adapterIndex, mode, a1, a2, a3, a4);
}

::gpk::error_t			gpk::tcpipAddress	(::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u2_t & address)										{
	char						host_name[257]								= {};
#if defined(GPK_ESP8266)
	if_fail_fe(::gpk::sprintf_s(host_name, "%s", wifi_station_get_hostname()));
#else	
	gethostname(host_name, 256);
#endif
	return ::gpk::tcpipAddress(host_name, portRequested, adapterIndex, mode, address);
}

::gpk::error_t			gpk::tcpipAddress	(const char* szHostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, ::gpk::u1_t * port)				{
	::gpk::SIPv4End 		addr; 
	gpk_necs(gpk::tcpipAddress(szHostName, portRequested, adapterIndex, mode, addr.IP, addr.Port));
	gpk_safe_assign(a1, addr[0]);
	gpk_safe_assign(a2, addr[1]);
	gpk_safe_assign(a3, addr[2]);
	gpk_safe_assign(a4, addr[3]);	
	gpk_safe_assign(port, addr.Port);		
	return 0;
}

::gpk::error_t		gpk::tcpipAddress		(::gpk::vcst_t hostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, ::gpk::u1_t* port) { 
	return ::gpk::tcpipAddress(::gpk::string(hostName).begin(), portRequested, adapterIndex, mode, a1, a2, a3, a4, port); 
}

::gpk::error_t			gpk::tcpipAddress	(const char* szHostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u2_t & address, ::gpk::u1_t & port) {
	char						portString			[6]					= {};
	if_fail_fe(::gpk::sprintf_s(portString, "%u", portRequested));

	// Setup the hints address info structure which is passed to the getaddrinfo() function
	::addrinfo					hints											= {};
	hints.ai_family			= AF_INET;
	hints.ai_socktype		= (TRANSPORT_PROTOCOL_TCP == mode) ? SOCK_STREAM	: SOCK_DGRAM	;
	hints.ai_protocol		= (TRANSPORT_PROTOCOL_TCP == mode) ? IPPROTO_TCP	: IPPROTO_UDP	;

	const ::addrinfo			* createdAddrInfo								= 0;
	if_fail_vef(-1, ::getaddrinfo(szHostName, portString, &hints, (::addrinfo**)&createdAddrInfo), "gettaddrinfo failed for host_name: %s, port: %s", szHostName, portString);
	//sockaddr_in6									* sockaddr_ipv6									= 0;

	::gpk::u2_t					iAddress										= 0;
	bool						addressFound									= false;
	for(const addrinfo* ptr = createdAddrInfo; ptr != NULL; ptr = ptr->ai_next)  {	// Retrieve each address and print out the hex bytes
		verbose_printf("getaddrinfo response at index %" GPK_FMT_U2 ".", iAddress);
		verbose_printf("Flags: 0x%x.", ptr->ai_flags);
		verbose_printf("%s", "Family: ");
		char						ipstringbuffer	[46]							= {};
#if defined(GPK_WINDOWS)
		wchar_t						ipwstringbuffer	[46]							= {};
#endif
		::sockaddr					* sockaddr_ip									=  0;
		::sockaddr_in				* sockaddr_ipv4									=  0;
#ifndef GPK_ESP8266
		::sockaddr_in6				* sockaddr_ipv6									=  0;
		(void)sockaddr_ipv6	;
#endif
		(void)sockaddr_ip	;
		(void)sockaddr_ipv4	;
		//DWORD dwRetval;
#if defined(GPK_WINDOWS)
		int32_t						iRetval;
		DWORD						ipbufferlength									= 46;
#else
		::gpk::u2_t					ipbufferlength									= 46;
#endif
		switch (ptr->ai_family)  {
		default			:	verbose_printf("Other %li.", ptr->ai_family	); break;
		//case AF_NETBIOS	:	info_printf("%s", "AF_NETBIOS (NetBIOS)"	); break;
		case AF_UNSPEC	:	verbose_printf("%s", "Unspecified"				); break;
		case AF_INET	:
			verbose_printf("%s", "AF_INET (IPv4)");
			sockaddr_ipv4								= (::sockaddr_in *) ptr->ai_addr;
			ipbufferlength								= 46;
 			::inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, ipbufferlength);
			verbose_printf("IPv4 address %s", ipstringbuffer);
			// Copy address
			if(adapterIndex == iAddress) {
				::gpk::tcpipAddressFromSockaddr(*sockaddr_ipv4, address, port);
				//printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );
				addressFound								= true;
			}
			break;
#ifndef GPK_ESP8266
		case AF_INET6	:
			verbose_printf("%s", "AF_INET6 (IPv6)");
			// the InetNtop function is available on Windows Vista and later
			sockaddr_ipv6								= (struct ::sockaddr_in6 *) ptr->ai_addr;
			//info_printf("IPv6 address %s", InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipwstringbuffer, 46) );

#	if defined(GPK_WINDOWS)
			sockaddr_ip									= (LPSOCKADDR)ptr->ai_addr;
			ipbufferlength								= 46;	// The buffer length is changed by each call to WSAAddresstoString, so we need to set it for each iteration through the loop for safety
			iRetval										= WSAAddressToStringW(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, ipwstringbuffer, &ipbufferlength );	// We use WSAAddressToString since it is supported on Windows XP and later
			if (iRetval) {
				warning_printf("WSAAddressToString failed with code 0x%X: '%s'.", ::WSAGetLastError(), ::gpk::getWindowsErrorAsString(::WSAGetLastError()).begin() );
			}
			else
				::wprintf(L"IPv6 address '%s'.\n", ipwstringbuffer);
#	endif
			break;
#endif
		}

		//verbose_printf("%s", "Socket type: ");
		//switch (ptr->ai_socktype) {
		//case 0				: verbose_printf("%s", "Unspecified"							);	break;
		//case SOCK_STREAM	: verbose_printf("%s", "SOCK_STREAM (stream)"					);	break;
		//case SOCK_DGRAM		: verbose_printf("%s", "SOCK_DGRAM (datagram)"					);	break;
		//case SOCK_RAW		: verbose_printf("%s", "SOCK_RAW (raw)"							);	break;
		//case SOCK_RDM		: verbose_printf("%s", "SOCK_RDM (reliable message datagram)"	);	break;
		//case SOCK_SEQPACKET	: verbose_printf("%s", "SOCK_SEQPACKET (pseudo-stream packet)"	);	break;
		//default:
		//	verbose_printf("Other %ld", ptr->ai_socktype);
		//	break;
		//}
		//verbose_printf("%s", "Protocol: ");
		//switch (ptr->ai_protocol) {
		//case 0				: verbose_printf("%s", "Unspecified"							);	break;
		//case IPPROTO_TCP	: verbose_printf("%s", "IPPROTO_TCP (TCP)"						);	break;
		//case IPPROTO_UDP	: verbose_printf("%s", "IPPROTO_UDP (UDP)"						);	break;
		//default:
		//	verbose_printf("Other %ld", ptr->ai_protocol);
		//	break;
		//}
		//++iAddress;
		verbose_printf("Length of this sockaddr: %" GPK_FMT_U2 "", (::gpk::u2_t)ptr->ai_addrlen);
		verbose_printf("Canonical name: %s", ptr->ai_canonname);
		if(addressFound)
			break;
	}
	::freeaddrinfo((::addrinfo*)createdAddrInfo);
//#else
//	struct hostent									* hp		= gethostbyname(host_name);
//
//	if_true_fef(hp == NULL, "Could not get host by name.");
//
//	b1											= hp->h_addr_list[0][0];
//	b2											= hp->h_addr_list[0][1];
//	b3											= hp->h_addr_list[0][2];
//	b4											= hp->h_addr_list[0][3];
//#endif
	return iAddress;
}
#endif // GPK_ESP8266
#endif // GPK_ATMEL


