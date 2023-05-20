#include "gpk_keyval.h"
#include "gpk_array_obj.h"

#ifndef GPK_WEB_CGI
#define GPK_WEB_CGI

namespace gpk
{
	::gpk::error_t										querystring_split			(const ::gpk::vcc & input_string, ::gpk::aobj<::gpk::vcc> & output_views);
	::gpk::error_t										writeCGIEnvironToFile		(::gpk::view<const ::gpk::TKeyValConstString> environViews);
} // namespace

#endif // GPK_WEB_CGI
