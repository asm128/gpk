#include "gpk_encoding.h"
#include "gpk_view_bit.h"

#include <ctime>
#include <random>

static constexpr const char								base64Symbols[]													= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

using namespace std;

			::gpk::error_t								gpk::base64Encode												(const ::gpk::view_array<const ubyte_t> & inputBytes, ::gpk::array_pod<char_t> & out_base64)	{
	rni_if(0 == inputBytes.size(), "%s", "Empty input stream.");
	const uint32_t												packsNeeded														= inputBytes.size() / 3 + one_if(inputBytes.size() % 3);
	uint32_t													iOutput64														= out_base64.size(); // append the result 
	gpk_necall(out_base64.resize(out_base64.size() + packsNeeded * 4), "Out of memory? out_base64.size() : %u. packsNeeded : %u.", iOutput64, packsNeeded);
	for(uint32_t iInputByte = 0, inputByteCount = inputBytes.size(); iInputByte < inputByteCount; iInputByte += 3) { // process each byte triplet and turn it into 4 bytes padded with 2 bit
		const bool													use1															= iInputByte < (inputByteCount - 1);
		const bool													use2															= iInputByte < (inputByteCount - 2);
		uint8_t														inputTriplet	[3]												= 
			{ inputBytes[iInputByte]
			, use1 ? inputBytes[iInputByte + 1] : (uint8_t)0U
			, use2 ? inputBytes[iInputByte + 2] : (uint8_t)0U
			};
		for(uint32_t iSingleIn = 0; iSingleIn < 3; ++iSingleIn) { // reverse bits of each input byte
			::gpk::view_bit<uint8_t>									inputBits													= {&inputTriplet[iSingleIn], 8};
			::gpk::reverse_bits(inputBits);
		}
		const ::gpk::view_bit<uint8_t>								inputBits														= inputTriplet;
		uint8_t														outputQuad		[4]												= {};
		uint32_t													iBitIn															= 0;
		for(uint32_t iSingleOut = 0; iSingleOut < ::gpk::size(outputQuad); ++iSingleOut) { // encode to four output bytes
			::gpk::view_bit<uint8_t>									outputBits														= {&outputQuad[iSingleOut], 6};
			for(uint32_t iBitOut = 0; iBitOut < 6; ++iBitOut) // copy the relevant input bits to the output bytes
				outputBits[iBitOut]										= (bool)inputBits[iBitIn++];
			::gpk::reverse_bits(outputBits);
			if(outputQuad[iSingleOut] >= ::gpk::size(base64Symbols) - 1) { 
				error_printf("%s", "Out of range. This could happen if the algorithm or the input character table got broken.");
				out_base64[iOutput64++]									= -1;
			}
			else
				out_base64[iOutput64++]									= base64Symbols[outputQuad[iSingleOut]];
		}
		//if(false == use2)							out_base64[out_base64.size() - 1]	= '=';
		//if(false == use1 && outputQuad[3] == 0)	out_base64[out_base64.size() - 2]	= '=';
	}
	const uint32_t												modTriplet														= inputBytes.size() % 3;
	if(0 != modTriplet) { // pad with '='
		out_base64[out_base64.size() - 1]						= '=';
		if(1 == modTriplet) // pad with another '='
			out_base64[out_base64.size() - 2]						= '=';
	}
	gpk_necall(out_base64.push_back(0), "%s", "Out of memory?"); // add a null byte at the end for safety.
	gpk_necall(out_base64.resize(out_base64.size() - 1), "%s", "There are no known reasons for this to fail so it probably indicates memory corruption or something really bad."); // restore the count to ignore the null byte just added.
	return 0;
}

			::gpk::error_t								gpk::base64Decode												(const ::gpk::view_array<const char_t> & in_base64, ::gpk::array_pod<ubyte_t> & outputBytes)	{
	rni_if(0 == in_base64.size(), "%s", "Empty base64 string.");
	int32_t														lengthInput														= in_base64.size();
	if(uint32_t mymod = in_base64.size() % 4) {
		ree_if((mymod != 1) || (in_base64.size() && in_base64[in_base64.size() - 1] != 0), "%s", "Invalid base64 string.");
		lengthInput												= in_base64.size() - 1;
	}
	// --- Generate symbol value remap table.
	uint8_t														base64SymbolRemap	[128]										= {};
	base64SymbolRemap[(uint32_t)'=']						= 0;
	for(uint32_t iSymbol = 0; iSymbol < 64; ++iSymbol)			
		base64SymbolRemap[(uint32_t)base64Symbols[iSymbol]]		= (uint8_t)iSymbol;

	// --- Decode input symbols
	const uint32_t												packsNeeded														= lengthInput / 4;
	uint32_t													iOutputByte														= outputBytes.size();
	gpk_necall(outputBytes.resize(outputBytes.size() + packsNeeded * 3), "Out of memory? outputBinary.size() : %u. packsNeeded : %u.", iOutputByte, packsNeeded); // Append result
	for(uint32_t iInput64 = 0, symbolCount = lengthInput; iInput64 < symbolCount; iInput64 += 4) {
		uint8_t														inputQuad	[4]													= 
			{ base64SymbolRemap[((uint8_t)in_base64[iInput64 + 0]) % 128]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 1]) % 128]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 2]) % 128]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 3]) % 128]
			};
		for(uint32_t iSingleIn = 0; iSingleIn < 4; ++iSingleIn) { // reverse bits before extracting
			::gpk::view_bit<uint8_t>									inputBits														= {&inputQuad[iSingleIn], 6};
			::gpk::reverse_bits(inputBits);
		}
		const ::gpk::view_bit<uint8_t>								inputBits														= inputQuad;
		uint8_t														outputTriplet		[3]											= {};
		uint32_t													iBitIn															= 0;
		for(uint32_t iSingleOut = 0; iSingleOut < 3; ++iSingleOut) {
			::gpk::view_bit<ubyte_t>									outputBits														= {&outputTriplet[iSingleOut], 8};
			for(uint32_t iBitOut = 0; iBitOut < 8; ++iBitOut) { // extract relevant bits from encoded bytes
				outputBits[iBitOut]										= inputBits[iBitIn + (iBitIn / 6) * 2];
				++iBitIn;
			}
			::gpk::reverse_bits(outputBits);
			outputBytes[iOutputByte++]								= outputTriplet[iSingleOut];
		}
	}
		 if(in_base64[in_base64.size() - 2] == '=') { outputBytes.resize(outputBytes.size() - 2); } // Remove leading nulls.
	else if(in_base64[in_base64.size() - 1] == '=') { outputBytes.resize(outputBytes.size() - 1); } // Remove leading nulls.
	return 0;
}

