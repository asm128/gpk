#include "gpk_stdstring.h"

#include <string>

::gpk::error_t									gpk::stoull							(const ::gpk::view_const_string & input, uint64_t * outputNumber)			{
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
	gpk_safe_assign(outputNumber, ::std::stoull({input.begin(), input.size()})); 
#else
	try {
		gpk_safe_assign(outputNumber, ::std::stoull({input.begin(), input.size()})); 
	} 
	catch(...) {
		return -1;
	}
#endif
	return 0;
}