#include "llc_test_core.h"
#include "llc_file.h"
#include "llc_chrono.h"
#include "llc_parse.h"
#include "llc_string_compose.h"

namespace llc {
sttc u3_t TestSeedOverride = {};
sttc bool TestSeedOverrideEnabled = false;
u3_t testSeed(u3_t fallback) { rtrn TestSeedOverrideEnabled ? TestSeedOverride : fallback; }

sttc STestLogBuffer * TestActiveLog = {};
sttc STestLogWriters TestConsoleWriters;
sttc bool TestConsoleEnabled = false;

sttc ::llc::err_t testCaptureWrite(const char * text, ::llc::u2_t length) {
	// LLC allocation failures can log through this same callback.
	sttc thread_local bool writing = false;
	if(TestActiveLog && !writing) {
		writing = true;
		try {
			::llc::lock_guard lock(TestActiveLog->Mutex);
			if(::llc::failed(TestActiveLog->Text.append({text, length}))) TestActiveLog->Failed = true;
		}
		catch(...) { TestActiveLog->Failed = true; }
		writing = false;
	}
	rtrn TestConsoleEnabled && TestConsoleWriters.Write ? TestConsoleWriters.Write(text, length) : 0;
}
sttc ::llc::err_t testCapturePrint(const char * text) { rtrn testCaptureWrite(text, (::llc::u2_t)strlen(text)); }

STestLogCapture::STestLogCapture(STestLogBuffer & buffer, bool console) : PreviousBuffer(TestActiveLog) {
	::llc::getLogCallbacks(Previous.Print, Previous.Write);
	if(!PreviousBuffer) {
		TestConsoleWriters = Previous;
		TestConsoleEnabled = console;
	}
	TestActiveLog = &buffer;
	if(Previous.Print != testCapturePrint || Previous.Write != testCaptureWrite)
		::llc::setupLogCallbacks(testCapturePrint, testCaptureWrite);
	if(!PreviousBuffer && !TestConsoleEnabled) Previous = {};
}
STestLogCapture::~STestLogCapture() {
	TestActiveLog = PreviousBuffer;
	if(Previous.Print != testCapturePrint || Previous.Write != testCaptureWrite)
		::llc::setupLogCallbacks(Previous.Print, Previous.Write);
}

sttc ::llc::err_t testJSONString(::llc::vcst_t text, ::llc::string & result) {
	result.clear();
	if_fail_fe(result.push_back('"'));
	stxp char hex[] = "0123456789abcdef";
	for(::llc::u2_t i = 0; i < text.size(); ++i) {
		cnst auto c = (::llc::u0_t)text[i];
		if(c == '"' || c == '\\') { if_fail_fe(::llc::append_strings(result, '\\', (::llc::sc_t)c)); }
		else if(c < 32) { if_fail_fe(::llc::append_strings(result, "\\u00", hex[c >> 4], hex[c & 15])); }
		else { if_fail_fe(result.push_back((::llc::sc_t)c)); }
	}
	rtrn result.push_back('"');
}

sttc ::llc::err_t testReportWrite(::llc::vcst_t folder, ::llc::vcst_t filename, ::llc::vcst_t contents) {
	::llc::string path;
	if_fail_fe(::llc::pathNameCompose(folder, filename, path));
	rtrn ::llc::fileFromMemory(path, contents);
}
sttc ::llc::err_t testSummaryJSON(cnst STestSuiteReport & report, ::llc::string & output) {
	::llc::string name, folder;
	if_fail_fe(testJSONString(report.Name, name));
	if_fail_fe(testJSONString(report.Folder, folder));
	output.clear();
	rtrn ::llc::append_strings(output, "{\"name\":", name
		, ",\"folder\":", folder
		, ",\"status\":", ::llc::str(report.Status)
		, ",\"report_status\":", ::llc::str(report.ReportStatus)
		, ",\"duration_us\":", ::llc::str(report.Microseconds)
		, ",\"checks\":", ::llc::str(report.Checks)
		, ",\"passed\":", ::llc::str(report.Checks - report.Failures)
		, ",\"failed\":", ::llc::str(report.Failures)
		, ",\"result_groups\":", ::llc::str(report.Groups));
}
::llc::err_t testSuiteReportWrite(::llc::vcst_t folder, cnst STestSuiteReport & report, cnst ATestError & results, cnst STestLogBuffer & log) {
	::llc::string json;
	if_fail_fe(testSummaryJSON(report, json));
	if_fail_fe(json.append_string(",\"schema_version\":1,\"results\":["));
	for(::llc::u2_t i = 0; i < results.size(); ++i) {
		cnst auto & item = results[i];
		::llc::string enumName, valueName, description;
		if_fail_fe(testJSONString(item.EnumName, enumName));
		if_fail_fe(testJSONString(item.ValueName, valueName));
		if_fail_fe(testJSONString(item.Description, description));
		if(i) { if_fail_fe(json.push_back(',')); }
		if_fail_fe(::llc::append_strings(json, "{\"enum\":", enumName
			, ",\"name\":", valueName
			, ",\"value\":", ::llc::str(item.Value)
			, ",\"description\":", description
			, ",\"passed\":", ::llc::str(item.SuccessCount)
			, ",\"failed\":", ::llc::str(item.Count), '}'));
	}
	if_fail_fe(json.append_string("]}\n"));
	cnst ::llc::err_t logResult = testReportWrite(folder, "console.log", log.Text);
	cnst ::llc::err_t jsonResult = testReportWrite(folder, "results.json", json);
	rtrn ::llc::min(logResult, jsonResult);
}

sttc ::llc::err_t testRunFolder(::llc::vcst_t parent, ::llc::string & folder) {
	::llc::string name;
	if_fail_fe(::llc::timeStringFormat("llc_test_core_{yy}{MM}{dd}_{hh}{mm}{ss}{ff}", ::llc::timeCurrentInMs(), name));
	folder.clear();
	if_fail_fe(::llc::pathNameCompose(parent, name, folder));
	rtrn ::llc::pathCreate(folder);
}

sttc ::llc::err_t testRunReportWrite(cnst STestRunReport & run, ::llc::err_t status, ::llc::u3_t duration, bool seedEnabled, ::llc::u3_t seed) {
	::llc::u3_t checks = 0, failures = 0;
	::llc::string suites;
	for(cnst auto & suite : run.Suites) {
		checks += suite.Checks; failures += suite.Failures;
		if(suites.size()) { if_fail_fe(suites.push_back(',')); }
		::llc::string summary, relativePath, relativeName;
		if_fail_fe(testSummaryJSON(suite, summary));
		if_fail_fe(::llc::append_strings(relativeName, suite.Folder, "/results.json"));
		if_fail_fe(testJSONString(relativeName, relativePath));
		if_fail_fe(::llc::append_strings(suites, summary, ",\"results\":", relativePath, '}'));
	}
#ifdef _DEBUG
	stxp ::llc::vcst_t configuration = "Debug";
#else
	stxp ::llc::vcst_t configuration = "Release";
#endif
	::llc::string config, json;
	if_fail_fe(testJSONString(configuration, config));
	if_fail_fe(::llc::append_strings(json, "{\"schema_version\":1,\"name\":\"llc_test_core\",\"configuration\":", config
		, ",\"pointer_bits\":", ::llc::str((::llc::u2_t)(sizeof(void*) * 8))
		, ",\"status\":", ::llc::str(status), ",\"duration_us\":", ::llc::str(duration), ",\"seed_override\":"));
	if(seedEnabled) { if_fail_fe(::llc::append_strings(json, '"', ::llc::str(seed), '"')); }
	else { if_fail_fe(json.append_string("null")); }
	if_fail_fe(::llc::append_strings(json, ",\"checks\":", ::llc::str(checks), ",\"passed\":", ::llc::str(checks - failures)
		, ",\"failed\":", ::llc::str(failures), ",\"suites\":[", suites, "]}\n"));
	rtrn testReportWrite(run.Folder, "results.json", json);
}

sttc ::llc::err_t testRootLogWrite(::llc::vcst_t folder, cnst STestLogBuffer & log) {
	// File diagnostics append to the active log; write from separate storage.
	::llc::string snapshot;
	if_fail_fe(snapshot.append(log.Text));
	if(folder.size() && !::llc::failed(testReportWrite(folder, "console.log", snapshot)))
		rtrn 0;
	snapshot.clear();
	if_fail_fe(snapshot.append(log.Text));
	if_fail_fe(testReportWrite(".", "llc_test_core.log", snapshot));
	rtrn -1;
}

err_t testRunPrepare(SRuntimeValues & runtimeValues, STestRunReport & run, aobj<vcst_t> & selectedSuites) {
	TestSeedOverride = {};
	TestSeedOverrideEnabled = false;
	vcst_t parent = ".";
	if(0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "output")) {
		if_fail_fe(argsOptionValue(runtimeValues.EntryPointArgs, "output", parent));
	}
	if_fail_fe(testRunFolder(parent, run.Folder));
	always_printf("Test output: %.*s", LLC_VIEW_ARGS(run.Folder));
	run.SuccessDetails	= 0 <= ::llc::argsOptionIndex(runtimeValues.EntryPointArgs, "success-details");
	if_fail_fe(::llc::argsOptionValues(runtimeValues.EntryPointArgs, "test", selectedSuites));
	if(0 <= ::llc::argsOptionIndex(runtimeValues.EntryPointArgs, "seed")) {
		::llc::vcst_t seedText = {};
		if_fail_fe(::llc::argsOptionValue(runtimeValues.EntryPointArgs, "seed", seedText));
		::llc::u3_t seed = {};
		cnst ::llc::err_t digits = ::llc::parseIntegerDecimal(seedText, seed);
		stxp ::llc::vcst_t seedMaximum = "18446744073709551615";
		::llc::u2_t significantOffset = 0;
		while(significantOffset < seedText.size() && '0' == seedText[significantOffset])
			++significantOffset;
		cnst ::llc::u2_t significantLength = seedText.size() - significantOffset;
		bool overflow = significantLength > seedMaximum.size();
		for(::llc::u2_t iDigit = 0; false == overflow && significantLength == seedMaximum.size() && iDigit < significantLength; ++iDigit) {
			if(seedText[significantOffset + iDigit] == seedMaximum[iDigit])
				continue;
			overflow = seedText[significantOffset + iDigit] > seedMaximum[iDigit];
			break;
		}
		if_true_fef(0 == seedText.size() || ::llc::failed(digits) || (::llc::u2_t)digits != seedText.size() || overflow, "Invalid --seed value:'%.*s'.", (int)seedText.size(), seedText.begin());
		TestSeedOverride = seed;
		TestSeedOverrideEnabled = true;
		always_printf("Using test seed:%" LLC_FMT_U3 ".", TestSeedOverride);
	}

