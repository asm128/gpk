#include "gpk_encoding.h"
#include "gpk_view_bit.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"

#include <string>
#include <ctime>
#include <random>

			::gpk::error_t								gpk::saltDataSalt												(const ::gpk::view_const_byte& binary, ::gpk::array_pod<byte_t> & salted)				{
	gpk_necall(salted.resize(binary.size() * 2), "%s", "Out of memory?");
	byte_t														* pSalted														= salted.begin();
	const byte_t												* pBinary														= binary.begin();
	for(uint32_t iBinary = 0; iBinary < binary.size(); ++iBinary) {
		pSalted[iBinary * 2]									= pBinary[iBinary];
		pSalted[iBinary * 2 + 1]								= (::gpk::noise1DBase(::gpk::timeCurrentInUs()) + ::gpk::timeCurrentInUs()) & 0xFF;
	}
	return 0;
}

			::gpk::error_t								gpk::saltDataUnsalt												(const ::gpk::view_const_byte& salted, ::gpk::array_pod<byte_t> & binary)				{
	gpk_necall(binary.resize(salted.size() / 2), "%s", "Out of memory?");
	const byte_t												* pSalted														= salted.begin();
	byte_t														* pBinary														= binary.begin();
	for(uint32_t iBinary = 0; iBinary < binary.size(); ++iBinary)
		pBinary[iBinary]										= pSalted[iBinary * 2];
	return 0;
}

static		::gpk::error_t								hexFromByte														(uint8_t i, char* hexed)																{
	char														converted [0x20]												= {};
	sprintf_s(converted, "%*.2X", 2, i);
	hexed[0]												= converted[0];
	hexed[1]												= converted[1];
	return 0;
}

static		::gpk::error_t								hexToByte														(const char* s, uint8_t& byte)															{
	char														temp [3]														= {};
	temp[0]													= s[0];
	temp[1]													= s[1];
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
		int32_t														hex																= ::std::stoi(temp, nullptr, 16);
		byte													= (char)hex;
#else
	try {
		int32_t														hex																= ::std::stoi(temp, nullptr, 16);
		byte													= (char)hex;
	}
	catch (...) {
		byte													= '?';
		return -1;	// we should never get here
	}
#endif
	return 0;
}

			::gpk::error_t								gpk::hexEncode													(const ::gpk::view_array<const ubyte_t	> & in_binary, ::gpk::array_pod<char_t	> & out_hexed	)	{
	uint32_t													offset															= out_hexed.size();
	gpk_necall(out_hexed.resize(offset + in_binary.size() * 2), "%s", "Out of memory?");
	byte_t														* pHexed														= out_hexed.begin();
	const ubyte_t												* pBinary														= in_binary.begin();
	for(uint32_t iByte = 0; iByte < in_binary.size(); ++iByte)
		hexFromByte(pBinary[iByte], &pHexed[offset + iByte * 2]);
	return 0;
}

			::gpk::error_t								gpk::hexDecode													(const ::gpk::view_array<const char_t	> & in_hexed	, ::gpk::array_pod<ubyte_t	> & out_binary)	{
	uint32_t													offset															= out_binary.size();
	uint32_t													binarySize														= in_hexed.size() >> 1;
	gpk_necall(out_binary.resize(offset + binarySize), "%s", "Out of memory?");
	const byte_t												* pHexed														= in_hexed.begin();
	ubyte_t														* pBinary														= out_binary.begin();
	for(uint32_t iByte = 0; iByte < binarySize; ++iByte)
		hexToByte(&pHexed[iByte * 2], pBinary[offset + iByte]);
	return 0;
}

