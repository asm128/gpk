#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_stdsocket.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

enum UDP_CONNECTION_STATE 
	{ UDP_CONNECTION_STATE_DISCONNECTED		= 0
	, UDP_CONNECTION_STATE_HANDSHAKE
	, UDP_CONNECTION_STATE_IDLE
	};

struct SUDPClient {
	::gpk::auto_socket_close				Socket										= {};
	::gpk::SIPv4							AddressServer								= {};
	uint16_t								PortServer									= {};
	UDP_CONNECTION_STATE					State										= UDP_CONNECTION_STATE_DISCONNECTED;
};

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::SImage<::gpk::SColorBGRA>									TextureFont							= {};
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen							= {};

		int32_t																IdExit								= -1;

		::std::mutex														LockGUI;
		::std::mutex														LockRender;

		::SUDPClient														Client;

																			SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};

	typedef ::std::lock_guard<::std::mutex>									mutex_guard;

} // namespace

#endif // APPLICATION_H_2078934982734
