#include "gpk_enum.h"
#include "gpk_mutex.h"
#include "gpk_path.h"
#include "gpk_runtime.h"
#include "gpk_timer.h"

#ifndef GPK_TEST_CORE_H
#define GPK_TEST_CORE_H

nmsp gpk
{
	struct STestError {
		s3_t		Value			= {};
		vcst_t		EnumName		= {};
		vcst_t		ValueName		= {};
		vcst_t		Description		= {};
		u2_t		Count			= {};
		u2_t		SuccessCount	= {};
	};

	tydf		aobj<STestError>	ATestError;

	u3_t		testSeed			(u3_t fallback);

	tplt<tpnm TEnum>
	err_t		testResultRecord	(ATestError & errors, TEnum value, bool returnedError) {
		vcst_c			enumName			= get_enum_namev(value);
		for(u2_t iError = 0; iError < errors.size(); ++iError)
			if(errors[iError].Value == (s3_t)value && errors[iError].EnumName == enumName) {
				returnedError ? ++errors[iError].Count : ++errors[iError].SuccessCount;
				return 0;
			}
		err_c			result				= errors.push_back
			({ (s3_t)value
			, enumName
			, get_value_namev(value)
			, get_value_descv(value)
			, returnedError ? 1U : 0U
			, returnedError ? 0U : 1U
			});
		return failed(result) ? result : 0;
	}
	tplt<tpnm TEnum>	stin	err_t testErrorRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, true); }
	tplt<tpnm TEnum>	stin	err_t testSuccessRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, false); }

	tplt<tpnm TGetter>
	inln u2_t	testResultCount		(cnst ATestError & errors, TGetter getter) {
		u2_t			count				= 0;
		for(u2_t iError = 0; iError < errors.size(); ++iError)
			count		+= getter(errors[iError]);
		return count;
	}
	stin u2_t	testErrorCount		(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.Count; }); }
	stin u2_t	testSuccessCount	(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.SuccessCount; }); }
	stin u2_t	testCheckCount		(cnst ATestError & errors) { return testErrorCount(errors) + testSuccessCount(errors); }

	tplt<tpnm... T, tpnm TCall>
	stin err_t	testTypePack		(type_pack<T...>, TCall call) {
		err_t			result				= 0;
		(void)(((result = call(type_tag<T>{})) >= 0) && ...);
		rtrn result;
	}


	tplt<tpnm TCall>
	bool		testThrows			(TCall call) {
		bool			didThrow			= false;
		try {
			call();
		}
		catch(...) {
			didThrow	= true;
		}
		return didThrow;
	}

#define GPK_TEST_CHECK_BASE(errors, result, condition, statement, format, ...) do {																		\
	::gpk::err_t recordResult;																															\
	if_true_block_logf(error_printf, (condition)																										\
		, { if_fail_fef(recordResult = ::gpk::testErrorRecord((errors), (result)),  "0x%X, %" GPK_FMT_S2 ", %" GPK_FMT_U2, GPKREP2(recordResult)); statement }	\
		, "%s::%s(%" GPK_FMT_S3 "):\"%s\"::!" format																											\
		, ::gpk::get_enum_namep(result), ::gpk::get_value_namep(result), ::gpk::s3_t(result), ::gpk::get_value_descp(result), __VA_ARGS__ )				\
	else {																																				\
		if_fail_fef(recordResult = testSuccessRecord((errors), (result)), "0x%X, %" GPK_FMT_S2 ", %" GPK_FMT_U2, GPKREP2(recordResult));				\
	}																																					\
} while(0)

#define GPK_TEST_CHECKF(errors, result, condition, format, ...)		GPK_TEST_CHECK_BASE(errors, result, (condition), , format, __VA_ARGS__ )
#define GPK_TEST_REQUIREF(errors, result, condition, format, ...)	GPK_TEST_CHECK_BASE(errors, result, (condition), return 0;, format, __VA_ARGS__ )

