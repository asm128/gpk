#include "llc_enum.h"
#include "llc_mutex.h"
#include "llc_path.h"
#include "llc_runtime.h"
#include "llc_timer.h"

#ifndef LLC_TEST_CORE_H
#define LLC_TEST_CORE_H

namespace llc
{

	struct STestError {
		::llc::s3_t		Value			= {};
		::llc::vcst_t	EnumName		= {};
		::llc::vcst_t	ValueName		= {};
		::llc::vcst_t	Description		= {};
		::llc::u2_t		Count			= {};
		::llc::u2_t		SuccessCount	= {};
	};

	tydf ::llc::aobj<STestError> ATestError;

	::llc::u3_t testSeed(::llc::u3_t fallback);

	tplt<tpnm TEnum>			::llc::err_t testResultRecord(ATestError & errors, TEnum value, bool failed) {
		cnst ::llc::vcst_t enumName = ::llc::get_enum_namev(value);
		for(::llc::u2_t iError = 0; iError < errors.size(); ++iError)
			if(errors[iError].Value == (::llc::s3_t)value && errors[iError].EnumName == enumName) {
				failed ? ++errors[iError].Count : ++errors[iError].SuccessCount;
				return 0;
			}
		cnst ::llc::err_t result = errors.push_back
			({ (::llc::s3_t)value
			, enumName
			, ::llc::get_value_namev(value)
			, ::llc::get_value_descv(value)
			, failed ? 1U : 0U
			, failed ? 0U : 1U
			});
		return ::llc::failed(result) ? result : 0;
	}
	tplt<tpnm TEnum>	stin	::llc::err_t testErrorRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, true); }
	tplt<tpnm TEnum>	stin	::llc::err_t testSuccessRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, false); }

	tplt<tpnm TGetter>
	inln ::llc::u2_t testResultCount(cnst ATestError & errors, TGetter getter) {
		::llc::u2_t count = 0;
		for(::llc::u2_t iError = 0; iError < errors.size(); ++iError)
			count += getter(errors[iError]);
		return count;
	}
	stin ::llc::u2_t testErrorCount		(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.Count; }); }
	stin ::llc::u2_t testSuccessCount	(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.SuccessCount; }); }
	stin ::llc::u2_t testCheckCount		(cnst ATestError & errors) { return testErrorCount(errors) + testSuccessCount(errors); }

	tplt<tpnm... T, tpnm TCall>
	stin ::llc::err_t testTypePack(type_pack<T...>, TCall call) {
		::llc::err_t result = 0;
		(void)(((result = call(type_tag<T>{})) >= 0) && ...);
		rtrn result;
	}

	#ifdef LLC_WINDOWS
	tplt<tpnm TCall>
	bool testThrows(TCall call) {
		bool didThrow = false;
		try { call(); }
		catch(...) { didThrow = true; }
		return didThrow;
	}
	#endif

	#define LLC_TEST_CHECK_BASE(errors, result, condition, statement, format, ...) do {																		\
		::llc::err_t recordResult;																															\
		if_true_block_logf(error_printf, (condition)																										\
			, { if_fail_fef(recordResult = ::llc::testErrorRecord((errors), (result)),  "0x%X, %" LLC_FMT_S2 ", %" LLC_FMT_U2, LLCREP2(recordResult)); statement }	\
			, "%s::%s(%" LLC_FMT_S3 "):\"%s\"::!" format																											\
			, ::llc::get_enum_namep(result), ::llc::get_value_namep(result), ::llc::s3_t(result), ::llc::get_value_descp(result), __VA_ARGS__ )				\
		else {																																				\
			if_fail_fef(recordResult = ::llc::testSuccessRecord((errors), (result)), "0x%X, %" LLC_FMT_S2 ", %" LLC_FMT_U2, LLCREP2(recordResult));				\
		}																																					\
	} while(0)

#define LLC_TEST_CHECKF(errors, result, condition, format, ...)		LLC_TEST_CHECK_BASE(errors, result, (condition), , format, __VA_ARGS__ )
#define LLC_TEST_REQUIREF(errors, result, condition, format, ...)	LLC_TEST_CHECK_BASE(errors, result, (condition), return 0;, format, __VA_ARGS__ )

