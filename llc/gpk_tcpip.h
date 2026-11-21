#include "gpk_array_static.h"

#ifndef GPK_TCPIP_H_23627
#define GPK_TCPIP_H_23627

namespace gpk
{
#pragma pack(push, 1)
	gpk::err_t		tcpipAddress		(::gpk::vcst_t strIP, ::gpk::u2_t & ipv4);
	gpk::err_t		tcpipAddress		(::gpk::vcst_t strIP, ::gpk::u2_t & ipv4, ::gpk::u1_t & port);

	struct SIPv4 {
		::gpk::u2_t			IP					= {};
		
							SIPv4				(gpk::vcst_t strIP)							{ ::gpk::tcpipAddress(strIP, IP); }
							
		inxp				SIPv4				()								noexcept	= default;
		inxp				SIPv4				(const SIPv4 &)					noexcept	= default;
		inxp				SIPv4				(::gpk::u2_t little_endian)		noexcept	: IP(little_endian) {}
		cxpr				SIPv4				(::gpk::u0_t _0, ::gpk::u0_t _1, ::gpk::u0_t _2, ::gpk::u0_t _3 = 0)
			: IP
			{ ::gpk::byte_to<::gpk::u2_t>(_0, 0)
			| ::gpk::byte_to<::gpk::u2_t>(_1, 1)
			| ::gpk::byte_to<::gpk::u2_t>(_2, 2)
			| ::gpk::byte_to<::gpk::u2_t>(_3, 3)
			} {}

		GPK_DEFAULT_OPERATOR(SIPv4, IP == other.IP);

		inxp	::gpk::u0_t		operator[]			(::gpk::u0_t index)		const	noexcept	{ return ::gpk::byte_at(IP, index); }
		::gpk::u0_t&			operator[]			(::gpk::u0_t index)				noexcept	{ return *(((::gpk::u0_t*)&IP) + index); }

		inxp	operator	::gpk::u2_t			()					const	noexcept	{ return IP; }
		cxpr	operator	gpk::astu0_t<4>		()					const	noexcept	{
			return 
				{ ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1) 
				, ::gpk::byte_at(IP, 2) 
				, ::gpk::byte_at(IP, 3) 
				};
		}

		template<size_t buflen>
		gpk::astsc_t<(buflen > 16) ? buflen : 16>	toString			(sc_t separator = '.')						const	noexcept	{
			gpk::astsc_t<(buflen > 16) ? buflen : 16>	result	= {};
			print(result.Storage, separator);
			return result; 
		}

		tplt<size_t bufferSize>
		gpk::err_t 		print				(sc_t (&target)[bufferSize], sc_t separator = '.')	const	noexcept	{
			const sc_t format[] = {'%', 'u', separator, '%', 'u', separator, '%', 'u', separator, '%', 'u', 0};
			return sprintf_s(target, format
				, ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1)
				, ::gpk::byte_at(IP, 2)
				, ::gpk::byte_at(IP, 3)
				);
		}
	};

	stin	::gpk::astsc_t<16>	str			(const gpk::SIPv4 ip)	{ return ip.toString<16>(); }

	typedef struct SIPv4End : SIPv4 {
		::gpk::u1_t			Port				= {};

		GPK_DEFAULT_OPERATOR(SIPv4End, IP == other.IP && Port == other.Port);

		template<size_t buflen>
		operator			gpk::astsc_t<min((size_t)22, buflen)>	()		const	noexcept	{
			if(0 == Port)
				return *(const SIPv4*)this;

			gpk::astsc_t<buflen>	result;
			sprintf_s(result.Storage, "%u.%u.%u.%u:%u"
				, ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1)
				, ::gpk::byte_at(IP, 2)
				, ::gpk::byte_at(IP, 3)
				, Port
				);
			return result; 
		}
	} SIPv4Endpoint;

	struct SIPv4Node {
		::gpk::SIPv4	IPv4		= {};
		::gpk::SIPv4	Gateway		= {};
		::gpk::SIPv4	NetMask		= {};

		cxpr	::gpk::SIPv4	NetworkID	()	const	noexcept	{
			return ::gpk::SIPv4
				( NetMask[0] & IPv4[0]
				, NetMask[1] & IPv4[1]
				, NetMask[2] & IPv4[2]
				, NetMask[3] & IPv4[3]
				);
		}

		cxpr	::gpk::SIPv4	BroadcastIP	()	const	noexcept	{
			return ::gpk::SIPv4
				( ~NetMask[0] & IPv4[0]
				, ~NetMask[1] & IPv4[1]
				, ~NetMask[2] & IPv4[2]
				, ~NetMask[3] & IPv4[3]
				);
		}
	};
	
	tplt<size_t count> using	astipv4		= ::gpk::astatic<::gpk::SIPv4, count>;
	tplt<size_t count> using	astipv4end	= ::gpk::astatic<::gpk::SIPv4End, count>;
