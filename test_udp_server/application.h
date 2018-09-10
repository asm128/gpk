#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_stdsocket.h"
#include "gpk_endpoint_command.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

struct SUDPClientMessage {
	::gpk::SEndpointCommand													Command;
	::gpk::array_pod<byte_t>												Payload;
	uint64_t																Time;
};

struct SUDPClientQueue {
	::gpk::array_obj<::SUDPClientMessage>									Received;
	::gpk::array_obj<::SUDPClientMessage>									Send;
	::gpk::array_obj<::SUDPClientMessage>									Sent;
};

struct SUDPClient {
	SOCKET																	Socket;
	SUDPClientQueue															Queue;
	::gpk::SIPv4															Address;
};

struct SUDPServer {
	SOCKET																	Socket;
	::gpk::SIPv4															Address								= {{}, 9999};
	::gpk::array_obj<::SUDPClient>											Clients;
};

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework														Framework;
		::gpk::SImage<::gpk::SColorBGRA>										TextureFont							= {};
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		Offscreen							= {};
		SUDPServer																Server								= {9999,};

		int32_t																	IdExit								= -1;

		::std::mutex															LockGUI;
		::std::mutex															LockRender;

																				SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};

	typedef ::std::lock_guard<::std::mutex>										mutex_guard;

} // namespace

#endif // APPLICATION_H_2078934982734