#define LLC_TEST_CHECK(errors, result, condition)					LLC_TEST_CHECKF		(errors, result, (condition), "%s", "")
#define LLC_TEST_REQUIRE(errors, result, condition)					LLC_TEST_REQUIREF	(errors, result, (condition), "%s", "")

	// Bounded previews keep generated binary cases readable; always disclose omitted bytes.
	stin void testTraceBytes(::llc::vcst_t label, ::llc::vcu0_t bytes) {
		stxp char digits[] = "0123456789ABCDEF";
		char text[64 * 3 + 1] = {};
		cnst ::llc::u2_t count = bytes.size() < 64 ? bytes.size() : 64;
		for(::llc::u2_t iByte = 0; iByte < count; ++iByte) {
			text[iByte * 3 + 0] = digits[bytes[iByte] >> 4];
			text[iByte * 3 + 1] = digits[bytes[iByte] & 15];
			text[iByte * 3 + 2] = ' ';
		}
		always_printf("%.*s: %u bytes; showing %u%s\n%s", LLC_VIEW_ARGS(label), bytes.size(), count, count < bytes.size() ? " (prefix only)" : "", text);
	}
struct STestLogWriters {
	::llc::log_print_t Print = {};
	::llc::log_write_t Write = {};
};

struct STestLogBuffer {
	::llc::string Text;
	::llc::mutex Mutex;
	bool Failed = false;
};
struct STestLogCapture {
	STestLogWriters Previous;
	STestLogBuffer * PreviousBuffer = {};
	STestLogCapture(STestLogBuffer & buffer, bool console = false);
	~STestLogCapture();
	STestLogCapture(cnst STestLogCapture &) = delete;
	STestLogCapture & operator=(cnst STestLogCapture &) = delete;
};

struct STestSuiteReport {
	::llc::vcst_t Name, Folder;
	::llc::err_t Status = 0, ReportStatus = 0;
	::llc::u3_t Microseconds = 0;
	::llc::u2_t Checks = 0, Failures = 0, Groups = 0;
};
struct STestRunReport {
	::llc::string Folder;
	::llc::aobj<STestSuiteReport> Suites;
	bool SuccessDetails = false;
};


	err_t testSuiteReportWrite(vcst_t folder, cnst STestSuiteReport & report, cnst ATestError & results, cnst STestLogBuffer & log);
	err_t testRunPrepare(SRuntimeValues & runtimeValues, STestRunReport & run, aobj<vcst_t> & selectedSuites);
	err_t testRunSummary(cnst ATestError & results, err_t result, u3_t microseconds);
	err_t testRunFinish(cnst STestRunReport & run, cnst STestLogBuffer & log, err_t status, u3_t microseconds);
	err_t testReporting(ATestError & errors, vcst_t outputFolder);

tplt<tpnm TFunction>
stct STestSuite {
	TFunction		& Function;
	::llc::vcst_t	Name, Folder;
};

tplt<tpnm TFunction>
sinx	STestSuite<TFunction>	testSuite			(TFunction & function, ::llc::vcst_t name, ::llc::vcst_t folder) nxpt { rtrn {function, name, folder}; }

