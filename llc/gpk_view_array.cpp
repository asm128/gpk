#include "gpk_view_array.h"

::gpk::error_t			gpk::rtrim				(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters)	{
	uint32_t					iChar					= original.size() - 1;
	while(iChar < original.size() && 0 <= ::gpk::is_any_of(characters, original[iChar]))
		--iChar;
	const uint32_t				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}

::gpk::error_t			gpk::ltrim				(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters)	{
	uint32_t					iChar					= 0;
	while(iChar < original.size() && 0 <= ::gpk::is_any_of(characters, original[iChar]))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}
