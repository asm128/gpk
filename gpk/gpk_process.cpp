#include "gpk_process.h"
#include "gpk_find.h"

#ifdef GPK_WINDOWS
#	include <windows.h>
#endif

// The following functions retrieves the process's environment block using GetEnvironmentStrings and prints the contents to the console. 
// On platforms other than windnows, we should get the variables some other way.
::gpk::error_t					gpk::getEnvironmentBlock		(::gpk::array_pod<char_t> & environmentBlock)		{
#ifdef GPK_WINDOWS
	char								* lpvEnv						= GetEnvironmentStringsA();	// Get a pointer to the environment block. 
	ree_if(lpvEnv == NULL, "GetEnvironmentStringsA failed (%x)\n", (uint32_t)GetLastError());	
	int32_t								lengthEnvStrings				= ::gpk::find_sequence_pod(::gpk::view_array<const char_t>{"\0", 2U}, ::gpk::view_array<const char_t>{(const char*)lpvEnv, (uint32_t)-1});
	environmentBlock				= ::gpk::view_array<const char_t>{lpvEnv, (uint32_t)lengthEnvStrings + 2U};
	FreeEnvironmentStringsA(lpvEnv);
#else
#	error "Not implemented"
#endif
	return 0;
}
