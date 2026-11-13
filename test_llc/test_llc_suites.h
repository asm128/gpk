#include "gpk_test_core.h"

#ifndef TEST_LLC_SUITES_H
#define TEST_LLC_SUITES_H

usns gpk;
// The runner creates outputFolder and owns its results.json and console.log.
// Suites may write any other files/subdirectories there; the runner does not inspect them.
// The folder view remains valid for the duration of the suite call.

::gpk::err_t test_adam7				(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_array_obj			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_array_pod			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_base64			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_ctti				(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_deflate			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_enum				(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_event			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_expression		(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_index				(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_index_catalog		(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_index_path		(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_json_expression	(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_png				(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_string			(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_array_static		(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);
::gpk::err_t test_view_serialize	(STestRunnerState & runner, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder);

#endif //TEST_GPK_CORE_SUITES_H
