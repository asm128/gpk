#include "gpk_array.h"

#ifndef GPK_STRING_HELPER_H_23627
#define GPK_STRING_HELPER_H_23627

namespace gpk
{
	::gpk::error_t			formatForSize			(::gpk::vcsc_t text, sc_t* output, uint32_t maxlen, const sc_t* pre = "", const sc_t* post = "");

	tplt<size_t _sizeOutput>
	inline	::gpk::error_t	formatForSize			(::gpk::vcsc_t text, sc_t (&output)[_sizeOutput], const sc_t* pre = "", const sc_t* post = "")		{
		return ::gpk::formatForSize(text, output, (uint32_t)_sizeOutput, pre, post);
	}
}

#endif // GPK_STRING_HELPER_H_23627
