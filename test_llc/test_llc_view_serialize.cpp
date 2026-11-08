#include "gpk_view_serialize.h"
#include "test_llc_suites.h"

#include <cstring>

GDEFINE_ENUM_TYPE(VIEW_SERIALIZE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(VIEW_SERIALIZE_TEST_RESULT, READ_RESULT	, 0, "viewPascal() rejected a valid packed view.");
GDEFINE_ENUM_VALUED(VIEW_SERIALIZE_TEST_RESULT, ELEMENTS		, 1, "viewPascal() did not expose the packed elements.");
GDEFINE_ENUM_VALUED(VIEW_SERIALIZE_TEST_RESULT, POD_RESULT	, 2, "loadPOD() rejected a complete value.");
GDEFINE_ENUM_VALUED(VIEW_SERIALIZE_TEST_RESULT, POD_VALUE	, 3, "loadPOD() did not restore or consume the value.");

::gpk::err_t test_view_serialize(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::u0_t bytes[szof(::gpk::u2_t) + szof(::gpk::u1_t) * 3] = {};
	cnst ::gpk::u2_t count = 3;
	cnst ::gpk::u1_t values[] = {0x1122, 0x3344, 0x5566};
	::memcpy(bytes, &count, szof(count));
	::memcpy(bytes + szof(count), values, szof(values));
	::gpk::view<cnst ::gpk::u1_t> restored;
	cnst ::gpk::err_t result = ::gpk::viewPascal(restored, ::gpk::vcu0_t{bytes});
	GPK_TEST_REQUIRE(errors, VIEW_SERIALIZE_TEST_RESULT_READ_RESULT, result != (::gpk::err_t)::gpk::size(bytes));
	GPK_TEST_CHECK(errors, VIEW_SERIALIZE_TEST_RESULT_ELEMENTS, restored.size() != 3 || restored[0] != values[0] || restored[1] != values[1] || restored[2] != values[2]);

	::gpk::vcu0_t input = {bytes, szof(::gpk::u2_t)};
	::gpk::u2_t loaded = 0;
	cnst ::gpk::err_t loadResult = ::gpk::loadPOD(input, loaded);
	GPK_TEST_CHECK(errors, VIEW_SERIALIZE_TEST_RESULT_POD_RESULT, loadResult != (::gpk::err_t)szof(loaded));
	GPK_TEST_CHECK(errors, VIEW_SERIALIZE_TEST_RESULT_POD_VALUE, loaded != count || input.size());
	rtrn 0;
}
