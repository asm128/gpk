/// Copyright 2017-2018 - asm128
#include "gpk_runtime.h"
#include "gpk_ptr.h"

#define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass)																																													\
static	::gpk::error_t																				rtMain										(::gpk::SRuntimeValues& runtimeValues);													\
		int																							main										()																					{	\
	::gpk::SRuntimeValues																					runtimeValues								= {{::GetModuleHandle(NULL), 0, 0, SW_SHOW},};									\
	return ::gpk::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																																						\
}																																																										\
																																																										\
		int	WINAPI																					WinMain																																\
	(	_In_		::HINSTANCE		hInstance																																															\
	,	_In_opt_	::HINSTANCE		hPrevInstance																																														\
	,	_In_		::LPSTR			lpCmdLine																																															\
	,	_In_		::INT			nShowCmd																																															\
	)																																																									\
{																																																										\
	::gpk::SRuntimeValues																					runtimeValues								=																				\
		{{	hInstance																																																					\
		,	hPrevInstance																																																				\
		,	lpCmdLine																																																					\
		,	nShowCmd																																																					\
		},};																																																							\
	return ::gpk::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																																						\
}																																																										\
																																																										\
static	::gpk::error_t																				setup										(_mainClass& applicationInstance);														\
static	::gpk::error_t																				cleanup										(_mainClass& applicationInstance);														\
static	::gpk::error_t																				update										(_mainClass& applicationInstance, bool systemRequestedExit);							\
static	::gpk::error_t																				draw										(_mainClass& applicationInstance);														\
		::gpk::error_t																				rtMain										(::gpk::SRuntimeValues& runtimeValues)												{	\
	{																																																									\
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);																																									\
		::gpk::ptr_obj<_mainClass>																			applicationInstance							= {};																			\
		reterr_error_if(0 == applicationInstance.create(runtimeValues), "Failed to create application instance. Out of memory?");																										\
		info_printf("Initializing application instance.");																																												\
		gpk_necall(::setup(*applicationInstance), "User reported error. Execution stopped.");																																			\
		info_printf("Application instance initialized successfully. Executing main loop...");																																			\
		while(true) {																																																					\
			::gpk::error_t																						updateResult								= ::update(*applicationInstance, ::GetAsyncKeyState(VK_ESCAPE) != 0);		\
			break_info_if(1 == updateResult, "Application requested termination.");																																						\
			break_error_if(errored(updateResult), "update() returned error.");																																							\
			error_if(::draw(*applicationInstance), "Why would this ever happen?");																																						\
		}																																																								\
		info_printf("Cleaning up application instance...");																																												\
		::cleanup(*applicationInstance);																																																\
		info_printf("Application instance destroyed.");																																													\
	}																																																									\
	return 0;																																																							\
}
