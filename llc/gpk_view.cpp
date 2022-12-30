#include "gpk_view.h"

::gpk::error_t			gpk::rtrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters)	{
	uint32_t					iChar					= original.size() - 1;
	while(iChar < original.size() && 0 <= ::gpk::is_any_of(characters, original[iChar]))
		--iChar;
	const uint32_t				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}

::gpk::error_t			gpk::ltrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters)	{
	uint32_t					iChar					= 0;
	while(iChar < original.size() && 0 <= ::gpk::is_any_of(characters, original[iChar]))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}
