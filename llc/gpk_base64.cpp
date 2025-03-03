#include "gpk_base64.h"
#include "gpk_view_bit.h"

static	::gpk::error_t	base64EncodeTriplet								(const ::gpk::vcc & base64Symbols, ::gpk::vu8 inputTriplet, ::gpk::vu8 out_base64) {
	for (uint32_t iSingleIn = 0; iSingleIn < 3; ++iSingleIn) { // reverse bits of each input byte
		::gpk::view_bit<uint8_t>									inputBits														= {&inputTriplet[iSingleIn], 8};
		::gpk::reverse_bits(inputBits);
	}
	const ::gpk::view_bit<uint8_t>								inputBits														= {inputTriplet.begin(), 24};
	uint32_t					iBitIn															= 0;
	uint8_t														outputQuad[4]													= {};
	for (uint32_t iSingleOut = 0; iSingleOut < ::gpk::size(outputQuad); ++iSingleOut) { // encode to four output bytes
		::gpk::view_bit<uint8_t>									outputBits														= {&outputQuad[iSingleOut], 6};
		for (uint32_t iBitOut = 0; iBitOut < 6; ++iBitOut) // copy the relevant input bits to the output bytes
			outputBits[iBitOut]										= (bool)inputBits[iBitIn++];
		::gpk::reverse_bits(outputBits);
	}
	for (uint32_t iSingleOut = 0; iSingleOut < ::gpk::size(outputQuad); ++iSingleOut) { // encode to four output bytes
		if (outputQuad[iSingleOut] >= base64Symbols.size()) {
			error_printf("%s", "Out of range. This could happen if the algorithm or the input character table got broken.");
			out_base64[iSingleOut]									= (uint8_t)-1;
		}
		else
			out_base64[iSingleOut]									= base64Symbols[outputQuad[iSingleOut]];
	}
	return 0;
}
::gpk::error_t			gpk::base64Encode								(const ::gpk::vcc & base64Symbols, sc_t base64PadSymbol, const ::gpk::vcu8 & inputBytes, ::gpk::au8 & out_base64)	{
	rni_if(0 == inputBytes.size(), "%s", "Empty input stream.");
	const uint32_t				packsNeeded														= inputBytes.size() / 3 + one_if(inputBytes.size() % 3);
	uint32_t					iOutput64														= out_base64.size(); //
	gpk_necall(out_base64.resize(out_base64.size() + packsNeeded * 4), "Out of memory? out_base64.size() : %u. packsNeeded : %u.", iOutput64, packsNeeded);
	for(uint32_t iInputByte = 0, inputByteCount = inputBytes.size(); iInputByte < inputByteCount; iInputByte += 3) { // process each byte triplet and turn it into 4 bytes padded with 2 bit
		const bool													use1															= ((int32_t)iInputByte) < (((int32_t)inputByteCount) - 1);
		const bool													use2															= ((int32_t)iInputByte) < (((int32_t)inputByteCount) - 2);
		uint8_t														inputTriplet	[3]												=
			{		 (uint8_t)(inputBytes[iInputByte]		& 0xFF)
			, use1 ? (uint8_t)(inputBytes[iInputByte + 1]	& 0xFF): (uint8_t)0U
			, use2 ? (uint8_t)(inputBytes[iInputByte + 2]	& 0xFF): (uint8_t)0U
			};
		::base64EncodeTriplet(base64Symbols, inputTriplet, {&out_base64[iOutput64], 4});
		iOutput64												+= 4;
	}
	const uint32_t				modTriplet														= inputBytes.size() % 3;
	if(0 != modTriplet) { // pad with '='
		out_base64[out_base64.size() - 1]						= base64PadSymbol;
		if(1 == modTriplet) // pad with another '='
			out_base64[out_base64.size() - 2]						= base64PadSymbol;
	}
	gpk_necs(out_base64.push_back(0)); // add a null byte at the end for safety.
	gpk_necs(out_base64.resize(out_base64.size() - 1)); // restore the count to ignore the null byte just added.
	return 0;
}

