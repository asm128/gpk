#include "gpk_udp_client.h"
#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

struct SGUIConsole {
	int32_t																IdControl				= -1;
	::gpk::array_pod<char_t>											Contents;
	::gpk::array_obj<::gpk::view_const_string>							Lines;

	::gpk::error_t														PushLine				(const ::gpk::view_const_string & line)			{
		const int32_t															offset					= Contents.size();
		if(line.size()) {
			Contents.append(line.begin(), line.size());
			Lines.push_back({&Contents[offset], line.size()});
		}
		return 0;
	}
};

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

		::SGUIConsole														Console;

																			SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
