#include "gpk_encoding.h"
#include "gpk_timer.h"

int										main						()			{
	::gpk::view_const_string					testStrings	[]				= 
		{ "Some string to test 0"
		, "Some string to test 01" 
		, "Some string to test 002" 
		, "Some string to test 0003" 
		, "Some string to test 00004" 
		, "Some string to test 000005" 
		, "Some string to test 0000006" 
		};
	const uint32_t								rounds						= 1000000;
	::gpk::array_pod<int32_t>					encodingCache				;

	//{
	//	::gpk::STimer								timer;
	//	uint64_t									timeTotal					= 0;
	//	for(uint32_t iRound=0; iRound < rounds; ++iRound) 
	//		for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
	//			::gpk::array_pod<byte_t>					encoded						;	
	//			::gpk::array_pod<byte_t>					decoded						;
	//			::gpk::ardellEncode(testStrings[iTest]	, iTest, true, encoded);
	//			::gpk::ardellDecode(encoded				, iTest, true, decoded);
	//			error_if(memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
	//
	//			::gpk::array_pod<byte_t>					encoded64					;	
	//			::gpk::base64Encode({(ubyte_t*)encoded.begin(), encoded.size()}, encoded64);
	//			::gpk::array_pod<ubyte_t>					decoded64					;
	//			::gpk::base64Decode(encoded64, decoded64);
	//			error_if(memcmp(encoded.begin(), decoded64.begin(), decoded64.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", encoded.begin(), decoded64.begin());
	//			timer.Frame();
	//			timeTotal								+= timer.LastTimeMicroseconds;
	//		}
	//	always_printf("------ Combined\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
	//}
	{
		::gpk::STimer								timer;
		uint64_t									timeTotal					= 0;
		::gpk::array_pod<byte_t>					encoded						;	
		::gpk::array_pod<byte_t>					decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(encodingCache, testStrings[iTest]	, iTest, false, encoded)), "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encodingCache, encoded			, iTest, false, decoded)), "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Ardell\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer								timer;
		uint64_t									timeTotal					= 0;
		::gpk::array_pod<byte_t>					encoded						;	
		::gpk::array_pod<byte_t>					decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(encodingCache, testStrings[iTest]	, iTest, true, encoded)), "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encodingCache, encoded			, iTest, true, decoded)), "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Ardell (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer								timer;
		uint64_t									timeTotal					= 0;
		::gpk::array_pod<byte_t>					encoded						;	
		::gpk::array_pod<byte_t>					decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(testStrings[iTest]	, iTest, false, encoded)), "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encoded			, iTest, false, decoded)), "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Ardell (cacheless)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer								timer;
		uint64_t									timeTotal					= 0;
		::gpk::array_pod<byte_t>					encoded						;	
		::gpk::array_pod<byte_t>					decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(testStrings[iTest]	, iTest, true, encoded)), "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encoded			, iTest, true, decoded)), "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Ardell (cacheless) (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer									timer;
		uint64_t										timeTotal					= 0;
		::gpk::array_obj<::gpk::array_pod<byte_t>	>	encodedList;	
		::gpk::array_obj<::gpk::array_pod<ubyte_t>	>	decodedList;
		encodedList.resize(rounds * ::gpk::size(testStrings));
		decodedList.resize(rounds * ::gpk::size(testStrings));

		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t											indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>						& encoded					= encodedList[indexBuffer];
				ce_if(errored(::gpk::base64Encode({(ubyte_t*)testStrings[iTest].begin(), testStrings[iTest].size()}, encoded)), "Out of memory?");
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Base64 encode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));
		timeTotal									= 0;
		for(uint32_t iRound=0; iRound < rounds; ++iRound) 
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t											indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>						& encoded					= encodedList[indexBuffer];
				::gpk::array_pod<ubyte_t>						& decoded					= decodedList[indexBuffer];
				ce_if(errored(::gpk::base64Decode(encoded, decoded)), "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal								+= timer.LastTimeMicroseconds;
			}
		always_printf("------ Base64 decode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal * .000001, timeTotal * .001 / rounds, timeTotal * .001 / rounds / ::gpk::size(testStrings));

	}
	return 0;
}