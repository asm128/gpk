#include "gpk_encoding.h"
#include "gpk_view_bit.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"
#include "gpk_parse.h"

#include <ctime>
#include <random>

::gpk::error_t			gpk::saltDataSalt				(const ::gpk::vcu8 & binary, ::gpk::au8 & salted)				{
	gpk_necall(salted.resize(binary.size() * 2), "%s", "Out of memory?");
	uint8_t											* pSalted						= salted.begin();
	const uint8_t									* pBinary						= binary.begin();
	for(uint32_t iBinary = 0; iBinary < binary.size(); ++iBinary) {
		pSalted[iBinary * 2]							= pBinary[iBinary];
		pSalted[iBinary * 2 + 1]						= (::gpk::noise1DBase(::gpk::timeCurrentInUs()) + ::gpk::timeCurrentInUs()) & 0xFF;
	}
	return 0;
}

::gpk::error_t			gpk::saltDataUnsalt				(const ::gpk::vcu8 & salted, ::gpk::au8 & binary)				{
	gpk_necall(binary.resize(salted.size() / 2), "%s", "Out of memory?");
	const uint8_t									* pSalted						= salted.begin();
	uint8_t											* pBinary						= binary.begin();
	for(uint32_t iBinary = 0; iBinary < binary.size(); ++iBinary)
		pBinary[iBinary]							= pSalted[iBinary * 2];
	return 0;
}

static	::gpk::error_t	hexFromByte						(uint8_t i, char * hexed)																{
	char														converted [0x20]				= {};
	snprintf(converted, ::gpk::size(converted) - 1, "%*.2X", 2, i);
	hexed[0]												= converted[0];
	hexed[1]												= converted[1];
	return 0;
}

static	::gpk::error_t	hexToByte		(const char* s, uint8_t & byte)															{
	char														temp [3]						= {s[0], s[1]};
	gpk_necs(::gpk::parseIntegerHexadecimal(::gpk::vcs{temp}, &byte));
	return 0;
}

static	::gpk::error_t	hexToByte		(const char* s, int8_t & byte)															{
	char														temp [3]						= {s[0], s[1]};
	gpk_necs(::gpk::parseIntegerHexadecimal(::gpk::vcs{temp}, &byte));
	return 0;
}

::gpk::error_t			gpk::hexEncode					(const ::gpk::vcu8 & in_binary, ::gpk::ac & out_hexed	)	{
	uint32_t					offset							= out_hexed.size();
	gpk_necs(out_hexed.resize(offset + in_binary.size() * 2));
	char														* pHexed						= out_hexed.begin();
	const uint8_t												* pBinary						= in_binary.begin();
	for(uint32_t iByte = 0; iByte < in_binary.size(); ++iByte)
		hexFromByte(pBinary[iByte], &pHexed[offset + iByte * 2]);
	return 0;
}

