#include "gpk_view.h"

gpk::err_t	gpk::trim			(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters) 	{
	u2_c		countChars				= ltrim(trimmed, original, characters);
	return countChars + rtrim(trimmed, trimmed, characters);
}

gpk::err_t	gpk::ltrim			(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters)	{
	u2_t			iChar				= 0;
	while(iChar < original.size() && original.size() >= (u2_t)find(original[iChar], characters))
		++iChar;
	trimmed		= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}

gpk::err_t	gpk::rtrim			(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters)	{
	u2_t			iChar				= original.size() - 1;
	while(iChar < original.size() && original.size() >= (u2_t)find(original[iChar], characters))
		--iChar;
	u2_c			trimmedCount		= original.size() - iChar;
	trimmed		= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}
