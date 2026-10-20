#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::error_t			gpk::stoull			(vcst_t input, uint64_t & outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}


