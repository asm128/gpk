#include "gpk_runtime_module.h"



			::gpk::error_t						gpk::loadRuntimeModule					(::gpk::SRuntimeModule& loadedModule, const ::gpk::view_const_string& moduleName)				{ 
	loadedModule.Handle								= GPK_LOAD_MODULE(moduleName.begin());
	ree_if(0 == loadedModule.Handle, "Cannot load module: %s.", moduleName.begin());
	loadedModule.Title								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleTitle		);
	loadedModule.Version							= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleVersion	);
	loadedModule.Create								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleCreate	);
	loadedModule.Delete								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleDelete	);
	loadedModule.Setup								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleSetup		);
	loadedModule.Cleanup							= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleCleanup	);
	loadedModule.Render								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleRender	);
	loadedModule.Update								= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, gpk_moduleUpdate	);
	return 0; 
}
