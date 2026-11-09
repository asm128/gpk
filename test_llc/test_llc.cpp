#include "test_llc_suites.h"

using ::gpk::testReporting;

sttc ::gpk::err_t test_llc_entry_point(::gpk::SRuntimeValues & runtimeValues);
GPK_OS_ENTRY_POINT(::test_llc_entry_point);

#if defined(GPK_WINDOWS) || defined(GPK_LINUX)
extern ::gpk::err_t testProcessEchoChild();
#endif
#ifdef GPK_LINUX
extern ::gpk::err_t testProcessMarkerChild();
extern ::gpk::err_t testProcessSignalChild();
#endif


sttc ::gpk::err_t test_llc_entry_point(::gpk::SRuntimeValues & runtimeValues) {
	STestRunnerState runnerState	= {};
	rtrn ::gpk::runSuites(runnerState, runtimeValues
		, GPK_TEST_SUITE(adam7)
		, GPK_TEST_SUITE(array_obj)
		, GPK_TEST_SUITE(array_pod)
		, GPK_TEST_SUITE(base64)
		, GPK_TEST_SUITE(deflate)
		, GPK_TEST_SUITE(expression)
		, GPK_TEST_SUITE(index)
		, GPK_TEST_SUITE(index_catalog)
		, GPK_TEST_SUITE(index_path)
		, GPK_TEST_SUITE(json_expression)
		, GPK_TEST_SUITE(png)
		);
}
