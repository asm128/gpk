#include "gpk_process.h"
#include "gpk_find.h"

#ifdef GPK_WINDOWS
#	include <windows.h>
#else
#	error "Not implemented"
#endif

// Example 3
// The following example retrieves the process's environment block using GetEnvironmentStrings and prints the contents to the console.
::gpk::error_t					gpk::getEnvironmentBlock		(::gpk::array_pod<char_t> & environmentBlock)		{
	char								* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block. 
	ree_if(lpvEnv == NULL, "GetEnvironmentStringsA failed (%d)\n", GetLastError());	
	int32_t								lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view_array<const char_t>{"\0", 2U}, ::gpk::view_array<const char_t>{(const char*)lpvEnv, (uint32_t)-1});
	environmentBlock				= ::gpk::view_array<const char_t>{lpvEnv, (uint32_t)lengthEnvStrings + 2U};
	FreeEnvironmentStringsA(lpvEnv);
	return 0;
}
