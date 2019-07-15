#include "gpk_stdsocket.h"
#include "gpk_http.h"

namespace gpk
{
	struct SHTTPClient {
		::gpk::auto_socket_close		Socket;
		::gpk::SIPv4					RemoteAddress;
	};

	::gpk::error_t					httpClientRequest			(::gpk::SHTTPClient	& clientToConnect, ::gpk::HTTP_METHOD method, const ::gpk::view_const_byte & input, ::gpk::array_pod<byte_t> & out_received);
}