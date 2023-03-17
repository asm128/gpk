#include "gpk_array.h"

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
					::gpk::error_t					hexEncode													(const ::gpk::vcub	& inputBinary	, ::gpk::apod<char_t	> & out_hexed	);
					::gpk::error_t					hexDecode													(const ::gpk::vcc	& in_hexed		, ::gpk::apod<ubyte_t	> & outputBinary);
	stainli	::gpk::error_t					hexEncode													(const ::gpk::vcb	& inputBinary	, ::gpk::apod<char_t	> & out_hexed	) { return hexEncode(::gpk::vcub{(const ubyte_t*)inputBinary.begin(), inputBinary.size()}, out_hexed); }
					::gpk::error_t					hexDecode													(const ::gpk::vcc	& in_hexed		, ::gpk::apod<byte_t	> & outputBinary);
	// Based on Gary Ardell's code for VB.
					::gpk::error_t					ardellEncode												(::gpk::apod<int32_t> & cache, const ::gpk::vcb & input, uint64_t key, bool salt, ::gpk::apod<byte_t> & output);
					::gpk::error_t					ardellDecode												(::gpk::apod<int32_t> & cache, const ::gpk::vcb & input, uint64_t key, bool salt, ::gpk::apod<byte_t> & output);
	stainli	::gpk::error_t					ardellEncode												(const ::gpk::vcb & input, int key, bool salt, ::gpk::apod<byte_t> & output)	{ ::gpk::apod<int32_t> tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	stainli	::gpk::error_t					ardellDecode												(const ::gpk::vcb & input, int key, bool salt, ::gpk::apod<byte_t> & output)	{ ::gpk::apod<int32_t> tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }

					::gpk::error_t					saltDataSalt												(const ::gpk::vcb & binary, ::gpk::apod<byte_t> & salted);
					::gpk::error_t					saltDataUnsalt												(const ::gpk::vcb & salted, ::gpk::apod<byte_t> & binary);

	template<typename _tBase>
					::gpk::error_t					rleEncode													(const ::gpk::view<_tBase>& decoded, ::gpk::apod<byte_t>& encoded) {
  		uint32_t											idxLatest													= 0;
		stacxpr	const uint32_t					sizeBlock													= sizeof(_tBase) + 1;
  		for(uint32_t iIn = 0; iIn < decoded.size(); ++iIn) {
			const _tBase 										& current													= decoded[iIn];
  			const _tBase										& latest													= decoded[idxLatest];
    		if(0 == iIn || 0 != memcmp(&current, &latest, sizeof(_tBase)) || 255 == (uint8_t)encoded[encoded.size() - 1]) {
				const uint32_t										newSize														= encoded.size() + sizeBlock;
				gpk_necall(encoded.resize(newSize), "%s", "Failed to resize.");
				memcpy(&encoded[encoded.size() - sizeBlock], &current, sizeof(_tBase));
				encoded[encoded.size() - 1]						= 0;
				idxLatest										= iIn;
			}
      		else
          		++encoded[encoded.size() - 1];
		}
		return 0;
	}

	template<typename _tBase>
					::gpk::error_t					rleDecode													(const ::gpk::vcb & encoded, ::gpk::apod<_tBase>& decoded) {
		stacxpr	const uint32_t					sizeBlock													= sizeof(_tBase) + 1;
  		for(uint32_t iIn = 0; iIn < encoded.size(); iIn += sizeBlock) {
			const _tBase 										& current													= *(_tBase*)&encoded[iIn];
			const uint8_t										count														= encoded[iIn + sizeof(_tBase)];
			gpk_necall(decoded.push_back(current), "%s", "Failed to resize.");
      		for(uint32_t iOut = 0; iOut < count; ++iOut)
				gpk_necall(decoded.push_back(current), "%s", "Failed to resize.");
		}
		return 0;
	}

	// Description at http://en.wikipedia.org/wiki/UTF-8
	::gpk::error_t									utf8FromCodePoint											(uint32_t codePoint, ::gpk::apod<char_t> & hexDigits);

	::gpk::error_t									digest														(const ::gpk::vcb & input, ::gpk::apod<uint32_t> & digest);
	::gpk::error_t									digest														(const ::gpk::vcb & input, ::gpk::apod<byte_t> & digest);
}

#endif // GPK_ENCODING_H_209873982374
