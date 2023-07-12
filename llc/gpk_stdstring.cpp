#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::error_t			gpk::toupper		(::gpk::view<char> input)											{
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
		::gpk::toupper(input[iByte]);
	return 0;
}

::gpk::error_t			gpk::tolower		(::gpk::view<char> input)											{
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
		::gpk::tolower(input[iByte]);
	return 0;
}

::gpk::error_t			gpk::stoull			(const ::gpk::vcc & input, uint64_t & outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}
