#include "gpk_json_core.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(JSON_CORE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, OK			, 0, "All JSON core tests passed.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, TOKEN_TYPE	, 1, "SJSONToken does not preserve its declared fields.");

::gpk::err_t test_json_core(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t) {
	cnst ::gpk::SJSONToken token = {-1, ::gpk::JSON_TYPE_STRING, {2, 7}, 11};
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_TOKEN_TYPE
		, token.ParentIndex != -1 || token.Type != ::gpk::JSON_TYPE_STRING || token.Span.Begin != 2 || token.Span.End != 7 || token.Value != 11
		);
	rtrn 0;
}