	rtrn 0;
}
err_t testRunSummary(cnst ATestError & results, err_t result, u3_t microseconds) {
	::llc::u2_c		successCount		= testSuccessCount(results);
	::llc::u2_c		failureCount		= testErrorCount(results);

	if_true_ef(::llc::failed(result), "Test run has suite execution or reporting failures. Result:%i.", result);
	if_true_ef(failureCount, "Test run completed in %" LLC_FMT_U3 " us: %u/%u checks passed, %u failed across %u result groups.", microseconds, successCount, successCount + failureCount, failureCount, results.size())
	else {
		always_printf("Test run completed in %" LLC_FMT_U3 " us: all %u checks passed across %u result groups.", microseconds, successCount, results.size());
	}
	rtrn ::llc::failed(result) ? result : failureCount ? ::llc::OS_ERROR : 0;
}

err_t testRunFinish(cnst STestRunReport & run, cnst STestLogBuffer & log, err_t status, u3_t microseconds) {
	if(log.Failed) status = -1;
	if(run.Folder.size()) {
		try { status = ::llc::min(status, testRunReportWrite(run, status, microseconds, TestSeedOverrideEnabled, TestSeedOverride)); }
		catch(...) { status = -1; error_printf("%s", "Cannot write run reports."); }
	}
	try { status = ::llc::min(status, testRootLogWrite(run.Folder, log)); }
	catch(...) { status = -1; }
	rtrn log.Failed ? -1 : status;
}
} // namespace llc

