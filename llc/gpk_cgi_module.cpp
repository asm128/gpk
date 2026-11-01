#include "gpk_cgi_module.h"
#include "gpk_module.h"
#include "gpk_string_compose.h"

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif

gpk::err_t		gpk::loadCGIModule				(SCGIModule & loadedModule, vcst_t moduleName)				{
	string		moduleNameNullTerminated;
	if_fail_fe(append_strings(moduleNameNullTerminated, moduleName));
	if_null_fef(loadedModule.Handle = GPK_LOAD_MODULE(moduleNameNullTerminated.begin()), "Cannot load module: %.*s.", GPK_VIEW_ARGS(moduleNameNullTerminated));
	typedef	SCGIModule::TRegistry				TRegistry;
	const view<const STypeIdentifier>		funcnames			= TRegistry::get_types();
	const view<const DATA_TYPE>			members			= TRegistry::get_data_type_ids();
	for(u2_t iMember = 0, countMembers = members.size(); iMember < countMembers; ++iMember) {
		if(members[iMember] != DATA_TYPE_FUN)
			break;
		if_null_ef((&loadedModule.Handle)[iMember + 1], "Failed to load module function: %.*s.", GPK_VIEW_ARGS(funcnames[iMember].Name));
	}
	return 0;
}
