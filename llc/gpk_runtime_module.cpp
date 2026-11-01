#include "gpk_runtime_module.h"

::gpk::err_t			gpk::loadRuntimeModule					(::gpk::SRuntimeModule& loadedModule, const ::gpk::vcst_t& moduleName)				{
	loadedModule.Handle								= GPK_LOAD_MODULE(moduleName.begin());
	if_true_vef(-1, 0 == loadedModule.Handle, "Cannot load module: %s.", moduleName.begin());
	typedef ::gpk::SRuntimeModule::TRegistry TRegistry;
	const ::gpk::view<const ::gpk::STypeIdentifier>	& funcnames		= TRegistry::get_types();
	const ::gpk::view<const ::gpk::DATA_TYPE>			& members		= TRegistry::get_data_type_ids();
	for(u2_t iMember = 0, countMembers = members.size(); iMember < countMembers; ++iMember) {
		if(members[iMember] == ::gpk::DATA_TYPE_FUN) {
			(&loadedModule.Handle)[iMember + 1]				= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, funcnames[iMember].Name.begin());
			if(0 == (&loadedModule.Handle)[iMember + 1])
				error_printf("Failed to load module function: %s.", funcnames[iMember].Name.begin());
		}
		else
			break;
	}
	return 0;
}
