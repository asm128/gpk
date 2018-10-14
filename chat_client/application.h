#include "gpk_udp_client.h"
#include "gpk_framework.h"
#include "gpk_gui.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

struct SGUIConsoleBuffer {
	::gpk::array_pod<char_t>											Contents;
	::gpk::array_obj<::gpk::view_const_string>							Lines;
	::gpk::error_t														PushLine							(const ::gpk::view_const_string & line)			{
		if(line.size()) {
			int32_t																	offset								= Contents.size();
			Contents.append(line.begin(), line.size());
			if(Lines.size() >= 256) 
				Lines.remove(0);
			Lines.push_back({&Contents[offset], line.size()});
			offset																= 0;
			for(uint32_t iLine = 0; iLine < Lines.size(); ++iLine) {
				Lines[iLine]														= {&Contents[offset], Lines[iLine].size()};
				offset																+= Lines[iLine].size();
			}
		}
		return 0;
	}
};

struct SGUIConsole {
	::SGUIConsoleBuffer													Buffer								= {};
	::gpk::array_pod<int32_t>											IdConsoleLines						= {};
	int32_t																IdInput								= -1;
	int32_t																IdControl							= -1;
	int32_t																IdCursor							= -1;
	int32_t																Cursor								= 0;
};

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework													Framework;
		::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen							= {};

		int32_t																IdExit								= -1;
		int32_t																IdConsole							= -1;
		int32_t																IdConsole1							= -1;
		int32_t																IdConsole2							= -1;

		::gpk::array_pod<char_t>											StringTest;

		::std::mutex														LockGUI;
		::std::mutex														LockRender;

		::gpk::SUDPClient													Client;

		::SGUIConsole														Console;

																			SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_2078934982734
