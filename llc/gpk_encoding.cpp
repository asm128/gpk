#include "gpk_encoding.h"
#include "gpk_cstring.h"
#include "gpk_view_bit.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"
#include "gpk_parse.h"


#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <ctime>
#	include <random>
#endif

GPK_USING_TYPEINT();

stxp ::gpk::vcst_c	HEX_DIGITS_UPPER	= "0123456789ABCDEF";
stxp ::gpk::vcst_c	HEX_DIGITS_LOWER	= "0123456789abcdef";

gpk::err_t			gpk::saltDataSalt				(vcu0_c & binary, ::gpk::au0_t & salted)				{
	if_fail_fe(salted.resize(binary.size() * 2));
	u0_t						* pSalted						= salted.begin();
	const u0_t				* pBinary						= binary.begin();
	for(::gpk::u2_t iBinary = 0; iBinary < binary.size(); ++iBinary) {
		pSalted[iBinary * 2]		= pBinary[iBinary];
		pSalted[iBinary * 2 + 1]	= (::gpk::noise1DBase(::gpk::timeCurrentInUs()) + ::gpk::timeCurrentInUs()) & 0xFF;
	}
	return 0;
}

gpk::err_t			gpk::saltDataUnsalt				(vcu0_c & salted, ::gpk::au0_t & binary)				{
	if_fail_fe(binary.resize(salted.size() / 2));
	const u0_t				* pSalted						= salted.begin();
	u0_t						* pBinary						= binary.begin();
	for(::gpk::u2_t iBinary = 0; iBinary < binary.size(); ++iBinary)
		pBinary[iBinary]		= pSalted[iBinary * 2];
	return 0;
}

static	gpk::err_t hexFromByte		(u0_t byteValue, char * hexed)																{
	hexed[0] = HEX_DIGITS_UPPER[byteValue >> 4];
	hexed[1] = HEX_DIGITS_UPPER[byteValue & 0xF];
	return 0;
}

static	gpk::err_t hexToByte		(const char* s, u0_t & byte)															{
	char						temp [3]		= {s[0], s[1]};
	if_fail_fe(gpk::parseIntegerHexadecimal(::gpk::vcst_t{temp}, byte));
	return 0;
}

static	gpk::err_t hexToByte		(const char* s, ::gpk::s0_t & byte)															{
	char						temp [3]		= {s[0], s[1]};
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
	const char					* pHexed						= in_hexed.begin();
	u0_t						* pBinary						= out_binary.begin();
	for(::gpk::u2_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

gpk::err_t			gpk::hexDecode					(::gpk::vcst_t in_hexed	, ::gpk::as0_t & out_binary)	{
	::gpk::u2_t					offset							= out_binary.size();
	::gpk::u2_t					binarySize						= in_hexed.size() >> 1;
	if_fail_fe(out_binary.resize(offset + binarySize));
	const char					* pHexed						= in_hexed.begin();
	s0_t						* pBinary						= out_binary.begin();
	for(::gpk::u2_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

gpk::err_t			gpk::ardellEncode				(as2_t & cache, vcu0_c & input, u3_t key, bool salt, ::gpk::au0_t & output)						{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	char						saltValue		[4]				= {};
	if (salt)
		for (s2_t i = 0; i < 4; i++) {
			s2_t					t								= 100 * (1 + saltValue[i]) * rand() * (((s2_t)time(0)) + 1);
			saltValue[i]		= t % 256;
		}
	const s2_t				keyFinal[8]						=
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
		outputFast[outputOffset + i]	= (char)sn[i];
	return 0;
}

gpk::err_t			gpk::ardellDecode				(::gpk::as2_t & cache, vcu0_c & input, u3_t key, bool salt, ::gpk::au0_t & output)		{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	const s2_t				keyFinal[8]						=
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
	const ::gpk::vcs2_t			finalValues						= {salt ? &sn[2] : sn, finalStringSize};
	if_fail_fe(output.resize(outputOffset + finalStringSize));
	u0_t						* outputFast					= output.begin();
	const s2_t				* finalValuesFast				= finalValues.begin();
	for( i = 0; i < (s2_t)finalStringSize; ++i)
		outputFast[outputOffset + i]	= (char)finalValuesFast[i];
	return 0;
}

gpk::err_t			gpk::utf8FromCodePoint			(::gpk::u2_t codePoint, ::gpk::string & output) {
	u2_c				offset							= output.size();
	if (codePoint <= 0x7f) {
		output.resize(offset + 1);
		output[offset + 0]	= static_cast<char>(codePoint);
	} else {
		if (codePoint <= 0x7FF) {
			output.resize(offset + 2);
			output[offset + 1]	= static_cast<char>(0x80 | (0x3f & codePoint));
			output[offset + 0]	= static_cast<char>(0xC0 | (0x1f & (codePoint >> 6)));
		} else if (codePoint <= 0xFFFF) {
			output.resize(offset + 3);
			output[offset + 2]	= static_cast<char>(0x80 | (0x3f &  codePoint));
			output[offset + 1]	= static_cast<char>(0x80 | (0x3f & (codePoint >> 6)));
			output[offset + 0]	= static_cast<char>(0xE0 | (0x0f & (codePoint >> 12)));
		} else if (codePoint <= 0x10FFFF) {
			output.resize(offset + 4);
			output[offset + 3]	= static_cast<char>(0x80 | (0x3f &  codePoint));
			output[offset + 2]	= static_cast<char>(0x80 | (0x3f & (codePoint >> 6)));
			output[offset + 1]	= static_cast<char>(0x80 | (0x3f & (codePoint >> 12)));
			output[offset + 0]	= static_cast<char>(0xF0 | (0x07 & (codePoint >> 18)));
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
	char						temp		[32]				= {};
	for(::gpk::u2_t i = 0; i < ::gpk::min(filtered.size(), (::gpk::u2_t)8U); ++i) {
		::gpk::sprintf_s(temp, "%" GPK_FMT_U2, filtered[i]);
		if_fail_fe(digest.append_string(temp));
	}
	return 0;
}
