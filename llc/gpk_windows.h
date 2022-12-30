#include "gpk_array.h"

#ifndef GPK_WINDOWS_H_0289347890823
#define GPK_WINDOWS_H_0289347890823

namespace gpk
{
#if defined(GPK_WINDOWS)
	::gpk::apod<char_t>				getWindowsErrorAsString					(const int32_t lastError); // Get the error message, if any.
	::gpk::error_t							wcstombs								(::gpk::apod<char_t> & output, const ::gpk::view_array<wchar_t> input);
	::gpk::error_t							mbstowcs								(::gpk::apod<wchar_t> & output, const ::gpk::vcc input);
#endif
} // namespace

#endif // GPK_WINDOWS_H_0289347890823