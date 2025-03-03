/// Copyright 2016-2018 - asm128
#include "gpk_array.h"
#if defined(GPK_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#elif defined(GPK_ANDROID)
#	include <android/native_activity.h>
#endif

#ifndef GPK_RUNTIME_H_23627
#define GPK_RUNTIME_H_23627

namespace gpk
{
#if defined(GPK_WINDOWS)
	struct SWindowsEntryPointArgs {
		HINSTANCE						hInstance			;
		HINSTANCE						hPrevInstance		;
		LPSTR							lpCmdLine			;
		int								nShowCmd			;
	};
#endif
	enum APPLICATION_STATE : uint8_t
		{	APPLICATION_STATE_NORMAL		= 0
		,	APPLICATION_STATE_EXIT			= 1
		,	APPLICATION_STATE_BUSY			= 2
		,	APPLICATION_STATE_STAY			= 3
		};

	enum RUNTIME_CALLBACK_ID : uint8_t
		{	RUNTIME_CALLBACK_ID_NONE		= 0
		,	RUNTIME_CALLBACK_ID_TITLE		= 0x01
		,	RUNTIME_CALLBACK_ID_VERSION		= 0x02
		,	RUNTIME_CALLBACK_ID_CREATE		= 0x04
		,	RUNTIME_CALLBACK_ID_DELETE		= 0x08
		,	RUNTIME_CALLBACK_ID_SETUP		= 0x10
		,	RUNTIME_CALLBACK_ID_CLEANUP		= 0x20
		,	RUNTIME_CALLBACK_ID_RENDER		= 0x40
		,	RUNTIME_CALLBACK_ID_UPDATE		= 0x80
		};

	struct SStandardEntryPointArgs {
		::gpk::view<const sc_t*>		ArgsCommandLine		;
		::gpk::apod<sc_t>				EnvironmentBlock	;
	};

	struct SRuntimeValuesDetail {
#if defined(GPK_ANDROID)
		ANativeActivity					* Activity			= nullptr;
		void							* SavedState		= nullptr;
		size_t							SavedStateSize		= 0;
#elif defined(GPK_WINDOWS)
		SWindowsEntryPointArgs			EntryPointArgsWin	;
		SStandardEntryPointArgs			EntryPointArgsStd	;
#else
		SStandardEntryPointArgs			EntryPointArgsStd	;
#endif
	};

	struct SRuntimeValues {
		SRuntimeValuesDetail			PlatformDetail		;
	};
}

#endif // GPK_RUNTIME_H_23627