static	::gpk::error_t	base64DecodeQuad												(::gpk::vu8 inputQuad, ::gpk::vu8 outputBytes)	{
	for(uint32_t iSingleIn = 0; iSingleIn < 4; ++iSingleIn) { // reverse bits before extracting
		::gpk::view_bit<uint8_t>									inputBits														= {&inputQuad[iSingleIn], 6};
		::gpk::reverse_bits(inputBits);
	}
	const ::gpk::view_bit<uint8_t>								inputBits														= {inputQuad.begin(), inputQuad.size() * 8};
	uint8_t														outputTriplet		[3]											= {};
	uint32_t					iBitIn															= 0;
	for(uint32_t iSingleOut = 0; iSingleOut < 3; ++iSingleOut) {
		::gpk::view_bit<uint8_t>									outputBits														= {&outputTriplet[iSingleOut], 8};
		for(uint32_t iBitOut = 0; iBitOut < 8; ++iBitOut) { // extract relevant bits from encoded bytes
			outputBits[iBitOut]										= inputBits[iBitIn + (iBitIn / 6) * 2];
			++iBitIn;
		}
		::gpk::reverse_bits(outputBits);
		outputBytes[iSingleOut]								= outputTriplet[iSingleOut];
	}
	return 0;
}

::gpk::error_t			gpk::base64Decode								(const ::gpk::vcc & base64Symbols, sc_t base64PadSymbol, const ::gpk::vcu8 & in_base64, ::gpk::au8 & outputBytes)	{
	rni_if(0 == in_base64.size(), "%s", "Empty base64 string.");
	int32_t														lengthInput														= in_base64.size();
	if(uint32_t mymod = in_base64.size() % 4) {
		ree_if((mymod != 1) || (in_base64.size() && in_base64[in_base64.size() - 1] != 0), "%s", "Invalid base64 string.");
		lengthInput												= in_base64.size() - 1;
	}
	// --- Generate symbol value remap table.
	uint8_t														base64SymbolRemap	[128]										= {};
	base64SymbolRemap[(uint32_t)base64PadSymbol]			= 0;
	for(uint32_t iSymbol = 0; iSymbol < 64; ++iSymbol)
		base64SymbolRemap[(uint32_t)base64Symbols[iSymbol]]		= (uint8_t)iSymbol;

	// --- Decode input symbols
	const uint32_t				packsNeeded														= lengthInput / 4;
	uint32_t					iOutputByte														= outputBytes.size();
	gpk_necall(outputBytes.resize(outputBytes.size() + packsNeeded * 3), "Out of memory? outputBinary.size() : %u. packsNeeded : %u.", iOutputByte, packsNeeded); // Append result
	for(uint32_t iInput64 = 0, symbolCount = lengthInput; iInput64 < symbolCount; iInput64 += 4) {
		uint8_t														inputQuad	[4]													=
			{ base64SymbolRemap[((uint8_t)in_base64[iInput64 + 0]) % ::gpk::size(base64SymbolRemap)]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 1]) % ::gpk::size(base64SymbolRemap)]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 2]) % ::gpk::size(base64SymbolRemap)]
			, base64SymbolRemap[((uint8_t)in_base64[iInput64 + 3]) % ::gpk::size(base64SymbolRemap)]
			};
		::base64DecodeQuad(inputQuad, {&outputBytes[iOutputByte], 3});
		iOutputByte												+= 3;
	}
		 if(in_base64[in_base64.size() - 2] == base64PadSymbol) { outputBytes.resize(outputBytes.size() - 2); } // Remove leading nulls.
	else if(in_base64[in_base64.size() - 1] == base64PadSymbol) { outputBytes.resize(outputBytes.size() - 1); } // Remove leading nulls.
	return 0;
}
