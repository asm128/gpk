/// Copyright 2017-2018 - asm128
#include "gpk_module.h"
#include "gpk_ptr_obj.h"
#include "gpk_process.h"
#include "gpk_runtime_module.h"
#include "gpk_timer.h"

#define GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)				\
	::gpk::error_t	setup		(_mainClass& app);									\
	::gpk::error_t	cleanup		(_mainClass& app);									\
	::gpk::error_t	update		(_mainClass& app, bool systemRequestedExit);		\
	::gpk::error_t	draw		(_mainClass& app);									\
	::gpk::error_t	GPK_STDCALL	gpk_moduleVersion	()															{ return 1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleCreate	(void**	instanceApp, ::gpk::SRuntimeValues* runtimeValues)	{ try { *instanceApp = new _mainClass{*runtimeValues};												return 0;		} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleDelete	(void**	instanceApp)										{ try { delete ((_mainClass*)*instanceApp);	*instanceApp = 0;										return 0;		} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleSetup		(void*	instanceApp)										{ try { const ::gpk::error_t result = setup		(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleCleanup	(void*	instanceApp)										{ try { const ::gpk::error_t result = cleanup	(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleUpdate	(void*	instanceApp, bool systemRequestedExit)				{ try { const ::gpk::error_t result = update	(*(_mainClass*)instanceApp, systemRequestedExit);	return result;	} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleRender	(void*	instanceApp)										{ try { const ::gpk::error_t result = draw		(*(_mainClass*)instanceApp);						return result;	} catch(...) {} return -1; }	\
	::gpk::error_t	GPK_STDCALL	gpk_moduleTitle		(sc_t* out_title, uint32_t *maxCount)						{	\
		stacxpr	const sc_t mylmoduleTitle[] = _moduleTitle;												\
		if(0 == out_title) 																				\
			return maxCount ? (*maxCount = ::gpk::size(mylmoduleTitle)) : ::gpk::size(mylmoduleTitle);	\
		memcpy(out_title, mylmoduleTitle, ::gpk::min(::gpk::size(mylmoduleTitle), *maxCount));			\
		return 0;																						\
	}

#if defined GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#		define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)	\
			GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)
#else
#	if defined(GPK_WINDOWS)
#		define GPK_SYSTEM_OS_DEBUG_INIT_FLAGS()	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
#	else
#		define GPK_SYSTEM_OS_DEBUG_INIT_FLAGS() do {} while(0)
#	endif

#	if defined(GPK_WINRT)
#		include <wrl.h>
#		define GPK_RO_INIT_MULTITHREADED() do { Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED); } while(0)
#	else
#		define GPK_RO_INIT_MULTITHREADED() do {} while(0)
#	endif

#	if defined(GPK_WINDOWS)
#		define GPK_SYSTEM_OS_ENTRY_POINT()														\
			static	::gpk::error_t	rtMain				(::gpk::SRuntimeValues & runtimeValues);	\
			int						main				(int argc, sc_t *argv[], sc_t *envp[])	{	\
				GPK_RO_INIT_MULTITHREADED();										\
				::gpk::SRuntimeValues		runtimeValues					= {};	\
				runtimeValues.PlatformDetail.EntryPointArgsWin					= {GetModuleHandle(NULL), 0, 0, SW_SHOW};								\
				runtimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine	= {(const sc_t**)argv, (uint32_t)argc};									\
				(void)envp;																																\
				::gpk::environmentBlockFromEnviron(runtimeValues.PlatformDetail.EntryPointArgsStd.EnvironmentBlock);											\
				return ::gpk::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																	\
			}																																					\
					int	WINAPI														WinMain																		\
				(	_In_		::HINSTANCE		hInstance																										\
				,	_In_opt_	::HINSTANCE		hPrevInstance																									\
				,	_In_		::LPSTR			lpCmdLine																										\
				,	_In_		::INT			nShowCmd																										\
				)																																				\
			{																																					\
				const uint32_t																argc							= __argc;							\
				const sc_t																	** argv							= (const sc_t**)__argv;				\
				ree_if(65535 < argc, "Invalid parameter count: %" GPK_FMT_U32 ".", argc);																						\
				::gpk::SRuntimeValues														runtimeValues					= {};								\
				runtimeValues.PlatformDetail.EntryPointArgsWin.hInstance				= hInstance		;														\
				runtimeValues.PlatformDetail.EntryPointArgsWin.hPrevInstance			= hPrevInstance	;														\
				runtimeValues.PlatformDetail.EntryPointArgsWin.lpCmdLine				= lpCmdLine		;														\
				runtimeValues.PlatformDetail.EntryPointArgsWin.nShowCmd					= nShowCmd		;														\
				runtimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine			= ::gpk::view<const sc_t*>{argv, argc};									\
				::gpk::environmentBlockFromEnviron(runtimeValues.PlatformDetail.EntryPointArgsStd.EnvironmentBlock);											\
				return ::gpk::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																	\
			}
#	else
#		define GPK_SYSTEM_OS_ENTRY_POINT()																													\
				int																main							(int argc, sc_t *argv[], sc_t *envp[])	{	\
			ree_if(65535 < argc, "Invalid parameter count: %" GPK_FMT_U32 ".", argc);																						\
			::gpk::SRuntimeValues														runtimeValues					= {};								\
			runtimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine			= ::gpk::view<const sc_t*>{(const sc_t**)argv, (uint32_t)argc};			\
			(void)envp;																																		\
			::gpk::environmentBlockFromEnviron(runtimeValues.PlatformDetail.EntryPointArgsStd.EnvironmentBlock);											\
			return ::gpk::failed(::rtMain(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;																	\
		}
#	endif

#	define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)									\
		GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)											\
		GPK_SYSTEM_OS_ENTRY_POINT()																			\
		::gpk::error_t			rtMain		(::gpk::SRuntimeValues & runtimeValues)	{						\
		{																									\
			GPK_SYSTEM_OS_DEBUG_INIT_FLAGS();																\
			{																								\
				::gpk::pobj<_mainClass>	app;																\
				rees_if(0 == app.create(runtimeValues));													\
				info_printf("%s", "Initializing application instance.");									\
				gpk_necs(::setup(*app));																	\
				info_printf("%s", "Application instance initialized successfully. Executing main loop...");	\
				while(true) {																				\
					::gpk::error_t			updateResult		= ::update(*app, false);					\
					bi_if(1 == updateResult);																\
					be_if(::gpk::failed(updateResult));														\
					e_if(::draw(*app));																		\
				}																							\
				info_printf("%s", "Cleaning up application instance...");									\
				::cleanup(*app);																			\
			}																								\
			info_printf("%s", "Application instance destroyed.");											\
		}																									\
		return 0;																							\
	}

#	define GPK_DEFINE_APPLICATION_ENTRY_POINT_MT(_mainClass, _moduleTitle)											\
		static	::gpk::error_t	rtMain		(::gpk::SRuntimeValues & runtimeValues);								\
		GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)													\
		GPK_SYSTEM_OS_ENTRY_POINT()																					\
																													\
		struct SRuntimeState {																						\
			::gpk::refcount_t		RenderThreadUsers	= 0;														\
			::gpk::pobj<_mainClass>	Application			= {};														\
			bool					Running				= false;													\
			bool					RequestedExit		= false;													\
			bool					Exit				= false;													\
		};																											\
																													\
		static	void				threadRender					(void* pRuntimeState)	{						\
			::SRuntimeState					& runtimeState					= *(::SRuntimeState*)pRuntimeState;		\
			re_if(!runtimeState.Application, "%s", "Main module is null!");											\
			gpk_sync_increment(runtimeState.RenderThreadUsers);														\
			int32_t			result								= 0;												\
			/*::gpk::sleep(1);*/																					\
			while(result = gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, 0, 1) - 1 && result != -1) {	\
				::draw(*runtimeState.Application);																	\
			}																										\
			info_printf("%s", "Render thread done.");																\
		}																											\
																													\
		static	::gpk::error_t	threadRenderStart	(::SRuntimeState& runtimeState)	{								\
			_beginthread(threadRender, 0, &runtimeState);															\
			return 0;																								\
		}																											\
																													\
		static	::gpk::error_t	grt_Loop		(::SRuntimeState & runtimeState)	{								\
			::gpk::error_t				updateResult	= ::update(*runtimeState.Application, false);				\
			if(1 == updateResult || failed(updateResult)) {														\
				is_if(::gpk::APPLICATION_STATE_EXIT == updateResult);												\
				es_if(failed(updateResult));																		\
			}																										\
			else {																									\
				gpk_sync_increment(runtimeState.RenderThreadUsers);	/* Report we're alive	*/						\
				gpk_necs(::threadRenderStart(runtimeState));														\
				info_printf("%s", "Application instance initialized successfully. Executing main loop...");			\
				while(true) {																						\
					updateResult	= ::update(*runtimeState.Application, false);									\
					bis_if(::gpk::APPLICATION_STATE_EXIT == updateResult);											\
					bes_if(failed(updateResult));																	\
					/*ef_if(mainModule.Render(app), "Why would this ever happen?");	*/							\
					/*Sleep(1);		*/																				\
				}																									\
				gpk_sync_decrement(runtimeState.RenderThreadUsers);	/* Report we're done	*/						\
			}																										\
			{																										\
				::gpk::STimer	timer;																				\
				double			elapsedTime		= 0;																\
				while(-1 != gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, -1, 0)) {	/* Wait until the render thread is closed.	*/	\
					if(elapsedTime > 5.0f)						\
						break;									\
					::gpk::sleep(10);							\
					timer.Frame();								\
					/*elapsedTime += timer.LastTimeSeconds;	*/	\
				}												\
			}													\
			return updateResult;								\
		}														\
																\
		static	::gpk::error_t	rtMain			(::gpk::SRuntimeValues & runtimeValues)	{	\
			GPK_SYSTEM_OS_DEBUG_INIT_FLAGS();									\
			SRuntimeState				runtimeState	= {};					\
			{																	\
				rees_if(0 == runtimeState.Application.create(runtimeValues));	\
				info_printf("%s", "Initializing application instance.");		\
				if(failed(::setup(*runtimeState.Application))) {				\
					error_printf("%s", "Setup() Failed!");						\
					gpk_necs(::cleanup(*runtimeState.Application));				\
					return -1;													\
				}																\
				::grt_Loop(runtimeState);										\
				info_printf("%s", "Cleaning up application instance...");		\
				gpk_necs(::cleanup(*runtimeState.Application));					\
			}																	\
			info_printf("%s", "Application instance destroyed.");				\
			return 0;															\
		}																		\


#endif // GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
