#include "gpk_view.h"
#include "gpk_safe.h"

#ifndef GPK_STDSTRING_H_23627
#define GPK_STDSTRING_H_23627
namespace gpk
{
	::gpk::error_t			stoull		(const ::gpk::vcc & input, uint64_t & output);
	::gpk::error_t			tolower		(::gpk::view<char> input);
}
#endif // GPK_STDSTRING_H_23627
