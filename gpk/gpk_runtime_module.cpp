#include "gpk_runtime_module.h"

#define	GPK_LOAD_MODULE(moduleName)						LoadLibraryA	(moduleName)
#define	GPK_LOAD_FUNCTION(moduleHandle, symbolName)		((GPK_MODULE_FUNCTION_NAME(symbolName))GetProcAddress((HMODULE)moduleHandle, #symbolName))
#if defined GPK_WINDOWS
#	define GPK_MODULE_EXTENSION "dll"
#else
#	define GPK_MODULE_EXTENSION "os"
#endif
			::gpk::error_t						gpk::loadRuntimeModule					(::gpk::SRuntimeModule& loadedModule, const ::gpk::view_const_string& moduleName)				{ 
	char												compositeName	[512]					= {};
	sprintf_s(compositeName, "%s.%s", moduleName.begin(), GPK_MODULE_EXTENSION);
	loadedModule.Handle								= GPK_LOAD_MODULE(compositeName);
	loadedModule.Title								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleTitle	);
	loadedModule.Version							= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleVersion	);
	loadedModule.Create								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleCreate	);
	loadedModule.Delete								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleDelete	);
	loadedModule.Setup								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleSetup	);
	loadedModule.Cleanup							= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleCleanup	);
	loadedModule.Render								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleRender	);
	loadedModule.Update								= GPK_LOAD_FUNCTION(loadedModule.Handle, gpk_moduleUpdate	);
	return 0; 
}
