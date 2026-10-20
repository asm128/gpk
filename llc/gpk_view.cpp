#include "gpk_view.h"

::gpk::error_t			gpk::trim				(::gpk::vcst_t & trimmed, cnst ::gpk::vcst_t & original, cnst ::gpk::vcst_t & characters) 	{
	u2_c				countChars				= ::gpk::ltrim(trimmed, original, characters);
	return countChars + ::gpk::rtrim(trimmed, trimmed, characters);
}

::gpk::error_t			gpk::ltrim				(::gpk::vcst_t & trimmed, cnst ::gpk::vcst_t & original, cnst ::gpk::vcst_t & characters)	{
	::gpk::u2_t					iChar					= 0;
	while(iChar < original.size() && original.size() >= (::gpk::u2_t)::gpk::find(original[iChar], characters))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}

::gpk::error_t			gpk::rtrim				(::gpk::vcst_t & trimmed, cnst ::gpk::vcst_t & original, cnst ::gpk::vcst_t & characters)	{
	::gpk::u2_t					iChar					= original.size() - 1;
	while(iChar < original.size() && original.size() >= (::gpk::u2_t)::gpk::find(original[iChar], characters))
		--iChar;
	u2_c				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}


