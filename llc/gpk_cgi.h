#include "gpk_keyval.h"
#include "gpk_array.h"
#include "gpk_coord.h"

#ifndef GPK_WEB_CGI
#define GPK_WEB_CGI

namespace gpk
{
	::gpk::error_t										querystring_split			(const ::gpk::view_const_char& input_string, ::gpk::array_obj<::gpk::view_const_char>& output_views);
	::gpk::error_t										writeCGIEnvironToFile		(::gpk::view_array<const ::gpk::TKeyValConstString> environViews);
} // namespace

#endif // GPK_WEB_CGI
