#include "gpk_stdstring.h"
#include "gpk_parse.h"

::gpk::err_t			gpk::stoull			(vcst_t input, u3_t & outputNumber)	{
	return ::gpk::parseIntegerDecimal(input, outputNumber);
}
