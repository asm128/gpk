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

::gpk::error_t			gpk::viewRead			(::gpk::vcs & headerToRead, const ::gpk::vcb & input)	{
	ree_if(input.size() < 4, "Invalid input size: %u", input.size());
	const uint32_t				elementCount			= *(uint32_t*)input.begin();
	ree_if(elementCount > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount);
	headerToRead			= {(input.size() > sizeof(uint32_t)) ? &input[sizeof(uint32_t)] : 0, elementCount};
	return sizeof(uint32_t) + headerToRead.size();
}

::gpk::error_t			gpk::viewLoad			(::gpk::vcs & headerToRead, const ::gpk::vcub & input)	{
	ree_if(input.size() < 4, "Invalid input size: %u", input.size());
	const uint32_t				elementCount			= *(uint32_t*)input.begin();
	ree_if(elementCount > (input.size() - sizeof(uint32_t)), "Invalid input size: %u. Expected: %u", input.size(), elementCount);
	headerToRead			= vcc{(input.size() > sizeof(uint32_t)) ? (const char*)&input[sizeof(uint32_t)] : 0, elementCount};
	return sizeof(uint32_t) + headerToRead.size();
}
