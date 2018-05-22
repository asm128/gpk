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
	SRuntimeState												runtimeState						= {};
	{
		::gpk::view_const_string										fileName						= "Tutorial0.1";
		::gpk::SRuntimeModule											mainModule						= {};
		::gpk::loadRuntimeModule(mainModule, fileName);
		void															* applicationInstance			= {};																		
		gpk_necall(mainModule.Create(&applicationInstance, &globalRuntimeValues), "Failed to create main module. %s.");
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

		int WINAPI											WinMain
	( HINSTANCE		hInstance			
	, HINSTANCE		hPrevInstance		
	, LPSTR			lpCmdLine			
	, INT			nShowCmd			
	)
{
	::gpk::SRuntimeValues											runtimeValues					= {};
	runtimeValues.PlatformDetail.EntryPointArgs.hInstance		= hInstance		;
	runtimeValues.PlatformDetail.EntryPointArgs.hPrevInstance	= hPrevInstance	;
	runtimeValues.PlatformDetail.EntryPointArgs.lpCmdLine		= lpCmdLine		;
	runtimeValues.PlatformDetail.EntryPointArgs.nShowCmd		= nShowCmd		;
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}

		int													main							(int argc, char *argv[], char *envp[])			{
	argc, argv, envp;
	::gpk::SRuntimeValues											runtimeValues					= {};
	runtimeValues.PlatformDetail.EntryPointArgs.hInstance		=  GetModuleHandle(NULL);
	runtimeValues.PlatformDetail.EntryPointArgs.nShowCmd		=  SW_SHOW;
	return ::gpk::failed(::grt_Main(runtimeValues)) ? EXIT_FAILURE : EXIT_SUCCESS;
}

//int														main							(int argc, char *argv[])						{
//	return main(argc, argv, 0);
//}
