#include "gpk_array.h"
#include "gpk_string.h"
#include "gpk_stdstring.h"

#ifndef GPK_ENCODING_H_23627
#define GPK_ENCODING_H_23627

namespace gpk
{
	err_t			hexEncode				(vcu0_c & inputBinary, string & out_hexed	);

	err_t			hexDecode				(vcst_t in_hexed, au0_t & outputBinary	);
	err_t			hexDecode				(vcst_t in_hexed, as0_t & outputBinary	);
	stin	err_t	hexEncode				(const vcs0_t & inputBinary, string & out_hexed	)			{ return hexEncode(vcu0_t{(const u0_t*)inputBinary.begin(), inputBinary.size()}, out_hexed); }

	// Based on Gary Ardell's code for VB.
	err_t			ardellEncode			(as2_t & cache, vcu0_c & input, u3_t key, bool salt, au0_t & output);
	err_t			ardellDecode			(as2_t & cache, vcu0_c & input, u3_t key, bool salt, au0_t & output);

	stin	err_t	ardellEncode			(vcu0_c & input, u3_t key, bool salt, au0_t & output)			{ as2_t tempCache; return ardellEncode(tempCache, input, key, salt, output); }
	stin	err_t	ardellDecode			(vcu0_c & input, u3_t key, bool salt, au0_t & output)			{ as2_t tempCache; return ardellDecode(tempCache, input, key, salt, output); }
	stin	err_t	ardellEncode			(const vcs0_t & input, u3_t key, bool salt, au0_t & output)		{ as2_t tempCache; return ardellEncode(tempCache, *(vcu0_c*)&input, key, salt, output); }
	stin	err_t	ardellDecode			(vcu0_c & input, u3_t key, bool salt, as0_t & output)			{ as2_t tempCache; return ardellDecode(tempCache, input, key, salt, *(au0_t*)&output); }

	stin	err_t	ardellEncode			(as2_t & cache, const vcs0_t & input, u3_t key, bool salt, au0_t & output)	{ return ardellEncode(cache, *(vcu0_c*)&input, key, salt, output); }
	stin	err_t	ardellDecode			(as2_t & cache, vcu0_c & input, u3_t key, bool salt, as0_t & output)		{ return ardellDecode(cache, input, key, salt, *(au0_t*)&output); }
	err_t			saltDataSalt			(vcu0_c & binary, au0_t & salted);
	err_t			saltDataUnsalt			(vcu0_c & salted, au0_t & binary);

	tplT err_t		rleEncode				(const view<T> & decoded, au0_t & encoded) {
		u2_t				idxLatest				= 0;
		stxp	u2_c		sizeBlock				= szof(T) + 1;
		for(u2_t iIn = 0; iIn < decoded.size(); ++iIn) {
			cnst T 				& current				= decoded[iIn];
			cnst T				& latest				= decoded[idxLatest];
			if(0 == iIn || 0 != memcmp(&current, &latest, szof(T)) || 255 == (u0_t)encoded[encoded.size() - 1]) {
				u2_c				newSize					= encoded.size() + sizeBlock;
				gpk_necs(encoded.resize(newSize));
				memcpy(&encoded[encoded.size() - sizeBlock], &current, szof(T));
				encoded[encoded.size() - 1]			= 0;
				idxLatest							= iIn;
			}
			else
				++encoded[encoded.size() - 1];
		}
		return 0;
	}

	tplT err_t		rleDecode			(vcu0_c & encoded, apod<T> & decoded) {
		stxp	u2_c		sizeBlock			= szof(T) + 1;
  		for(u2_t iIn = 0; iIn < encoded.size(); iIn += sizeBlock) {
			cnst T 				& current			= *(T*)&encoded[iIn];
			u0_c				count				= encoded[iIn + szof(T)];
			gpk_necs(decoded.push_back(current));
			for(u2_t iOut = 0; iOut < count; ++iOut)
				gpk_necs(decoded.push_back(current));
		}
		return 0;
	}

	// Description at http://en.wikipedia.org/wiki/UTF-8
	err_t			utf8FromCodePoint	(u2_t codePoint, string & output);
	err_t			digest				(vcu0_c & input, au2_t & digest);
	err_t			digest				(vcu0_c & input, string & digest);
	stin	err_t	digest				(const vcs0_t & input, string & digest) { return ::gpk::digest(*(vcu0_c*)&input, digest); }
}

#endif // GPK_ENCODING_H_23627


