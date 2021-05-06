#include "gpk_runtime_module.h"
#include "gpk_type_registry.h"
#include "gpk_sync.h"
#include "gpk_timer.h"
#include "gpk_process.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#	include <crtdbg.h>
#	include <process.h>
#endif
#include <cstdlib>		// for EXIT_SUCCESS

struct SRuntimeState {
			::gpk::refcount_t										RenderThreadUsers				= 0;
			::gpk::SRuntimeModule									* MainModule					= 0;
			bool													Running							= false;
			bool													RequestedExit					= false;
			bool													Exit							= false;
};

static	void													threadRender					(void* pRuntimeState)												{
	::SRuntimeState														& runtimeState					= *(::SRuntimeState*)pRuntimeState;
	::gpk::SRuntimeModule												* runtimeModule					= runtimeState.MainModule;
	re_if(0 == runtimeModule, "Main module is null!");
	gpk_sync_increment(runtimeState.RenderThreadUsers);
	int32_t																result								= 0;
	while(result = gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, 0, 1) - 1 && result != -1) {
		runtimeModule->Render(runtimeModule->Application);
		//::Sleep(1);
	}
	info_printf("Render thread done.");
}

static	int														threadRenderStart				(::SRuntimeState& runtimeState)										{
	_beginthread(threadRender, 0, &runtimeState);
	return 0;
}

static	int														grt_Loop						(SRuntimeState & runtimeState, ::gpk::SRuntimeModule & mainModule)	{
	::gpk::error_t														updateResult					= mainModule.Update(mainModule.Application, ::GetAsyncKeyState(VK_ESCAPE) != 0);
	if(1 == updateResult || errored(updateResult)) {
		ginfo_if(1 == updateResult, "%s", "Application requested termination.");
		gerror_if(errored(updateResult), "%s", "update() returned error.");
	}
	else {
		gpk_sync_increment(runtimeState.RenderThreadUsers);	// Report we're alive
		gpk_necall(::threadRenderStart(runtimeState), "%s", "Unknown error");
		info_printf("%s", "Application instance initialized successfully. Executing main loop...");
		while(true) {
			updateResult												= mainModule.Update(mainModule.Application, ::GetAsyncKeyState(VK_ESCAPE) != 0);
			break_ginfo_if(1 == updateResult, "Application requested termination.");
			break_gerror_if(errored(updateResult), "update() returned error.");
			//gerror_if(mainModule.Render(applicationInstance), "Why would this ever happen?");
			//Sleep(1);
		}
		gpk_sync_decrement(runtimeState.RenderThreadUsers);	// Report we're done
	}
	{
		::gpk::STimer														timer;
		double																elapsedTime						= 0;
		while(-1 != gpk_sync_compare_exchange(runtimeState.RenderThreadUsers, -1, 0)) {		// Wait until the render thread is closed.
			if(elapsedTime > 5.0f)
				break;
			::gpk::sleep(10);
			timer.Frame();
			//elapsedTime													+= timer.LastTimeSeconds;
		}
	}
	return updateResult;
}

static	int														grt_Main						(::gpk::SRuntimeValues& globalRuntimeValues)						{
	globalRuntimeValues;
#if defined(GPK_WINDOWS)
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif
	SRuntimeState														runtimeState					= {};
	{
		::gpk::view_const_string											fileName						= "gme";
		if(globalRuntimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine.size() > 1)
			fileName														= {globalRuntimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine[1], (uint32_t)strlen(globalRuntimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine[1])};
		char																mainModuleName	[512]			= {};
		snprintf(mainModuleName, ::gpk::size(mainModuleName) - 2, "%s.%s", fileName.begin(), GPK_MODULE_EXTENSION);
		::gpk::SRuntimeModule												mainModule						= {};
		gpk_necall(::gpk::loadRuntimeModule(mainModule, mainModuleName), "Failed to create main module. %s.", mainModuleName);
		info_printf("%s", "Created main module. %s.", mainModuleName);
		void																* applicationInstance;
		gpk_necall(mainModule.Create(&applicationInstance, &globalRuntimeValues), "Failed to instantiate main module class. %s.", mainModuleName);
		mainModule.Application											= applicationInstance;
		info_printf("%s", "Initializing application instance.");
		if errored(mainModule.Setup(applicationInstance)) {
			error_printf("%s", "Setup() Failed!");
			gpk_necall(mainModule.Delete(&applicationInstance), "Failed to create main module. %s.");
			return -1;
		}
		runtimeState.MainModule											= &mainModule;
		::grt_Loop(runtimeState, mainModule);
		info_printf("%s", "Cleaning up application instance...");
		gerror_if(errored(mainModule.Cleanup(applicationInstance)), "%s", "Failed.");
		info_printf("%s", "Application instance destroyed.");
		gpk_necall(mainModule.Delete(&applicationInstance), "");
	}
	return 0;
}

#if defined(GPK_WINDOWS)
		int WINAPI												WinMain
	( _In_		HINSTANCE		hInstance
	, _In_opt_	HINSTANCE		hPrevInstance
	, _In_		LPSTR			lpCmdLine
	, _In_		INT				nShowCmd
	)
{
	const uint32_t														argc							= __argc;
	const char															** argv							= (const char**)__argv;
	const char															** envp							= (const char**)_environ;
	ree_if(65535 < argc, "Invalid parameter count: %u.", argc);
	::gpk::STypeRegistry												& typeRegistry						= ::gpk::typeRegistrySingleton();
	for(uint32_t iType = 0; iType < typeRegistry.Names.size(); ++iType)
		info_printf("Type at index %u: '%s'.", iType, typeRegistry.Names[iType].begin());

	::gpk::SRuntimeValues												runtimeValues					= {};
	runtimeValues.PlatformDetail.EntryPointArgsWin.hInstance		= hInstance		;
	runtimeValues.PlatformDetail.EntryPointArgsWin.hPrevInstance	= hPrevInstance	;
	runtimeValues.PlatformDetail.EntryPointArgsWin.lpCmdLine		= lpCmdLine		;
	runtimeValues.PlatformDetail.EntryPointArgsWin.nShowCmd			= nShowCmd		;
	runtimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine	= {argv, argc};
	(void)envp;
	::gpk::environmentBlockFromEnviron(runtimeValues.PlatformDetail.EntryPointArgsStd.EnvironmentBlock);
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}
#endif // defined(GPK_WINDOWS)

		int														main							(int argc, char *argv[], char *envp[])			{
	ree_if(65535 < argc, "Invalid parameter count: %u.", argc);
	::gpk::SRuntimeValues												runtimeValues					= {};
#if defined(GPK_WINDOWS)
	runtimeValues.PlatformDetail.EntryPointArgsWin					= {GetModuleHandle(NULL), 0, 0, SW_SHOW};
#endif // defined(GPK_WINDOWS)
	runtimeValues.PlatformDetail.EntryPointArgsStd.ArgsCommandLine	= {(const char**)argv, (uint32_t)argc};
	(void)envp;
	::gpk::environmentBlockFromEnviron(runtimeValues.PlatformDetail.EntryPointArgsStd.EnvironmentBlock);
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}
