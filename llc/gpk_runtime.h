/// Copyright 2016-2018 - asm128
#include "gpk_args.h"
#include "gpk_enum.h"
#include "gpk_queue_event.h"
#include "gpk_system_event.h"

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
	struct SRuntimeValuesDetail {
#ifdef GPK_ANDROID
		ANativeActivity						* Activity		= {};
		void								* SavedState	= {};
		u2_t								SavedStateSize	= {};
#elif defined(GPK_WINDOWS)
		HINSTANCE				hInstance		{};
		HINSTANCE				hPrevInstance	{};
		LPSTR					lpCmdLine		{};
		int						nShowCmd		{};
#endif
	};
	struct SRuntimeValues {
		SRuntimeValuesDetail	PlatformDetail	= {};
		SCommandLineArgs		EntryPointArgs	= {};
		qpevt<SYSTEM_EVENT>		EventsIn		= {};	// Direction is relative to the application/module receiving these values.
		qpevt<SYSTEM_EVENT>		EventsOut		= {};	// Direction is relative to the application/module receiving these values.
	};

} // namespace

#if defined(GPK_WINDOWS)
#	define GPK_SYSTEM_OS_DEBUG_INIT_FLAGS()	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF)
#else
#	define GPK_SYSTEM_OS_DEBUG_INIT_FLAGS() do {} while(0)
#endif

#if defined(GPK_WINRT)
#	include <wrl.h>
#	define GPK_RO_INIT_MULTITHREADED() do { Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED); } while(0)
#else
#	define GPK_RO_INIT_MULTITHREADED() do {} while(0)
#endif

#define GPK_OS_ENTRY_POINT_COMMON_INIT(gpk_app_entry_point, main_argc, main_argv, main_envp)	\
	::gpk::setupDefaultLogCallbacks();															\
	GPK_SYSTEM_OS_DEBUG_INIT_FLAGS();															\
	if_true_fef(65535 < main_argc, "Invalid parameter count: %" GPK_FMT_U2 ".", main_argc);		\
	GPK_RO_INIT_MULTITHREADED();																\
	::gpk::SRuntimeValues		runtimeValues		= {};										\
	if_fail_fe(::gpk::argsParse(runtimeValues.EntryPointArgs, main_argc, main_argv, main_envp))	//

#define GPK_OS_ENTRY_POINT_ANY(gpk_app_entry_point)												\
	int						main				(int argc, char *argv[]) {						\
		GPK_OS_ENTRY_POINT_COMMON_INIT(gpk_app_entry_point, argc, argv, 0);						\
		return ::gpk::failed(gpk_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;	\
	} // main
#define GPK_OS_ENTRY_POINT_NOENVP(gpk_app_entry_point)											\
	int						main				(int argc, char *argv[], char *envp[]) {		\
		GPK_OS_ENTRY_POINT_COMMON_INIT(gpk_app_entry_point, argc, argv, envp);					\
		return ::gpk::failed(gpk_app_entry_point(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;	\
	} // main 

#ifndef GPK_WINDOWS
#	define GPK_OS_ENTRY_POINT	GPK_OS_ENTRY_POINT_ANY
#else // GPK_WINDOWS
#	define GPK_OS_ENTRY_POINT(gpk_app_entry_point)											\
	GPK_OS_ENTRY_POINT_ANY(gpk_app_entry_point);											\
	int	WINAPI				WinMain															\
		(	_In_		::HINSTANCE		hInstance											\
		,	_In_opt_	::HINSTANCE		hPrevInstance										\
		,	_In_		::LPSTR			lpCmdLine											\
		,	_In_		::INT			nShowCmd											\
		) {																					\
		GPK_OS_ENTRY_POINT_COMMON_INIT(gpk_app_entry_point, (__argc), (__argv), (environ));	\
		runtimeValues.PlatformDetail	= {hInstance, hPrevInstance, lpCmdLine, nShowCmd};	\
		if_fail_ve(EXIT_FAILURE, gpk_app_entry_point(runtimeValues));						\
		rtrn EXIT_SUCCESS;																	\
	} //
#endif // GPK_WINDOWS

#endif // GPK_RUNTIME_H_23627
