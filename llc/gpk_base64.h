#include "gpk_array_pod.h"

#ifndef GPK_BASE64_H_23627
#define GPK_BASE64_H_23627

namespace gpk
{
	stxp	vcsc_t		b64Symbols		= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	stxp	vcsc_t		b64SymbolsFS	= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"};

	err_t			base64Encode	(cnst vcsc_t & base64Symbols, char base64PadSymbol, cnst vcu0_t & in_binary	, au0_t & out_base64);
	err_t			base64Decode	(cnst vcsc_t & base64Symbols, char base64PadSymbol, cnst vcu0_t & in_base64	, au0_t & out_binary);

	stin	err_t	base64Encode	(cnst vcu0_t & in_binary, au0_t & out_base64) { return base64Encode(b64Symbols		, '=', in_binary, out_base64); }
	stin	err_t	base64Decode	(cnst vcu0_t & in_base64, au0_t & out_binary) { return base64Decode(b64Symbols		, '=', in_base64, out_binary); }
	stin	err_t	base64EncodeFS	(cnst vcu0_t & in_binary, au0_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', in_binary, out_base64); }
	stin	err_t	base64DecodeFS	(cnst vcu0_t & in_base64, au0_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', in_base64, out_binary); }
																					 
	stin	err_t	base64Encode	(cnst vcs0_t & in_binary, au0_t & out_base64) { return base64Encode(b64Symbols		, '=', *(vcu0_t*)&in_binary, out_base64); }
	stin	err_t	base64Decode	(cnst vcs0_t & in_base64, au0_t & out_binary) { return base64Decode(b64Symbols		, '=', *(vcu0_t*)&in_base64, out_binary); }
	stin	err_t	base64EncodeFS	(cnst vcs0_t & in_binary, au0_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', *(vcu0_t*)&in_binary, out_base64); }
	stin	err_t	base64DecodeFS	(cnst vcs0_t & in_base64, au0_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', *(vcu0_t*)&in_base64, out_binary); }
																					 
	stin	err_t	base64Encode	(cnst vcu0_t & in_binary, as0_t & out_base64) { return base64Encode(b64Symbols		, '=', in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64Decode	(cnst vcu0_t & in_base64, as0_t & out_binary) { return base64Decode(b64Symbols		, '=', in_base64, *(au0_t*)&out_binary); }
	stin	err_t	base64EncodeFS	(cnst vcu0_t & in_binary, as0_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64DecodeFS	(cnst vcu0_t & in_base64, as0_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', in_base64, *(au0_t*)&out_binary); }
																					 
	stin	err_t	base64Encode	(cnst vcs0_t & in_binary, as0_t & out_base64) { return base64Encode(b64Symbols		, '=', *(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64Decode	(cnst vcs0_t & in_base64, as0_t & out_binary) { return base64Decode(b64Symbols		, '=', *(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }
	stin	err_t	base64EncodeFS	(cnst vcs0_t & in_binary, as0_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', *(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64DecodeFS	(cnst vcs0_t & in_base64, as0_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', *(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }

	stin	err_t	base64Encode	(cnst vcsc_t & in_binary, au0_t & out_base64) { return base64Encode(b64Symbols		, '=', *(vcu0_t*)&in_binary, out_base64	); }
	stin	err_t	base64Decode	(cnst vcu0_t & in_base64, asc_t & out_binary) { return base64Decode(b64Symbols		, '=', in_base64, *(au0_t*)&out_binary	); }
	stin	err_t	base64EncodeFS	(cnst vcsc_t & in_binary, au0_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', *(vcu0_t*)&in_binary, out_base64	); }
	stin	err_t	base64DecodeFS	(cnst vcu0_t & in_base64, asc_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', in_base64, *(au0_t*)&out_binary	); }

	stin	err_t	base64Encode	(cnst vcu0_t & in_binary, asc_t & out_base64) { return base64Encode(b64Symbols  	, '=', in_binary, *(au0_t*)&out_base64 	); }
	stin	err_t	base64Decode	(cnst vcsc_t & in_base64, au0_t & out_binary) { return base64Decode(b64Symbols  	, '=', *(vcu0_t*)&in_base64, out_binary	); }
	stin	err_t	base64EncodeFS	(cnst vcu0_t & in_binary, asc_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', in_binary, *(au0_t*)&out_base64 	); }
	stin	err_t	base64DecodeFS	(cnst vcsc_t & in_base64, au0_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', *(vcu0_t*)&in_base64, out_binary	); }

	stin	err_t	base64Encode	(cnst vcsc_t & in_binary, asc_t & out_base64) { return base64Encode(b64Symbols  	, '=', *(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64Decode	(cnst vcsc_t & in_base64, asc_t & out_binary) { return base64Decode(b64Symbols  	, '=', *(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }
	stin	err_t	base64EncodeFS	(cnst vcsc_t & in_binary, asc_t & out_base64) { return base64Encode(b64SymbolsFS	, '=', *(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64DecodeFS	(cnst vcsc_t & in_base64, asc_t & out_binary) { return base64Decode(b64SymbolsFS	, '=', *(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }
} // namespace

#endif // GPK_BASE64_H_23627
