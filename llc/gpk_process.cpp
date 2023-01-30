#include "gpk_process.h"
#include "gpk_keyval.h"

#ifdef GPK_WINDOWS
#	include <Windows.h>
#else
#	include <unistd.h>
#endif

::gpk::error_t			gpk::environmentBlockViews		(const ::gpk::vcb & environmentBlock, ::gpk::aobj<::gpk::TKeyValConstString> & out_Views)	{
	uint32_t					offsetEndVar					= 0;
	while(offsetEndVar < environmentBlock.size() - 1) {
		uint32_t					offsetBeginVar					= offsetEndVar + (offsetEndVar ? 1 : 0);
		if(environmentBlock.size() - 1 == (offsetEndVar = ::gpk::find('\0', environmentBlock, offsetBeginVar)))
			break;
		::gpk::vcc		viewEnvironVar					= {}; // {&environmentBlock[offsetBeginVar], offsetEndVar - offsetBeginVar};
		gpk_necall(environmentBlock.slice(viewEnvironVar, offsetBeginVar, offsetEndVar - offsetBeginVar), "%s", "");
		::gpk::TKeyValConstString	newKeyVal						= {};
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

::gpk::error_t			gpk::environmentBlockFromEnviron(::gpk::apod<byte_t> & environmentBlock)	{
    for (char **env = environ; *env; ++env)
		environmentBlock.append(*env, (uint32_t)strlen(*env) + 1);
	environmentBlock.push_back(0);

	// The following function retrieves the process's environment block using GetEnvironmentStrings and adds it to the environmentBlock parameter.
	//char						* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block.
	//ree_if(lpvEnv == NULL, "GetEnvironmentStringsA failed (%x)\n", (uint32_t)GetLastError());
	//int32_t						lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view<const char_t>{"\0", 2U}, ::gpk::view<const char_t>{(const char*)lpvEnv, (uint32_t)-1});
	//environmentBlock		= ::gpk::view<const char_t>{lpvEnv, (uint32_t)lengthEnvStrings + 2U};
	//FreeEnvironmentStringsA(lpvEnv);
	return 0;
}
