#include "gpk_view_array.h"

#ifndef GPK_KEYVAL_H_298037928374
#define GPK_KEYVAL_H_298037928374

namespace gpk
{
	template<typename _tKey, typename _tVal> 
	struct SKeyVal {
				_tKey					Key;	
				_tVal					Val;	
	};

			::gpk::error_t			token_split			(char token, const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views);
	inline	::gpk::error_t			keyval_split		(const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views) { return ::gpk::token_split('=', input_string, output_views); }

} // namespace

#endif // GPK_KEYVAL_H_298037928374