#pragma pack(pop)

	gpk::err_t			tcpipInitialize						();
	gpk::err_t			tcpipShutdown						();

	enum TRANSPORT_PROTOCOL
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP
		};


#define GPK_IPV4_EXPAND_IP(addr)  ::gpk::byte_at(addr.IP, 0)	\
								, ::gpk::byte_at(addr.IP, 1)	\
								, ::gpk::byte_at(addr.IP, 2)	\
								, ::gpk::byte_at(addr.IP, 3)	\

#define GPK_IPV4_EXPAND(addr) (::gpk::u2_t)::gpk::byte_at(addr.IP, 0)	\
							, (::gpk::u2_t)::gpk::byte_at(addr.IP, 1)	\
							, (::gpk::u2_t)::gpk::byte_at(addr.IP, 2)	\
							, (::gpk::u2_t)::gpk::byte_at(addr.IP, 3)	\
							, (::gpk::u2_t)addr.Port

	stin	gpk::err_t	tcpipAddress		(::gpk::vcst_t strIP, ::gpk::SIPv4 & ipv4) { return ::gpk::tcpipAddress(strIP, ipv4.IP); }


			gpk::err_t	tcpipAddress		(const sc_t* szHostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u2_t & address, ::gpk::u1_t & port);
			gpk::err_t	tcpipAddress		(const sc_t* szHostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u0_t * a1, ::gpk::u0_t * a2, ::gpk::u0_t * a3, ::gpk::u0_t * a4, ::gpk::u1_t* port = 0);

			gpk::err_t	tcpipAddress		(::gpk::vcst_t hostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u0_t * a1, ::gpk::u0_t * a2, ::gpk::u0_t * a3, ::gpk::u0_t * a4, ::gpk::u1_t* port = 0);
	stin	gpk::err_t	tcpipAddress		(const sc_t* szHostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u2_t & address)				{
		::gpk::u1_t port = 0;
		return gpk::tcpipAddress(szHostName, portRequested, adapterIndex, mode, address, port);
	}

			gpk::err_t	tcpipAddress		(::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u2_t & address);
			gpk::err_t	tcpipAddress		(::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, ::gpk::u0_t * a1, ::gpk::u0_t * a2, ::gpk::u0_t * a3, ::gpk::u0_t * a4);
	stin	gpk::err_t	tcpipAddress		(::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)							{
		address.Port			= portRequested;
		if_fail_fe(gpk::tcpipAddress(portRequested, adapterIndex, mode, address.IP));
		return 0;
	}
	stin	gpk::err_t	tcpipAddress		(const sc_t * hostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)	{
		return ::gpk::tcpipAddress(hostName, portRequested, adapterIndex, mode, address.IP, (address.Port = portRequested));
	}
	stin	gpk::err_t	tcpipAddress		(const ::gpk::vcst_t & hostName, ::gpk::u1_t portRequested, ::gpk::u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)	{
		return ::gpk::tcpipAddress(hostName.begin(), portRequested, adapterIndex, mode, address.IP, (address.Port = portRequested));
	}

	gpk::err_t			tcpipAddress		(::gpk::vcst_t strAddress, ::gpk::vcst_t strPort, ::gpk::SIPv4End & ipv4end);
	
} // namespace

#endif // GPK_TCPIP_H_23627

// 50y-5p4C3.41