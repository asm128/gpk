#include "gpk_array.h"

#ifndef GPK_ENCODING_H_23627
#define GPK_ENCODING_H_23627

namespace gpk
{
	::gpk::error_t						hexEncode				(const ::gpk::vcu0_t & inputBinary	, ::gpk::asc_t & out_hexed	);
	::gpk::error_t						hexDecode				(const ::gpk::vcsc_t  & in_hexed	, ::gpk::au0_t & outputBinary	);
	::gpk::error_t						hexDecode				(const ::gpk::vcsc_t  & in_hexed	, ::gpk::as0_t & outputBinary	);
	stainli	::gpk::error_t				hexEncode				(const ::gpk::vcs0_t & inputBinary	, ::gpk::asc_t & out_hexed	)			{ return hexEncode(::gpk::vcu0_t{(const uint8_t*)inputBinary.begin(), inputBinary.size()}, out_hexed); }

	// Based on Gary Ardell's code for VB.
	::gpk::error_t						ardellEncode			(::gpk::as2_t & cache, const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output);
	::gpk::error_t						ardellDecode			(::gpk::as2_t & cache, const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output);

	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }
	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vcs0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellEncode(tempCache, *(const ::gpk::vcu0_t*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::as0_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, *(::gpk::au0_t*)&output); }
	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vcsc_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellEncode(tempCache, *(const ::gpk::vcu0_t*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::asc_t & output)	{ ::gpk::as2_t tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, *(::gpk::au0_t*)&output); }

	stainli	::gpk::error_t				ardellEncode			(::gpk::as2_t & cache, const ::gpk::vcs0_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ return ::gpk::ardellEncode(cache, *(const ::gpk::vcu0_t*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(::gpk::as2_t & cache, const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::as0_t & output)	{ return ::gpk::ardellDecode(cache, input, key, salt, *(::gpk::au0_t*)&output); }
	stainli	::gpk::error_t				ardellEncode			(::gpk::as2_t & cache, const ::gpk::vcsc_t & input, uint64_t key, bool salt, ::gpk::au0_t & output)	{ return ::gpk::ardellEncode(cache, *(const ::gpk::vcu0_t*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(::gpk::as2_t & cache, const ::gpk::vcu0_t & input, uint64_t key, bool salt, ::gpk::asc_t & output)	{ return ::gpk::ardellDecode(cache, input, key, salt, *(::gpk::au0_t*)&output); }

	::gpk::error_t						saltDataSalt			(const ::gpk::vcu0_t & binary, ::gpk::au0_t & salted);
	::gpk::error_t						saltDataUnsalt			(const ::gpk::vcu0_t & salted, ::gpk::au0_t & binary);

	tplt<tpnm _tBase>
	::gpk::error_t						rleEncode				(const ::gpk::view<_tBase> & decoded, ::gpk::au0_t & encoded) {
		uint32_t								idxLatest				= 0;
		stacxpr	const uint32_t					sizeBlock				= sizeof(_tBase) + 1;
		for(uint32_t iIn = 0; iIn < decoded.size(); ++iIn) {
			const _tBase 							& current				= decoded[iIn];
			const _tBase							& latest				= decoded[idxLatest];
			if(0 == iIn || 0 != memcmp(&current, &latest, sizeof(_tBase)) || 255 == (uint8_t)encoded[encoded.size() - 1]) {
				const uint32_t							newSize					= encoded.size() + sizeBlock;
				gpk_necs(encoded.resize(newSize));
				memcpy(&encoded[encoded.size() - sizeBlock], &current, sizeof(_tBase));
				encoded[encoded.size() - 1]			= 0;
				idxLatest							= iIn;
			}
			else
				++encoded[encoded.size() - 1];
		}
		return 0;
	}

	tplt<tpnm _tBase>
	::gpk::error_t						rleDecode			(const ::gpk::vcu0_t & encoded, ::gpk::apod<_tBase> & decoded) {
		stacxpr	const uint32_t					sizeBlock			= sizeof(_tBase) + 1;
  		for(uint32_t iIn = 0; iIn < encoded.size(); iIn += sizeBlock) {
			const _tBase 							& current			= *(_tBase*)&encoded[iIn];
			const uint8_t							count				= encoded[iIn + sizeof(_tBase)];
			gpk_necs(decoded.push_back(current));
			for(uint32_t iOut = 0; iOut < count; ++iOut)
				gpk_necs(decoded.push_back(current));
		}
		return 0;
	}

	// Description at http://en.wikipedia.org/wiki/UTF-8
	::gpk::error_t			utf8FromCodePoint	(uint32_t codePoint, ::gpk::asc_t & hexDigits);
	::gpk::error_t			digest				(const ::gpk::vcu0_t & input, ::gpk::au2_t & digest);
	::gpk::error_t			digest				(const ::gpk::vcu0_t & input, ::gpk::asc_t & digest);
	stainli	::gpk::error_t	digest				(const ::gpk::vcsc_t & input, ::gpk::asc_t & digest) { return ::gpk::digest(*(const ::gpk::vcu0_t*)&input, digest); }
}

#endif // GPK_ENCODING_H_23627
