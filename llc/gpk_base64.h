#include "gpk_array_pod.h"

#ifndef GPK_BASE64_H_23627
#define GPK_BASE64_H_23627

namespace gpk
{
	stacxpr	::gpk::vcc		b64Symbols		= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	stacxpr	::gpk::vcc		b64SymbolsFS	= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"};

	::gpk::error_t			base64Encode	(const ::gpk::vcc & base64Symbols, char base64PadSymbol, const ::gpk::vcu8 & inputBinary	, ::gpk::au8 & out_base64	);
	::gpk::error_t			base64Decode	(const ::gpk::vcc & base64Symbols, char base64PadSymbol, const ::gpk::vcu8 & in_base64	, ::gpk::au8 & outputBinary	);

	stainli	::gpk::error_t	base64Encode	(const ::gpk::vcu8 & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64Symbols	, '=', inputBinary	, out_base64	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vcu8 & in_base64  , ::gpk::au8 & outputBinary	) { return ::gpk::base64Decode(b64Symbols	, '=', in_base64	, outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vcu8 & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64SymbolsFS	, '=', inputBinary	, out_base64	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vcu8 & in_base64  , ::gpk::au8 & outputBinary	) { return ::gpk::base64Decode(b64SymbolsFS	, '=',  in_base64, outputBinary	); }
																						   
	stainli	::gpk::error_t	base64Encode	(const ::gpk::vci8 & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64Symbols	, '=', *(::gpk::vcu8*)&inputBinary	, out_base64	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vci8 & in_base64  , ::gpk::au8 & outputBinary	) { return ::gpk::base64Decode(b64Symbols	, '=', *(::gpk::vcu8*)&in_base64	, outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vci8 & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64SymbolsFS	, '=', *(::gpk::vcu8*)&inputBinary	, out_base64	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vci8 & in_base64  , ::gpk::au8 & outputBinary	) { return ::gpk::base64Decode(b64SymbolsFS	, '=', *(::gpk::vcu8*)& in_base64, outputBinary	); }
																						   
	stainli	::gpk::error_t	base64Encode	(const ::gpk::vcu8 & inputBinary, ::gpk::ai8 & out_base64	) { return ::gpk::base64Encode(b64Symbols	, '=', inputBinary	, *(::gpk::au8*)&out_base64		); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vcu8 & in_base64  , ::gpk::ai8 & outputBinary	) { return ::gpk::base64Decode(b64Symbols	, '=', in_base64	, *(::gpk::au8*)&outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vcu8 & inputBinary, ::gpk::ai8 & out_base64	) { return ::gpk::base64Encode(b64SymbolsFS	, '=', inputBinary	, *(::gpk::au8*)&out_base64		); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vcu8 & in_base64  , ::gpk::ai8 & outputBinary	) { return ::gpk::base64Decode(b64SymbolsFS	, '=', in_base64	, *(::gpk::au8*)&outputBinary	); }
																						   
	stainli	::gpk::error_t	base64Encode	(const ::gpk::vci8 & inputBinary, ::gpk::ai8 & out_base64	) { return ::gpk::base64Encode(b64Symbols	, '=', *(::gpk::vcu8*)&inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vci8 & in_base64  , ::gpk::ai8 & outputBinary	) { return ::gpk::base64Decode(b64Symbols	, '=', *(::gpk::vcu8*)&in_base64	, *(::gpk::au8*)&outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vci8 & inputBinary, ::gpk::ai8 & out_base64	) { return ::gpk::base64Encode(b64SymbolsFS	, '=', *(::gpk::vcu8*)&inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vci8 & in_base64  , ::gpk::ai8 & outputBinary	) { return ::gpk::base64Decode(b64SymbolsFS	, '=', *(::gpk::vcu8*)&in_base64	, *(::gpk::au8*)&outputBinary	); }

	stainli	::gpk::error_t	base64Encode	(const ::gpk::vcc  & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64Symbols	, '=', *(::gpk::vcu8*)&inputBinary, out_base64	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vcu8 & in_base64  , ::gpk::ac  & outputBinary	) { return ::gpk::base64Decode(b64Symbols	, '=', in_base64, *(::gpk::au8*)&outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vcc  & inputBinary, ::gpk::au8 & out_base64	) { return ::gpk::base64Encode(b64SymbolsFS	, '=', *(::gpk::vcu8*)&inputBinary, out_base64	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vcu8 & in_base64  , ::gpk::ac  & outputBinary	) { return ::gpk::base64Decode(b64SymbolsFS	, '=', in_base64, *(::gpk::au8*)&outputBinary	); }

	stainli	::gpk::error_t	base64Encode	(const ::gpk::vcu8 & inputBinary, ::gpk::ac  & out_base64  ) { return ::gpk::base64Encode(b64Symbols  , '=', inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vcc  & in_base64  , ::gpk::au8 & outputBinary) { return ::gpk::base64Decode(b64Symbols  , '=', *(::gpk::vcu8*)&in_base64	, outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vcu8 & inputBinary, ::gpk::ac  & out_base64  ) { return ::gpk::base64Encode(b64SymbolsFS, '=', inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vcc  & in_base64  , ::gpk::au8 & outputBinary) { return ::gpk::base64Decode(b64SymbolsFS, '=', *(::gpk::vcu8*)&in_base64	, outputBinary	); }

	stainli	::gpk::error_t	base64Encode	(const ::gpk::vcc &  inputBinary, ::gpk::ac & out_base64  ) { return ::gpk::base64Encode(b64Symbols  , '=', *(::gpk::vcu8*)&inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64Decode	(const ::gpk::vcc &  in_base64  , ::gpk::ac & outputBinary) { return ::gpk::base64Decode(b64Symbols  , '=', *(::gpk::vcu8*)&in_base64	, *(::gpk::au8*)&outputBinary	); }
	stainli	::gpk::error_t	base64EncodeFS	(const ::gpk::vcc &  inputBinary, ::gpk::ac & out_base64  ) { return ::gpk::base64Encode(b64SymbolsFS, '=', *(::gpk::vcu8*)&inputBinary	, *(::gpk::au8*)&out_base64 	); }
	stainli	::gpk::error_t	base64DecodeFS	(const ::gpk::vcc &  in_base64  , ::gpk::ac & outputBinary) { return ::gpk::base64Decode(b64SymbolsFS, '=', *(::gpk::vcu8*)&in_base64	, *(::gpk::au8*)&outputBinary	); }
} // namespace

#endif // GPK_BASE64_H_23627
