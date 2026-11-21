#include "gpk_parse.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(PARSE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, HEX_COUNT		, 0, "The hexadecimal parser consumed an unexpected character count.");
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, HEX_VALUE		, 1, "The hexadecimal parser returned an unexpected value.");
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, BASE_LIMIT_COUNT	, 2, "The parser searched beyond the requested base.");
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, BASE_LIMIT_VALUE	, 3, "The base-limited parser returned an unexpected value.");
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, INVALID_COUNT		, 4, "The parser consumed an invalid leading symbol.");
GDEFINE_ENUM_VALUED(PARSE_TEST_RESULT, INVALID_VALUE		, 5, "The parser changed the zero result for invalid input.");

::gpk::err_t test_parse(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::u3_t value = 0;
	::gpk::err_t result = ::gpk::parseArbitraryBaseInteger(16, "0123456789abcdef", "1aFz", &value);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_HEX_COUNT, result != 3);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_HEX_VALUE, value != 0x1AF);

	value = 0;
	result = ::gpk::parseArbitraryBaseInteger(10, "0123456789abcdef", "9a", &value);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_BASE_LIMIT_COUNT, result != 1);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_BASE_LIMIT_VALUE, value != 9);

	value = 0;
	result = ::gpk::parseArbitraryBaseInteger(16, "0123456789abcdef", "x1", &value);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_INVALID_COUNT, result != 0);
	GPK_TEST_CHECK(errors, PARSE_TEST_RESULT_INVALID_VALUE, value != 0);
	rtrn 0;
}
