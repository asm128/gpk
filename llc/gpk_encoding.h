#include "gpk_array.h"

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
	::gpk::error_t						hexEncode				(const ::gpk::vcu8 & inputBinary, ::gpk::ac  & out_hexed	);
	::gpk::error_t						hexDecode				(const ::gpk::vcc  & in_hexed   , ::gpk::au8 & outputBinary	);
	::gpk::error_t						hexDecode				(const ::gpk::vcc  & in_hexed   , ::gpk::ai8 & outputBinary	);
	stainli	::gpk::error_t				hexEncode				(const ::gpk::vci8 & inputBinary, ::gpk::ac  & out_hexed	)			{ return hexEncode(::gpk::vcu8{(const uint8_t*)inputBinary.begin(), inputBinary.size()}, out_hexed); }

	// Based on Gary Ardell's code for VB.
	::gpk::error_t						ardellEncode			(::gpk::ai32 & cache, const ::gpk::vcu8 & input, uint64_t key, bool salt, ::gpk::au8 & output);
	::gpk::error_t						ardellDecode			(::gpk::ai32 & cache, const ::gpk::vcu8 & input, uint64_t key, bool salt, ::gpk::au8 & output);

	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vcu8 & input, int key, bool salt, ::gpk::au8 & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu8 & input, int key, bool salt, ::gpk::au8 & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }
	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vci8 & input, int key, bool salt, ::gpk::au8 & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellEncode(tempCache, *(const ::gpk::vcu8*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu8 & input, int key, bool salt, ::gpk::ai8 & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, *(::gpk::au8*)&output); }
	stainli	::gpk::error_t				ardellEncode			(const ::gpk::vcc  & input, int key, bool salt, ::gpk::au8 & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellEncode(tempCache, *(const ::gpk::vcu8*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(const ::gpk::vcu8 & input, int key, bool salt, ::gpk::ac  & output)	{ ::gpk::ai32 tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, *(::gpk::au8*)&output); }

	stainli	::gpk::error_t				ardellEncode			(::gpk::ai32 & cache, const ::gpk::vci8 & input, int key, bool salt, ::gpk::au8 & output)	{ return ::gpk::ardellEncode(cache, *(const ::gpk::vcu8*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(::gpk::ai32 & cache, const ::gpk::vcu8 & input, int key, bool salt, ::gpk::ai8 & output)	{ return ::gpk::ardellDecode(cache, input, key, salt, *(::gpk::au8*)&output); }
	stainli	::gpk::error_t				ardellEncode			(::gpk::ai32 & cache, const ::gpk::vcc  & input, int key, bool salt, ::gpk::au8 & output)	{ return ::gpk::ardellEncode(cache, *(const ::gpk::vcu8*)&input, key, salt, output); }
	stainli	::gpk::error_t				ardellDecode			(::gpk::ai32 & cache, const ::gpk::vcu8 & input, int key, bool salt, ::gpk::ac  & output)	{ return ::gpk::ardellDecode(cache, input, key, salt, *(::gpk::au8*)&output); }

	::gpk::error_t						saltDataSalt			(const ::gpk::vcu8 & binary, ::gpk::au8 & salted);
	::gpk::error_t						saltDataUnsalt			(const ::gpk::vcu8 & salted, ::gpk::au8 & binary);

	template<typename _tBase>
	::gpk::error_t						rleEncode				(const ::gpk::view<_tBase> & decoded, ::gpk::au8 & encoded) {
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

	template<typename _tBase>
	::gpk::error_t						rleDecode			(const ::gpk::vcu8 & encoded, ::gpk::apod<_tBase>& decoded) {
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
	::gpk::error_t			utf8FromCodePoint	(uint32_t codePoint, ::gpk::ac & hexDigits);
	::gpk::error_t			digest				(const ::gpk::vcu8 & input, ::gpk::au32 & digest);
	::gpk::error_t			digest				(const ::gpk::vcu8 & input, ::gpk::ac & digest);
	stainli	::gpk::error_t	digest				(const ::gpk::vcc & input, ::gpk::ac & digest) { return ::gpk::digest(*(const ::gpk::vcu8*)&input, digest); }
}

#endif // GPK_ENCODING_H_209873982374