GDEFINE_ENUM_TYPE(TEST_REPORT_RESULT, ::llc::u0_t);
GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, CAPTURE, 0, "Log capture must preserve print/write bytes and restore the enclosing buffer.");
GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, QUIET, 1, "File-only capture must not forward to the console writer.");
GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, CONSOLE, 2, "Opt-in console capture must forward each byte once.");
GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, ROOT_ERROR, 3, "Report-writing errors must survive in the root log.");

::llc::err_t llc::testReporting(ATestError & errors, ::llc::vcst_t outputFolder) {
	STestLogBuffer captured;
	cnst STestLogWriters previousWriters = TestConsoleWriters;
	cnst bool previousConsole = TestConsoleEnabled;
	STestLogBuffer * previousBuffer = TestActiveLog;
	sttc ::llc::u2_t forwarded = 0;
	forwarded = 0;
	::llc::u2_t quietForwarded = 0;
	::llc::err_t captureResult = 0;
	TestConsoleWriters.Write = [](const char *, ::llc::u2_t length) -> ::llc::err_t { forwarded += length; rtrn 0; };
	{
		STestLogCapture capture(captured);
		TestConsoleEnabled = false;
		captureResult = ::llc::log_print("quiet");
		captureResult = ::llc::min(captureResult, ::llc::log_write(":raw", 4));
		quietForwarded = forwarded;
		TestConsoleEnabled = true;
		captureResult = ::llc::min(captureResult, ::llc::log_print("echo"));
		captureResult = ::llc::min(captureResult, ::llc::log_write(":raw", 4));
	}
	TestConsoleEnabled = previousConsole;
	TestConsoleWriters = previousWriters;
	STestLogWriters restored;
	if_fail_fe(::llc::getLogCallbacks(restored.Print, restored.Write));
	LLC_TEST_CHECK(errors, TEST_REPORT_RESULT_CAPTURE, ::llc::failed(captureResult) || captured.Failed || captured.Text != "quiet:rawecho:raw" || TestActiveLog != previousBuffer || restored.Print != testCapturePrint || restored.Write != testCaptureWrite);
	LLC_TEST_CHECK(errors, TEST_REPORT_RESULT_QUIET, quietForwarded != 0);
	LLC_TEST_CHECK(errors, TEST_REPORT_RESULT_CONSOLE, forwarded != 8);

	STestRunReport failedRun;
	::llc::string blockedFile;
	if_fail_fe(::llc::pathNameCompose(outputFolder, "report_write_failure", failedRun.Folder));
	if_fail_fe(::llc::pathCreate(failedRun.Folder));
	if_fail_fe(::llc::pathNameCompose(failedRun.Folder, "results.json", blockedFile));
	if_fail_fe(::llc::pathCreate(blockedFile)); // A directory cannot be replaced with the JSON file.
	STestLogBuffer failureLog;
	::llc::string expected;
	::llc::err_t reportResult, logResult;
	{
		STestLogCapture capture(failureLog);
		reportResult = testRunReportWrite(failedRun, -1, 0, false, 0);
		if_fail_fe(expected.append(failureLog.Text));
		logResult = testRootLogWrite(failedRun.Folder, failureLog);
	}
	::llc::au0_t saved;
	if_fail_fe(::llc::fileToMemory(failedRun.Folder, "console.log", saved));
	LLC_TEST_CHECK(errors, TEST_REPORT_RESULT_ROOT_ERROR, 0 <= reportResult || ::llc::failed(logResult) || failureLog.Failed || !expected.size() || saved != vcu0_c((u0_c*)expected.begin(), expected.size()));
	rtrn 0;
}
