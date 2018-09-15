#include "gpk_stdsocket.h"
#include "gpk_endpoint_command.h"
#include "gpk_array.h"
#include "gpk_ptr.h"

#ifndef GPK_UDP_SERVER_H_28874928347
#define GPK_UDP_SERVER_H_28874928347

namespace gpk 
{
	struct SUDPConnectionMessage {
		::gpk::SEndpointCommand													Command;
		::gpk::array_pod<byte_t>												Payload;
		uint64_t																Time;
	};

	struct SUDPClientQueue {
		::gpk::array_obj<::gpk::SUDPConnectionMessage>							Received;
		::gpk::array_obj<::gpk::SUDPConnectionMessage>							Send;
		::gpk::array_obj<::gpk::SUDPConnectionMessage>							Sent;
	};

	struct SUDPConnection {
		::gpk::auto_socket_close												Socket								;
		SUDPClientQueue															Queue								;
		::gpk::SIPv4															Address								= {};
		int64_t																	LastPing							= 0;
		::std::mutex															Mutex								;
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