::gpk::error_t											gpk::ardellEncode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output)						{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	char														saltValue		[4]												= {};
	if (salt)
		for (int32_t i = 0; i < 4; i++) {
			int32_t														t																= 100 * (1 + saltValue[i]) * rand() * (((int32_t)time(0)) + 1);
			saltValue[i]											= t % 256;
		}
	int32_t    keyFinal[8]   =
	{ (int32_t)(11 + (key % 233))
	, (int32_t)( 7 + (key % 239))
	, (int32_t)( 5 + (key % 241))
	, (int32_t)( 3 + (key % 251))
	};
	int32_t														n																= salt ? input.size() + 4 : input.size();
	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t														* sn															= cache.begin();
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
	for(i = 1		; i  < n; ++i)	sn[i]				= sn[i] ^ sn[i - 1] ^ ((keyFinal[0] * sn[i - 1]) % 256);
	for(i = n - 2	; i >= 0; --i)	sn[i]				= sn[i] ^ sn[i + 1] ^  (keyFinal[1] * sn[i + 1]) % 256 ;
	for(i = 2		; i  < n; ++i)	sn[i]				= sn[i] ^ sn[i - 2] ^  (keyFinal[2] * sn[i - 1]) % 256 ;
	for(i = n - 3	; i >= 0; --i)	sn[i]				= sn[i] ^ sn[i + 2] ^  (keyFinal[3] * sn[i + 1]) % 256 ;

	uint32_t													outputOffset													= output.size();
	gpk_necall(output.resize(outputOffset + n), "%s", "Out of memory?");
	byte_t														* outputFast													= output.begin();
	for( i = 0; i < n; ++i)
		outputFast[outputOffset + i]							= (char)sn[i];
	return 0;
}

::gpk::error_t											gpk::ardellDecode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& output)		{
	// Originally written by Gary Ardell as Visual Basic code. free from all copyright restrictions.
	int32_t    keyFinal[8]   =
	{ (int32_t)(11 + (key % 233))
	, (int32_t)( 7 + (key % 239))
	, (int32_t)( 5 + (key % 241))
	, (int32_t)( 3 + (key % 251))
	};
	int32_t														n																= (int32_t)input.size();

	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t														* sn															= cache.begin();
	int32_t														i;
	for(i = 0		; i  < n	; ++i)	sn[i]			= input[i];
	for(i = 0		; i  < n - 2; ++i)	sn[i]			= sn[i] ^ sn[i + 2] ^ (keyFinal[3] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 2	; --i)	sn[i]			= sn[i] ^ sn[i - 2] ^ (keyFinal[2] * sn[i - 1]) % 256;
	for(i = 0		; i  < n - 1; ++i)	sn[i]			= sn[i] ^ sn[i + 1] ^ (keyFinal[1] * sn[i + 1]) % 256;
	for(i = n - 1	; i >= 1	; --i)	sn[i]			= sn[i] ^ sn[i - 1] ^ (keyFinal[0] * sn[i - 1]) % 256;

	uint32_t													outputOffset													= output.size();
	const uint32_t												finalStringSize													= salt ? n - 4 : n;
	const ::gpk::view_array<const int32_t>						finalValues														= {salt ? &sn[2] : sn, finalStringSize};
	gpk_necall(output.resize(outputOffset + finalStringSize), "%s", "Out of memory?");
	byte_t														* outputFast													= output.begin();
	const int32_t												* finalValuesFast												= finalValues.begin();
	for( i = 0; i < (int32_t)finalStringSize; ++i)
		outputFast[outputOffset + i]							= (char)finalValuesFast[i];
	return 0;
}

::gpk::error_t											gpk::utf8FromCodePoint											(uint32_t codePoint, ::gpk::array_pod<char_t> & hexDigits) {
	const uint32_t			offset = hexDigits.size();
	if (codePoint <= 0x7f) {
		hexDigits.resize(offset + 1);
		hexDigits[offset + 0] = static_cast<char>(codePoint);
	} else {
		if (codePoint <= 0x7FF) {
			hexDigits.resize(offset + 2);
			hexDigits[offset + 1] = static_cast<char>(0x80 | (0x3f & codePoint));
			hexDigits[offset + 0] = static_cast<char>(0xC0 | (0x1f & (codePoint >> 6)));
		} else if (codePoint <= 0xFFFF) {
			hexDigits.resize(offset + 3);
			hexDigits[offset + 2] = static_cast<char>(0x80 | (0x3f	&  codePoint));
			hexDigits[offset + 1] = static_cast<char>(0x80 | (0x3f	& (codePoint >> 6)));
			hexDigits[offset + 0] = static_cast<char>(0xE0 | (0xf	& (codePoint >> 12)));
		} else if (codePoint <= 0x10FFFF) {
			hexDigits.resize(offset + 4);
			hexDigits[offset + 3] = static_cast<char>(0x80 | (0x3f	&  codePoint));
			hexDigits[offset + 2] = static_cast<char>(0x80 | (0x3f	& (codePoint >> 6)));
			hexDigits[offset + 1] = static_cast<char>(0x80 | (0x3f	& (codePoint >> 12)));
			hexDigits[offset + 0] = static_cast<char>(0xF0 | (0x7	& (codePoint >> 18)));
		}
	}
	return 0;
}
