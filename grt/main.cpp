#include "gpk_runtime_module.h"

#include <Windows.h>
#include <stdlib.h>		// for EXIT_SUCCESS
#include <crtdbg.h>

struct SRuntimeState {
	volatile long	RenderThreadUsers	= 0;
	bool			Running				;
	bool			RequestedExit		;
	bool			Exit				;
};

static	int													grt_Main						(::gpk::SRuntimeValues& globalRuntimeValues)	{
	globalRuntimeValues;
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
	SRuntimeState													runtimeState					= {};
	{
		::gpk::view_const_string										fileName						= "gme";
		if(globalRuntimeValues.PlatformDetail.EntryPointArgsStd.argc > 1) 
			fileName													= {globalRuntimeValues.PlatformDetail.EntryPointArgsStd.argv[1], (uint32_t)strlen(globalRuntimeValues.PlatformDetail.EntryPointArgsStd.argv[1])};
		char															mainModuleName	[512]			= {};
		sprintf_s(mainModuleName, "%s.%s", fileName.begin(), GPK_MODULE_EXTENSION);
		::gpk::SRuntimeModule											mainModule						= {};
		gpk_necall(::gpk::loadRuntimeModule(mainModule, mainModuleName), "Failed to create main module. %s.", mainModuleName);
		info_printf("Created main module. %s.", mainModuleName);
		void															* applicationInstance			= {};
		gpk_necall(mainModule.Create(&applicationInstance, &globalRuntimeValues), "Failed to instantiate main module class. %s.", mainModuleName);
		info_printf("Initializing application instance.");
		if errored(mainModule.Setup(applicationInstance)) {
			error_printf("Setup() Failed!");			
			gpk_necall(mainModule.Delete(&applicationInstance), "Failed to create main module. %s.");
			return -1;
		}
		info_printf("Application instance initialized successfully. Executing main loop...");
		while(true) {
			::gpk::error_t													updateResult					= mainModule.Update(applicationInstance, ::GetAsyncKeyState(VK_ESCAPE) != 0);	
			break_info_if(1 == updateResult, "Application requested termination.");
			break_error_if(errored(updateResult), "update() returned error.");
			error_if(mainModule.Render(applicationInstance), "Why would this ever happen?");
		}
		info_printf("Cleaning up application instance...");
		error_if(errored(mainModule.Cleanup(applicationInstance)), "Failed.");
		info_printf("Application instance destroyed.");
		gpk_necall(mainModule.Delete(&applicationInstance), "");
	}
	return 0;
}

#if defined(GPK_WINDOWS)
		int WINAPI											WinMain
	( _In_		HINSTANCE		hInstance			
	, _In_opt_	HINSTANCE		hPrevInstance		
	, _In_		LPSTR			lpCmdLine			
	, _In_		INT				nShowCmd			
	)
{
	::gpk::SRuntimeValues											runtimeValues					= {};
	runtimeValues.PlatformDetail.EntryPointArgsWin.hInstance		= hInstance		;
	runtimeValues.PlatformDetail.EntryPointArgsWin.hPrevInstance	= hPrevInstance	;
	runtimeValues.PlatformDetail.EntryPointArgsWin.lpCmdLine		= lpCmdLine		;
	runtimeValues.PlatformDetail.EntryPointArgsWin.nShowCmd			= nShowCmd		;
	runtimeValues.PlatformDetail.EntryPointArgsStd.argc				= __argc;
	runtimeValues.PlatformDetail.EntryPointArgsStd.argv				= __argv;
	runtimeValues.PlatformDetail.EntryPointArgsStd.envp				= _environ;
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}
#endif // defined(GPK_WINDOWS)

		int													main							(int argc, char *argv[], char *envp[])			{
	argc, argv, envp;
	::gpk::SRuntimeValues											runtimeValues					= {};
#if defined(GPK_WINDOWS)
	runtimeValues.PlatformDetail.EntryPointArgsWin				= {GetModuleHandle(NULL), 0, 0, SW_SHOW};
#endif // defined(GPK_WINDOWS)
	runtimeValues.PlatformDetail.EntryPointArgsStd				= {argc, argv, envp};
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}

//int														main							(int argc, char *argv[])						{
//	return main(argc, argv, 0);
//}
