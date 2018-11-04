#include "gpk_array.h"
#include <string>

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
					::gpk::error_t					hexEncode													(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_hexed	);
					::gpk::error_t					hexDecode													(const ::gpk::view_array<const char_t	> & in_hexed	, ::gpk::array_pod<ubyte_t	> & outputBinary);

					::gpk::error_t					base64Encode												(const ::gpk::view_array<const char_t> & base64Symbols, char_t base64PadSymbol, const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_base64	);
					::gpk::error_t					base64Decode												(const ::gpk::view_array<const char_t> & base64Symbols, char_t base64PadSymbol, const ::gpk::view_array<const char_t	> & in_base64	, ::gpk::array_pod<ubyte_t	> & outputBinary);
	static inline	::gpk::error_t					base64Encode												(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_base64	) { 
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Encode(base64Symbols, base64PadSymbol, inputBinary, out_base64);
	}
	static inline	::gpk::error_t					base64Decode												(const ::gpk::view_array<const char_t	> & in_base64	, ::gpk::array_pod<ubyte_t	> & outputBinary) { 
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Decode(base64Symbols, base64PadSymbol, in_base64, outputBinary);
	}
	static inline	::gpk::error_t					base64EncodeFS												(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_base64	) { 
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Encode(base64Symbols, base64PadSymbol, inputBinary, out_base64);
	}
	static inline	::gpk::error_t					base64DecodeFS												(const ::gpk::view_array<const char_t	> & in_base64	, ::gpk::array_pod<ubyte_t	> & outputBinary) { 
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Decode(base64Symbols, base64PadSymbol, in_base64, outputBinary);
	}

	// Based on Gary Ardell's code for VB.
					::gpk::error_t					ardellEncode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output);
					::gpk::error_t					ardellDecode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output);
	static inline	::gpk::error_t					ardellEncode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	static inline	::gpk::error_t					ardellDecode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }

					::gpk::error_t					saltDataSalt												(const ::gpk::view_const_byte& binary, ::gpk::array_pod<byte_t> & salted);
					::gpk::error_t					saltDataUnsalt												(const ::gpk::view_const_byte& salted, ::gpk::array_pod<byte_t> & binary);
}

#endif // GPK_ENCODING_H_209873982374
