#include "gpk_view_array.h"

#ifndef GPK_TCPIP_H_2874982374
#define GPK_TCPIP_H_2874982374

namespace gpk
{
#pragma pack(push, 1)
	struct SIPv4 {
						ubyte_t											IP		[4]							;
						uint16_t										Port								;

		inline			bool											operator!=							(const SIPv4& other)	const	noexcept	{ return !operator==(other); }
						bool											operator==							(const SIPv4& other)	const	noexcept	{
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
					::gpk::error_t									tcpipAddress						(const char_t* hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4);
	static inline	::gpk::error_t									tcpipAddress						(uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4 & address)							{
		address.Port													= portRequested;
		return tcpipAddress(portRequested, adapterIndex, mode, GPK_IPV4_EXPAND_PTR_IP(address));
	}

	static inline	::gpk::error_t									tcpipAddress						(const char_t* hostName, uint16_t portRequested, uint32_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4 & address)	{
		address.Port													= portRequested;
		return tcpipAddress(hostName, portRequested, adapterIndex, mode, GPK_IPV4_EXPAND_PTR_IP(address));
	}

					::gpk::error_t									tcpipAddress						(const ::gpk::view_array<const char>& strRemoteIP, const ::gpk::view_array<const char>& strRemotePort, ::gpk::SIPv4 & remoteIP);
} // namespace

#endif // GPK_TCPIP_H_2874982374
