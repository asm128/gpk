#include "gpk_array_static.h"

#ifndef GPK_STRING_TCPIP_H_23627
#define GPK_STRING_TCPIP_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	err_t		tcpipAddress		(vcst_t strIP, u2_t & ipv4);
	err_t		tcpipAddress		(vcst_t strIP, u2_t & ipv4, u1_t & port);

	struct SIPv4 {
		u2_t			IP					= {};
		
						SIPv4				(vcst_t strIP)							{ 	tcpipAddress(strIP, IP); }
						
		inxp			SIPv4				()								nxpt	= default;
		inxp			SIPv4				(cnst SIPv4 &)					nxpt	= default;
		inxp			SIPv4				(u2_t little_endian)		nxpt	: IP(little_endian) {}
		cxpr			SIPv4				(u0_t _0, u0_t _1, u0_t _2, u0_t _3 = 0)
			: IP
			{ 	byte_to<u2_t>(_0, 0)
			| 	byte_to<u2_t>(_1, 1)
			| 	byte_to<u2_t>(_2, 2)
			| 	byte_to<u2_t>(_3, 3)
			} {}

		GPK_DEFAULT_OPERATOR(SIPv4, IP == other.IP);

		inxp	u0_t	oper[]			(u0_t index)		csnx	{ return 	byte_at(IP, index); }
		u0_t&			oper[]			(u0_t index)				nxpt	{ return *(((u0_t*)&IP) + index); }

		inxp	oper	u2_t			()					csnx	{ return IP; }
		cxpr	oper	astu0_t<4>		()					csnx	{
			return 
				{ 	byte_at(IP, 0)
				, 	byte_at(IP, 1) 
				, 	byte_at(IP, 2) 
				, 	byte_at(IP, 3) 
				};
		}

		tplt<size_t buflen>
		astsc_t<(buflen > 16) ? buflen : 16>	toString			(sc_t separator = '.')						csnx	{
			astsc_t<(buflen > 16) ? buflen : 16>	result	= {};
			print(result.Storage, separator);
			return result; 
		}

		tplt<size_t bufferSize>
			err_t 		print				(sc_t (&target)[bufferSize], sc_t separator = '.')	csnx	{
			sc_c format[] = {'%', 'u', separator, '%', 'u', separator, '%', 'u', separator, '%', 'u', 0};
			return sprintf_s(target, format
				, 	byte_at(IP, 0)
				, 	byte_at(IP, 1)
				, 	byte_at(IP, 2)
				, 	byte_at(IP, 3)
				);
		}
	};

	stin		astsc_t<16>	str			(cnst SIPv4 ip)	{ return ip.toString<16>(); }

	tydf struct SIPv4End : SIPv4 {
		u1_t			Port				= {};

		GPK_DEFAULT_OPERATOR(SIPv4End, IP == other.IP && Port == other.Port);

		tplt<size_t buflen>
		oper			astsc_t<min((size_t)22, buflen)>	()		csnx	{
			if(0 == Port)
				return *(cnst SIPv4*)this;

			astsc_t<buflen>	result;
			sprintf_s(result.Storage, "%u.%u.%u.%u:%u"
				, 	byte_at(IP, 0)
				, 	byte_at(IP, 1)
				, 	byte_at(IP, 2)
				, 	byte_at(IP, 3)
				, Port
				);
			return result; 
		}
	} SIPv4Endpoint;

	struct SIPv4Node {
			SIPv4	IPv4		= {};
			SIPv4	Gateway		= {};
			SIPv4	NetMask		= {};

		cxpr		SIPv4	NetworkID	()	csnx	{
			return 	SIPv4
				( NetMask[0] & IPv4[0]
				, NetMask[1] & IPv4[1]
				, NetMask[2] & IPv4[2]
				, NetMask[3] & IPv4[3]
				);
		}

		cxpr		SIPv4	BroadcastIP	()	csnx	{
			return 	SIPv4
				( ~NetMask[0] & IPv4[0]
				, ~NetMask[1] & IPv4[1]
				, ~NetMask[2] & IPv4[2]
				, ~NetMask[3] & IPv4[3]
				);
		}
	};
			err_t	tcpipAddress		(vcst_t strAddress, vcst_t strPort, SIPv4End & ipv4end);
	stin	err_t	tcpipAddress		(vcst_t strIP, SIPv4 & ipv4) { return tcpipAddress(strIP, ipv4.IP); }
	
	tplt<size_t count> usng	sstipv4		= 	astatic<	SIPv4, count>;
	tplt<size_t count> usng	sstipv4end	= 	astatic<	SIPv4End, count>;
#pragma pack(pop)

} // nmsp

#define GPK_IPV4_EXPAND_IP(addr)  	::gpk::byte_at(addr.IP, 0)	\
								, 	::gpk::byte_at(addr.IP, 1)	\
								, 	::gpk::byte_at(addr.IP, 2)	\
								, 	::gpk::byte_at(addr.IP, 3)	\

#define GPK_IPV4_EXPAND(addr) (::gpk::u2_t)	::gpk::byte_at(addr.IP, 0)	\
							, (::gpk::u2_t)	::gpk::byte_at(addr.IP, 1)	\
							, (::gpk::u2_t)	::gpk::byte_at(addr.IP, 2)	\
							, (::gpk::u2_t)	::gpk::byte_at(addr.IP, 3)	\
							, (::gpk::u2_t)addr.Port
#endif // GPK_TCPIP_H_23627 // 50y-5p4C3.41
