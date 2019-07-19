#include "gpk_array.h"

#ifndef GPK_BASE64_H_23984293847
#define GPK_BASE64_H_23984293847

namespace gpk
{
					::gpk::error_t					base64Encode												(const ::gpk::view_const_byte & base64Symbols, char_t base64PadSymbol, const ::gpk::view_const_byte & inputBinary	, ::gpk::array_pod<char_t> & out_base64	);
					::gpk::error_t					base64Decode												(const ::gpk::view_const_byte & base64Symbols, char_t base64PadSymbol, const ::gpk::view_const_char & in_base64		, ::gpk::array_pod<byte_t> & outputBinary);
	static inline	::gpk::error_t					base64Encode												(const ::gpk::view_const_byte & inputBinary	, ::gpk::array_pod<char_t> & out_base64	) {
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Encode(base64Symbols, base64PadSymbol, inputBinary, out_base64);
	}
	static inline	::gpk::error_t					base64Decode												(const ::gpk::view_const_byte & in_base64	, ::gpk::array_pod<byte_t> & outputBinary) {
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Decode(base64Symbols, base64PadSymbol, in_base64, outputBinary);
	}
	static inline	::gpk::error_t					base64EncodeFS												(const ::gpk::view_const_byte & inputBinary	, ::gpk::array_pod<char_t> & out_base64	) {
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Encode(base64Symbols, base64PadSymbol, inputBinary, out_base64);
	}
	static inline	::gpk::error_t					base64DecodeFS												(const ::gpk::view_const_byte & in_base64	, ::gpk::array_pod<byte_t> & outputBinary) {
		static constexpr	const char						base64Symbols[]												= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
		static constexpr	const char						base64PadSymbol												= '=';
		return base64Decode(base64Symbols, base64PadSymbol, in_base64, outputBinary);
	}
}

#endif // 

