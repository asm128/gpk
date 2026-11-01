#include "gpk_cgi_module.h"
#include "gpk_module.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif

gpk::err_t		gpk::loadCGIModule				(SCGIModule & loadedModule, vcst_t moduleName)				{
	if_null_fef(loadedModule.Handle = GPK_LOAD_MODULE(moduleName.begin()), "Cannot load module: %s.", moduleName.begin());
	typedef	SCGIModule::TRegistry				TRegistry;
	const view<const STypeIdentifier>		funcnames			= TRegistry::get_types();
	const view<const DATA_TYPE>			members			= TRegistry::get_data_type_ids();
	for(u2_t iMember = 0, countMembers = members.size(); iMember < countMembers; ++iMember) {
		if(members[iMember] == DATA_TYPE_FUN) {
			(&loadedModule.Handle)[iMember + 1]					= GPK_LOAD_MODULE_FUNCTION(loadedModule.Handle, funcnames[iMember].Name.begin());
			if(0 == (&loadedModule.Handle)[iMember + 1])
				error_printf("Failed to load module function: %s.", funcnames[iMember].Name.begin());
		}
		else
			break;
	}
	return 0;
}
