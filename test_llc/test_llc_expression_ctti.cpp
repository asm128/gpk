#include "test_llc_suites.h"
#include "gpk_expression.h"
#include "gpk_expression_ctti.h"

enum EXPRESSION_CTTI_TEST_RESULT : ::gpk::u0_t
	{ EXPRESSION_CTTI_TEST_RESULT_PARSE		= 0
	, EXPRESSION_CTTI_TEST_RESULT_COUNT		= 1
	, EXPRESSION_CTTI_TEST_RESULT_TOKEN		= 2
	};

stxp auto expressionCTTIValid = ::gpk::expressionParseCTTI("parents['1'].name");
static_assert(expressionCTTIValid.Success());
static_assert(5 == expressionCTTIValid.Count);
static_assert(::gpk::EXPRESSION_READER_TYPE_TERM_ANY == expressionCTTIValid.Token.Storage[0].Type);

stxp auto expressionCTTIInvalid = ::gpk::expressionParseCTTI("left & right");
static_assert(false == expressionCTTIInvalid.Success());
static_assert(::gpk::CTTI_EXPRESSION_PARSE_ERROR_INVALID_OPERATOR == expressionCTTIInvalid.Error);

tplt<size_t N>
sttc ::gpk::err_t testExpressionCTTI(::gpk::ATestError & errors, ::gpk::sc_c (&source)[N]) {
	cnst auto fixed = ::gpk::expressionParseCTTI(source);
	GPK_TEST_REQUIREF(errors, EXPRESSION_CTTI_TEST_RESULT_PARSE, false == fixed.Success(), "Error:%u, offset:%u, expression:'%s'.", fixed.Error, fixed.ErrorOffset, source);
	::gpk::SExpressionReader dynamic = {};
	cnst ::gpk::err_t parseResult = ::gpk::expressionReaderParse(dynamic, source);
	GPK_TEST_REQUIREF(errors, EXPRESSION_CTTI_TEST_RESULT_PARSE, ::gpk::failed(parseResult), "Result:%i, expression:'%s'.", parseResult, source);
	GPK_TEST_REQUIREF(errors, EXPRESSION_CTTI_TEST_RESULT_COUNT, fixed.Count != dynamic.Token.size(), "Static:%u, dynamic:%u, expression:'%s'.", fixed.Count, dynamic.Token.size(), source);
	for(::gpk::u2_t iToken = 0; iToken < fixed.Count; ++iToken) {
		cnst ::gpk::SExpressionToken & tokenFixed = fixed.Token.Storage[iToken];
		cnst ::gpk::SExpressionToken & tokenDynamic = dynamic.Token[iToken];
		GPK_TEST_CHECKF(errors, EXPRESSION_CTTI_TEST_RESULT_TOKEN
			, tokenFixed.ParentIndex != tokenDynamic.ParentIndex
			|| tokenFixed.Type != tokenDynamic.Type
			|| tokenFixed.Span != tokenDynamic.Span
			|| tokenFixed.ClosingCondition != tokenDynamic.ClosingCondition
			, "Token:%u, static:{parent:%i,type:%i,span:%u..%u,closing:%u}, dynamic:{parent:%i,type:%i,span:%u..%u,closing:%u}, expression:'%s'."
			, iToken
			, tokenFixed.ParentIndex, tokenFixed.Type, tokenFixed.Span.Begin, tokenFixed.Span.End, tokenFixed.ClosingCondition
			, tokenDynamic.ParentIndex, tokenDynamic.Type, tokenDynamic.Span.Begin, tokenDynamic.Span.End, tokenDynamic.ClosingCondition
			, source
			);
	}
	rtrn 0;
}

::gpk::err_t test_expression_ctti(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(::testExpressionCTTI(errors, "name"));
	if_fail_fe(::testExpressionCTTI(errors, "parents['1'].name"));
	if_fail_fe(::testExpressionCTTI(errors, "!alive == !married"));
	if_fail_fe(::testExpressionCTTI(errors, "alive && !married ? 'alive' : 'dead'"));
	if_fail_fe(::testExpressionCTTI(errors, "children[(selection.index)]"));
	if_fail_fe(::testExpressionCTTI(errors, "parents['1'].property.{selection.default_property_name_path}"));
	if_fail_fe(::testExpressionCTTI(errors, "!!(!!alive) ? ('true') : ('false')"));
	rtrn 0;
}
