#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::error_t			gpk::toupper		(::gpk::view<char> input)											{
	int			countChanged		= 0;
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
		if(::gpk::toupper(input[iByte]))
			++countChanged;
	return countChanged;
}

::gpk::error_t			gpk::tolower		(::gpk::view<char> input)											{
	int			countChanged		= 0;
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
		if(::gpk::tolower(input[iByte]))
			++countChanged;
	return countChanged;
}

::gpk::error_t			gpk::stoull			(const ::gpk::vcc & input, uint64_t & outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}
