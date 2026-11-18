#include "test_llc_suites.h"
#include "gpk_expression_core.h"

::gpk::err_t test_expression_core(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	GPK_TEST_CHECK(errors, 0, sizeof(::gpk::SExpressionToken) != sizeof(::gpk::s2_t) + sizeof(::gpk::EXPRESSION_READER_TYPE) + sizeof(::gpk::sliceu2_t) + sizeof(::gpk::b8_t));
	rtrn 0;
}
