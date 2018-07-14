#include "gpk_windows.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>

		::gpk::array_pod<char_t>				gpk::getWindowsErrorAsString		(const int32_t lastError)					{	// Get the error message, if any.
	if(0 == lastError) 
		return {};
	char_t												* messageBuffer						= nullptr;
	const uint32_t										size								= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	retval_error_if({}, nullptr == messageBuffer, "FormatMessage() failed.")
	else {
		const ::gpk::array_pod<char_t>						message								= ::gpk::view_array<const char_t>{messageBuffer, size >= 2 ? size - 2 : size};
		LocalFree(messageBuffer); 
		return message;
	}
}

#endif