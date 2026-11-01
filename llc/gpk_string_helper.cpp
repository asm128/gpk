#include "gpk_string_helper.h"

//::gpk::err_t			gpk::formatForSize				(::gpk::view<sc_t> text, sc_t * output, u2_t maxlen, const sc_t * pre, const sc_t * post)	{
//	::gpk::apod<sc_t>									format							= {};
//	format.resize((u2_t)(strlen(pre) + strlen(post) + text.size() + 32));
//	memset(format.begin(), 0, format.size());
//	snprintf(format.begin(), format.size(), "%s" "%%" ".%u" "s" "%s", pre, text.size(), post);
//	return snprintf(output, maxlen, format.begin(), text.begin());
//
//}
//