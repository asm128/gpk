/// Copyright 2016-2018 - asm128
#include "llc_args.h"
#include "llc_enum.h"

#if defined(LLC_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#elif defined(LLC_ANDROID)
#	include <android/native_activity.h>
#endif

#ifndef LLC_RUNTIME_H_23627
#define LLC_RUNTIME_H_23627

namespace llc
{
	struct SRuntimeValuesDetail {
#ifdef LLC_ANDROID
		ANativeActivity					* Activity			= nullptr;
		void							* SavedState		= nullptr;
		size_t							SavedStateSize		= 0;
#elif defined(LLC_WINDOWS)
		HINSTANCE						hInstance			;
		HINSTANCE						hPrevInstance		;
		LPSTR							lpCmdLine			;
		int								nShowCmd			;
#endif
	};

	struct SRuntimeValues {
		SRuntimeValuesDetail			PlatformDetail		;
		SCommandLineArgs				EntryPointArgs		;
	};
} // namespace

#ifdef LLC_WINDOWS
#	define LLC_SYSTEM_OS_DEBUG_INIT_FLAGS()	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
#else
#	define LLC_SYSTEM_OS_DEBUG_INIT_FLAGS() do {} while(0)
#endif

#ifndef LLC_WINDOWS
#	define LLC_SYSTEM_OS_ENTRY_POINT_NOENVP(llc_app_entry_point)									\
	int						main				(int argc, char *argv[])	{						\
		if_true_fef(65535 < argc, "Invalid parameter count: %" LLC_FMT_U2 ".", argc);				\
		::llc::SRuntimeValues		runtimeValues					= {};							\
		if_fail_fe(::llc::argsParse(runtimeValues.EntryPointArgs, argc, argv, 0));					\
		return ::llc::failed(llc_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;		\
	}
#	define LLC_SYSTEM_OS_ENTRY_POINT(llc_app_entry_point)											\
	int						main				(int argc, char *argv[], char *envp[])	{			\
		if_true_fef(65535 < argc, "Invalid parameter count: %" LLC_FMT_U2 ".", argc);				\
		::llc::SRuntimeValues		runtimeValues					= {};							\
		if_fail_fe(::llc::argsParse(runtimeValues.EntryPointArgs, argc, argv, envp));				\
		return ::llc::failed(llc_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;		\
	}
#else // LLC_WINDOWS
#	ifndef LLC_WINRT
#		define LLC_RO_INIT_MULTITHREADED() do {} while(0)
#	else
#		include <wrl.h>
#		define LLC_RO_INIT_MULTITHREADED() do { Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED); } while(0)
#	endif // LLC_WINRT

#	define LLC_SYSTEM_OS_ENTRY_POINT(llc_app_entry_point)														\
	int						main				(int argc, char *argv[], char *envp[])	{						\
		if_true_fef(65535 < (llc::u2_t)argc, "Invalid parameter count: %" LLC_FMT_U2 ".", (llc::u2_t)argc);		\
		LLC_RO_INIT_MULTITHREADED();																			\
		::llc::SRuntimeValues		runtimeValues		= {};													\
		if_fail_fe(::llc::argsParse(runtimeValues.EntryPointArgs, argc, argv, envp));							\
		runtimeValues.PlatformDetail		= {GetModuleHandle(NULL), 0, 0, SW_SHOW};							\
		return ::llc::failed(llc_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;					\
	}																											\
	int	WINAPI				WinMain																				\
		(	_In_		::HINSTANCE		hInstance																\
		,	_In_opt_	::HINSTANCE		hPrevInstance															\
		,	_In_		::LPSTR			lpCmdLine																\
		,	_In_		::INT			nShowCmd																\
		)																										\
	{																											\
		if_true_fef(65535 < (llc::u2_t)__argc, "Invalid parameter count: %" LLC_FMT_U2 ".", (llc::u2_t)__argc);	\
		LLC_RO_INIT_MULTITHREADED();																			\
		::llc::SRuntimeValues		runtimeValues		= {};													\
		if_fail_fe(::llc::argsParse(runtimeValues.EntryPointArgs, __argc, __argv, 0));							\
		runtimeValues.PlatformDetail		= {hInstance, hPrevInstance, lpCmdLine, nShowCmd};					\
		return ::llc::failed(llc_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;					\
	}
#endif // LLC_WINDOWS


#endif // LLC_RUNTIME_H_23627

