#include "gpk_array.h"

#ifndef GPK_WINDOWS_H_0289347890823
#define GPK_WINDOWS_H_0289347890823

namespace gpk
{
#if defined(GPK_WINDOWS)
	::gpk::array_pod<char_t>				getWindowsErrorAsString					(const int32_t lastError); // Get the error message, if any.
#endif
} // namespace

#endif // GPK_WINDOWS_H_0289347890823