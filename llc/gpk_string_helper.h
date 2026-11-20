#include "gpk_array.h"

#ifndef GPK_STRING_HELPER_H_23627
#define GPK_STRING_HELPER_H_23627

namespace gpk
{
	gpk::err_t			formatForSize			(::gpk::vcsc_t text, sc_t* output, u2_t maxlen, const sc_t* pre = "", const sc_t* post = "");

	tplt<size_t _sizeOutput>
	inline	gpk::err_t	formatForSize			(::gpk::vcsc_t text, sc_t (&output)[_sizeOutput], const sc_t* pre = "", const sc_t* post = "")		{
		return ::gpk::formatForSize(text, output, (u2_t)_sizeOutput, pre, post);
	}
}

#endif // GPK_STRING_HELPER_H_23627
