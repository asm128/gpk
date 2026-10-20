#include "gpk_array.h"

#ifndef GPK_WINDOWS_H
#define GPK_WINDOWS_H

namespace gpk
{
#ifdef GPK_WINDOWS
	apod<char>	getWindowsErrorAsString	(const int32_t lastError); // Get the error message, if any.
	error_t		wcstombs				(apod<char> & output, const view<wchar_t> input);
	error_t		mbstowcs				(apod<wchar_t> & output, vcs0_t input);
#endif
} // namespace

#endif // GPK_WINDOWS_H

