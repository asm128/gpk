#include "gpk_array.h"
#include "gpk_string.h"

#ifndef GPK_WINDOWS_H
#define GPK_WINDOWS_H

nmsp gpk
{
#ifdef GPK_WINDOWS
	string		getWindowsErrorAsString	(s2_t lastError); // Get the error message, if any.
	err_t		wcstombs				(string & output, view<const wchar_t> input);
	err_t		mbstowcs				(apod<wchar_t> & output, vcst_t input);
#endif
} // nmsp

#endif // GPK_WINDOWS_H