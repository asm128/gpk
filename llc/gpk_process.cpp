#include "gpk_process.h"
#include "gpk_keyval.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#else
#	include <unistd.h>
#endif

gpk::err_t			gpk::environmentBlockViews		(const ::gpk::vcsc_t & environmentBlock, ::gpk::aobj<::gpk::kv<vcsc_t, vcsc_t>> & out_Views)	{
	::gpk::u2_t					offsetEndVar					= 0;
	while(offsetEndVar < environmentBlock.size() - 1) {
		::gpk::u2_t					offsetBeginVar					= offsetEndVar + (offsetEndVar ? 1 : 0);
		if(environmentBlock.size() - 1 == (offsetEndVar = ::gpk::find('\0', environmentBlock, offsetBeginVar)))
			break;
		::gpk::vcsc_t					viewEnvironVar					= {}; // {&environmentBlock[offsetBeginVar], offsetEndVar - offsetBeginVar};
		if_fail_fe(environmentBlock.slice(viewEnvironVar, offsetBeginVar, offsetEndVar - offsetBeginVar));
		::gpk::kv<vcsc_t, vcsc_t>	newKeyVal						= {};
		if(-1 != ::gpk::keyval_split({viewEnvironVar.begin(), viewEnvironVar.size()}, newKeyVal))
			out_Views.push_back(newKeyVal);
		else if(viewEnvironVar.size()) {
			newKeyVal.Key			= {viewEnvironVar.begin(), viewEnvironVar.size()};
			out_Views.push_back(newKeyVal);
			break;
		}
		else
			break;
	}
	return 0;
}

gpk::err_t			gpk::environmentBlockFromEnviron(::gpk::apod<sc_t> & environmentBlock)	{
#ifndef GPK_ATMEL
    for (sc_t **env = environ; *env; ++env)
		environmentBlock.append(*env, (::gpk::u2_t)strlen(*env) + 1);
#endif
	environmentBlock.push_back(0);

	// The following function retrieves the process's environment block using GetEnvironmentStrings and adds it to the environmentBlock parameter.
	//sc_t						* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block.
	//if_true_fef(lpvEnv == NULL, "GetEnvironmentStringsA failed (%x)\n", (::gpk::u2_t)GetLastError());
	//::gpk::s2_t						lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view<const sc_t>{"\0", 2U}, ::gpk::view<const sc_t>{(const sc_t*)lpvEnv, (::gpk::u2_t)-1});
	//environmentBlock		= ::gpk::view<const sc_t>{lpvEnv, (::gpk::u2_t)lengthEnvStrings + 2U};
	//FreeEnvironmentStringsA(lpvEnv);
	return 0;
}
