#include "gpk_process.h"
#include "gpk_keyval.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#else
#	include <unistd.h>
#endif

::gpk::error_t			gpk::environmentBlockViews		(vcs0_t environmentBlock, aobj<kv<vcst_t>> & out_Views)	{
	::gpk::aobj<::gpk::kv<::gpk::vcst_t>>	views						= {};
	for(::gpk::u2_t offsetBeginVar = 0; offsetBeginVar < environmentBlock.size(); ) {
		cnst ::gpk::err_t			offsetEndVar					= ::gpk::find('\0', environmentBlock, offsetBeginVar);
		if_true_fef(0 > offsetEndVar, "Unterminated environment variable at offset:%u.", offsetBeginVar);
		if((::gpk::u2_t)offsetEndVar == offsetBeginVar)
			break;
		::gpk::vcst_t					viewEnvironVar					= {};
		gpk_necs(environmentBlock.slice(viewEnvironVar, offsetBeginVar, offsetEndVar - offsetBeginVar));
		::gpk::TKeyValConstString	newKeyVal						= {};
		if(-1 != ::gpk::keyval_split({viewEnvironVar.begin(), viewEnvironVar.size()}, newKeyVal))
			gpk_necs(views.push_back(newKeyVal));
		else {
			newKeyVal.Key			= {viewEnvironVar.begin(), viewEnvironVar.size()};
			gpk_necs(views.push_back(newKeyVal));
		}
		offsetBeginVar				= offsetEndVar + 1;
	}
	gpk_necs(out_Views.append(views));
	return 0;
}

::gpk::error_t			gpk::environmentBlockFromEnviron(::gpk::string & environmentBlock)	{
#ifndef GPK_ATMEL
    for (sc_t **env = environ; *env; ++env)
		environmentBlock.append(*env, (::gpk::u2_t)strlen(*env) + 1);
#endif
	environmentBlock.push_back(0);

	// The following function retrieves the process's environment block using GetEnvironmentStrings and adds it to the environmentBlock parameter.
	//sc_t						* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block.
	//if_true_fef(lpvEnv == NULL, "GetEnvironmentStringsA failed (%x)\n", (::gpk::u2_t)GetLastError());
	//int32_t						lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view<const sc_t>{"\0", 2U}, ::gpk::view<const sc_t>{(const sc_t*)lpvEnv, (::gpk::u2_t)-1});
	//environmentBlock		= ::gpk::view<const sc_t>{lpvEnv, (::gpk::u2_t)lengthEnvStrings + 2U};
	//FreeEnvironmentStringsA(lpvEnv);
	return 0;
}


