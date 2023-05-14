#include "gpk_view.h"

::gpk::error_t			gpk::rtrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters)	{
	uint32_t					iChar					= original.size() - 1;
	while(iChar < original.size() && original.size() >= (uint32_t)::gpk::find(original[iChar], characters))
		--iChar;
	const uint32_t				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}

::gpk::error_t			gpk::ltrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters)	{
	uint32_t					iChar					= 0;
	while(iChar < original.size() && original.size() >= (uint32_t)::gpk::find(original[iChar], characters))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}

::gpk::error_t			gpk::trim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters) 	{
	const uint32_t				countChars				= ::gpk::ltrim(trimmed, original, characters);
	return countChars + ::gpk::rtrim(trimmed, trimmed, characters);
}
