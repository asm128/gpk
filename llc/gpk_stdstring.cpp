#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::error_t			gpk::tolower						(::gpk::view_char input)											{
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte) {
		char & curByte = input[iByte];
		if(curByte >= 'A' && curByte <= 'Z')
			curByte						= int8_t(curByte | 0x20);
	}
	return 0;
}

::gpk::error_t			gpk::stoull			(const ::gpk::vcc & input, uint64_t * outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}
