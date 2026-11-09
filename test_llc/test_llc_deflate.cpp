#include "gpk_deflate.h"
#include "test_llc_suites.h"

#include "gpk_file.h"
#include "gpk_string_compose.h"

#include <cstring>

GDEFINE_ENUM_TYPE(DEFLATE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, COMPRESS_RESULT	, 0, "arrayDeflate() rejected valid input.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, INFLATE_RESULT	, 1, "arrayInflate() rejected a valid compressed stream.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, ROUND_TRIP_SIZE	, 2, "The inflated byte count differs from the source.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, ROUND_TRIP_BYTES	, 3, "The inflated bytes differ from the source.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, EMPTY_REJECTED	, 4, "arrayInflate() accepted an empty compressed stream.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, TRUNCATED_REJECTED, 5, "arrayInflate() accepted a truncated compressed stream.");
GDEFINE_ENUM_VALUED(DEFLATE_TEST_RESULT, CORRUPT_REJECTED	, 6, "arrayInflate() accepted a corrupted compressed stream.");

::gpk::err_t test_deflate(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::u0_t source[1025] = {};
	for(::gpk::u2_t iByte = 0; iByte < ::gpk::size(source); ++iByte)
		source[iByte] = (::gpk::u0_t)((iByte * 73U) ^ (iByte >> 3));

	cnst ::gpk::u2_t lengths[] = {0, 1, ::gpk::size(source)};
	if_fail_fe(::gpk::fileFromMemory(outputFolder, "source.bin", ::gpk::vcu0_t{source}));
	always_printf("%s", "DEFLATE fixtures: source.bin; each case saves its compressed and restored bytes. Expected restoration: first input-length bytes of source.bin.");
	cnst ::gpk::u2_t chunks [] = {1, 7, ::gpk::DEFLATE_DEFAULT_CHUNK_SIZE};
	for(::gpk::u2_t length : lengths)
		for(::gpk::u2_t chunkSize : chunks) {
			::gpk::au0_t compressed = {}, restored = {};
			cnst ::gpk::err_t compression = ::gpk::arrayDeflate({source, length}, compressed, chunkSize);
			always_printf("DEFLATE input:%u bytes, chunk:%u; compression result:%i, compressed:%u bytes; expected successful round trip.", length, chunkSize, compression, compressed.size());
			::gpk::testTraceBytes("Input", {source, length});
			::gpk::testTraceBytes("Compressed", compressed);
			GPK_TEST_CHECKF(errors, DEFLATE_TEST_RESULT_COMPRESS_RESULT, ::gpk::failed(compression), "Input:%u, chunk:%u, result:%i.", length, chunkSize, compression);
			if(::gpk::failed(compression))
				continue;
			cnst ::gpk::err_t inflation = ::gpk::arrayInflate(compressed, restored, chunkSize);
			always_printf("Inflation result:%i; restored:%u, expected:%u bytes; byte equality:%s.", inflation, restored.size(), length, (restored == source) ? "PASS" : "FAIL");
			::gpk::testTraceBytes("Restored", restored);
			char name[96] = {};
			if_fail_fe(::gpk::sprintf_s(name, "input_%u_chunk_%u.deflate", length, chunkSize));
			if_fail_fe(::gpk::fileFromMemory(outputFolder, ::gpk::vcst_t{name}, compressed));
			if_fail_fe(::gpk::sprintf_s(name, "input_%u_chunk_%u.restored.bin", length, chunkSize));
			if_fail_fe(::gpk::fileFromMemory(outputFolder, ::gpk::vcst_t{name}, restored));
			GPK_TEST_CHECKF(errors, DEFLATE_TEST_RESULT_INFLATE_RESULT, ::gpk::failed(inflation), "Input:%u, chunk:%u, result:%i.", length, chunkSize, inflation);
			if(::gpk::failed(inflation))
				continue;
			GPK_TEST_CHECKF(errors, DEFLATE_TEST_RESULT_ROUND_TRIP_SIZE, restored.size() != length, "Input:%u, chunk:%u, output:%u.", length, chunkSize, restored.size());
			if(restored.size() == length)
				GPK_TEST_CHECKF(errors, DEFLATE_TEST_RESULT_ROUND_TRIP_BYTES, length && 0 != ::memcmp(restored.begin(), source, length), "Input:%u, chunk:%u.", length, chunkSize);
		}

#if !defined(GPK_ESP32) && !defined(GPK_ARDUINO)
	::gpk::au0_t output = {}, compressed = {};
	cnst ::gpk::err_t emptyResult = ::gpk::arrayInflate(::gpk::vcu0_t{}, output, 7);
	always_printf("Inflate empty stream: result:%i; expected negative (rejection).", emptyResult);
	GPK_TEST_CHECK(errors, DEFLATE_TEST_RESULT_EMPTY_REJECTED, false == ::gpk::failed(emptyResult));
	cnst ::gpk::err_t compression = ::gpk::arrayDeflate({source, ::gpk::size(source)}, compressed, 7);
	GPK_TEST_REQUIREF(errors, DEFLATE_TEST_RESULT_COMPRESS_RESULT, ::gpk::failed(compression), "Invalid-stream fixture result:%i.", compression);
	GPK_TEST_REQUIRE(errors, DEFLATE_TEST_RESULT_COMPRESS_RESULT, compressed.size() < 2);
	output.clear();
	cnst ::gpk::err_t truncatedResult = ::gpk::arrayInflate({compressed.begin(), compressed.size() - 1}, output, 7);
	always_printf("Inflate stream missing final byte: input:%u, result:%i; expected negative.", compressed.size() - 1, truncatedResult);
	GPK_TEST_CHECK(errors, DEFLATE_TEST_RESULT_TRUNCATED_REJECTED, false == ::gpk::failed(truncatedResult));
	compressed[0] ^= 0xFF;
	output.clear();
	cnst ::gpk::err_t corruptResult = ::gpk::arrayInflate(compressed, output, 7);
	always_printf("Inflate stream with first byte XOR FF: result:%i; expected negative.", corruptResult);
	GPK_TEST_CHECK(errors, DEFLATE_TEST_RESULT_CORRUPT_REJECTED, false == ::gpk::failed(corruptResult));
#endif
	rtrn 0;
}
