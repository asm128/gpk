#include "test_blitter_suites.h"

#include "gpk_path.h"

#include <cstdlib>

sttc ::gpk::string g_ExecutableFolder;

::gpk::err_t testBlitterRunFrontend(::gpk::vcst_t executableName, ::gpk::vcst_t outputFolder, ::gpk::vcst_t arguments, int & exitCode) {
	::gpk::string executablePath = g_ExecutableFolder;
	if_fail_fe(executablePath.append(executableName));
#if defined(GPK_WINDOWS)
	if_fail_fe(executablePath.append(::gpk::vcst_t{".exe"}));
#endif

	::gpk::string command = ::gpk::vcst_t{"\""};
	if_fail_fe(command.append(executablePath));
	if_fail_fe(command.append(::gpk::vcst_t{"\" "}));
	if_fail_fe(command.append(arguments));
	if_fail_fe(command.push_back(0));

#if defined(GPK_WINDOWS)
	::gpk::as0_t previousFolder;
	if_fail_fe(previousFolder.resize(32768));
	cnst ::DWORD previousLength = ::GetCurrentDirectoryA(previousFolder.size(), previousFolder.begin());
	if_true_fe(0 == previousLength || previousLength >= previousFolder.size());
	if_fail_fe(previousFolder.resize(previousLength));
	if_fail_fe(previousFolder.push_back(0));

	::gpk::string suiteFolder = outputFolder;
	if_fail_fe(suiteFolder.push_back(0));
	if_zero_fe(::SetCurrentDirectoryA(suiteFolder.begin()));
	exitCode = ::system(command.begin());
	if_zero_fe(::SetCurrentDirectoryA(previousFolder.begin()));
#else
	(void)outputFolder;
	exitCode = ::system(command.begin());
#endif
	rtrn 0;
}

sttc ::gpk::err_t test_blitter_entry_point(::gpk::SRuntimeValues & runtimeValues);
GPK_OS_ENTRY_POINT(::test_blitter_entry_point);

sttc ::gpk::err_t test_blitter_entry_point(::gpk::SRuntimeValues & runtimeValues) {
	::gpk::string executablePath;
	if_fail_fe(::gpk::pathAbsolute(runtimeValues.EntryPointArgs.ProgramName, executablePath));
	::gpk::vcst_t executableFolder;
	if_fail_fe(::gpk::pathDirectory(executablePath, executableFolder));
	g_ExecutableFolder = executableFolder;

	STestRunnerState runnerState = {};
	rtrn ::gpk::runSuites(runnerState, runtimeValues
		, GPK_TEST_SUITE(reason)
		, GPK_TEST_SUITE(season)
		, GPK_TEST_SUITE(blitter)
		);
}