tplt<tpnm TFunction>
sttc ::llc::err_t testSuiteRun(ATestError & results, STestRunReport & run, ::llc::view<cnst ::llc::vcst_t> selectedSuites, cnst STestSuite<TFunction> & suite) {
	if(selectedSuites.size() && -1 == selectedSuites.find(suite.Name))
		rtrn 0;
	STestSuiteReport report = {suite.Name, suite.Folder};
	::llc::string folder;
	if_fail_fe(::llc::pathNameCompose(run.Folder, suite.Folder, folder));
	if(::llc::failed(report.Status = ::llc::pathCreate(folder))) {
		report.Status = report.ReportStatus = -1;
		if_fail_fe(run.Suites.push_back(report));
		error_printf("Cannot create suite folder '%.*s'.", LLC_VIEW_ARGS(folder));
		rtrn -1;
	}
	ATestError suiteResults;
	STestLogBuffer log;
	{
		STestLogCapture capture(log);
		::llc::STimer timer;
		try { report.Status = suite.Function(suiteResults, folder); }
		catch(...) { report.Status = -1; error_printf("%s", "Unhandled suite exception."); }
		timer.Frame();
		report.Microseconds = timer.LastTimeMicroseconds;
		report.Checks = testCheckCount(suiteResults);
		report.Failures = testErrorCount(suiteResults);
		report.Groups = suiteResults.size();
		for(cnst auto & item : suiteResults) {
			if(run.SuccessDetails && item.SuccessCount)
				always_printf("PASS %.*s::%.*s(%" LLC_FMT_S3 ") x %u: %.*s", LLC_VIEW_ARGS(item.EnumName), LLC_VIEW_ARGS(item.ValueName), item.Value, item.SuccessCount, LLC_VIEW_ARGS(item.Description));
			if(item.Count)
				error_printf("FAIL %.*s::%.*s(%" LLC_FMT_S3 ") x %u: %.*s", LLC_VIEW_ARGS(item.EnumName), LLC_VIEW_ARGS(item.ValueName), item.Value, item.Count, LLC_VIEW_ARGS(item.Description));
		}
		if(::llc::failed(report.Status) || report.Failures)
			error_printf("%.*s() failed: result:%i, %u/%u checks passed.", LLC_VIEW_ARGS(suite.Name), report.Status, report.Checks - report.Failures, report.Checks);
		else
			always_printf("%.*s() result OK in %" LLC_FMT_U3 " us: %u checks passed.", LLC_VIEW_ARGS(suite.Name), report.Microseconds, report.Checks);
	}
	if(log.Failed) report.ReportStatus = -1;
	try { report.ReportStatus = ::llc::min(report.ReportStatus, testSuiteReportWrite(folder, report, suiteResults, log)); }
	catch(...) { report.ReportStatus = -1; error_printf("Could not serialize suite '%.*s'.", LLC_VIEW_ARGS(suite.Name)); }
	if_fail_fe(run.Suites.push_back(report));
	for(cnst auto & item : suiteResults)
		if_fail_fe(results.push_back(item));
	always_printf("Suite %.*s: %u checks, %u failures, status:%i, reporting:%i.", LLC_VIEW_ARGS(suite.Name), report.Checks, report.Failures, report.Status, report.ReportStatus);
	rtrn (::llc::failed(report.Status) || ::llc::failed(report.ReportStatus) || report.Failures) ? -1 : 0;
}


	tplt<tpnm... TSuites>
	err_t runSuites(SRuntimeValues & runtimeValues, cnst TSuites &... suites) {
		STestLogBuffer log;
		STestLogCapture capture(log, 0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "console"));
		STestRunReport run;
		err_t status = 0;
		STimer timer;
		try {
			status = [&]() -> err_t {
				aobj<vcst_t> selectedSuites;
				if_fail_fe(testRunPrepare(runtimeValues, run, selectedSuites));
				for(cnst vcst_t & selectedSuite : selectedSuites) {
					cnst bool found = ((suites.Name == selectedSuite) || ...);
					if_zero_fef(found, "Unknown test suite:'%.*s'.", LLC_VIEW_ARGS(selectedSuite));
				}
				ATestError results;
				err_t result = 0;
				STimer testTimer;
				((result = ::llc::min(result, testSuiteRun(results, run, selectedSuites, suites))), ...);
				testTimer.Frame();
				rtrn testRunSummary(results, result, testTimer.LastTimeMicroseconds);
			}();
		}
		catch(...) { status = -1; error_printf("%s", "Runner setup or execution exception."); }
		timer.Frame();
		rtrn testRunFinish(run, log, status, timer.LastTimeMicroseconds);
	}
} // namespace

#define LLC_TEST_SUITE(funcSuite, folder) ::llc::testSuite(funcSuite, #funcSuite, folder)

#endif // LLC_TEST_CORE_H
