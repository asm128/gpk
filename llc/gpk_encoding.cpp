#include "gpk_encoding.h"
#include "gpk_cstring.h"
#include "gpk_view_bit.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"
#include "gpk_parse.h"
#include "gpk_string_compose.h"


#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <ctime>
#	include <random>
#endif

GPK_USING_TYPEINT();

stxp ::gpk::vcst_c	HEX_DIGITS_UPPER	= "0123456789ABCDEF";
stxp ::gpk::vcst_c	HEX_DIGITS_LOWER	= "0123456789abcdef";

::gpk::astu2_c<128> gpk::CP437Map = {{
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7, 0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9, 0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA, 0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F, 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B, 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4, 0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248, 0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0,
}};

gpk::u2_t		gpk::unicodeFromCP437		(u0_t glyph) {
	return glyph < 0x80 ? glyph : CP437Map[glyph - 0x80];
}

gpk::u0_t		gpk::cp437FromUnicode		(u2_t codePoint, u0_t fallback) {
	if(codePoint >= 0x20 && codePoint < 0x7F)
		return (u0_t)codePoint;
	cnst err_t			iGlyph				= ::gpk::find(codePoint, CP437Map);
	return 0 <= iGlyph ? (u0_t)(iGlyph + 0x80) : fallback;
}

gpk::err_t			gpk::saltDataSalt				(vcu0_c & binary, ::gpk::au0_t & salted)				{
	if_fail_fe(salted.resize(binary.size() * 2));
	u0_t						* pSalted						= salted.begin();
	cnst u0_t				* pBinary						= binary.begin();
	for(::gpk::u2_t iBinary = 0; iBinary < binary.size(); ++iBinary) {
		pSalted[iBinary * 2]		= pBinary[iBinary];
		pSalted[iBinary * 2 + 1]	= (::gpk::noise1DBase(::gpk::timeCurrentInUs()) + ::gpk::timeCurrentInUs()) & 0xFF;
	}
	return 0;
}

gpk::err_t			gpk::saltDataUnsalt				(vcu0_c & salted, ::gpk::au0_t & binary)				{
	if_fail_fe(binary.resize(salted.size() / 2));
	cnst u0_t				* pSalted						= salted.begin();
	u0_t						* pBinary						= binary.begin();
	for(::gpk::u2_t iBinary = 0; iBinary < binary.size(); ++iBinary)
		pBinary[iBinary]		= pSalted[iBinary * 2];
	return 0;
}

sttc	gpk::err_t hexFromByte		(u0_t byteValue, sc_t * hexed)																{
	hexed[0] = HEX_DIGITS_UPPER[byteValue >> 4];
	hexed[1] = HEX_DIGITS_UPPER[byteValue & 0xF];
	return 0;
}

sttc	gpk::err_t hexToByte		(cnst sc_t* s, u0_t & byte)															{
	sc_t						temp [3]		= {s[0], s[1]};
	if_fail_fe(gpk::parseIntegerHexadecimal(::gpk::vcst_t{temp}, byte));
	return 0;
}

sttc	gpk::err_t hexToByte		(cnst sc_t* s, ::gpk::s0_t & byte)															{
	sc_t						temp [3]		= {s[0], s[1]};
	if_fail_fe(gpk::parseIntegerHexadecimal(::gpk::vcst_t{temp}, byte));
	return 0;
}

gpk::err_t			gpk::hexEncode					(vcu0_c & in_binary, ::gpk::string & out_hexed	)	{
	::gpk::u2_t					offset							= out_hexed.size();
	if_fail_fe(out_hexed.resize(offset + in_binary.size() * 2));
	for(::gpk::u2_t iByte = 0; iByte < in_binary.size(); ++iByte)
		hexFromByte(in_binary[iByte], &out_hexed[offset + iByte * 2]);
	return 0;
}

