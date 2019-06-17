#include "gpk_array.h"
#include "gpk_coord.h"

#ifndef GPK_WEB_CGI
#define GPK_WEB_CGI

namespace gpk
{
	template<typename _tKey, typename _tVal> 
	struct SKeyVal {
		_tKey		Key;	
		_tVal		Val;	
	};

	::gpk::error_t										token_split					(char token, const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views);
	::gpk::error_t										keyval_split				(const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views);
	::gpk::error_t										querystring_split			(const ::gpk::view_const_string& input_string, ::gpk::array_obj<::gpk::view_const_string>& output_views);
} // namespace

#endif // GPK_WEB_CGI
