#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::error_t									gpk::tolower						(::gpk::view_char input)											{
	for(uint32_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte) {
		char_t & curByte = input[iByte];
		if(curByte >= 'A' && curByte <= 'Z')
			curByte						= byte_t(curByte | 0x20);
	}
	return 0;
}

::gpk::error_t									gpk::stoull							(const ::gpk::view_const_char & input, uint64_t * outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}
