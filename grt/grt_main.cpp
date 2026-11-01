#include "gpk_runtime_module.h"
#include "gpk_type_registry.h"
#include "gpk_sync.h"
#include "gpk_timer.h"
#include "gpk_app_impl.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#	include <crtdbg.h>
#	include <process.h>
#endif
#include <cstdlib>		// for EXIT_SUCCESS

using namespace gpk;


struct SRuntimeState {
	::gpk::refcount_t		RenderThreadUsers	= 0; 
	::gpk::SRuntimeModule	* MainModule		= 0; 
	bool					Running				= false;
	bool					RequestedExit		= false; 
	bool					Exit				= false; 
};

static	void			threadRender		(void* pRuntimeState)												{
	::SRuntimeState				& runtimeState		= *(::SRuntimeState*)pRuntimeState;
	::gpk::SRuntimeModule		* runtimeModule		= runtimeState.MainModule;
	if_null_re(runtimeModule);
	gpk_sync_increment(runtimeState.RenderThreadUsers);
	int32_t						result				= 0;
	while(result = gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, 0, 1) - 1 && result != -1) {
		runtimeModule->Render(runtimeModule->Application);
	}
	info_printf("Render thread done.");
}

static	int				threadRenderStart	(::SRuntimeState& runtimeState)										{
	_beginthread(threadRender, 0, &runtimeState);
	return 0;
}

static	int				grt_Loop			(SRuntimeState & runtimeState, ::gpk::SRuntimeModule & mainModule)	{
#if defined(GPK_WINDOWS)
	gpk::err_t 			updateResult		= mainModule.Update(mainModule.Application, ::GetAsyncKeyState(VK_ESCAPE) != 0);
#else
	gpk::err_t 			updateResult		= mainModule.Update(mainModule.Application, false);
#endif
	if(1 == updateResult || ::gpk::failed(updateResult)) {
		if_true_i(::gpk::APPLICATION_STATE_EXIT == updateResult);
		if_true_e(::gpk::failed(updateResult));
	}
	else {
		gpk_sync_increment(runtimeState.RenderThreadUsers);	// Report we're alive
		if_fail_fef(::threadRenderStart(runtimeState), "%s", "Unknown error");
		info_printf("%s", "Application instance initialized successfully. Executing main loop...");
		while(true) {
			if_fail_bef(updateResult = mainModule.Update(mainModule.Application, false), "updateResult:%i", updateResult);
			if_true_bi(gpk::APPLICATION_STATE_EXIT == updateResult);
			//if_true_ef(mainModule.Render(app), "Why would this ever happen?");
		}
		gpk_sync_decrement(runtimeState.RenderThreadUsers);	// Report we're done
	}
	{
		::gpk::STimer				timer;
		f3_t						elapsedTime						= 0;
		while(-1 != gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, -1, 0)) {		// Wait until the render thread is closed.
			if(elapsedTime > 5.0f)
				break;
			::gpk::sleep(10);
			timer.Frame();
			//elapsedTime			+= timer.LastTimeSeconds;
		}
	}
	return updateResult;
}

static	int					grt_Main						(::gpk::SRuntimeValues& globalRuntimeValues)						{
	SRuntimeState					runtimeState					= {};
	{
		::gpk::vcst_t						fileName						= "gme";
		if(globalRuntimeValues.EntryPointArgs.Positionals.size())
			fileName					= {globalRuntimeValues.EntryPointArgs.Positionals[0], (uint32_t)strlen(globalRuntimeValues.EntryPointArgs.Positionals[0])};
		char							mainModuleName	[512]			= {};
		snprintf(mainModuleName, ::gpk::size(mainModuleName) - 2, "%s.%s", fileName.begin(), GPK_MODULE_EXTENSION);
		::gpk::SRuntimeModule			mainModule						= {};
		if_fail_fef(gpk::loadRuntimeModule(mainModule, mainModuleName), "Failed to create main module. %s.", mainModuleName);
		info_printf("Created main module: '%s'.", mainModuleName);
		void							* app;
		if_fail_fef(mainModule.Create(&app, &globalRuntimeValues), "Failed to instantiate main module class. %s.", mainModuleName);
		mainModule.Application		= app;
		info_printf("%s", "Initializing application instance.");
		if(::gpk::failed(mainModule.Setup(app))) {
			error_printf("%s", "Setup() Failed!");
			if_fail_fef(mainModule.Delete(&app), "Failed to create main module. %s.", mainModuleName);
			return -1;
		}
		runtimeState.MainModule		= &mainModule;
		::grt_Loop(runtimeState, mainModule);
		info_printf("%s", "Cleaning up application instance...");
		if_true_e(::gpk::failed(mainModule.Cleanup(app)));
		info_printf("%s", "Application instance destroyed.");
		if_fail_fe(mainModule.Delete(&app));
	}
	return 0;
}

GPK_OS_ENTRY_POINT(grt_Main);
