#include "gpk_test_core.h"

#ifndef TEST_BLITTER_SUITES_H
#define TEST_BLITTER_SUITES_H

usns gpk;

::gpk::err_t test_reason	(STestRunnerState & runner, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder);
::gpk::err_t test_season	(STestRunnerState & runner, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder);
::gpk::err_t test_blitter	(STestRunnerState & runner, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder);

::gpk::err_t testBlitterRunFrontend(::gpk::vcst_t executableName, ::gpk::vcst_t outputFolder, ::gpk::vcst_t arguments, int & exitCode);

#endif // TEST_BLITTER_SUITES_H
