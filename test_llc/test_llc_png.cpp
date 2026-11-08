#include "gpk_png.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(PNG_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(PNG_TEST_RESULT, CRC32, 0, "PNG CRC32 does not match the standard check value.");

::gpk::err_t test_png(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	stxp u0_c bytes[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	GPK_TEST_CHECKF(errors, PNG_TEST_RESULT_CRC32, 0xCBF43926U != ::gpk::pngCRCGet(bytes), "Expected:0x%08X.", 0xCBF43926U);
	rtrn 0;
}
