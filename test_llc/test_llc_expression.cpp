#include "gpk_expression.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(EXPRESSION_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(EXPRESSION_TEST_RESULT, PARSE_RESULT	, 0, "expressionReaderParse() rejected a valid key.");
GDEFINE_ENUM_VALUED(EXPRESSION_TEST_RESULT, TOKENS		, 1, "The expression reader did not produce tokens and matching views.");
GDEFINE_ENUM_VALUED(EXPRESSION_TEST_RESULT, ROOT_VIEW		, 2, "The root expression view does not preserve the source text.");

::gpk::err_t test_expression(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	stxp ::gpk::vcst_t expression = "alpha";
	::gpk::SExpressionReader reader;
	cnst ::gpk::err_t result = ::gpk::expressionReaderParse(reader, expression);
	GPK_TEST_REQUIRE(errors, EXPRESSION_TEST_RESULT_PARSE_RESULT, ::gpk::failed(result));
	GPK_TEST_REQUIRE(errors, EXPRESSION_TEST_RESULT_TOKENS, 0 == reader.Token.size() || reader.Token.size() != reader.View.size());
	GPK_TEST_CHECK(errors, EXPRESSION_TEST_RESULT_ROOT_VIEW, reader.View[0] != expression);
	rtrn 0;
}
