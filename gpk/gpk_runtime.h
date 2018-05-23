/// Copyright 2016-2018 - asm128
#include "gpk_platform.h"
#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif

#ifndef GPK_RUNTIME_H_28903749028374
#define GPK_RUNTIME_H_28903749028374

namespace gpk 
{
#if defined(GPK_WINDOWS)
	struct SWindowsEntryPointArgs {
				HINSTANCE														hInstance							;
				HINSTANCE														hPrevInstance						;
				LPSTR															lpCmdLine							;
				int																nShowCmd							;
	};
#endif

	struct SStandardEntryPointArgs {
				int																argc								;
				char															**argv								;
				char															**envp								;
	};
	struct SRuntimeValuesDetail {	
#if defined(GPK_ANDROID)
				ANativeActivity													* Activity							= nullptr;
				void															* SavedState						= nullptr;
				size_t															SavedStateSize						= 0;
#elif defined(GPK_WINDOWS)
				SWindowsEntryPointArgs											EntryPointArgsWin					;
				SStandardEntryPointArgs											EntryPointArgsStd					;
#else
				SStandardEntryPointArgs											EntryPointArgsStd					;
#endif
	};

	struct SRuntimeValues {
				SRuntimeValuesDetail											PlatformDetail						;
	};
}

#endif // GPK_RUNTIME_H_28903749028374

