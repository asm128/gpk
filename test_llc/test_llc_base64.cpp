#include "gpk_base64.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(BASE64_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ENCODE_RESULT	, 0, "base64Encode() rejected valid bytes.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ENCODED_TEXT	, 1, "base64Encode() produced unexpected text.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, DECODE_RESULT	, 2, "base64Decode() rejected valid Base64 text.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ROUND_TRIP		, 3, "Base64 decoding did not restore the source bytes.");

::gpk::err_t test_base64(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	cnst ::gpk::u0_t source[] = {'h', 'e', 'l', 'l', 'o'};
	cnst ::gpk::vcu0_t input = {source};
	::gpk::string encoded;
	cnst ::gpk::err_t encodeResult = ::gpk::base64Encode(input, encoded);
	GPK_TEST_REQUIRE(errors, BASE64_TEST_RESULT_ENCODE_RESULT, ::gpk::failed(encodeResult));
	GPK_TEST_CHECK(errors, BASE64_TEST_RESULT_ENCODED_TEXT, encoded != "aGVsbG8=");

	::gpk::au0_t decoded;
	cnst ::gpk::err_t decodeResult = ::gpk::base64Decode(encoded, decoded);
	GPK_TEST_REQUIRE(errors, BASE64_TEST_RESULT_DECODE_RESULT, ::gpk::failed(decodeResult));
	GPK_TEST_CHECK(errors, BASE64_TEST_RESULT_ROUND_TRIP, decoded.size() != input.size() || 0 != ::memcmp(decoded.begin(), input.begin(), input.size()));
	rtrn 0;
}
