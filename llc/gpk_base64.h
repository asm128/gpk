#include "gpk_array_pod.h"
#include "gpk_array_static.h"
#include "gpk_string.h"

#ifndef GPK_BASE64_H_23627
#define GPK_BASE64_H_23627

namespace gpk
{
	stxp	vcst_t		b64Symbols		= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};
	stxp	vcst_t		b64SymbolsFS	= {64, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"};

	stct SBase64SymbolMap {
		vcst_t			Symbols		= {};
		sc_t			Pad			= '=';
		astu0_t<256>	Decode		= {};
		err_t			Error		= 0;
		u2_t			ErrorIndex	= 0;
	};

	inxp SBase64SymbolMap	base64SymbolMap	(vcst_t symbols, sc_t pad = '=') {
		SBase64SymbolMap	result						= {symbols, pad};
		for(u2_t iSymbol = 0; iSymbol < result.Decode.size(); ++iSymbol)
			result.Decode.Storage[iSymbol]			= 0xFF;
		if(64 != symbols.size()) {
			result.Error							= -1;
			rtrn result;
		}
		for(u2_t iSymbol = 0; iSymbol < symbols.size(); ++iSymbol) {
			u0_c				symbol				= (u0_t)symbols.begin()[iSymbol];
			if(symbol == (u0_t)pad) {
				result.Error						= -2;
				result.ErrorIndex					= iSymbol;
				rtrn result;
			}
			if(0xFF != result.Decode.Storage[symbol]) {
				result.Error						= -3;
				result.ErrorIndex					= iSymbol;
				rtrn result;
			}
			result.Decode.Storage[symbol]			= (u0_t)iSymbol;
		}
		result.Decode.Storage[(u0_t)pad]			= 0;
		rtrn result;
	}

	err_t			base64Encode	(cnst SBase64SymbolMap & symbolMap, vcu0_c & in_binary	, au0_t & out_base64);
	err_t			base64Decode	(cnst SBase64SymbolMap & symbolMap, vcu0_c & in_base64	, au0_t & out_binary);
	stin	err_t	base64Encode	(vcst_t base64Symbols, char base64PadSymbol, vcu0_c & in_binary, au0_t & out_base64) { rtrn base64Encode(base64SymbolMap(base64Symbols, base64PadSymbol), in_binary, out_base64); }
	stin	err_t	base64Decode	(vcst_t base64Symbols, char base64PadSymbol, vcu0_c & in_base64, au0_t & out_binary) { rtrn base64Decode(base64SymbolMap(base64Symbols, base64PadSymbol), in_base64, out_binary); }

	sinx	SBase64SymbolMap	B64_SYMBOL_MAP							= base64SymbolMap(b64Symbols, '=');
	sinx	SBase64SymbolMap	B64_SYMBOL_MAP_FS						= base64SymbolMap(b64SymbolsFS, '=');
	static_assert(0 == B64_SYMBOL_MAP.Error);
	static_assert(0 == B64_SYMBOL_MAP_FS.Error);

	stin	err_t	base64Encode	(vcu0_c & in_binary, au0_t & out_base64) { rtrn base64Encode(B64_SYMBOL_MAP, in_binary, out_base64); }
	stin	err_t	base64Decode	(vcu0_c & in_base64, au0_t & out_binary) { rtrn base64Decode(B64_SYMBOL_MAP, in_base64, out_binary); }
	stin	err_t	base64EncodeFS	(vcu0_c & in_binary, au0_t & out_base64) { rtrn base64Encode(B64_SYMBOL_MAP_FS, in_binary, out_base64); }
	stin	err_t	base64DecodeFS	(vcu0_c & in_base64, au0_t & out_binary) { rtrn base64Decode(B64_SYMBOL_MAP_FS, in_base64, out_binary); }
																	 
	stin	err_t	base64Encode	(vcu0_c & in_binary, asc_t & out_base64) { rtrn base64Encode		(in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64EncodeFS	(vcu0_c & in_binary, asc_t & out_base64) { rtrn base64EncodeFS		(in_binary, *(au0_t*)&out_base64); }
																			
	stin	err_t	base64Encode	(vcs0_c & in_binary, asc_t & out_base64) { rtrn base64Encode		(*(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64EncodeFS	(vcs0_c & in_binary, asc_t & out_base64) { rtrn base64EncodeFS		(*(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }

	stin	err_t	base64Encode	(vcs0_c & in_binary, au0_t & out_base64) { rtrn base64Encode		(*(vcu0_t*)&in_binary, out_base64	); }
	stin	err_t	base64Decode	(vcu0_c & in_base64, asc_t & out_binary) { rtrn base64Decode		(in_base64, *(au0_t*)&out_binary	); }
	stin	err_t	base64EncodeFS	(vcs0_c & in_binary, au0_t & out_base64) { rtrn base64EncodeFS		(*(vcu0_t*)&in_binary, out_base64	); }
	stin	err_t	base64DecodeFS	(vcu0_c & in_base64, asc_t & out_binary) { rtrn base64DecodeFS		(in_base64, *(au0_t*)&out_binary	); }

	stin	err_t	base64Encode	(vcu0_c & in_binary	, string & out_base64) { rtrn base64Encode		(in_binary, *(au0_t*)&out_base64 	); }
	stin	err_t	base64Decode	(vcst_t in_base64	, au0_t  & out_binary) { rtrn base64Decode		(*(vcu0_t*)&in_base64, out_binary	); }
	stin	err_t	base64EncodeFS	(vcu0_c & in_binary	, string & out_base64) { rtrn base64EncodeFS	(in_binary, *(au0_t*)&out_base64 	); }
	stin	err_t	base64DecodeFS	(vcst_t in_base64	, au0_t  & out_binary) { rtrn base64DecodeFS	(*(vcu0_t*)&in_base64, out_binary	); }

	stin	err_t	base64Encode	(vcs0_c & in_binary	, string & out_base64) { rtrn base64Encode		(*(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64Decode	(vcst_t in_base64	, asc_t  & out_binary) { rtrn base64Decode		(*(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }
	stin	err_t	base64EncodeFS	(vcs0_c & in_binary	, string & out_base64) { rtrn base64EncodeFS	(*(vcu0_t*)&in_binary, *(au0_t*)&out_base64); }
	stin	err_t	base64DecodeFS	(vcst_t in_base64	, asc_t  & out_binary) { rtrn base64DecodeFS	(*(vcu0_t*)&in_base64, *(au0_t*)&out_binary); }
} // namespace

#endif // GPK_BASE64_H_23627


