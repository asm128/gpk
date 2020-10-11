#include "gpk_view_array.h"

static	bool			isAnyOfCharacters						(const ::gpk::view_const_char& charactersToFind, const char characterToTest)		{
	for(uint32_t iCharacter = 0; iCharacter < charactersToFind.size(); ++iCharacter)
		if(characterToTest == charactersToFind[iCharacter])
			return true;
	return false;
}

::gpk::error_t			gpk::trim				(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters)	{
	const uint32_t				countChars				= ::gpk::ltrim(trimmed, original, characters);
	return countChars + ::gpk::rtrim(trimmed, trimmed, characters);
}

::gpk::error_t			gpk::rtrim				(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters)	{
	uint32_t					iChar					= original.size() - 1;
	while(iChar < original.size() && ::isAnyOfCharacters(characters, original[iChar]))
		--iChar;
	const uint32_t				trimmedCount			= original.size() - iChar;
	trimmed					= {original.begin(), original.size() - trimmedCount + 1};
	return trimmedCount;
}

::gpk::error_t			gpk::ltrim				(::gpk::view_const_char & trimmed, const ::gpk::view_const_char & original, const ::gpk::view_const_char & characters)	{
	uint32_t					iChar					= 0;
	while(iChar < original.size() && ::isAnyOfCharacters(characters, original[iChar]))
		++iChar;
	trimmed					= {original.begin() + iChar, original.size() - iChar};
	return iChar;
}
