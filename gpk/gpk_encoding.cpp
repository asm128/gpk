#include "gpk_encoding.h"
#include "gpk_bit_view.h"

static constexpr const char								base64Symbols[]													= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			::gpk::error_t								gpk::base64Encode												(const ::gpk::array_view<const ubyte_t> & inputBytes, ::gpk::array_pod<char_t> & out_base64)	{
	rni_if(0 == inputBytes.size(), "Empty input stream.");
	const uint32_t												packsNeeded														= inputBytes.size() / 3 + one_if(inputBytes.size() % 3);
	uint32_t													iOutput64														= out_base64.size(); // append the result 
	gpk_necall(out_base64.resize(out_base64.size() + packsNeeded * 4), "Out of memory? out_base64.size() : %u. packsNeeded : %u.", iOutput64, packsNeeded);
	for(uint32_t iInputByte = 0, inputByteCount = inputBytes.size(); iInputByte < inputByteCount; iInputByte += 3) { // process each byte triplet and turn it into 4 bytes padded with 2 bit
		const bool													use1															= iInputByte < (inputByteCount - 1);
		const bool													use2															= iInputByte < (inputByteCount - 2);
		uint8_t														inputTriplet	[3]												= 
			{ inputBytes[iInputByte]
			, use1 ? inputBytes[iInputByte + 1] : 0U
			, use2 ? inputBytes[iInputByte + 2] : 0U
			};
		for(uint32_t iSingleIn = 0; iSingleIn < 3; ++iSingleIn) { // reverse bits of each input byte
			::gpk::bit_view<uint8_t>									inputBits													= {&inputTriplet[iSingleIn], 8};
			::gpk::reverse_bits(inputBits);
		}
		const ::gpk::bit_view<uint8_t>								inputBits														= inputTriplet;
		uint8_t														outputQuad		[4]												= {};
		uint32_t													iBitIn															= 0;
		for(uint32_t iSingleOut = 0; iSingleOut < ::gpk::size(outputQuad); ++iSingleOut) { // encode to four output bytes
			::gpk::bit_view<uint8_t>									outputBits														= {&outputQuad[iSingleOut], 6};
			for(uint32_t iBitOut = 0; iBitOut < 6; ++iBitOut) // copy the relevant input bits to the output bytes
				outputBits[iBitOut]										= (bool)inputBits[iBitIn++];
			::gpk::reverse_bits(outputBits);
			if(outputQuad[iSingleOut] >= ::gpk::size(base64Symbols) - 1) { 
				error_printf("Out of range. This could happen if the algorithm or the input character table got broken.");
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
	gpk_necall(out_base64.push_back(0), "Out of memory?"); // add a null byte at the end for safety.
	gpk_necall(out_base64.resize(out_base64.size() - 1), "There are no known reasons for this to fail so it probably indicates memory corruption or something really bad."); // restore the count to ignore the null byte just added.
	return 0;
}

			::gpk::error_t								gpk::base64Decode												(const ::gpk::array_view<const char_t> & in_base64, ::gpk::array_pod<ubyte_t> & outputBytes)	{
	rni_if(0 == in_base64.size(), "Empty base64 string.");
	int32_t														lengthInput														= in_base64.size();
	if(uint32_t mymod = in_base64.size() % 4) {
		ree_if((mymod != 1) || (in_base64.size() && in_base64[in_base64.size() - 1] != 0), "Invalid base64 string.");
		lengthInput												= in_base64.size() - 1;
	}
	// --- Generate symbol value remap table.
	uint8_t														base64SymbolRemap	[128]										= {};
	base64SymbolRemap['=']									= 0;
	for(uint32_t iSymbol = 0; iSymbol < 64; ++iSymbol)			
		base64SymbolRemap[base64Symbols[iSymbol]]				= (uint8_t)iSymbol;

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
			::gpk::bit_view<uint8_t>									inputBits														= {&inputQuad[iSingleIn], 6};
			::gpk::reverse_bits(inputBits);
		}
		const ::gpk::bit_view<uint8_t>								inputBits														= inputQuad;
		uint8_t														outputTriplet		[3]											= {};
		uint32_t													iBitIn															= 0;
		for(uint32_t iSingleOut = 0; iSingleOut < 3; ++iSingleOut) {
			::gpk::bit_view<ubyte_t>									outputBits														= {&outputTriplet[iSingleOut], 8};
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
