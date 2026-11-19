#include "gpk_test_core.h"
#include "gpk_file.h"
#include "gpk_chrono.h"
#include "gpk_parse.h"
#include "gpk_append_json.h"
#include "gpk_string_compose.h"
#include "gpk_string_time.h"

#include <cstdio>

#if defined(GPK_WINDOWS)
#	include <io.h>
#elif defined(GPK_LINUX)
#	include <unistd.h>
#endif

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_ASTATIC();

usng gpk::err_t, gpk::SRuntimeValues, gpk::STestRunReport, gpk::aobj;

#if defined(GPK_WINDOWS)
#	define gpk_test_dup		_dup
#	define gpk_test_dup2		_dup2
#	define gpk_test_close	_close
#	define gpk_test_fileno	_fileno
#elif defined(GPK_LINUX)
#	define gpk_test_dup		dup
#	define gpk_test_dup2		dup2
#	define gpk_test_close	close
#	define gpk_test_fileno	fileno
#endif

gpk::STestLogCapture::~STestLogCapture() {
	close();
}

gpk::err_t gpk::STestLogCapture::open(vcst_t fileName) {
	if_true_fe(Active);
	if_true_fe(0 == fileName.size());
	if(0 != ::fflush(stdout) || 0 != ::fflush(stderr))
		rtrn -1;

	cnst int stdOut = gpk_test_fileno(stdout);
	cnst int stdErr = gpk_test_fileno(stderr);
	if(0 > stdOut || 0 > stdErr)
		rtrn -1;
	SavedStdOut = (s2_t)gpk_test_dup(stdOut);
	SavedStdErr = (s2_t)gpk_test_dup(stdErr);
	if(0 > SavedStdOut || 0 > SavedStdErr) {
		if(0 <= SavedStdOut) gpk_test_close(SavedStdOut);
		if(0 <= SavedStdErr) gpk_test_close(SavedStdErr);
		SavedStdOut = SavedStdErr = -1;
		rtrn -1;
	}
	FILE * outputFile = {};
#if defined(GPK_WINDOWS)
	cnst int openResult = ::fopen_s(&outputFile, fileName.begin(), "wb");
#elif defined(GPK_LINUX)
	outputFile = ::fopen(fileName.begin(), "wb");
	cnst int openResult = outputFile ? 0 : -1;
#endif
	if(0 != openResult || 0 == outputFile) {
		gpk_test_close(SavedStdOut);
		gpk_test_close(SavedStdErr);
		SavedStdOut = SavedStdErr = -1;
		rtrn -1;
	}
	cnst int output = gpk_test_fileno(outputFile);
	cnst int stdoutResult = gpk_test_dup2(output, stdOut);
	cnst int stderrResult = gpk_test_dup2(output, stdErr);
	::fclose(outputFile);
	if(0 > stdoutResult || 0 > stderrResult) {
		gpk_test_dup2(SavedStdOut, stdOut);
		gpk_test_dup2(SavedStdErr, stdErr);
		gpk_test_close(SavedStdOut);
		gpk_test_close(SavedStdErr);
		SavedStdOut = SavedStdErr = -1;
		rtrn -1;
	}
	Active = true;
	rtrn 0;
}

gpk::err_t gpk::STestLogCapture::close() {
	if(false == Active)
		rtrn 0;
	err_t result = 0;
	if(0 != ::fflush(stdout) || 0 != ::fflush(stderr))
		result = -1;
	cnst int stdOut = gpk_test_fileno(stdout);
	cnst int stdErr = gpk_test_fileno(stderr);
	if(0 > gpk_test_dup2(SavedStdOut, stdOut)) result = -1;
	if(0 > gpk_test_dup2(SavedStdErr, stdErr)) result = -1;
	if(0 != gpk_test_close(SavedStdOut)) result = -1;
	if(0 != gpk_test_close(SavedStdErr)) result = -1;
	SavedStdOut = SavedStdErr = -1;
	Active = false;
	rtrn result;
}

sttc gpk::err_t	testReportWrite		(::gpk::vcst_t folder, ::gpk::vcst_t filename, ::gpk::vcst_t contents) {
	::gpk::string path;
	if_fail_fe(::gpk::pathNameCompose(folder, filename, path));
	rtrn ::gpk::fileFromMemory(path, contents);
}
sttc gpk::err_t	testSummaryJSON		(cnst gpk::STestSuiteReport & report, ::gpk::string & output) {
	::gpk::string name, folder;
	if_fail_fe(::gpk::appendJSONString(name, report.Name));
	if_fail_fe(::gpk::appendJSONString(folder, report.Folder));
	output.clear();
	rtrn ::gpk::append_strings(output, "{\"name\":", name
		, ",\"folder\":"		, folder
		, ",\"status\":"		, ::gpk::str(report.Status)
		, ",\"report_status\":"	, ::gpk::str(report.ReportStatus)
		, ",\"duration_us\":"	, ::gpk::str(report.Microseconds)
		, ",\"checks\":"		, ::gpk::str(report.Checks)
		, ",\"passed\":"		, ::gpk::str(report.Checks - report.Failures)
		, ",\"failed\":"		, ::gpk::str(report.Failures)
		, ",\"result_groups\":"	, ::gpk::str(report.Groups));
}
gpk::err_t	gpk::testSuiteReportWrite		(vcst_t folder, cnst gpk::STestSuiteReport & report, cnst gpk::ATestError & results) {
	string		json;
	if_fail_fe(testSummaryJSON(report, json));
	if_fail_fe(append_strings(json, ",\"schema_version\":1,\"results\":["));
	for(u2_t i = 0; i < results.size(); ++i) {
		cnst auto & item = results[i];
		string enumName, valueName, description;
		if_fail_fe(appendJSONString(enumName, item.EnumName));
		if_fail_fe(appendJSONString(valueName, item.ValueName));
		if_fail_fe(appendJSONString(description, item.Description));
		if(i) {
			if_fail_fe(json.push_back(','));
		}
		if_fail_fe(append_strings(json, "{\"enum\":", enumName
			, ",\"name\":", valueName
			, ",\"value\":", str(item.Value)
			, ",\"description\":", description
			, ",\"passed\":", str(item.SuccessCount)
			, ",\"failed\":", str(item.Count), '}'));
	}
	if_fail_fe(append_strings(json, "]}\n"));
	rtrn testReportWrite(folder, "results.json", json);
}

