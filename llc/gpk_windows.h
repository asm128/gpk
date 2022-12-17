#include "gpk_array.h"

#ifndef GPK_WINDOWS_H_0289347890823
#define GPK_WINDOWS_H_0289347890823

namespace gpk
{
#if defined(GPK_WINDOWS)
	::gpk::array_pod<char_t>				getWindowsErrorAsString					(const int32_t lastError); // Get the error message, if any.
	::gpk::error_t							wcstombs								(::gpk::array_pod<char_t> & output, const ::gpk::view_array<wchar_t> input);
	::gpk::error_t							mbstowcs								(::gpk::array_pod<wchar_t> & output, const ::gpk::view_const_char input);

#endif
} // namespace

#endif // GPK_WINDOWS_H_0289347890823