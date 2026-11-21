#include "gpk_base64.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(BASE64_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ENCODE_RESULT	, 0, "base64Encode() rejected valid bytes.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ENCODED_TEXT	, 1, "base64Encode() produced unexpected text.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, DECODE_RESULT	, 2, "base64Decode() rejected valid Base64 text.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, ROUND_TRIP		, 3, "Base64 decoding did not restore the source bytes.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, TEMPLATE_ENCODE_RESULT	, 4, "base64() rejected valid bytes while encoding.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, TEMPLATE_ENCODED_TEXT	, 5, "base64() produced unexpected text.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, TEMPLATE_DECODE_RESULT	, 6, "base64() rejected valid Base64 text while decoding.");
GDEFINE_ENUM_VALUED(BASE64_TEST_RESULT, TEMPLATE_ROUND_TRIP		, 7, "base64() decoding did not restore the source bytes.");

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

	cnst ::gpk::vcst_t templateInput = "hello";
	::gpk::string templateEncoded;
	cnst ::gpk::err_t templateEncodeResult = ::gpk::base64(templateInput, templateEncoded);
	GPK_TEST_REQUIRE(errors, BASE64_TEST_RESULT_TEMPLATE_ENCODE_RESULT, ::gpk::failed(templateEncodeResult));
	GPK_TEST_CHECK(errors, BASE64_TEST_RESULT_TEMPLATE_ENCODED_TEXT, templateEncoded != "aGVsbG8=");

	::gpk::string templateDecoded;
	cnst ::gpk::err_t templateDecodeResult = ::gpk::base64(::gpk::vcst_t{templateEncoded}, templateDecoded, true);
	GPK_TEST_REQUIRE(errors, BASE64_TEST_RESULT_TEMPLATE_DECODE_RESULT, ::gpk::failed(templateDecodeResult));
	GPK_TEST_CHECK(errors, BASE64_TEST_RESULT_TEMPLATE_ROUND_TRIP, templateDecoded != templateInput);
	rtrn 0;
}