sttc gpk::err_t	testRunFolder			(::gpk::vcst_t parent, ::gpk::string & folder) {
	::gpk::string name;
	if_fail_fe(::gpk::timeStringFormat("gpk_test_core_{yy}{MM}{dd}_{hh}{mm}{ss}{ff}", ::gpk::timeCurrentInMs(), name));
	folder.clear();
	if_fail_fe(::gpk::pathNameCompose(parent, name, folder));
	rtrn ::gpk::pathCreate(folder);
}

sttc gpk::err_t	testRunReportWrite		(cnst gpk::STestRunReport & run, gpk::err_t status, ::gpk::u3_t duration, b8_t seedEnabled, ::gpk::u3_t seed) {
	::gpk::u3_t checks = 0, failures = 0;
	::gpk::string suites;
	for(cnst auto & suite : run.Suites) {
		checks += suite.Checks; failures += suite.Failures;
		if(suites.size()) { if_fail_fe(suites.push_back(',')); }
		::gpk::string summary, relativePath, relativeName;
		if_fail_fe(testSummaryJSON(suite, summary));
		if_fail_fe(::gpk::append_strings(relativeName, suite.Folder, "/results.json"));
		if_fail_fe(::gpk::appendJSONString(relativePath, relativeName));
		if_fail_fe(::gpk::append_strings(suites, summary, ",\"results\":", relativePath, '}'));
	}
#ifdef _DEBUG
	stxp ::gpk::vcst_t configuration = "Debug";
#else
	stxp ::gpk::vcst_t configuration = "Release";
#endif
	::gpk::string config, json;
	if_fail_fe(::gpk::appendJSONString(config, configuration));
	if_fail_fe(::gpk::append_strings(json
		, "{\"schema_version\":1,\"name\":\"gpk_test_core\",\"configuration\":", config
		, ",\"pointer_bits\":", ::gpk::str((::gpk::u2_t)(sizeof(void*) * 8)), ",\"status\":", ::gpk::str(status)
		, ",\"duration_us\":", ::gpk::str(duration), ",\"seed_override\":")
	);
	if(seedEnabled) {
		if_fail_fe(::gpk::append_strings(json, '"', ::gpk::str(seed), '"'));
	}
	else {
		if_fail_fe(::gpk::append_strings(json, "null"));
	}
	if_fail_fe(::gpk::append_strings(json
		, ",\"checks\":", ::gpk::str(checks), ",\"passed\":", ::gpk::str(checks - failures)
		, ",\"failed\":", ::gpk::str(failures), ",\"suites\":[", suites, "]}\n")
	);
	rtrn testReportWrite(run.Folder, "results.json", json);
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
		u2_c significantLength = seedText.size() - significantOffset;
		b8_t overflow = significantLength > seedMaximum.size();
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
gpk::err_t	gpk::testRunSummary	(cnst ATestError & results, view<err_c> returnValues, u3_t microseconds) {
	u2_c		successCount		= testSuccessCount(results);
	u2_c		failureCount		= testErrorCount(results);
	b8_t		suiteFailed			= false;
	for(err_c returnValue : returnValues)
		if(failed(returnValue))
			suiteFailed			= true;

	if(failureCount || suiteFailed)
		error_printf("Test run completed in %" GPK_FMT_U3 " us: %u/%u checks passed, %u failed across %u result groups; suite status:%s.", microseconds, successCount, successCount + failureCount, failureCount, results.size(), suiteFailed ? "failed" : "successful");
	else {
		always_printf("Test run completed in %" GPK_FMT_U3 " us: all %u checks passed across %u result groups.", microseconds, successCount, results.size());
	}
	rtrn (failureCount || suiteFailed) ? -1 : 0;
}

gpk::err_t	gpk::testRunFinish		(STestRunnerState & runner, cnst STestRunReport & run, err_t status, u3_t microseconds) {
	if(run.Folder.size()) {
		try {
			if_fail_e(status = ::gpk::min(status, testRunReportWrite(run, status, microseconds, runner.SeedOverrideEnabled, runner.SeedOverride)));
		}
		catch(...) {
			status = -1;
			error_printf("%s", "Cannot write run reports.");
		}
	}
	rtrn status;
}
