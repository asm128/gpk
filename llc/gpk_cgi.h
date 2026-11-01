#include "gpk_keyval.h"
#include "gpk_array_obj.h"

#ifndef GPK_WEB_CGI
#define GPK_WEB_CGI

namespace gpk
{
	err_t				querystring_split			(vcst_t input_string, aobj<vcst_t> & output_views);
	err_t				writeCGIEnvironToFile		(view<cnst kv<vcst_t>> environViews);
} // namespace

#endif // GPK_WEB_CGI