::gpk::error_t			gpk::hexDecode					(const ::gpk::vcc & in_hexed	, ::gpk::au8 & out_binary)	{
	uint32_t					offset							= out_binary.size();
	uint32_t					binarySize						= in_hexed.size() >> 1;
	gpk_necall(out_binary.resize(offset + binarySize), "%s", "Out of memory?");
	const char													* pHexed						= in_hexed.begin();
	uint8_t														* pBinary						= out_binary.begin();
	for(uint32_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

::gpk::error_t			gpk::hexDecode					(const ::gpk::vcc & in_hexed	, ::gpk::ai8 & out_binary)	{
	uint32_t					offset							= out_binary.size();
	uint32_t					binarySize						= in_hexed.size() >> 1;
	gpk_necall(out_binary.resize(offset + binarySize), "%s", "Out of memory?");
	const char													* pHexed						= in_hexed.begin();
	int8_t														* pBinary						= out_binary.begin();
	for(uint32_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

::gpk::error_t			gpk::ardellEncode				(::gpk::apod<int32_t> & cache, const ::gpk::vcu8 & input, uint64_t key, bool salt, ::gpk::au8 & output)						{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	char														saltValue		[4]				= {};
	if (salt)
		for (int32_t i = 0; i < 4; i++) {
			int32_t														t								= 100 * (1 + saltValue[i]) * rand() * (((int32_t)time(0)) + 1);
			saltValue[i]											= t % 256;
		}
	const int32_t												keyFinal[8]						=
	{ (int32_t)(11 + (key % 233))
	, (int32_t)( 7 + (key % 239))
	, (int32_t)( 5 + (key % 241))
	, (int32_t)( 3 + (key % 251))
	};
	int32_t														n								= salt ? input.size() + 4 : input.size();
	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t														* sn							= cache.begin();
	if(salt) {
		for(int32_t i = 0; i < 2; ++i)
			sn[i]													= saltValue[i];
		for(int32_t i = 0; i < n - 4; ++i)
			sn[2 + i]												= input[i];
		for(int32_t i = 0; i < 2; ++i)
			sn[2 + n + i]											= saltValue[2 + i];
	}
	else
		for(int32_t i = 0; i < n; ++i)
			sn[i]													= input[i];
	int32_t														i;
	for(i = 1		; i  < n; ++i) sn[i] = sn[i] ^ sn[i - 1] ^ ((keyFinal[0] * sn[i - 1]) % 256);
	for(i = n - 2	; i >= 0; --i) sn[i] = sn[i] ^ sn[i + 1] ^  (keyFinal[1] * sn[i + 1]) % 256 ;
	for(i = 2		; i  < n; ++i) sn[i] = sn[i] ^ sn[i - 2] ^  (keyFinal[2] * sn[i - 1]) % 256 ;
	for(i = n - 3	; i >= 0; --i) sn[i] = sn[i] ^ sn[i + 2] ^  (keyFinal[3] * sn[i + 1]) % 256 ;

	uint32_t					outputOffset					= output.size();
	gpk_necall(output.resize(outputOffset + n), "%s", "Out of memory?");
	uint8_t														* outputFast					= output.begin();
	for( i = 0; i < n; ++i)
		outputFast[outputOffset + i]							= (char)sn[i];
	return 0;
}

::gpk::error_t			gpk::ardellDecode				(::gpk::apod<int32_t> & cache, const ::gpk::vcu8 & input, uint64_t key, bool salt, ::gpk::au8 & output)		{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	const int32_t												keyFinal[8]						=
		{ (int32_t)(11 + (key % 233))
		, (int32_t)( 7 + (key % 239))
		, (int32_t)( 5 + (key % 241))
		, (int32_t)( 3 + (key % 251))
		};
	int32_t														n								= (int32_t)input.size();

	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t														* sn							= cache.begin();
	int32_t														i;
	for(i = 0		; i  < n	; ++i)	sn[i]			= input[i];
	for(i = 0		; i  < n - 2; ++i)	sn[i]			= sn[i] ^ sn[i + 2] ^ (keyFinal[3] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 2	; --i)	sn[i]			= sn[i] ^ sn[i - 2] ^ (keyFinal[2] * sn[i - 1]) % 256;
	for(i = 0		; i  < n - 1; ++i)	sn[i]			= sn[i] ^ sn[i + 1] ^ (keyFinal[1] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 1	; --i)	sn[i]			= sn[i] ^ sn[i - 1] ^ (keyFinal[0] * sn[i - 1]) % 256;

	uint32_t					outputOffset					= output.size();
	const uint32_t				finalStringSize					= salt ? n - 4 : n;
	const ::gpk::vci32											finalValues						= {salt ? &sn[2] : sn, finalStringSize};
	gpk_necall(output.resize(outputOffset + finalStringSize), "%s", "Out of memory?");
	uint8_t														* outputFast					= output.begin();
	const int32_t												* finalValuesFast				= finalValues.begin();
	for( i = 0; i < (int32_t)finalStringSize; ++i)
		outputFast[outputOffset + i]							= (char)finalValuesFast[i];
	return 0;
}

::gpk::error_t			gpk::utf8FromCodePoint			(uint32_t codePoint, ::gpk::ac & hexDigits) {
	const uint32_t				offset							= hexDigits.size();
	if (codePoint <= 0x7f) {
		hexDigits.resize(offset + 1);
		hexDigits[offset + 0]									= static_cast<char>(codePoint);
	} else {
		if (codePoint <= 0x7FF) {
			hexDigits.resize(offset + 2);
			hexDigits[offset + 1]									= static_cast<char>(0x80 | (0x3f & codePoint));
			hexDigits[offset + 0]									= static_cast<char>(0xC0 | (0x1f & (codePoint >> 6)));
		} else if (codePoint <= 0xFFFF) {
			hexDigits.resize(offset + 3);
			hexDigits[offset + 2]									= static_cast<char>(0x80 | (0x3f &  codePoint));
			hexDigits[offset + 1]									= static_cast<char>(0x80 | (0x3f & (codePoint >> 6)));
			hexDigits[offset + 0]									= static_cast<char>(0xE0 | (0x0f & (codePoint >> 12)));
		} else if (codePoint <= 0x10FFFF) {
			hexDigits.resize(offset + 4);
			hexDigits[offset + 3]									= static_cast<char>(0x80 | (0x3f &  codePoint));
			hexDigits[offset + 2]									= static_cast<char>(0x80 | (0x3f & (codePoint >> 6)));
			hexDigits[offset + 1]									= static_cast<char>(0x80 | (0x3f & (codePoint >> 12)));
			hexDigits[offset + 0]									= static_cast<char>(0xF0 | (0x07 & (codePoint >> 18)));
		}
	}
	return 0;
}

::gpk::error_t			gpk::digest				(const ::gpk::vcu8 & input, ::gpk::au32 & digest)		{
	uint32_t											x								= 0;
	::gpk::au32								filtered						= {};
	for(uint32_t i = 0; i < input.size() - 8; ++i) {
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
	for(uint32_t i = 0; i < filtered.size() - 8; ++i) {
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
	for(uint32_t i = 2; i < (filtered.size() - 32); i += 2) {
		for(uint32_t j = 0; j < 32; j++)
			filtered[j]										+= filtered[i + j];
	}
	digest.append({filtered.begin(), ::gpk::min(32U, filtered.size())});
	return 0;
}

::gpk::error_t			gpk::digest									(const ::gpk::vcu8 & input, ::gpk::ac & digest)		{
	uint32_t											x								= 0;
	::gpk::au32								filtered						= {};
	for(uint32_t i = 0; i < input.size() - 8; ++i) {
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
	for(uint32_t i = 0; i < filtered.size() - 8; ++i) {
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
	for(uint32_t i = 2, count = (filtered.size() - 8); i < count; i += 2) {
		for(uint32_t j = 0; j < 8; j++)
			filtered[j]										+= filtered[i + j];
	}
	char										temp		[32]				= {};
	for(uint32_t i = 0; i < ::gpk::min(filtered.size(), 8U); ++i) {
		snprintf(temp, ::gpk::size(temp) - 2, "%i", filtered[i]);
		digest.append_string(temp);
	}
	return 0;
}
