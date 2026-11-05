#include "gpk_test_core.h"
#include "gpk_file.h"
#include "gpk_chrono.h"
#include "gpk_parse.h"
#include "gpk_string_compose.h"
#include "gpk_string_time.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_ASTATIC();

usng gpk::err_t, gpk::SRuntimeValues, gpk::STestRunReport, gpk::aobj;

sttc gpk::err_t testJSONString		(::gpk::vcst_t text, ::gpk::string & result) {
	result.clear();
	if_fail_fe(result.push_back('"'));
	stxp char hex[] = "0123456789abcdef";
	for(::gpk::u2_t i = 0; i < text.size(); ++i) {
		cnst auto c = (::gpk::u0_t)text[i];
		if(c == '"' || c == '\\') { if_fail_fe(::gpk::append_strings(result, '\\', (::gpk::sc_t)c)); }
		else if(c < 32) { if_fail_fe(::gpk::append_strings(result, "\\u00", hex[c >> 4], hex[c & 15])); }
		else { if_fail_fe(result.push_back((::gpk::sc_t)c)); }
	}
	rtrn result.push_back('"');
}

sttc gpk::err_t	testReportWrite		(::gpk::vcst_t folder, ::gpk::vcst_t filename, ::gpk::vcst_t contents) {
	::gpk::string path;
	if_fail_fe(::gpk::pathNameCompose(folder, filename, path));
	rtrn ::gpk::fileFromMemory(path, contents);
}
sttc gpk::err_t	testSummaryJSON		(cnst gpk::STestSuiteReport & report, ::gpk::string & output) {
	::gpk::string name, folder;
	if_fail_fe(testJSONString(report.Name, name));
	if_fail_fe(testJSONString(report.Folder, folder));
	output.clear();
	rtrn ::gpk::append_strings(output, "{\"name\":", name
		, ",\"folder\":", folder
		, ",\"status\":", ::gpk::str(report.Status)
		, ",\"report_status\":", ::gpk::str(report.ReportStatus)
		, ",\"duration_us\":", ::gpk::str(report.Microseconds)
		, ",\"checks\":", ::gpk::str(report.Checks)
		, ",\"passed\":", ::gpk::str(report.Checks - report.Failures)
		, ",\"failed\":", ::gpk::str(report.Failures)
		, ",\"result_groups\":", ::gpk::str(report.Groups));
}
gpk::err_t	gpk::testSuiteReportWrite		(::gpk::vcst_t folder, cnst gpk::STestSuiteReport & report, cnst gpk::ATestError & results, cnst gpk::STestLogBuffer & log) {
	::gpk::string		json;
	if_fail_fe(testSummaryJSON(report, json));
	if_fail_fe(json.append_string(",\"schema_version\":1,\"results\":["));
	for(::gpk::u2_t i = 0; i < results.size(); ++i) {
		cnst auto & item = results[i];
		::gpk::string enumName, valueName, description;
		if_fail_fe(testJSONString(item.EnumName, enumName));
		if_fail_fe(testJSONString(item.ValueName, valueName));
		if_fail_fe(testJSONString(item.Description, description));
		if(i) { if_fail_fe(json.push_back(',')); }
		if_fail_fe(::gpk::append_strings(json, "{\"enum\":", enumName
			, ",\"name\":", valueName
			, ",\"value\":", ::gpk::str(item.Value)
			, ",\"description\":", description
			, ",\"passed\":", ::gpk::str(item.SuccessCount)
			, ",\"failed\":", ::gpk::str(item.Count), '}'));
	}
	if_fail_fe(json.append_string("]}\n"));
	cnst gpk::err_t logResult = testReportWrite(folder, "console.log", log.Text);
	cnst gpk::err_t jsonResult = testReportWrite(folder, "results.json", json);
	rtrn ::gpk::min(logResult, jsonResult);
}

sttc gpk::err_t	testRunFolder			(::gpk::vcst_t parent, ::gpk::string & folder) {
	::gpk::string name;
	if_fail_fe(::gpk::timeStringFormat("gpk_test_core_{yy}{MM}{dd}_{hh}{mm}{ss}{ff}", ::gpk::timeCurrentInMs(), name));
	folder.clear();
	if_fail_fe(::gpk::pathNameCompose(parent, name, folder));
	rtrn ::gpk::pathCreate(folder);
}

