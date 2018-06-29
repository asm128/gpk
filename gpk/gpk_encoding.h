#include "gpk_array.h"
#include <string>

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
	::gpk::error_t									base64Encode												(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_base64	);
	::gpk::error_t									base64Decode												(const ::gpk::view_array<const char_t	> & in_base64	, ::gpk::array_pod<ubyte_t	> & outputBinary);
}

#endif // GPK_ENCODING_H_209873982374