::gpk::error_t									gpk::ardellEncode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, int32_t key, bool salt, ::gpk::array_pod<byte_t>& output)						{
	char												saltValue		[4]												= {};
	if (salt) 
		for (int32_t i = 0; i < 4; i++) {
			int32_t												t																= 100 * (1 + saltValue[i]) * rand() * (((int32_t)time(0)) + 1);
			saltValue[i]									= t % 256;
		}
	int32_t												k1																= 11 + (key % 233);
	int32_t												k2																=  7 + (key % 239);
	int32_t												k3																=  5 + (key % 241);
	int32_t												k4																=  3 + (key % 251);
	int32_t												n																= salt ? input.size() + 4 : input.size();
	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t												* sn															= cache.begin();
	if(salt) {
		for(int32_t i = 0; i < 2; ++i)	
			sn[i]										= saltValue[i];
		for(int32_t i = 0; i < n - 4; ++i)	
			sn[2 + i]									= input[i];
		for(int32_t i = 0; i < 2; ++i)	
			sn[2 + n + i]								= saltValue[2 + i];
	}
	else 
		for(int32_t i = 0; i  < n; ++i)	
			sn[i]										= input[i];

	for(int32_t i = 1		; i  < n; ++i)	sn[i]		= sn[i] ^ sn[i - 1] ^ ((k1 * sn[i - 1]) % 256);
	for(int32_t i = n - 2	; i >= 0; --i)	sn[i]		= sn[i] ^ sn[i + 1] ^  (k2 * sn[i + 1]) % 256 ;
	for(int32_t i = 2		; i  < n; ++i)	sn[i]		= sn[i] ^ sn[i - 2] ^  (k3 * sn[i - 1]) % 256 ;
	for(int32_t i = n - 3	; i >= 0; --i)	sn[i]		= sn[i] ^ sn[i + 2] ^  (k4 * sn[i + 1]) % 256 ;

	uint32_t											outputOffset													= output.size();
	gpk_necall(output.resize(outputOffset + n), "%s", "Out of memory?");
	for(int32_t i = 0; i < n; ++i)	
		output[outputOffset + i]						= (char)sn[i];
	return 0;
}

::gpk::error_t									gpk::ardellDecode												(::gpk::array_pod<int32_t> & cache, const ::gpk::view_array<const byte_t>& input, int key, bool salt, ::gpk::array_pod<byte_t>& output)		{
	int32_t												k1																= 11 + (key % 233);
	int32_t												k2																=  7 + (key % 239);
	int32_t												k3																=  5 + (key % 241);
	int32_t												k4																=  3 + (key % 251);
	int32_t												n																= (int32_t)input.size();

	gpk_necall(cache.resize(n), "%s", "Out of memory?");
	int32_t												* sn															= cache.begin();
	for(int32_t i = 0		; i  < n	; ++i)	sn[i]	= input[i];
	for(int32_t i = 0		; i  < n - 2; ++i)	sn[i]	= sn[i] ^ sn[i + 2] ^ (k4 * sn[i + 1]) % 256;
	for(int32_t i = n - 1	; i >= 2	; --i)	sn[i]	= sn[i] ^ sn[i - 2] ^ (k3 * sn[i - 1]) % 256;
	for(int32_t i = 0		; i  < n - 1; ++i)	sn[i]	= sn[i] ^ sn[i + 1] ^ (k2 * sn[i + 1]) % 256;
	for(int32_t i = n - 1	; i >= 1	; --i)	sn[i]	= sn[i] ^ sn[i - 1] ^ (k1 * sn[i - 1]) % 256;
	
	uint32_t											outputOffset													= output.size();
	const uint32_t										finalStringSize													= salt ? n - 4 : n;
	const ::gpk::view_array<const int32_t>				finalValues														= {salt ? &sn[2] : sn, finalStringSize};
	gpk_necall(output.resize(outputOffset + finalStringSize), "%s", "Out of memory?");
	for(int32_t i = 0, count = finalStringSize; i < count; ++i)	
		output[outputOffset + i]						= (char)finalValues[i];
	return 0;
}
