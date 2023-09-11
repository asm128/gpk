#include "gpk_array.h"
#include "gpk_array_static.h"

#ifndef GPK_TCPIP_H_23627
#define GPK_TCPIP_H_23627

namespace gpk
{
#pragma pack(push, 1)


	struct SIPv4 {
		uint32_t			IP					= {};
		
		inlcxpr				SIPv4				()								noexcept	= default;
		inlcxpr				SIPv4				(const SIPv4 &)					noexcept	= default;
		inlcxpr				SIPv4				(uint32_t little_endian)		noexcept	: IP(little_endian) {}
		cnstxpr				SIPv4				(uint8_t _0, uint8_t _1, uint8_t _2, uint8_t _3 = 0)
			: IP
			{ ::gpk::byte_to<uint32_t>(_0, 0)
			| ::gpk::byte_to<uint32_t>(_1, 1)
			| ::gpk::byte_to<uint32_t>(_2, 2)
			| ::gpk::byte_to<uint32_t>(_3, 3)
			} {}

		GPK_DEFAULT_OPERATOR(SIPv4, IP == other.IP);

		inlcxpr	operator	uint32_t			()						const	noexcept	{ return IP; }
		cnstxpr	operator	gpk::astu8<4>		()						const	noexcept	{
			return 
				{ ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1) 
				, ::gpk::byte_at(IP, 2) 
				, ::gpk::byte_at(IP, 3) 
				};
		}
		operator			gpk::astchar<16>	()						const	noexcept	{
			gpk::astchar<16>	result;
			sprintf_s(result.Storage, "%u.%u.%u.%u"
				, ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1)
				, ::gpk::byte_at(IP, 2)
				, ::gpk::byte_at(IP, 3)
				);
			return result; 
		}

		tplt<size_t bufferSize>
		::gpk::error_t 		print				(char (&target)[bufferSize])	const	noexcept	{
			return sprintf_s(target, "%u.%u.%u.%u"
				, ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1)
				, ::gpk::byte_at(IP, 2)
				, ::gpk::byte_at(IP, 3)
				);

		}
	};	

	struct SIPv4Endpoint : SIPv4 {
		uint16_t			Port				= {};

		GPK_DEFAULT_OPERATOR(SIPv4Endpoint, IP == other.IP && Port == other.Port);

		operator			gpk::astchar<22>	()						const	noexcept	{
			gpk::astchar<22>	result;
			sprintf_s(result.Storage, "%u.%u.%u.%u:%u"
				, ::gpk::byte_at(IP, 0)
				, ::gpk::byte_at(IP, 1)
				, ::gpk::byte_at(IP, 2)
				, ::gpk::byte_at(IP, 3)
				, Port
				);
			return result; 
		}
	};
#pragma pack(pop)

	::gpk::error_t			tcpipInitialize						();
	::gpk::error_t			tcpipShutdown						();

	enum TRANSPORT_PROTOCOL
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP
		};


#define GPK_IPV4_EXPAND_IP(addr)  ::gpk::byte_at(addr.IP, 0)	\
								, ::gpk::byte_at(addr.IP, 1)	\
								, ::gpk::byte_at(addr.IP, 2)	\
								, ::gpk::byte_at(addr.IP, 3)	\

#define GPK_IPV4_EXPAND(addr) (uint32_t)::gpk::byte_at(addr.IP, 0)	\
							, (uint32_t)::gpk::byte_at(addr.IP, 1)	\
							, (uint32_t)::gpk::byte_at(addr.IP, 2)	\
							, (uint32_t)::gpk::byte_at(addr.IP, 3)	\
							, (uint32_t)addr.Port

			::gpk::error_t	tcpipAddress		(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t * a1, uint8_t * a2, uint8_t * a3, uint8_t * a4);
			::gpk::error_t	tcpipAddress		(const char* hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t * a1, uint8_t * a2, uint8_t * a3, uint8_t * a4, uint16_t* port = 0);
	stainli	::gpk::error_t	tcpipAddress		(const ::gpk::vcc & hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t * a1, uint8_t * a2, uint8_t * a3, uint8_t * a4, uint16_t* port = 0) { return ::gpk::tcpipAddress(::gpk::toString(hostName).begin(), portRequested, adapterIndex, mode, a1, a2, a3, a4, port); }
	stainli	::gpk::error_t	tcpipAddress		(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4Endpoint & address)							{
		address.Port			= portRequested;
		uint8_t						addr[4]				= {};
		gpk_necs(::gpk::tcpipAddress(portRequested, adapterIndex, mode, &addr[0], &addr[1], &addr[2], &addr[3]));
		address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]};
		return 0;
	}
	stainli	::gpk::error_t	tcpipAddress		(const char * hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4Endpoint & address)	{
		address.Port			= portRequested;
		uint8_t						addr[4]				= {};
		gpk_necs(::gpk::tcpipAddress(hostName, portRequested, adapterIndex, mode, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port));
		address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]};
		return 0;
	}

	stainli	::gpk::error_t	tcpipAddress		(const ::gpk::vcs & hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4Endpoint & address)	{
		address.Port													= portRequested;
		uint8_t						addr[4]				= {};
		gpk_necs(::gpk::tcpipAddress(hostName, portRequested, adapterIndex, mode, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port));
		address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]};
		return 0;
	}

	::gpk::error_t			tcpipAddress		(const ::gpk::vcs & strRemoteIP, const ::gpk::vcs & strRemotePort, ::gpk::SIPv4Endpoint & remoteIP);
} // namespace

#endif // GPK_TCPIP_H_23627
