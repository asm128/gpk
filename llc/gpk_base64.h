#include "gpk_array.h"

#ifndef GPK_BASE64_H_23984293847
#define GPK_BASE64_H_23984293847

namespace gpk
{
	static			constexpr ::gpk::vcc			b64Symbols													= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	static			constexpr ::gpk::vcc			b64SymbolsFS												= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"};

					::gpk::error_t					base64Encode												(const ::gpk::vcb & base64Symbols, char_t base64PadSymbol, const ::gpk::vcb & inputBinary	, ::gpk::apod<char_t> & out_base64	);
					::gpk::error_t					base64Decode												(const ::gpk::vcb & base64Symbols, char_t base64PadSymbol, const ::gpk::vcc & in_base64		, ::gpk::apod<byte_t> & outputBinary);
	static inline	::gpk::error_t					base64Encode												(const ::gpk::vcb & inputBinary	, ::gpk::apod<char_t> & out_base64		) { return base64Encode(b64Symbols	, '=', inputBinary	, out_base64	); }
	static inline	::gpk::error_t					base64Decode												(const ::gpk::vcb & in_base64	, ::gpk::apod<byte_t> & outputBinary	) { return base64Decode(b64Symbols	, '=', in_base64	, outputBinary	); }
	static inline	::gpk::error_t					base64EncodeFS												(const ::gpk::vcb & inputBinary	, ::gpk::apod<char_t> & out_base64		) { return base64Encode(b64SymbolsFS, '=', inputBinary	, out_base64	); }
	static inline	::gpk::error_t					base64DecodeFS												(const ::gpk::vcb & in_base64	, ::gpk::apod<byte_t> & outputBinary	) { return base64Decode(b64SymbolsFS, '=', in_base64	, outputBinary	); }
}

#endif //