#define GPK_TEST_CHECK(errors, result, condition)					GPK_TEST_CHECKF		(errors, result, (condition), "%s", "")
#define GPK_TEST_REQUIRE(errors, result, condition)					GPK_TEST_REQUIREF	(errors, result, (condition), "%s", "")

	// Bounded previews keep generated binary cases readable; always disclose omitted bytes.
	//stin void testTraceBytes(vcst_t label, vcu0_t bytes) {
	//	stxp char digits[] = "0123456789ABCDEF";
	//	char text[64 * 3 + 1] = {};
	//	cnst u2_t count = bytes.size() < 64 ? bytes.size() : 64;
	//	for(u2_t iByte = 0; iByte < count; ++iByte) {
	//		text[iByte * 3 + 0] = digits[bytes[iByte] >> 4];
	//		text[iByte * 3 + 1] = digits[bytes[iByte] & 15];
	//		text[iByte * 3 + 2] = ' ';
	//	}
	//	always_printf("%.*s: %u bytes; showing %u%s\n%s", GPK_VIEW_ARGS(label), bytes.size(), count, count < bytes.size() ? " (prefix only)" : "", text);
	//}
	struct STestLogBuffer {
		mutex				Mutex				= {};
		string				Text				= {};
		bool				Failed				= {};
	};
	struct STestRunnerState {
		u3_t				SeedOverride		= {};
		b8_t				SeedOverrideEnabled = {};

		bool				Console				= {};

		u3_t				testSeed			(u3_t fallback)		{ rtrn SeedOverrideEnabled ? SeedOverride : fallback; }
	};
	err_t				testCaptureWrite	(sc_c * text, u2_t length);
	err_t				testCapturePrint	(sc_c * text);

	struct STestLogCapture {
		STestRunnerState	& Runner;	
		STestLogBuffer		* PreviousBuffer	= {};

							STestLogCapture		(cnst STestLogCapture &)	= delete;
		STestLogCapture		& oper=				(cnst STestLogCapture &)	= delete;

							~STestLogCapture	()							{
		}
							STestLogCapture		(STestRunnerState & runner, STestLogBuffer & buffer, bool console)
			: Runner			(runner)
			, PreviousBuffer	(&buffer)									{
			if(0 == PreviousBuffer) {
				Runner.Console		= console;
			}
		}

	};
	struct STestSuiteReport {
		vcst_t				Name			= {};
		vcst_t				Folder			= {};
		err_t				Status			= {};
		err_t				ReportStatus	= {};
		u3_t				Microseconds	= {};
		u2_t				Checks			= {};
		u2_t				Failures		= {};
		u2_t				Groups			= {};
	};
	struct STestRunReport {
		string					Folder			= {};
		aobj<STestSuiteReport>	Suites			= {};
		bool					SuccessDetails	= {};
	};
	tplt<tpnm TFunction>
	stct STestSuite {
		TFunction		& Function	;
		vcst_t			Name		= {};
		vcst_t			Folder		= {};
	};

	tplt<tpnm TFunction>
	sinx	STestSuite<TFunction>
					testSuite				(TFunction & function, vcst_t name, vcst_t folder)	nxpt	{ rtrn {function, name, folder}; }
	err_t			testRunSummary			(cnst ATestError & results, view<err_c> returnValues, u3_t microseconds);
	err_t			testRunPrepare			(STestRunnerState & runner, SRuntimeValues & runtimeValues, STestRunReport & run, aobj<vcst_t> & selectedSuites);
	err_t			testRunFinish			(STestRunnerState & runner, cnst STestRunReport & run, cnst STestLogBuffer & log, err_t status, u3_t microseconds);
	err_t			testReporting			(STestRunnerState & runner, ATestError & errors, vcst_t outputFolder);
	err_t			testSuiteReportWrite	(vcst_t folder, cnst STestSuiteReport & report, cnst ATestError & results, cnst STestLogBuffer & log);

	tplt<tpnm TFunction>
	sttc	err_t	testSuiteRun			(ATestError & results, STestRunReport & run, view<cnst vcst_t> selectedSuites, cnst STestSuite<TFunction> & suite) {
		if(selectedSuites.size() && -1 == selectedSuites.find(suite.Name))
			rtrn 0;
		STestSuiteReport		report		= {suite.Name, suite.Folder};
		string					folder;
		if_fail_fe(pathNameCompose(run.Folder, suite.Folder, folder));
		if(failed(report.Status = pathCreate(folder))) {
			report.Status = report.ReportStatus = -1;
			if_fail_fe(run.Suites.push_back(report));
			error_printf("Cannot create suite folder '%.*s'.", GPK_VIEW_ARGS(folder));
			rtrn -1;
		}
		ATestError			suiteResults;
		STestLogBuffer		log;
		{
			STestLogCapture		capture		(log);
			STimer				timer;
			try {
				report.Status = suite.Function(suiteResults, folder);
			}
			catch(...) {
				report.Status = -1;
				error_printf("%s", "Unhandled suite exception.");
			}
			timer.Frame();
			report.Microseconds	= timer.LastTimeMicroseconds;
			report.Checks		= testCheckCount(suiteResults);
			report.Failures		= testErrorCount(suiteResults);
			report.Groups		= suiteResults.size();
			for(cnst auto & item : suiteResults) {
				if(run.SuccessDetails && item.SuccessCount)
					always_printf("PASS %.*s::%.*s(%" GPK_FMT_S3 ") x %u: %.*s", GPK_VIEW_ARGS(item.EnumName), GPK_VIEW_ARGS(item.ValueName), item.Value, item.SuccessCount, GPK_VIEW_ARGS(item.Description));
				if(item.Count)
					error_printf("FAIL %.*s::%.*s(%" GPK_FMT_S3 ") x %u: %.*s", GPK_VIEW_ARGS(item.EnumName), GPK_VIEW_ARGS(item.ValueName), item.Value, item.Count, GPK_VIEW_ARGS(item.Description));
			}
			if(failed(report.Status) || report.Failures)
				error_printf("%.*s() failed: result:%i, %u/%u checks passed.", GPK_VIEW_ARGS(suite.Name), report.Status, report.Checks - report.Failures, report.Checks);
			else
				always_printf("%.*s() result OK in %" GPK_FMT_U3 " us: %u checks passed.", GPK_VIEW_ARGS(suite.Name), report.Microseconds, report.Checks);
		}
		if(log.Failed) {
			report.ReportStatus = -1;
		}
		try {
			report.ReportStatus = min(report.ReportStatus, testSuiteReportWrite(folder, report, suiteResults, log));
		}
		catch(...) {
			report.ReportStatus = -1; error_printf("Could not serialize suite '%.*s'.", GPK_VIEW_ARGS(suite.Name));
		}
		if_fail_fe(run.Suites.push_back(report));
		for(cnst auto & item : suiteResults) {
			if_fail_fe(results.push_back(item));
		}
		always_printf("Suite %.*s: %u checks, %u failures, status:%i, reporting:%i.", GPK_VIEW_ARGS(suite.Name), report.Checks, report.Failures, report.Status, report.ReportStatus);
		rtrn (failed(report.Status) || failed(report.ReportStatus) || report.Failures) ? -1 : 0;
	}


	tplt<tpnm... TSuites>
	err_t			runSuites		(STestRunnerState & runner, SRuntimeValues & runtimeValues, cnst TSuites &... suites) {
		STestLogBuffer		log;
		STestLogCapture		capture			(log, 0 <= argsOptionIndex(runtimeValues.EntryPointArgs, "console"));
		STestRunReport		run;
		err_t				status			= 0;
		STimer				timer;
		try {
			status = [&]() -> err_t {
				aobj<vcst_t>		selectedSuites;
				if_fail_fe(testRunPrepare(runner, runtimeValues, run, selectedSuites));
				for(cnst vcst_t & selectedSuite : selectedSuites) {
					cnst b8_t			found				= ((suites.Name == selectedSuite) || ...);
					if_zero_fef(found, "Unknown test suite:'%.*s'.", GPK_VIEW_ARGS(selectedSuite));
				}
				ATestError			results;
				STimer				testTimer;
				err_t				returnValues[] = {testSuiteRun(runner, results, run, selectedSuites, suites)...};
				testTimer.Frame();
				rtrn testRunSummary(runner, results, returnValues, testTimer.LastTimeMicroseconds);
			}();
		}
		catch(...) { status = -1; error_printf("%s", "Runner setup or execution exception."); }
		timer.Frame();
		rtrn testRunFinish(runner, run, log, status, timer.LastTimeMicroseconds);
	}

	GDEFINE_ENUM_TYPE(TEST_REPORT_RESULT, u0_t);
	GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, CAPTURE, 0, "Log capture must preserve print/write bytes and restore the enclosing buffer.");
	GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, QUIET, 1, "File-only capture must not forward to the console writer.");
	GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, CONSOLE, 2, "Opt-in console capture must forward each byte once.");
	GDEFINE_ENUM_VALUED(TEST_REPORT_RESULT, ROOT_ERROR, 3, "Report-writing errors must survive in the root log.");

} // nmsp

#define GPK_TEST_SUITE(funcSuite, folder) testSuite(funcSuite, #funcSuite, folder)

#endif // GPK_TEST_CORE_H
