#include "gpk_connection.h"

#ifndef GPK_UDP_SERVER_H_28874928347
#define GPK_UDP_SERVER_H_28874928347

namespace gpk
{
	struct SUDPServer {
		::gpk::auto_socket_close												Socket;
		::gpk::aobj<::gpk::pobj<::gpk::SUDPConnection>>					Clients;
		::std::mutex															Mutex;
		::gpk::SIPv4															Address								= {};
		int16_t																	AdapterIndex;
		bool																	Listen								= false;
		int32_t																	Timeout								= 3000000;	// Kill clients timeout
	};

	::gpk::error_t															serverStart							(::gpk::SUDPServer& serverInstance, uint16_t port, int16_t adapterIndex = 0);
	::gpk::error_t															serverStop							(::gpk::SUDPServer& serverInstance);

} // namespace

#endif // GPK_UDP_SERVER_H_28874928347
