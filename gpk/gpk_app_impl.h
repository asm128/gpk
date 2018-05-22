/// Copyright 2017-2018 - asm128
#include "gpk_runtime.h"
#include "gpk_ptr.h"

#if defined GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#	define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)																																																				\
		::gpk::error_t																				setup										(_mainClass& applicationInstance);																									\
		::gpk::error_t																				cleanup										(_mainClass& applicationInstance);																									\
		::gpk::error_t																				update										(_mainClass& applicationInstance, bool systemRequestedExit);																		\
		::gpk::error_t																				draw										(_mainClass& applicationInstance);																									\
		::gpk::error_t																				gpk_moduleVersion							()															{ return 1; }															\
		::gpk::error_t																				gpk_moduleCreate							(void**	instanceApp, ::gpk::SRuntimeValues* runtimeValues)	{ *instanceApp = new _mainClass{*runtimeValues}; return 0; }			\
		::gpk::error_t																				gpk_moduleDelete							(void**	instanceApp)										{ delete ((_mainClass*)*instanceApp); return 0; }						\
		::gpk::error_t																				gpk_moduleSetup								(void*	instanceApp)										{ return setup	(*(::SApplication*)instanceApp); }						\
		::gpk::error_t																				gpk_moduleCleanup							(void*	instanceApp)										{ return cleanup(*(::SApplication*)instanceApp); }						\
		::gpk::error_t																				gpk_moduleUpdate							(void*	instanceApp, bool systemRequestedExit)				{ return update	(*(::SApplication*)instanceApp, systemRequestedExit); }	\
		::gpk::error_t																				gpk_moduleRender							(void*	instanceApp)										{ return draw	(*(::SApplication*)instanceApp); }						\
		::gpk::error_t																				gpk_moduleTitle								(char* out_title, uint32_t *maxCount)						{																		\
	static constexpr const char mylmoduleTitle[] = _moduleTitle;									\
	if(0 == out_title) 																				\
		return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);	\
	memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));			\
	return 0;																						\
}	
#else
	enum APPLICATION_STATE : uint8_t
		{	APPLICATION_STATE_NORMAL							= 0
		,	APPLICATION_STATE_EXIT								= 1
		,	APPLICATION_STATE_BUSY								= 2
		,	APPLICATION_STATE_STAY								= 3
		};

	enum RUNTIME_CALLBACK_ID : uint8_t
		{	RUNTIME_CALLBACK_ID_NONE							= 0
		,	RUNTIME_CALLBACK_ID_TITLE							= 0x01
		,	RUNTIME_CALLBACK_ID_VERSION							= 0x02
		,	RUNTIME_CALLBACK_ID_CREATE							= 0x04
		,	RUNTIME_CALLBACK_ID_DELETE							= 0x08
		,	RUNTIME_CALLBACK_ID_SETUP							= 0x10
		,	RUNTIME_CALLBACK_ID_CLEANUP							= 0x20
		,	RUNTIME_CALLBACK_ID_RENDER							= 0x40
		,	RUNTIME_CALLBACK_ID_UPDATE							= 0x80
		};

#define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)																																													\
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
		::gpk::error_t																				setup										(_mainClass& applicationInstance);														\
		::gpk::error_t																				cleanup										(_mainClass& applicationInstance);														\
		::gpk::error_t																				update										(_mainClass& applicationInstance, bool systemRequestedExit);							\
		::gpk::error_t																				draw										(_mainClass& applicationInstance);														\
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
#endif // GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
