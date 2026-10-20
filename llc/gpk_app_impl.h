/// Copyright 2017-2018 - asm128
#include "gpk_module.h"
#include "gpk_ptr_obj.h"
#include "gpk_process.h"
#include "gpk_runtime_module.h"
#include "gpk_timer.h"


#if defined GPK_AVOID_LOCAL_APPLICATION_MODEL_EXECUTABLE_RUNTIME
#		define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)		GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle)
#else
#	define GPK_DEFINE_APPLICATION_ENTRY_POINT(_mainClass, _moduleTitle)										\
namespace gpk																								\
{																											\
	struct SRuntimeState {																					\
		refcount_t			RenderThreadUsers	= {};														\
		pobj<_mainClass>	Application			= {};														\
		b8_t				RequestedExit		= {};														\
	};																										\
}																											\
		GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle);											\
		static	::gpk::err_t	rtMain		(::gpk::SRuntimeValues & runtimeValues);						\
		GPK_OS_ENTRY_POINT(rtMain);																			\
		::gpk::err_t			rtMain		(::gpk::SRuntimeValues & runtimeValues)	{						\
		{																									\
			GPK_SYSTEM_OS_DEBUG_INIT_FLAGS();																\
			{																								\
				::gpk::pobj<_mainClass>	app;																\
				if_true_fe(0 == app.create(runtimeValues));													\
				info_printf("%s", "Initializing application instance.");									\
				gpk_necs(::setup(*app));																	\
				info_printf("%s", "Application instance initialized successfully. Executing main loop...");	\
				while(true) {																				\
					::gpk::err_t			updateResult		= ::update(*app, false);					\
					if_true_bi(::gpk::APPLICATION_STATE_EXIT == updateResult);								\
					if_true_be(::gpk::failed(updateResult));												\
					if_true_e(::draw(*app));																\
				}																							\
				info_printf("%s", "Cleaning up application instance...");									\
				::cleanup(*app);																			\
			}																								\
			info_printf("%s", "Application instance destroyed.");											\
		}																									\
		return 0;																							\
	} // 
#	define GPK_DEFINE_APPLICATION_ENTRY_POINT_MT(_mainClass, _moduleTitle)										\
	namespace gpk																								\
	{																											\
		struct SRuntimeState {																					\
			refcount_t			RenderThreadUsers	= {};														\
			pobj<_mainClass>	Application			= {};														\
			b8_t				RequestedExit		= {};														\
		};																										\
	}																											\
 	GPK_DEFINE_APP_MODULE_INTERFACE(_mainClass, _moduleTitle);													\
	static	::gpk::err_t	rtMain		(::gpk::SRuntimeValues & runtimeValues);								\
	GPK_OS_ENTRY_POINT(rtMain);																				\
																												\
	static	void				threadRender		(void * pRuntimeState)	{									\
		::gpk::SRuntimeState			& runtimeState		= *(::gpk::SRuntimeState*)pRuntimeState;			\
		if_true_ref(!runtimeState.Application, "Main module is null:(%s)\"%s\"", #_mainClass, _moduleTitle);	\
		gpk_sync_increment(runtimeState.RenderThreadUsers);														\
		int32_t							result				= 0;									\
		/*::gpk::sleep(1);*/																					\
		while(result = gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, 0, 1) - 1 && result != -1) {	\
			::draw(*runtimeState.Application);																	\
		}																										\
		info_printf("%s", "Render thread done.");																\
	}																											\
																												\
	static	::gpk::err_t	threadRenderStart	(::gpk::SRuntimeState& runtimeState)	{						\
		_beginthread(threadRender, 0, &runtimeState);															\
		return 0;																								\
	}																											\
																												\
	static	::gpk::err_t	lrt_Loop			(::gpk::SRuntimeState & runtimeState)	{						\
		::gpk::err_t				updateResult	= ::update(*runtimeState.Application, false);				\
		if(1 == updateResult || ::gpk::failed(updateResult)) {													\
			if_true_i(::gpk::APPLICATION_STATE_EXIT == updateResult);											\
			if_true_e(::gpk::failed(updateResult));																\
		}																										\
		else {																									\
			gpk_sync_increment(runtimeState.RenderThreadUsers);	/* Report we're alive	*/						\
			gpk_necs(::threadRenderStart(runtimeState));														\
			info_printf("%s", "Application instance initialized successfully. Executing main loop...");			\
			while(true) {																						\
				updateResult	= ::update(*runtimeState.Application, false);									\
				if_true_bi(::gpk::APPLICATION_STATE_EXIT == updateResult);										\
				if_true_be(::gpk::failed(updateResult));														\
				/*if_true_ef(mainModule.Render(app), "Why would this ever happen?");	*/						\
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
	sttc	::gpk::err_t	rtMain			(::gpk::SRuntimeValues & runtimeValues)	{	\
		::gpk::SRuntimeState		runtimeState	= {};								\
		{																				\
			if_null_fe(runtimeState.Application.create(runtimeValues));					\
			info_printf("%s", "Initializing application instance.");					\
			if(::gpk::failed(::setup(*runtimeState.Application))) {						\
				error_printf("%s", "Setup() Failed!");									\
				gpk_necs(::cleanup(*runtimeState.Application));							\
				return -1;																\
			}																			\
			::lrt_Loop(runtimeState);													\
			info_printf("%s", "Cleaning up application instance...");					\
			gpk_necs(::cleanup(*runtimeState.Application));								\
		}																				\
		info_printf("%s", "Application instance destroyed.");							\
		return 0;																		\
	} //


#endif // GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME


