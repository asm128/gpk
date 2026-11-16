#include "gpk_framework_module.h"
#include "gpk_string_compose.h"

gpk::err_t gpk::frameworkModuleUnload(SRuntimeModule & loadedModule) {
	if(0 == loadedModule.Handle)
		rtrn 0;
	if_zero_fe(GPK_FREE_MODULE(loadedModule.Handle));
	loadedModule = {};
	rtrn 0;
}
gpk::err_t gpk::frameworkModuleLoad(SRuntimeModule & loadedModule, vcst_c & moduleName) {
	string strModuleName;
	if_fail_fe(append_strings(strModuleName, moduleName));
	if_null_fef(loadedModule.Handle = GPK_LOAD_MODULE(strModuleName.begin()), "moduleName:%.*s.", GPK_VIEW_ARGS(moduleName));

	tydf SRuntimeModule::TRegistry TRegistry;
	cnst view<cnst STypeIdentifier> & functionNames	= TRegistry::get_types();
	cnst view<cnst DATA_TYPE>		& memberTypes	= TRegistry::get_data_type_ids();
	for(u2_t iMember = 0; iMember < memberTypes.size(); ++iMember) {
		if(DATA_TYPE_FUN != memberTypes[iMember])
			break;
		(&loadedModule.Handle)[iMember + 1] = GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, functionNames[iMember].Name.begin());
		if(0 == (&loadedModule.Handle)[iMember + 1]) {
			error_printf("Failed to load module function:%.*s.", GPK_VIEW_ARGS(functionNames[iMember].Name));
			GPK_FREE_MODULE(loadedModule.Handle);
			loadedModule = {};
			rtrn -1;
		}
	}
	rtrn 0;
}
