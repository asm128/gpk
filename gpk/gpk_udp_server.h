#include "gpk_stdsocket.h"
#include "gpk_array.h"
#include "gpk_ptr.h"
#include "gpk_connection.h"

#ifndef GPK_UDP_SERVER_H_28874928347
#define GPK_UDP_SERVER_H_28874928347

namespace gpk 
{
	struct SUDPConnection {
		::gpk::auto_socket_close												Socket								;
		::gpk::SUDPClientQueue													Queue								;
		::gpk::SIPv4															Address								= {};
		int64_t																	LastPing							= 0;
		::gpk::UDP_CONNECTION_STATE												State;
	};

	struct SUDPServer {
		::gpk::auto_socket_close												Socket;
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnection>>					Clients;
		::std::mutex															Mutex;
		::gpk::SIPv4															Address								= {};
		bool																	Listen								= false;
		int32_t																	Timeout								= 3000;	// Kill clients timeout
	};
} // namespace

#endif // GPK_UDP_SERVER_H_28874928347
