#include "gpk_view_array.h"

static	bool			isAnyOfCharacters						(const ::gpk::view_const_string& charactersToFind, const char characterToTest)		{
	for(uint32_t iCharacter = 0; iCharacter < charactersToFind.size(); ++iCharacter)
		if(characterToTest == charactersToFind[iCharacter])
			return true;
	return false;
}

::gpk::error_t			gpk::rtrim				(const ::gpk::view_const_string & original, ::gpk::view_const_string & trimmed)	{ uint32_t iChar = original.size(); while(iChar < original.size() && ::isAnyOfCharacters(" \t\n\b\r", trimmed[iChar])) --iChar; trimmed = {original.begin(), original.size() - (original.size() - (iChar + 1))}; return iChar; }
::gpk::error_t			gpk::ltrim				(const ::gpk::view_const_string & original, ::gpk::view_const_string & trimmed)	{ uint32_t iChar = 0; while(iChar < original.size() && ::isAnyOfCharacters(" \t\n\b\r", trimmed[iChar])) ++iChar; trimmed = {original.begin() + iChar, original.size() - iChar}; return iChar;}
::gpk::error_t			gpk::trim				(const ::gpk::view_const_string & original, ::gpk::view_const_string & trimmed)	{ uint32_t countChars = ::gpk::ltrim(original, trimmed); return countChars + ::gpk::rtrim(trimmed); }
