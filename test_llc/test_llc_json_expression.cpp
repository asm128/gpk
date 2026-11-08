#include "gpk_json_expression.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(JSON_EXPRESSION_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, TRUE_RESULT	, 0, "SEvaluationStepResult::SetBoolCarry() rejected true.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, TRUE_STATE	, 1, "SEvaluationStepResult::SetBoolCarry() did not publish true state.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FALSE_RESULT	, 2, "SEvaluationStepResult::SetBoolCarry() rejected false.");
GDEFINE_ENUM_VALUED(JSON_EXPRESSION_TEST_RESULT, FALSE_STATE	, 3, "SEvaluationStepResult::SetBoolCarry() did not publish false state.");

::gpk::err_t test_json_expression(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::SEvaluationStepResult step = {};
	::gpk::vcst_t output;
	cnst ::gpk::err_t trueResult = step.SetBoolCarry(true, output);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_TRUE_RESULT, ::gpk::failed(trueResult));
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_TRUE_STATE, false == step.LastValueCarry || 1 != step.LastValue || output != "true");
	cnst ::gpk::err_t falseResult = step.SetBoolCarry(false, output);
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_FALSE_RESULT, ::gpk::failed(falseResult));
	GPK_TEST_CHECK(errors, JSON_EXPRESSION_TEST_RESULT_FALSE_STATE, false == step.LastValueCarry || 0 != step.LastValue || output != "false");
	rtrn 0;
}
