#include "gpk_process.h"
#include "gpk_find.h"

#ifdef GPK_WINDOWS
#	include <windows.h>
#else
#	include <unistd.h>
#endif

// The following function retrieves the process's environment block using GetEnvironmentStrings and adds it to the environmentBlock parameter. 
::gpk::error_t					gpk::getEnvironmentBlock		(::gpk::array_pod<char_t> & environmentBlock)		{
    for (char **env = environ; *env; ++env) 
		environmentBlock.append(*env, (uint32_t)strlen(*env) + 1);
	environmentBlock.push_back(0);
	//char								* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block. 
	//ree_if(lpvEnv == NULL, "GetEnvironmentStringsA failed (%x)\n", (uint32_t)GetLastError());	
	//int32_t								lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view_array<const char_t>{"\0", 2U}, ::gpk::view_array<const char_t>{(const char*)lpvEnv, (uint32_t)-1});
	//environmentBlock				= ::gpk::view_array<const char_t>{lpvEnv, (uint32_t)lengthEnvStrings + 2U};
	//FreeEnvironmentStringsA(lpvEnv);
	return 0;
}