gpk::err_t			gpk::hexDecode					(::gpk::vcst_t in_hexed	, ::gpk::au0_t & out_binary)	{
	::gpk::u2_t					offset							= out_binary.size();
	::gpk::u2_t					binarySize						= in_hexed.size() >> 1;
	if_fail_fe(out_binary.resize(offset + binarySize));
	cnst sc_t					* pHexed						= in_hexed.begin();
	u0_t						* pBinary						= out_binary.begin();
	for(::gpk::u2_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

gpk::err_t			gpk::hexDecode					(::gpk::vcst_t in_hexed	, ::gpk::as0_t & out_binary)	{
	::gpk::u2_t					offset							= out_binary.size();
	::gpk::u2_t					binarySize						= in_hexed.size() >> 1;
	if_fail_fe(out_binary.resize(offset + binarySize));
	cnst sc_t					* pHexed						= in_hexed.begin();
	s0_t						* pBinary						= out_binary.begin();
	for(::gpk::u2_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

gpk::err_t			gpk::ardellEncode				(as2_t & cache, vcu0_c & input, u3_t key, b8_t salt, ::gpk::au0_t & output)						{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	sc_t						saltValue		[4]				= {};
	if (salt)
		for (s2_t i = 0; i < 4; i++) {
			s2_t					t								= 100 * (1 + saltValue[i]) * rand() * (((s2_t)time(0)) + 1);
			saltValue[i]		= t % 256;
		}
	cnst s2_t				keyFinal[8]						=
	{ (s2_t)(11 + (key % 233))
	, (s2_t)( 7 + (key % 239))
	, (s2_t)( 5 + (key % 241))
	, (s2_t)( 3 + (key % 251))
	};
	s2_t						n								= salt ? input.size() + 4 : input.size();
	if_fail_fe(cache.resize(n));
	s2_t						* sn							= cache.begin();
	if(salt) {
		for(s2_t i = 0; i < 2; ++i)
			sn[i]					= saltValue[i];
		for(s2_t i = 0; i < n - 4; ++i)
			sn[2 + i]				= input[i];
		for(s2_t i = 0; i < 2; ++i)
			sn[2 + n + i]			= saltValue[2 + i];
	}
	else
		for(s2_t i = 0; i < n; ++i)
			sn[i]					= input[i];
	s2_t						i;
	for(i = 1		; i  < n; ++i) sn[i] = sn[i] ^ sn[i - 1] ^ ((keyFinal[0] * sn[i - 1]) % 256);
	for(i = n - 2	; i >= 0; --i) sn[i] = sn[i] ^ sn[i + 1] ^  (keyFinal[1] * sn[i + 1]) % 256 ;
	for(i = 2		; i  < n; ++i) sn[i] = sn[i] ^ sn[i - 2] ^  (keyFinal[2] * sn[i - 1]) % 256 ;
	for(i = n - 3	; i >= 0; --i) sn[i] = sn[i] ^ sn[i + 2] ^  (keyFinal[3] * sn[i + 1]) % 256 ;

	::gpk::u2_t					outputOffset					= output.size();
	if_fail_fe(output.resize(outputOffset + n));
	u0_t					* outputFast					= output.begin();
	for( i = 0; i < n; ++i)
		outputFast[outputOffset + i]	= (sc_t)sn[i];
	return 0;
}

gpk::err_t			gpk::ardellDecode				(::gpk::as2_t & cache, vcu0_c & input, u3_t key, b8_t salt, ::gpk::au0_t & output)		{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	cnst s2_t				keyFinal[8]						=
		{ (s2_t)(11 + (key % 233))
		, (s2_t)( 7 + (key % 239))
		, (s2_t)( 5 + (key % 241))
		, (s2_t)( 3 + (key % 251))
		};
	s2_t						n								= (s2_t)input.size();

	if_fail_fe(cache.resize(n));
	s2_t						* sn							= cache.begin();
	s2_t						i;
	for(i = 0		; i  < n	; ++i)	sn[i]			= input[i];
	for(i = 0		; i  < n - 2; ++i)	sn[i]			= sn[i] ^ sn[i + 2] ^ (keyFinal[3] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 2	; --i)	sn[i]			= sn[i] ^ sn[i - 2] ^ (keyFinal[2] * sn[i - 1]) % 256;
	for(i = 0		; i  < n - 1; ++i)	sn[i]			= sn[i] ^ sn[i + 1] ^ (keyFinal[1] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 1	; --i)	sn[i]			= sn[i] ^ sn[i - 1] ^ (keyFinal[0] * sn[i - 1]) % 256;

	::gpk::u2_t					outputOffset					= output.size();
	u2_c				finalStringSize					= salt ? n - 4 : n;
	cnst ::gpk::vcs2_t			finalValues						= {salt ? &sn[2] : sn, finalStringSize};
	if_fail_fe(output.resize(outputOffset + finalStringSize));
	u0_t						* outputFast					= output.begin();
	cnst s2_t				* finalValuesFast				= finalValues.begin();
	for( i = 0; i < (s2_t)finalStringSize; ++i)
		outputFast[outputOffset + i]	= (sc_t)finalValuesFast[i];
	return 0;
}

gpk::err_t			gpk::utf8FromCodePoint			(::gpk::u2_t codePoint, ::gpk::string & output) {
	u2_c				offset							= output.size();
	if (codePoint <= 0x7f) {
		output.resize(offset + 1);
		output[offset + 0]	= static_cast<sc_t>(codePoint);
	} else {
		if (codePoint <= 0x7FF) {
			output.resize(offset + 2);
			output[offset + 1]	= static_cast<sc_t>(0x80 | (0x3f & codePoint));
			output[offset + 0]	= static_cast<sc_t>(0xC0 | (0x1f & (codePoint >> 6)));
		} else if (codePoint <= 0xFFFF) {
			output.resize(offset + 3);
			output[offset + 2]	= static_cast<sc_t>(0x80 | (0x3f &  codePoint));
			output[offset + 1]	= static_cast<sc_t>(0x80 | (0x3f & (codePoint >> 6)));
			output[offset + 0]	= static_cast<sc_t>(0xE0 | (0x0f & (codePoint >> 12)));
		} else if (codePoint <= 0x10FFFF) {
			output.resize(offset + 4);
			output[offset + 3]	= static_cast<sc_t>(0x80 | (0x3f &  codePoint));
			output[offset + 2]	= static_cast<sc_t>(0x80 | (0x3f & (codePoint >> 6)));
			output[offset + 1]	= static_cast<sc_t>(0x80 | (0x3f & (codePoint >> 12)));
			output[offset + 0]	= static_cast<sc_t>(0xF0 | (0x07 & (codePoint >> 18)));
		}
	}
	return 0;
}

gpk::err_t			gpk::digest				(vcu0_c & input, ::gpk::au2_t & digest)		{
	::gpk::u2_t					x								= 0;
	::gpk::au2_t					filtered						= {};
	for(::gpk::u2_t i = 0; i < input.size() - 8; ++i) {
		x	+= ::gpk::noise1DBase32(input[i])
			+  ::gpk::noise1DBase32(input[i + 1])
			+  ::gpk::noise1DBase32(input[i + 2])
			+  ::gpk::noise1DBase32(input[i + 3])
			+  ::gpk::noise1DBase32(input[i + 4])
			+  ::gpk::noise1DBase32(input[i + 5])
			+  ::gpk::noise1DBase32(input[i + 6])
			+  ::gpk::noise1DBase32(input[i + 7])
			;
		x												+= x ^ (x << 11);
		filtered.push_back(x);
	}
	for(::gpk::u2_t i = 0; i < filtered.size() - 8; ++i) {
		filtered[i]	^= ::gpk::noise1DBase32(filtered[i])
					+  ::gpk::noise1DBase32(filtered[i + 1])
					+  ::gpk::noise1DBase32(filtered[i + 2])
					+  ::gpk::noise1DBase32(filtered[i + 3])
					+  ::gpk::noise1DBase32(filtered[i + 4])
					+  ::gpk::noise1DBase32(filtered[i + 5])
					+  ::gpk::noise1DBase32(filtered[i + 6])
					+  ::gpk::noise1DBase32(filtered[i + 7])
					;
	}
	for(::gpk::u2_t i = 2; i < (filtered.size() - 32); i += 2) {
		for(::gpk::u2_t j = 0; j < 32; j++)
			filtered[j]										+= filtered[i + j];
	}
	digest.append({filtered.begin(), ::gpk::min((::gpk::u2_t)32U, filtered.size())});
	return 0;
}

gpk::err_t			gpk::digest						(vcu0_c & input, ::gpk::string & digest)		{
	u2_t						x								= 0;
	::gpk::au2_t				filtered						= {};
	for(::gpk::u2_t i = 0; i < input.size() - 8; ++i) {
		x	+= ::gpk::noise1DBase32(input[i])
			+  ::gpk::noise1DBase32(input[i + 1])
			+  ::gpk::noise1DBase32(input[i + 2])
			+  ::gpk::noise1DBase32(input[i + 3])
			+  ::gpk::noise1DBase32(input[i + 4])
			+  ::gpk::noise1DBase32(input[i + 5])
			+  ::gpk::noise1DBase32(input[i + 6])
			+  ::gpk::noise1DBase32(input[i + 7])
			;
		x						+= x ^ (x << 11);
		filtered.push_back(x);
	}
	for(::gpk::u2_t i = 0; i < filtered.size() - 8; ++i) {
		filtered[i]	^= ::gpk::noise1DBase32(filtered[i])
					+  ::gpk::noise1DBase32(filtered[i + 1])
					+  ::gpk::noise1DBase32(filtered[i + 2])
					+  ::gpk::noise1DBase32(filtered[i + 3])
					+  ::gpk::noise1DBase32(filtered[i + 4])
					+  ::gpk::noise1DBase32(filtered[i + 5])
					+  ::gpk::noise1DBase32(filtered[i + 6])
					+  ::gpk::noise1DBase32(filtered[i + 7])
					;
	}
	for(::gpk::u2_t i = 2, count = (filtered.size() - 8); i < count; i += 2) {
		for(::gpk::u2_t j = 0; j < 8; j++)
			filtered[j]				+= filtered[i + j];
	}
	sc_t						temp		[32]				= {};
	for(::gpk::u2_t i = 0; i < ::gpk::min(filtered.size(), (::gpk::u2_t)8U); ++i) {
		::gpk::sprintf_s(temp, "%" GPK_FMT_U2, filtered[i]);
		if_fail_fe(::gpk::append_strings(digest, temp));
	}
	return 0;
}
