#include "gpk_array.h"

#ifndef GPK_TCPIP_H_2874982374
#define GPK_TCPIP_H_2874982374

namespace gpk
{
#pragma pack(push, 1)
	struct SIPv4 {
						uint8_t											IP		[4]							;
						uint16_t										Port								;

		inline			bool											operator!=			(const SIPv4& other)	const	noexcept	{ return !operator==(other); }
						bool											operator==			(const SIPv4& other)	const	noexcept	{
			return IP[0]	== other.IP[0]
				&& IP[1]	== other.IP[1]
				&& IP[2]	== other.IP[2]
				&& IP[3]	== other.IP[3]
				&& Port		== other.Port
				;
		}
	};
#pragma pack(pop)

					::gpk::error_t									tcpipInitialize						();
					::gpk::error_t									tcpipShutdown						();

	enum TRANSPORT_PROTOCOL
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP
		};


#define GPK_IPV4_EXPAND_IP(addr)  addr.IP[0]	\
								, addr.IP[1]	\
								, addr.IP[2]	\
								, addr.IP[3]	\

#define GPK_IPV4_EXPAND(addr) (uint32_t)addr.IP[0]	\
							, (uint32_t)addr.IP[1]	\
							, (uint32_t)addr.IP[2]	\
							, (uint32_t)addr.IP[3]	\
							, (uint32_t)addr.Port

#define GPK_IPV4_EXPAND_PTR(addr) &addr.IP[0]	\
								, &addr.IP[1]	\
								, &addr.IP[2]	\
								, &addr.IP[3]	\
								, &addr.Port

#define GPK_IPV4_EXPAND_PTR_IP(addr) &addr.IP[0]	\
									, &addr.IP[1]	\
									, &addr.IP[2]	\
									, &addr.IP[3]	\

					::gpk::error_t									tcpipAddress						(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4);
					::gpk::error_t									tcpipAddress		(const char* hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port = 0);
	stainli	::gpk::error_t									tcpipAddress		(const ::gpk::vcc & hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port = 0) { return ::gpk::tcpipAddress(::gpk::toString(hostName).begin(), portRequested, adapterIndex, mode, a1, a2, a3, a4, port); }
	stainli	::gpk::error_t									tcpipAddress						(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4 & address)							{
		address.Port													= portRequested;
		return ::gpk::tcpipAddress(portRequested, adapterIndex, mode, GPK_IPV4_EXPAND_PTR_IP(address));
	}

	stainli	::gpk::error_t									tcpipAddress		(const char* hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4 & address)	{
		address.Port													= portRequested;
		return ::gpk::tcpipAddress(hostName, portRequested, adapterIndex, mode, GPK_IPV4_EXPAND_PTR_IP(address), &address.Port);
	}

	stainli	::gpk::error_t									tcpipAddress		(const ::gpk::vcc & hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4 & address)	{
		address.Port													= portRequested;
		return ::gpk::tcpipAddress(hostName, portRequested, adapterIndex, mode, GPK_IPV4_EXPAND_PTR_IP(address), &address.Port);
	}

					::gpk::error_t									tcpipAddress		(const ::gpk::view<const char>& strRemoteIP, const ::gpk::view<const char>& strRemotePort, ::gpk::SIPv4 & remoteIP);
} // namespace

#endif // GPK_TCPIP_H_2874982374
