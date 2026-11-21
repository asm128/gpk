#include "test_llc_suites.h"
#include "gpk_json_expression.h"
#include "gpk_json_expression_ctti.h"

enum JSON_EXPRESSION_CTTI_TEST_RESULT : ::gpk::u0_t
	{ JSON_EXPRESSION_CTTI_TEST_RESULT_PARSE	= 0
	, JSON_EXPRESSION_CTTI_TEST_RESULT_RESOLVE	= 1
	, JSON_EXPRESSION_CTTI_TEST_RESULT_VALUE	= 2
	};

stxp auto jsonExpressionCTTI = ::gpk::jsonExpressionParseCTTI("application.name", "{\"application\":{\"name\":\"gpk\"}}");
static_assert(jsonExpressionCTTI.Success());
static_assert(3 == jsonExpressionCTTI.Expression.Count);
static_assert(9 == jsonExpressionCTTI.JSON.Count);

tplt<size_t NExpression, size_t NJSON>
sttc ::gpk::err_t testJSONExpressionCTTI
	( ::gpk::ATestError & errors
	, ::gpk::sc_c (&expression)[NExpression]
	, ::gpk::sc_c (&json)[NJSON]
	, ::gpk::vcst_t expected
	) {
	cnst auto reader = ::gpk::jsonExpressionParseCTTI(expression, json);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_CTTI_TEST_RESULT_PARSE, false == reader.Success()
		, "Expression error:%u at %u; JSON error:%u at %u."
		, reader.Expression.Error, reader.Expression.ErrorOffset, reader.JSON.Error, reader.JSON.ErrorOffset
		);
	::gpk::vcst_t output = "unchanged";
	cnst ::gpk::err_t result = ::gpk::jsonExpressionResolve(reader.Expression.Tokens(), expression, reader.JSON.Tokens(), json, 0, output);
	GPK_TEST_REQUIREF(errors, JSON_EXPRESSION_CTTI_TEST_RESULT_RESOLVE, ::gpk::failed(result), "Result:%i, expression:'%s', JSON:'%s'.", result, expression, json);
	GPK_TEST_CHECKF(errors, JSON_EXPRESSION_CTTI_TEST_RESULT_VALUE, output != expected, "Output:'%.*s', expected:'%.*s', expression:'%s'.", GPK_VIEW_ARGS(output), GPK_VIEW_ARGS(expected), expression);
	rtrn 0;
}

::gpk::err_t test_json_expression_ctti(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(::testJSONExpressionCTTI(errors, "application.name", "{\"application\":{\"name\":\"gpk\",\"enabled\":true,\"value\":12.5}}", "gpk"));
	if_fail_fe(::testJSONExpressionCTTI(errors, "application.enabled", "{\"application\":{\"name\":\"gpk\",\"enabled\":true}}", "true"));
	if_fail_fe(::testJSONExpressionCTTI(errors, "application.value", "{\"application\":{\"value\":12.5}}", "12.5"));
	if_fail_fe(::testJSONExpressionCTTI(errors, "application.enabled ? application.name : 'no'", "{\"application\":{\"name\":\"gpk\",\"enabled\":true}}", "gpk"));
	if_fail_fe(::testJSONExpressionCTTI(errors, "items['1']", "{\"items\":[\"zero\",\"one\",\"two\"]}", "one"));
	rtrn 0;
}