sttc gpk::err_t	testRunReportWrite		(cnst gpk::STestRunReport & run, gpk::err_t status, ::gpk::u3_t duration, bool seedEnabled, ::gpk::u3_t seed) {
	::gpk::u3_t checks = 0, failures = 0;
	::gpk::string suites;
	for(cnst auto & suite : run.Suites) {
		checks += suite.Checks; failures += suite.Failures;
		if(suites.size()) { if_fail_fe(suites.push_back(',')); }
		::gpk::string summary, relativePath, relativeName;
		if_fail_fe(testSummaryJSON(suite, summary));
		if_fail_fe(::gpk::append_strings(relativeName, suite.Folder, "/results.json"));
		if_fail_fe(testJSONString(relativeName, relativePath));
		if_fail_fe(::gpk::append_strings(suites, summary, ",\"results\":", relativePath, '}'));
	}
#ifdef _DEBUG
	stxp ::gpk::vcst_t configuration = "Debug";
#else
	stxp ::gpk::vcst_t configuration = "Release";
#endif
	::gpk::string config, json;
	if_fail_fe(testJSONString(configuration, config));
	if_fail_fe(::gpk::append_strings(json, "{\"schema_version\":1,\"name\":\"gpk_test_core\",\"configuration\":", config
		, ",\"pointer_bits\":", ::gpk::str((::gpk::u2_t)(sizeof(void*) * 8))
		, ",\"status\":", ::gpk::str(status), ",\"duration_us\":", ::gpk::str(duration), ",\"seed_override\":"));
	if(seedEnabled) { if_fail_fe(::gpk::append_strings(json, '"', ::gpk::str(seed), '"')); }
	else { if_fail_fe(json.append_string("null")); }
	if_fail_fe(::gpk::append_strings(json, ",\"checks\":", ::gpk::str(checks), ",\"passed\":", ::gpk::str(checks - failures)
		, ",\"failed\":", ::gpk::str(failures), ",\"suites\":[", suites, "]}\n"));
	rtrn testReportWrite(run.Folder, "results.json", json);
}

sttc gpk::err_t testRootLogWrite(::gpk::vcst_t folder, cnst ::gpk::STestLogBuffer & log) {
	// File diagnostics append to the active log; write from separate storage.
	::gpk::string snapshot;
	if_fail_fe(snapshot.append(log.Text));
	if(folder.size() && !::gpk::failed(testReportWrite(folder, "console.log", snapshot)))
		rtrn 0;
	snapshot.clear();
	if_fail_fe(snapshot.append(log.Text));
	if_fail_fe(testReportWrite(".", "gpk_test_core.log", snapshot));
	rtrn -1;
}

gpk::err_t	gpk::testRunPrepare		(STestRunnerState & runner, SRuntimeValues & runtimeValues, STestRunReport & run, aobj<vcst_t> & selectedSuites) {
	runner.SeedOverride			= {};
	runner.SeedOverrideEnabled	= false;
	vcst_t parent = ".";
	if(0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "output")) {
		if_fail_fe(argsOptionValue(runtimeValues.EntryPointArgs, "output", parent));
	}
	if_fail_fe(testRunFolder(parent, run.Folder));
	always_printf("Test output: %.*s", GPK_VIEW_ARGS(run.Folder));
	run.SuccessDetails	= 0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "success-details");
	if_fail_fe(argsOptionValues(runtimeValues.EntryPointArgs, "test", selectedSuites));
	if(0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "seed")) {
		vcst_t seedText = {};
		if_fail_fe(argsOptionValue(runtimeValues.EntryPointArgs, "seed", seedText));
		u3_t seed = {};
		cnst gpk::err_t digits = parseIntegerDecimal(seedText, seed);
		stxp vcst_t seedMaximum = "18446744073709551615";
		u2_t significantOffset = 0;
		while(significantOffset < seedText.size() && '0' == seedText[significantOffset])
			++significantOffset;
		cnst u2_t significantLength = seedText.size() - significantOffset;
		bool overflow = significantLength > seedMaximum.size();
		for(u2_t iDigit = 0; false == overflow && significantLength == seedMaximum.size() && iDigit < significantLength; ++iDigit) {
			if(seedText[significantOffset + iDigit] == seedMaximum[iDigit])
				continue;
			overflow = seedText[significantOffset + iDigit] > seedMaximum[iDigit];
			break;
		}
		if_true_fef(0 == seedText.size() || failed(digits) || (u2_t)digits != seedText.size() || overflow, "Invalid --seed value:'%.*s'.", (int)seedText.size(), seedText.begin());
		runner.SeedOverride			= seed;
		runner.SeedOverrideEnabled	= true;
		always_printf("Using test seed:%" GPK_FMT_U3 ".", runner.SeedOverride);
	}

	rtrn 0;
}
gpk::err_t	gpk::testRunSummary	(cnst ATestError & results, view<err_c> /*returnValues*/, u3_t microseconds) {
	u2_c		successCount		= testSuccessCount(results);
	u2_c		failureCount		= testErrorCount(results);

	//if_fail_ef(returnValues, "Test run has suite execution or reporting failures. Result:%i.", returnValues);
	if_true_ef(failureCount, "Test run completed in %" GPK_FMT_U3 " us: %u/%u checks passed, %u failed across %u result groups.", microseconds, successCount, successCount + failureCount, failureCount, results.size())
	else {
		always_printf("Test run completed in %" GPK_FMT_U3 " us: all %u checks passed across %u result groups.", microseconds, successCount, results.size());
	}
	rtrn failureCount; //(0 > returnValues.count([](err_c & returnValue) { return returnValue < 0; })) ? "" : failureCount ? ::gpk::OS_ERROR : 0;
}

