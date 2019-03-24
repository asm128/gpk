#include "gpk_connection.h"

#ifndef GPK_HTTP_SERVER_H_28874928347
#define GPK_HTTP_SERVER_H_28874928347

namespace gpk
{
	struct SHTTPServer {
		::gpk::auto_socket_close												Socket;
		::gpk::array_obj<::gpk::ptr_obj<::gpk::STCPConnection>>					Clients;
		::std::mutex															Mutex;
		::gpk::SIPv4															Address								= {};
		bool																	Listen								= false;
		int32_t																	Timeout								= 3000000;	// Kill clients timeout
	};

	::gpk::error_t															serverStart							(::gpk::SHTTPServer& serverInstance, uint16_t port);
	::gpk::error_t															serverStop							(::gpk::SHTTPServer& serverInstance);

} // namespace

#endif // GPK_UDP_SERVER_H_28874928347
