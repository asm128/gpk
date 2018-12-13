#include "gpk_aes.h"
#include "gpk_rsa.h"
#include "gpk_encoding.h"
#include "gpk_timer.h"
#include "gpk_sync.h"
#include "gpk_noise.h"
#include <string>

static ::gpk::error_t								phex						(uint8_t* str, ::gpk::AES_LEVEL level);
static ::gpk::error_t								test_encrypt_cbc			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_decrypt_cbc			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_encrypt_ctr			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_decrypt_ctr			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_encrypt_ecb			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_decrypt_ecb			(::gpk::AES_LEVEL level);
static ::gpk::error_t								test_encrypt_ecb_verbose	(::gpk::AES_LEVEL level);

static	int											primalityTest						(uint64_t number)						{
	uint64_t												j									= (uint64_t)sqrt((double)number);
	if(0 == (number & 1))
		return 0;
	if(number > 1 && number < 9)
		return 1;
	for(uint64_t i = 3; i <= j; ++i) {
		if(0 == (number % i))
			return 0;
	}
	return 1;
}

int													main						()			{
	const ::gpk::view_const_string							testStrings	[]				=
		{ "0Some string to test 0"
		//,
		, "1Some string to test 01"
		, "2Some string to test 002"
		, "3Some string to test 0003"
		, "4Some string to test 00004"
		, "5Some string to test 000005"
		, "6Some string to test 0000007"
		, "7Some string to test 00000008"
		, "8Some string to test 000000009"
		, "9Some string to test 000000000A"
		, "ASome string to test 0000000000B"
		, "BSome string to test 00000000000C"
		, "CSome string to test 000000000000D"
		, "DSome string to test 0000000000000E"
		, "ESome string to test 00000000000000F"
		};
	const uint32_t											rounds						= 10;
	::gpk::array_pod<int32_t>								encodingCache;
	//{
	//	::gpk::STimer											timer;
	//	double													timeTotal					= 0;
	//	::gpk::array_pod<byte_t>								encoded		;
	//	::gpk::array_pod<byte_t>								decoded		;
	//	::gpk::array_pod<byte_t>								encoded64	;
	//	::gpk::array_pod<ubyte_t>								decoded64	;
	//	for(uint32_t iRound=0; iRound < rounds; ++iRound)
	//		for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
	//			encoded.clear();
	//			decoded.clear();
	//			::gpk::ardellEncode(encodingCache, testStrings[iTest]	, iTest, true, encoded);
	//			::gpk::ardellDecode(encodingCache, encoded				, iTest, true, decoded);
	//			error_if(memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
	//
	//			encoded64.clear();
	//			decoded64.clear();
	//			::gpk::base64Encode({(ubyte_t*)encoded.begin(), encoded.size()}, encoded64);
	//			::gpk::base64Decode(encoded64, decoded64);
	//			error_if(memcmp(encoded.begin(), decoded64.begin(), decoded64.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", encoded.begin(), decoded64.begin());
	//			timer.Frame();
	//			timeTotal											+= timer.LastTimeSeconds;
	//		}
	//	always_printf("------ Combined\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	//}
	{
		::gpk::STimer											timer;
		double													timeTotal					= 0;
		::gpk::array_pod<byte_t>								encoded						;
		::gpk::array_pod<byte_t>								decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(encodingCache, testStrings[iTest]	, (int)::gpk::noise1DBase(iTest), false, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encodingCache, encoded			, (int)::gpk::noise1DBase(iTest), false, decoded)), "%s", "Out of memory?");
				error_if(0 == decoded.size() || ::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Ardell\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer											timer;
		double													timeTotal					= 0;
		::gpk::array_pod<byte_t>								encoded						;
		::gpk::array_pod<byte_t>								decoded						;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(encodingCache, testStrings[iTest]	, (int)::gpk::noise1DBase(iTest), true, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encodingCache, encoded			, (int)::gpk::noise1DBase(iTest), true, decoded)), "%s", "Out of memory?");
				error_if(0 == decoded.size() || ::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Ardell (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}
	{
		::gpk::STimer											timer;
		double													timeTotal					= 0;
		::gpk::array_pod<byte_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(testStrings[iTest], (int)::gpk::noise1DBase(iTest), false, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encoded			, (int)::gpk::noise1DBase(iTest), false, decoded)), "%s", "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Ardell (cacheless)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_pod<byte_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				ce_if(errored(::gpk::ardellEncode(testStrings[iTest], (int)::gpk::noise1DBase(iTest), true, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::ardellDecode(encoded			, (int)::gpk::noise1DBase(iTest), true, decoded)), "%s", "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Ardell (cacheless) (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_obj<::gpk::array_pod<byte_t>	>			encodedList;
		::gpk::array_obj<::gpk::array_pod<ubyte_t>	>			decodedList;
		encodedList.resize(rounds * ::gpk::size(testStrings));
		decodedList.resize(rounds * ::gpk::size(testStrings));
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
				ce_if(errored(::gpk::base64Encode({(const ubyte_t*)testStrings[iTest].begin(), testStrings[iTest].size()}, encoded)), "%s", "Out of memory?");
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Base64 encode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		timeTotal											= 0;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
				::gpk::array_pod<ubyte_t>								& decoded					= decodedList[indexBuffer];
				if errored(::gpk::base64Decode(encoded, decoded)) {
					error_printf( "%s", "Out of memory?");
					encoded.clear_pointer();
					continue;
				}
				encoded.clear_pointer();
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Base64 decode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}

	{
		double													timeTotal					= 0;
		::gpk::array_obj<::gpk::array_pod<byte_t>	>			encodedList;
		::gpk::array_obj<::gpk::array_pod<ubyte_t>	>			decodedList;
		encodedList.resize(rounds * ::gpk::size(testStrings));
		decodedList.resize(rounds * ::gpk::size(testStrings));
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
				ce_if(errored(::gpk::hexEncode({(const ubyte_t*)testStrings[iTest].begin(), testStrings[iTest].size()}, encoded)), "%s", "Out of memory?");
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Hex encode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		timeTotal											= 0;
		for(uint32_t iRound=0; iRound < rounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
				::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
				::gpk::array_pod<ubyte_t>								& decoded					= decodedList[indexBuffer];
				if errored(::gpk::hexDecode(encoded, decoded)) {
					error_printf( "%s", "Out of memory?");
					encoded.clear_pointer();
					continue;
				}
				encoded.clear_pointer();
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ Hex decode\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average decode time: %g milliseconds.", rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
	}

	for(uint32_t iAESLevel = 0; iAESLevel < 3; ++iAESLevel) {
		{
			::gpk::STimer											timer;
			double													timeTotal					= 0;
			for(uint32_t iRound = 0; iRound < rounds; ++iRound)
				for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
					error_if(errored(::test_encrypt_cbc((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					error_if(errored(::test_decrypt_cbc((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					timer.Frame();
					timeTotal											+= timer.LastTimeSeconds;
				}
			always_printf("------ AES Level %u encode/decode (CBC)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", iAESLevel, rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		}
		{
			::gpk::STimer											timer;
			double													timeTotal					= 0;
			for(uint32_t iRound = 0; iRound < rounds; ++iRound)
				for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
					error_if(errored(::test_encrypt_ctr((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					error_if(errored(::test_decrypt_ctr((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					timer.Frame();
					timeTotal											+= timer.LastTimeSeconds;
				}
			always_printf("------ AES Level %u encode/decode (CTR)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", iAESLevel, rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		}
		{
			::gpk::STimer											timer;
			double													timeTotal					= 0;
			for(uint32_t iRound = 0; iRound < rounds; ++iRound)
				for(uint32_t iTest = 0; iTest < ::gpk::size(testStrings); ++iTest) {
					error_if(errored(::test_decrypt_ecb((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					error_if(errored(::test_encrypt_ecb((::gpk::AES_LEVEL)iAESLevel)), "%s", "Algorithm got broken?");
					timer.Frame();
					timeTotal											+= timer.LastTimeSeconds;
				}
			always_printf("------ AES Level %u encode/decode (ECB)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", iAESLevel, rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		}
		::test_encrypt_ecb_verbose((::gpk::AES_LEVEL)iAESLevel);
	}

	for(uint32_t iAESLevel = 0; iAESLevel < 3; ++iAESLevel) {
		::gpk::array_obj<::gpk::array_pod<byte_t>	>			encodedList;
		::gpk::array_obj<::gpk::array_pod<byte_t>	>			decodedList;
		gpk_necall(encodedList.resize(rounds * ::gpk::size(testStrings)), "%s", "Out of memory?");
		gpk_necall(decodedList.resize(rounds * ::gpk::size(testStrings)), "%s", "Out of memory?");
		{
			::gpk::STimer											timer;
			double													timeTotal					= 0;
			for(uint32_t iRound=0; iRound < rounds; ++iRound)
				for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
					int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
					::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
					ce_if(errored(::gpk::aesEncode({testStrings[iTest].begin(), testStrings[iTest].size()}, ::gpk::view_array<const ubyte_t>{(const ubyte_t*)"RandomnessAtLargeQuantities1234", 32}, (::gpk::AES_LEVEL)iAESLevel, encoded)), "%s", "Out of memory?");
					timer.Frame();
					timeTotal											+= timer.LastTimeSeconds;
				}
			always_printf("------ AES encode (level %u)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode time: %g milliseconds.", iAESLevel, rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		}
		{
			::gpk::STimer											timer;
			double													timeTotal					= 0;
			for(uint32_t iRound=0; iRound < rounds; ++iRound)
				for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
					int32_t													indexBuffer					= iRound * ::gpk::size(testStrings) + iTest;
					::gpk::array_pod<byte_t>								& encoded					= encodedList[indexBuffer];
					::gpk::array_pod<byte_t>								& decoded					= decodedList[indexBuffer];
					if errored(::gpk::aesDecode(encoded.begin(), encoded.size(), ::gpk::view_array<const ubyte_t>{(const ubyte_t*)"RandomnessAtLargeQuantities1234", 32}, (::gpk::AES_LEVEL)iAESLevel, decoded)) {
						error_printf("%s", "Out of memory?");
						encoded.clear_pointer();
						continue;
					}
					encoded.clear_pointer();
					error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
					timer.Frame();
					timeTotal											+= timer.LastTimeSeconds;
				}
			always_printf("------ AES decode (level %u)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average decode time: %g milliseconds.", iAESLevel, rounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rounds, timeTotal * 1000 / rounds / ::gpk::size(testStrings));
		}
	}

	::gpk::array_pod<::gpk::SRSAKeyPair>					rsaKeys						= {};	//, temp;
	uint32_t												rsa_n						= 0;
	{
		uint32_t												prime1						= 48593;
		//uint32_t												prime1						= 1093;
		uint32_t												prime2						= 48611;
		//uint32_t												prime2						= 3041;
		//uint32_t												prime1						= 19;
		//uint32_t												prime2						= 23;
		//uint32_t												prime1						= 251;
		//uint32_t												prime2						= 241;
		//uint32_t												prime1						= 521;
		//uint32_t												prime2						= 541;
		rsa_n												= prime1 * prime2;
		::gpk::STimer											timer;
		e_if(errored(::gpk::rsaKeys(prime1, prime2, 2, 0xFFFF, rsaKeys)), "Failed to initialize: %u, %u.", prime1, prime2);
		for(uint32_t iPair = 0; iPair < rsaKeys.size(); ++iPair)
			always_printf("Private: %llu. Public: %llu.", rsaKeys[iPair].Private, rsaKeys[iPair].Public);
		timer.Frame();
		always_printf("------ RSA Key generation time: %g seconds. Key count: %u.", timer.LastTimeSeconds, rsaKeys.size());
	}
	const uint32_t											rsaRounds					= (rsaKeys[rsaKeys.size() - 1].Public == rsaKeys[rsaKeys.size() - 1].Private) ? rsaKeys.size() - 1 : rsaKeys.size();
	{
		double													timeTotal					= 0;
		::gpk::array_pod<uint64_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rsaRounds; ++iRound) {
			double													timeRoundStart				= timeTotal;
			const uint32_t											pair						= iRound; //rand() % rsaKeys.size();
			always_printf("------ RSA (cacheless)\nStarting round of %u tests. Public: %llu, Private: %llu.", ::gpk::size(testStrings), rsaKeys[pair].Public, rsaKeys[pair].Private);
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				::gpk::STimer											timerEncode;
				ce_if(errored(::gpk::rsaEncode(testStrings[iTest]	, rsa_n, rsaKeys[pair].Public	, 0, encoded)), "%s", "Out of memory?");
				timerEncode.Frame();
				always_printf("------ RSA (cacheless)\nEncoding time for this step of %u size: %llu microseconds.", testStrings[iTest].size(), timerEncode.LastTimeMicroseconds);
				ce_if(errored(::gpk::rsaDecode(encoded				, rsa_n, rsaKeys[pair].Private	, decoded)), "%s", "Out of memory?");
				timerEncode.Frame();
				always_printf("------ RSA (cacheless)\nDecoding time for this step of %u size: %g milliseconds.", testStrings[iTest].size(), timerEncode.LastTimeMicroseconds / 1000.0);
				//always_printf("RSA:"
				//	"\nEncoded: %s."
				//	"\nDecoded: %s."
				//	, encoded.begin()
				//	, decoded.begin()
				//	);
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
				always_printf("------ RSA (cacheless)\nTime this step of %u size: %g milliseconds.", testStrings[iTest].size(), timer.LastTimeMicroseconds / 1000.0);
			}
			always_printf("------ RSA (cacheless)\nTime this round of %u tests: %g seconds. Public: %llu, Private: %llu.", ::gpk::size(testStrings), timeTotal - timeRoundStart, rsaKeys[pair].Public, rsaKeys[pair].Private);
		}
		always_printf("------ RSA (cacheless)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rsaRounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rsaRounds, timeTotal * 1000 / rsaRounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_pod<uint64_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rsaRounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				const uint32_t											pair						= iRound; //rand() % rsaKeys.size();
				ce_if(errored(::gpk::rsaEncode(testStrings[iTest]	, rsa_n, rsaKeys[pair].Public	, 0, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::rsaDecode(encoded				, rsa_n, rsaKeys[pair].Private	, decoded)), "%s", "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ RSA (cacheless) (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rsaRounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rsaRounds, timeTotal * 1000 / rsaRounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_pod<uint64_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rsaRounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				const int32_t											pair						= iRound; //rand() % ::gpk::max(0, ((int32_t)rsaKeys.size() - 1));
				ce_if(errored(::gpk::gpcEncode(testStrings[iTest]	, rsa_n, rsaKeys[pair].Public	, 0, false, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::gpcDecode(encoded				, rsa_n, rsaKeys[pair].Private	, false, decoded)), "%s", "Out of memory?");
				//always_printf("RSA:"
				//	"\nEncoded: %s."
				//	"\nDecoded: %s."
				//	, encoded.begin()
				//	, decoded.begin()
				//	);
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ GPC (cacheless)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rsaRounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rsaRounds, timeTotal * 1000 / rsaRounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_pod<uint64_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rsaRounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				const int32_t											pair						= 0; //rand() % rsaKeys.size();
				ce_if(errored(::gpk::gpcEncode(testStrings[iTest]	, rsa_n, rsaKeys[pair].Public	, 0, true, encoded)), "%s", "Out of memory?");
				ce_if(errored(::gpk::gpcDecode(encoded				, rsa_n, rsaKeys[pair].Private	, true, decoded)), "%s", "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ GPC (cacheless) (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rsaRounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rsaRounds, timeTotal * 1000 / rsaRounds / ::gpk::size(testStrings));
	}
	{
		double													timeTotal					= 0;
		::gpk::array_pod<uint64_t>								encoded;
		::gpk::array_pod<byte_t>								decoded;
		::gpk::STimer											timer;
		for(uint32_t iRound=0; iRound < rsaRounds; ++iRound)
			for(uint32_t iTest=0; iTest < ::gpk::size(testStrings); ++iTest) {
				encoded.clear();
				decoded.clear();
				const int32_t											pair						= iRound; //rand() % rsaKeys.size();
				ce_if(errored(::gpk::gpcEncodeWithHash(testStrings[iTest]	, rsa_n, rsaKeys[pair].Public	, 0, false, encoded)), "%s", "Out of memory?");
				e_if(errored(::gpk::gpcDecodeWithHash(encoded				, rsa_n, rsaKeys[pair].Private	, false, decoded)), "%s", "Out of memory?");
				error_if(::memcmp(testStrings[iTest].begin(), decoded.begin(), decoded.size()), "Failed to encode/decode! \nOriginal: %s\nDecoded: %s.", testStrings[iTest].begin(), decoded.begin());
				timer.Frame();
				timeTotal											+= timer.LastTimeSeconds;
			}
		always_printf("------ GPC hashed (cacheless) (salt)\nTotal time for %u rounds of %u sizes: %g seconds. Average round time: %g milliseconds. Average encode/decode time: %g milliseconds.", rsaRounds, ::gpk::size(testStrings), timeTotal, timeTotal * 1000 / rsaRounds, timeTotal * 1000 / rsaRounds / ::gpk::size(testStrings));
	}


	::gpk::array_pod<uint64_t>				primes;
	uint32_t								maxPrime		= 0xFFFFU;
	for(uint32_t i = 0xF000U; i < maxPrime; ++i) {
		if(primalityTest(i)) {
			primes.push_back(i);
			//always_printf("Prime found: %u.", i);
			::gpk::sleep(1);
		}
	}
	always_printf("Primes found: %u (%f%%).", primes.size(), (primes.size() / (float)maxPrime) * 100);

	return 0;
}

// prints string as hex
static	::gpk::error_t				phex							(uint8_t* str, ::gpk::AES_LEVEL level)			{
    uint8_t									len								= (uint8_t)::gpk::AES_LEVEL_PROPERTIES[level].KeyLength;
    unsigned char							i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
	return 0;
}

static	::gpk::error_t				test_encrypt_ecb_verbose		(::gpk::AES_LEVEL level)						{
    uint8_t i, buf[64], buf2[64];	    // Example of more verbose verification
    // 128bit key
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
    // 512bit text
    uint8_t plain_text[64] = {(uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,
							  (uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51,
							  (uint8_t) 0x30, (uint8_t) 0xc8, (uint8_t) 0x1c, (uint8_t) 0x46, (uint8_t) 0xa3, (uint8_t) 0x5c, (uint8_t) 0xe4, (uint8_t) 0x11, (uint8_t) 0xe5, (uint8_t) 0xfb, (uint8_t) 0xc1, (uint8_t) 0x19, (uint8_t) 0x1a, (uint8_t) 0x0a, (uint8_t) 0x52, (uint8_t) 0xef,
							  (uint8_t) 0xf6, (uint8_t) 0x9f, (uint8_t) 0x24, (uint8_t) 0x45, (uint8_t) 0xdf, (uint8_t) 0x4f, (uint8_t) 0x9b, (uint8_t) 0x17, (uint8_t) 0xad, (uint8_t) 0x2b, (uint8_t) 0x41, (uint8_t) 0x7b, (uint8_t) 0xe6, (uint8_t) 0x6c, (uint8_t) 0x37, (uint8_t) 0x10 };
	memset(buf, 0, 64);
	memset(buf2, 0, 64);
	// print text to encrypt, key and IV
	printf("ECB encrypt verbose:\n\n");
	printf("plain text:\n");
	for (i = (uint8_t) 0; i < (uint8_t) 4; ++i)
		phex(plain_text + i * (uint8_t) 16, level);
	printf("\n");
	printf("key:\n");
	phex(key, level);
	printf("\nciphertext:\n");	    // print the resulting cipher as 4 x 16 byte strings
	::gpk::SAESContext ctx;
	::gpk::aesInitCtx(&ctx, key, level);
	for (i = 0; i < 4; ++i) {
		::gpk::aesECBEncrypt(&ctx, plain_text + (i * 16));
		phex(plain_text + (i * 16), level);
    }
    printf("\n");
	return 0;
}

static	::gpk::error_t				test_encrypt_ecb				(::gpk::AES_LEVEL level)						{
    uint8_t									key_128[16]						= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t									out_128[::gpk::AES_SIZEBLOCK]	= { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };
    uint8_t									key_192[24]						= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
																				0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t									out_192[::gpk::AES_SIZEBLOCK]	= { 0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc };
    uint8_t									key_256[32]						= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
																				0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t									out_256[::gpk::AES_SIZEBLOCK]	= { 0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8 };

    uint8_t									in[::gpk::AES_SIZEBLOCK]		= { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };

    ::gpk::SAESContext						ctx;
	uint8_t									* key							= 0;
	uint8_t									* out							= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: key = key_128; out = out_128; break;
	case ::gpk::AES_LEVEL_192: key = key_192; out = out_192; break;
	case ::gpk::AES_LEVEL_256: key = key_256; out = out_256; break;
	default:
		error_printf("Invalid AES level: %u.", (uint32_t)level);
		return -1;
	}
    ::gpk::aesInitCtx(&ctx, key, level);
    ::gpk::aesECBEncrypt(&ctx, in);
	return (0 == memcmp((char*) out, (char*) in, 16)) ? 0 : -1;
}

static	::gpk::error_t				test_decrypt_cbc				(::gpk::AES_LEVEL level)						{
    uint8_t									key_128[16]						= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t									in_128[64]						= { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
																				0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
																				0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
																				0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };
    uint8_t									key_192[24]						= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t									in_192[64]						= { 0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d, 0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8,
																				0xb4, 0xd9, 0xad, 0xa9, 0xad, 0x7d, 0xed, 0xf4, 0xe5, 0xe7, 0x38, 0x76, 0x3f, 0x69, 0x14, 0x5a,
																				0x57, 0x1b, 0x24, 0x20, 0x12, 0xfb, 0x7a, 0xe0, 0x7f, 0xa9, 0xba, 0xac, 0x3d, 0xf1, 0x02, 0xe0,
																				0x08, 0xb0, 0xe2, 0x79, 0x88, 0x59, 0x88, 0x81, 0xd9, 0x20, 0xa9, 0xe6, 0x4f, 0x56, 0x15, 0xcd };
    uint8_t									key_256[32]						= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
																				0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t									in_256[64]						= { 0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
																				0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
																				0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
																				0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b };
    uint8_t									iv[::gpk::AES_SIZEIV]			= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t									out[64]							= { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
																			  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
																			  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
																			  0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };
//  uint8_t buffer[64];
    ::gpk::SAESContext						ctx;
	uint8_t									* key							= 0;
	uint8_t									* in							= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: key = key_128; in = in_128; break;
	case ::gpk::AES_LEVEL_192: key = key_192; in = in_192; break;
	case ::gpk::AES_LEVEL_256: key = key_256; in = in_256; break;
	}
    ::gpk::aesInitCtxIV(&ctx, key, level, iv);
    ::gpk::aesCBCDecryptBuffer(&ctx, in, 64);
    //printf("CBC decrypt: ");
	return (0 == memcmp((char*) out, (char*) in, 64)) ? 0 : -1;
}

static	::gpk::error_t				test_encrypt_cbc				(::gpk::AES_LEVEL level)						{
    uint8_t									key_128[16]						= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t									out_128[64]						= { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
																				0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
																				0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
																				0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };
    uint8_t									key_192[24]						= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t									out_192[64]						= { 0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d, 0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8,
																				0xb4, 0xd9, 0xad, 0xa9, 0xad, 0x7d, 0xed, 0xf4, 0xe5, 0xe7, 0x38, 0x76, 0x3f, 0x69, 0x14, 0x5a,
																				0x57, 0x1b, 0x24, 0x20, 0x12, 0xfb, 0x7a, 0xe0, 0x7f, 0xa9, 0xba, 0xac, 0x3d, 0xf1, 0x02, 0xe0,
																				0x08, 0xb0, 0xe2, 0x79, 0x88, 0x59, 0x88, 0x81, 0xd9, 0x20, 0xa9, 0xe6, 0x4f, 0x56, 0x15, 0xcd };
    uint8_t									key_256[32]						= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
																				0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t									out_256[64]						= { 0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
																				0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
																				0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
																				0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b };
    uint8_t									iv[::gpk::AES_SIZEIV]			= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t									in[64]							= { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
																				0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
																				0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
																				0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };
	uint8_t									* key							= 0;
	uint8_t									* out							= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: key = key_128; out = out_128; break;
	case ::gpk::AES_LEVEL_192: key = key_192; out = out_192; break;
	case ::gpk::AES_LEVEL_256: key = key_256; out = out_256; break;
	}

	::gpk::SAESContext						ctx;
    ::gpk::aesInitCtxIV(&ctx, key, level, iv);
    ::gpk::aesCBCEncryptBuffer(&ctx, in, 64);
    //printf("CBC encrypt: ");
	return (0 == memcmp((char*)out, (char*)in, 64)) ? 0 : -1;
}

static	::gpk::error_t				test_xcrypt_ctr					(const char* xcrypt, ::gpk::AES_LEVEL level);
static	::gpk::error_t				test_encrypt_ctr				(::gpk::AES_LEVEL level)						{ return test_xcrypt_ctr("encrypt", level); }
static	::gpk::error_t				test_decrypt_ctr				(::gpk::AES_LEVEL level)						{ return test_xcrypt_ctr("decrypt", level); }
static	::gpk::error_t				test_xcrypt_ctr					(const char* xcrypt, ::gpk::AES_LEVEL level)	{
    uint8_t									key_128	[16]					= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t									in_128	[64]					= { 0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26, 0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
																				0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff, 0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
																				0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e, 0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
																				0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1, 0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee };
    uint8_t									key_192	[24]					= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
																				0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t									in_192	[64]					= { 0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2, 0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b,
																				0x09, 0x03, 0x39, 0xec, 0x0a, 0xa6, 0xfa, 0xef, 0xd5, 0xcc, 0xc2, 0xc6, 0xf4, 0xce, 0x8e, 0x94,
																				0x1e, 0x36, 0xb2, 0x6b, 0xd1, 0xeb, 0xc6, 0x70, 0xd1, 0xbd, 0x1d, 0x66, 0x56, 0x20, 0xab, 0xf7,
																				0x4f, 0x78, 0xa7, 0xf6, 0xd2, 0x98, 0x09, 0x58, 0x5a, 0x97, 0xda, 0xec, 0x58, 0xc6, 0xb0, 0x50 };
    uint8_t									key_256	[32]					= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
																				0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t									in_256	[64]					= { 0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5, 0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28,
																				0xf4, 0x43, 0xe3, 0xca, 0x4d, 0x62, 0xb5, 0x9a, 0xca, 0x84, 0xe9, 0x90, 0xca, 0xca, 0xf5, 0xc5,
																				0x2b, 0x09, 0x30, 0xda, 0xa2, 0x3d, 0xe9, 0x4c, 0xe8, 0x70, 0x17, 0xba, 0x2d, 0x84, 0x98, 0x8d,
																				0xdf, 0xc9, 0xc5, 0x8d, 0xb6, 0x7a, 0xad, 0xa6, 0x13, 0xc2, 0xdd, 0x08, 0x45, 0x79, 0x41, 0xa6 };
	uint8_t									* key							= 0;
	uint8_t									* in							= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: key = key_128; in = in_128; break;
	case ::gpk::AES_LEVEL_192: key = key_192; in = in_192; break;
	case ::gpk::AES_LEVEL_256: key = key_256; in = in_256; break;
	}

    uint8_t									iv	[::gpk::AES_SIZEIV]			= { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
    uint8_t									out	[64]						= { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
																				0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
																				0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
																				0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };
    ::gpk::SAESContext						ctx;

    aesInitCtxIV(&ctx, key, level, iv);
    ::gpk::aesCTRXCryptBuffer(&ctx, in, 64);
	xcrypt;
    //printf("CTR %s: ", xcrypt);
    return (0 == memcmp((char *) out, (char *) in, 64)) ? 0 : -1;
}

static	::gpk::error_t				test_decrypt_ecb							(::gpk::AES_LEVEL level) {
	uint8_t									key_128	[16]								= { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	uint8_t									in_128	[::gpk::AES_SIZEBLOCK]				= { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };
	uint8_t									key_192	[24]								= { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
																							0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
	uint8_t									in_192	[::gpk::AES_SIZEBLOCK]				= { 0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc };
	uint8_t									key_256	[32]								= { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
																							0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
	uint8_t									in_256	[::gpk::AES_SIZEBLOCK]				= { 0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8 };


	uint8_t									out[::gpk::AES_SIZEBLOCK]					= { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
	::gpk::SAESContext						ctx;
	uint8_t									* key										= 0;
	uint8_t									* in										= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: key = key_128; in = in_128; break;
	case ::gpk::AES_LEVEL_192: key = key_192; in = in_192; break;
	case ::gpk::AES_LEVEL_256: key = key_256; in = in_256; break;
	}
	::gpk::aesInitCtx	(&ctx, key, level);
	::gpk::aesECBDecrypt(&ctx, in);
	return (0 == memcmp((char*) out, (char*) in, 16)) ? 0 : -1;
}


