#include "gpk_enum.h"

#ifndef GPK_TEST_CORE_H
#define GPK_TEST_CORE_H

struct STestError {
	::gpk::s3_t		Value			= {};
	::gpk::vcst_t	EnumName		= {};
	::gpk::vcst_t	ValueName		= {};
	::gpk::vcst_t	Description		= {};
	::gpk::u2_t		Count			= {};
	::gpk::u2_t		SuccessCount	= {};
};

tydf ::gpk::aobj<STestError> ATestError;

::gpk::u3_t testSeed(::gpk::u3_t fallback);

tplt<tpnm TEnum>			::gpk::err_t testResultRecord(ATestError & errors, TEnum value, bool failed) {
	cnst ::gpk::vcst_t enumName = ::gpk::get_enum_namev(value);
	for(::gpk::u2_t iError = 0; iError < errors.size(); ++iError)
		if(errors[iError].Value == (::gpk::s3_t)value && errors[iError].EnumName == enumName) {
			failed ? ++errors[iError].Count : ++errors[iError].SuccessCount;
			return 0;
		}
	cnst ::gpk::err_t result = errors.push_back
		({ (::gpk::s3_t)value
		, enumName
		, ::gpk::get_value_namev(value)
		, ::gpk::get_value_descv(value)
		, failed ? 1U : 0U
		, failed ? 0U : 1U
		});
	return ::gpk::failed(result) ? result : 0;
}
tplt<tpnm TEnum>	stin	::gpk::err_t testErrorRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, true); }
tplt<tpnm TEnum>	stin	::gpk::err_t testSuccessRecord	(ATestError & errors, TEnum value) { return testResultRecord(errors, value, false); }

tplt<tpnm TGetter>
inln ::gpk::u2_t testResultCount(cnst ATestError & errors, TGetter getter) {
	::gpk::u2_t count = 0;
	for(::gpk::u2_t iError = 0; iError < errors.size(); ++iError)
		count += getter(errors[iError]);
	return count;
}
stin ::gpk::u2_t testErrorCount		(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.Count; }); }
stin ::gpk::u2_t testSuccessCount	(cnst ATestError & errors) { return testResultCount(errors, [](cnst STestError & error) { return error.SuccessCount; }); }
stin ::gpk::u2_t testCheckCount		(cnst ATestError & errors) { return testErrorCount(errors) + testSuccessCount(errors); }

#ifdef GPK_WINDOWS
tplt<tpnm TCall>
bool testThrows(TCall call) {
	bool didThrow = false;
	::gpk::setupLogCallbacks(0, 0);
	try { call(); }
	catch(...) { didThrow = true; }
	::gpk::setupDefaultLogCallbacks();
	return didThrow;
}
#endif

#define GPK_TEST_CHECK_BASE(errors, result, condition, statement, format, ...) do {																		\
	::gpk::err_t recordResult;																															\
	if_true_block_logf(error_printf, (condition)																										\
		, { if_fail_fef(recordResult = testErrorRecord((errors), (result)),  "0x%X, %" GPK_FMT_S2 ", %" GPK_FMT_U2, GPKREP2(recordResult)); statement }	\
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

#endif // GPK_TEST_CORE_H


