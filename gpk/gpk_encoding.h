#include "gpk_array.h"
#include <string>

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
	::gpk::error_t									base64Encode												(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_base64	);
	::gpk::error_t									base64Decode												(const ::gpk::view_array<const char_t	> & in_base64	, ::gpk::array_pod<ubyte_t	> & outputBinary);

	// Based on Gary Ardell's code for VB.
	::gpk::error_t									ardellEncode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output);
	::gpk::error_t									ardellDecode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output);
	static inline ::gpk::error_t					ardellEncode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	static inline ::gpk::error_t					ardellDecode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }
}

#endif // GPK_ENCODING_H_209873982374
