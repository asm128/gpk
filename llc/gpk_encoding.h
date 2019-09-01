#include "gpk_array.h"

#ifndef GPK_ENCODING_H_209873982374
#define GPK_ENCODING_H_209873982374

namespace gpk
{
					::gpk::error_t					hexEncode													(const ::gpk::view_array<const ubyte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_hexed	);
					::gpk::error_t					hexDecode													(const ::gpk::view_array<const char_t	> & in_hexed	, ::gpk::array_pod<ubyte_t	> & outputBinary);
	static inline	::gpk::error_t					hexEncode													(const ::gpk::view_array<const byte_t	> & inputBinary	, ::gpk::array_pod<char_t	> & out_hexed	) { return hexEncode(::gpk::view_const_ubyte{(const ubyte_t*)inputBinary.begin(), inputBinary.size()}, out_hexed); }
					::gpk::error_t					hexDecode													(const ::gpk::view_array<const char_t	> & in_hexed	, ::gpk::array_pod<byte_t	> & outputBinary);
	// Based on Gary Ardell's code for VB.
					::gpk::error_t					ardellEncode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output);
					::gpk::error_t					ardellDecode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output);
	static inline	::gpk::error_t					ardellEncode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellEncode(tempCache, input, key, salt, output); }
	static inline	::gpk::error_t					ardellDecode												(const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{ ::gpk::array_pod<int32_t>	tempCache; return ::gpk::ardellDecode(tempCache, input, key, salt, output); }

					::gpk::error_t					saltDataSalt												(const ::gpk::view_const_byte& binary, ::gpk::array_pod<byte_t> & salted);
					::gpk::error_t					saltDataUnsalt												(const ::gpk::view_const_byte& salted, ::gpk::array_pod<byte_t> & binary);

	template<typename _tBase>
					::gpk::error_t					rleEncode													(const ::gpk::view_array<_tBase>& decoded, ::gpk::array_pod<byte_t>& encoded) {
  		uint32_t											idxLatest													= 0;
		static constexpr	const uint32_t					sizeBlock													= sizeof(_tBase) + 1;
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
					::gpk::error_t					rleDecode													(const ::gpk::view_const_byte& encoded, ::gpk::array_pod<_tBase>& decoded) {
		static constexpr	const uint32_t					sizeBlock													= sizeof(_tBase) + 1;
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
	::gpk::error_t									utf8FromCodePoint											(uint32_t codePoint, ::gpk::array_pod<char_t> & hexDigits);

	::gpk::error_t									digest														(const ::gpk::view_const_byte & input, ::gpk::array_pod<uint32_t> & digest);
	::gpk::error_t									digest														(const ::gpk::view_const_byte & input, ::gpk::array_pod<byte_t> & digest);
}

#endif // GPK_ENCODING_H_209873982374
