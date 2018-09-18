#include "gpk_udp_client.h"
#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen							= {};

		int32_t																IdExit								= -1;
		int32_t																IdText								= -1;
		::gpk::array_pod<char_t>											StringTest;

		::std::mutex														LockGUI;
		::std::mutex														LockRender;

		::gpk::SUDPClient													Client;

																			SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};

	typedef ::std::lock_guard<::std::mutex>									mutex_guard;

} // namespace

#endif // APPLICATION_H_2078934982734
