#include "gpk_log.h"
#include "gpk_file.h"

#include "test_llc_suites.h"

#include <cstdio>

GDEFINE_ENUM_TYPE(LOG_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, COMPILE_TIME, 0, "append_log() or gpk_log() failed during constant evaluation.");
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, RUNTIME_COUNT, 1, "gpk_log() returned the wrong runtime byte count.");
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, OUTER_TEXT, 2, "The outer capture did not receive stdout, stderr, diagnostics and restored output exactly once.");
GDEFINE_ENUM_VALUED(LOG_TEST_RESULT, INNER_TEXT, 3, "The nested capture did not isolate its stdout and stderr bytes.");

static_assert(::gpk::append_log("value", ':', "42") == 8);
static_assert(::gpk::_gpk_log(3, "file.cpp", 42, "func", "value", ':', "42") == 30);
stxp ::gpk::b8_t testLogMacroConstant() { rtrn gpk_log(3, "x") > 2; }
static_assert(::testLogMacroConstant());

::gpk::err_t test_log(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	::gpk::string outerPath, innerPath;
	if_fail_fe(::gpk::pathNameCompose(outputFolder, "capture_outer.log", outerPath));
	if_fail_fe(::gpk::pathNameCompose(outputFolder, "capture_inner.log", innerPath));

	::gpk::STestLogCapture outer;
	if_fail_fe(outer.open(outerPath));
	::fprintf(stdout, "%s", "stdout\n");
	::fflush(stdout);
	::fprintf(stderr, "%s", "stderr\n");
	::fflush(stderr);
	cnst ::gpk::err_t result = ::gpk::_gpk_log(3, "file.cpp", 42, "func", "value", ':', "42");

	::gpk::STestLogCapture inner;
	if_fail_fe(inner.open(innerPath));
	::fprintf(stdout, "%s", "inner-out\n");
	::fflush(stdout);
	::fprintf(stderr, "%s", "inner-err\n");
	::fflush(stderr);
	if_fail_fe(inner.close());
	::fprintf(stderr, "%s", "restored\n");
	::fflush(stderr);
	if_fail_fe(outer.close());

	::gpk::au0_t outerBytes, innerBytes;
	if_fail_fe(::gpk::fileToMemory(outerPath, outerBytes));
	if_fail_fe(::gpk::fileToMemory(innerPath, innerBytes));
	stxp ::gpk::sc_c expectedOuter[] = "stdout\nstderr\n3|file.cpp(42){func}:value:42\nrestored\n";
	stxp ::gpk::sc_c expectedInner[] = "inner-out\ninner-err\n";
	cnst ::gpk::vcu0_c outerExpected = {(cnst ::gpk::u0_t*)expectedOuter, ::gpk::size(expectedOuter) - 1};
	cnst ::gpk::vcu0_c innerExpected = {(cnst ::gpk::u0_t*)expectedInner, ::gpk::size(expectedInner) - 1};
	GPK_TEST_CHECK(errors, LOG_TEST_RESULT_RUNTIME_COUNT, result != 30);
	GPK_TEST_CHECK(errors, LOG_TEST_RESULT_OUTER_TEXT, outerBytes != outerExpected);
	GPK_TEST_CHECK(errors, LOG_TEST_RESULT_INNER_TEXT, innerBytes != innerExpected);
	rtrn 0;
}
