#include "gpk_log.h"

#include <Windows.h>
#include <stdlib.h>		// for EXIT_SUCCESS

namespace gpk
{
	struct SStartupParameters {
#if defined(GPK_WINDOWS)
		HINSTANCE		hInstance		;
		HINSTANCE		hPrevInstance	;
		LPSTR			cmdLine			;
		INT				nShowCmd		;
#endif
	};

	struct SRuntimeValues {
		SStartupParameters			StartupParameters;
	};
} // namespace 

int grt_Main(::gpk::SRuntimeValues& globalRuntimeValues) {
	globalRuntimeValues;
	return 0;
}

int WINAPI WinMain
	( HINSTANCE		hInstance
	, HINSTANCE		hPrevInstance
	, LPSTR			cmdLine	
	, INT			nShowCmd
	)
{
	::gpk::SRuntimeValues	runtimeValues = {
		{ hInstance
		, hPrevInstance
		, cmdLine	
		, nShowCmd
		},
	};
	return errored(grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main( int, char** )
{
	::gpk::SRuntimeValues	runtimeValues = {
		{ GetModuleHandle(NULL)
		, 0
		, 0
		, SW_SHOW
		},
	};
	return errored(grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}