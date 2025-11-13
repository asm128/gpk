#include "gpk_view.h"

::gpk::error_t			gpk::trim				(::gpk::vcsc_t & trimmed, ::gpk::vcsc_c & original, ::gpk::vcsc_c & characters) 	{
	u2_c				countChars				= ::gpk::ltrim(trimmed, original, characters);
	return countChars + ::gpk::rtrim(trimmed, trimmed, characters);
}

::gpk::error_t			gpk::ltrim				(::gpk::vcsc_t & trimmed, ::gpk::vcsc_c & original, ::gpk::vcsc_c & characters)	{
	uint32_t					iChar					= 0;
	while(iChar < original.size() && original.size() >= (uint32_t)::gpk::find(original[iChar], characters))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}

::gpk::error_t			gpk::rtrim				(::gpk::vcsc_t & trimmed, ::gpk::vcsc_c & original, ::gpk::vcsc_c & characters)	{
	uint32_t					iChar					= original.size() - 1;
	while(iChar < original.size() && original.size() >= (uint32_t)::gpk::find(original[iChar], characters))
		--iChar;
	u2_c				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}
