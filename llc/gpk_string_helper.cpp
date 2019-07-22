#include "gpk_string_helper.h"

::gpk::error_t										gpk::formatForSize				(::gpk::view_const_string text, char* output, uint32_t maxlen, const char* pre, const char* post)	{
	::gpk::array_pod<char>									format							= {};
	format.resize((uint32_t)(strlen(pre) + strlen(post) + text.size() + 32));
	memset(format.begin(), 0, format.size());
	sprintf_s(format.begin(), format.size(), "%s" "%%" ".%u" "s" "%s", pre, text.size(), post);
	return sprintf_s(output, maxlen, format.begin(), text.begin());

}
