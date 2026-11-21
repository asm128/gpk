#include "gpk_string_tcpip.h"

#ifndef GPK_TCPIP_H_23627
#define GPK_TCPIP_H_23627

namespace gpk
{
#pragma pack(push, 1)
	err_t			tcpipInitialize						();
	err_t			tcpipShutdown						();

	enum TRANSPORT_PROTOCOL
		{ TRANSPORT_PROTOCOL_UDP			= 0
		, TRANSPORT_PROTOCOL_TCP
		};

			err_t	tcpipAddress		(const sc_t* szHostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u2_t & address, u1_t & port);
			err_t	tcpipAddress		(const sc_t* szHostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, u1_t* port = 0);

			err_t	tcpipAddress		(vcst_t hostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, u1_t* port = 0);
	stin	err_t	tcpipAddress		(const sc_t* szHostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u2_t & address)				{
		u1_t port = 0;
		return tcpipAddress(szHostName, portRequested, adapterIndex, mode, address, port);
	}

			err_t	tcpipAddress		(u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u2_t & address);
			err_t	tcpipAddress		(u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4);
	stin	err_t	tcpipAddress		(u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)							{
		address.Port			= portRequested;
		if_fail_fe(tcpipAddress(portRequested, adapterIndex, mode, address.IP));
		return 0;
	}
	stin	err_t	tcpipAddress		(const sc_t * hostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)	{
		return tcpipAddress(hostName, portRequested, adapterIndex, mode, address.IP, (address.Port = portRequested));
	}
	stin	err_t	tcpipAddress		(const vcst_t & hostName, u1_t portRequested, u2_t adapterIndex, TRANSPORT_PROTOCOL mode, SIPv4End & address)	{
		return tcpipAddress(hostName.begin(), portRequested, adapterIndex, mode, address.IP, (address.Port = portRequested));
	}

			err_t	tcpipAddress		(vcst_t strAddress, vcst_t strPort, SIPv4End & ipv4end);
#pragma pack(pop)
} // namespace

#endif // GPK_TCPIP_H_23627

// 50y-5p4C3.41