gpk::err_t	gpk::testRunFinish		(STestRunnerState & runner, cnst STestRunReport & run, cnst STestLogBuffer & log, err_t status, u3_t microseconds) {
	if(log.Failed) status = -1;
	if(run.Folder.size()) {
		try { status = ::gpk::min(status, testRunReportWrite(run, status, microseconds, runner.SeedOverrideEnabled, runner.SeedOverride)); }
		catch(...) { status = -1; error_printf("%s", "Cannot write run reports."); }
	}
	try { status = ::gpk::min(status, testRootLogWrite(run.Folder, log)); }
	catch(...) { status = -1; }
	rtrn log.Failed ? -1 : status;
}

nmsp gpk
{
	sttc gpk::STestRunnerState * g_Runner = 0;

	gpk::err_t	gpk::testCaptureWrite	(sc_c * text, u2_t length) {
		// GPK allocation failures can log through this same callback.
		sttc thread_local bool writing = false;
		if(g_Runner->ActiveLog && !writing) {
			writing = true;
			try {
				lock_guard lock(g_Runner->ActiveLog->Mutex);
				if(failed(g_Runner->ActiveLog->Text.append({text, length})))
					g_Runner->ActiveLog->Failed = true;
			}
			catch(...) { g_Runner->ActiveLog->Failed = true; }
			writing = false;
		}
		rtrn g_Runner->Console && g_Runner->Writers.Write ? g_Runner->Writers.Write(text, length) : 0;
	}
}
gpk::err_t	gpk::testCapturePrint	(sc_c * text)	{ rtrn ::gpk::testCaptureWrite(text, (u2_t)strlen(text)); }

gpk::err_t	gpk::testReporting		(STestRunnerState & runner, ATestError & errors, vcst_t outputFolder) {
	STestLogBuffer		captured			= {};
	cnst STestLogWriters
						previousWriters		= runner.Writers;
	cnst bool			previousConsole		= runner.Console;
	STestLogBuffer		* previousBuffer	= runner.ActiveLog;
	sttc u2_t			forwarded			= 0;
	forwarded	= 0;
	u2_t				quietForwarded		= 0;
	err_t				captureResult		= 0;
	{
		runner.Writers.Write = [](const char *, u2_t length) -> gpk::err_t { forwarded += length; rtrn 0; };
	}
	{
		STestLogCapture		capture				(runner, captured, false);
		captureResult	= log_print("quiet");
		captureResult	= min(captureResult, log_write(":raw", 4));
		quietForwarded	= forwarded;
		runner.Console	= true;
		captureResult	= min(captureResult, log_print("echo"));
		captureResult	= min(captureResult, log_write(":raw", 4));
	}
	runner.Console	= previousConsole;
	runner.Writers	= previousWriters;
	STestLogWriters		restored;
	if_fail_fe(getLogCallbacks(restored.Print, restored.Write));
	GPK_TEST_CHECK(errors, TEST_REPORT_RESULT_CAPTURE, failed(captureResult) || captured.Failed || captured.Text != "quiet:rawecho:raw" || runner.ActiveLog != previousBuffer || restored.Print != testCapturePrint || restored.Write != testCaptureWrite);
	GPK_TEST_CHECK(errors, TEST_REPORT_RESULT_QUIET, quietForwarded != 0);
	GPK_TEST_CHECK(errors, TEST_REPORT_RESULT_CONSOLE, forwarded != 8);

	STestRunReport		failedRun;
	string				blockedFile;
	if_fail_fe(pathNameCompose(outputFolder, "report_write_failure", failedRun.Folder));
	if_fail_fe(pathCreate(failedRun.Folder));
	if_fail_fe(pathNameCompose(failedRun.Folder, "results.json", blockedFile));
	if_fail_fe(pathCreate(blockedFile)); // A directory cannot be replaced with the JSON file.
	STestLogBuffer		failureLog;
	string				expected;
	err_t				reportResult;
	err_t				logResult;
	{
		STestLogCapture		capture		(runner, failureLog, true);
		reportResult	= testRunReportWrite(failedRun, -1, 0, false, 0);
		if_fail_fe(expected.append(failureLog.Text));
		logResult		= testRootLogWrite(failedRun.Folder, failureLog);
	}
	au0_t				saved;
	if_fail_fe(fileToMemory(failedRun.Folder, "console.log", saved));
	GPK_TEST_CHECK(errors, TEST_REPORT_RESULT_ROOT_ERROR, 0 <= reportResult || failed(logResult) || failureLog.Failed || !expected.size() || saved != vcu0_c((u0_c*)expected.begin(), expected.size()));
	rtrn 0;
}
