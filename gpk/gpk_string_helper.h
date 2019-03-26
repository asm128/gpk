#include "gpk_array.h"

#ifndef GPK_STRING_HELPER_H_928347928347
#define GPK_STRING_HELPER_H_928347928347

namespace gpk
{
	::gpk::error_t						formatForSize			(::gpk::view_const_string text, char* output, uint32_t maxlen, const char* pre = "", const char* post = "");

	template<size_t _sizeOutput>
	inline	::gpk::error_t				formatForSize			(::gpk::view_const_string text, char (&output)[_sizeOutput], const char* pre = "", const char* post = "")		{
		return ::gpk::formatForSize(text, output, (uint32_t)_sizeOutput, pre, post);
	}
}

#endif // GPK_STRING_HELPER_H_928347928347