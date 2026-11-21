#include "gpk_pack.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(PACK_FILTER_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, WRAPPER_RESULT	, 0, "A numbered pack-filter wrapper returned the wrong element count.");
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, WRAPPER_OUTPUT	, 1, "A numbered pack-filter wrapper did not append converted elements.");
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, DISPATCH_RESULT	, 2, "packFilter() returned the wrong element count for a valid filter.");
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, DISPATCH_OUTPUT	, 3, "packFilter() did not append converted elements for a valid filter.");
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, INVALID_RESULT	, 4, "packFilter() accepted an invalid filter value.");
GDEFINE_ENUM_VALUED(PACK_FILTER_TEST_RESULT, INVALID_OUTPUT	, 5, "packFilter() changed the output for an invalid filter value.");

using FPackFilter = ::gpk::err_t (*)(::gpk::apod<::gpk::u1_t> &, ::gpk::vcu0_t);

::gpk::err_t test_pack_filter(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	sttc cnst FPackFilter wrappers[] =
		{ &::gpk::packFilter0<::gpk::u0_t, ::gpk::u1_t>
		, &::gpk::packFilter1<::gpk::u0_t, ::gpk::u1_t>
		, &::gpk::packFilter2<::gpk::u0_t, ::gpk::u1_t>
		, &::gpk::packFilter3<::gpk::u0_t, ::gpk::u1_t>
		, &::gpk::packFilter4<::gpk::u0_t, ::gpk::u1_t>
		};
	cnst ::gpk::u0_t input[] = {1, 127, 255};

	for(::gpk::u0_t iFilter = 0; iFilter < 5; ++iFilter) {
		::gpk::apod<::gpk::u1_t> wrapperOutput = {};
		if_fail_fe(wrapperOutput.push_back(0x1FF));
		cnst ::gpk::err_t wrapperResult = wrappers[iFilter](wrapperOutput, input);
		GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_WRAPPER_RESULT, wrapperResult != 3);
		GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_WRAPPER_OUTPUT, wrapperOutput.size() != 4 || wrapperOutput[0] != 0x1FF || wrapperOutput[1] != 1 || wrapperOutput[2] != 127 || wrapperOutput[3] != 255);

		::gpk::apod<::gpk::u1_t> dispatchOutput = {};
		if_fail_fe(dispatchOutput.push_back(0x1FF));
		cnst ::gpk::err_t dispatchResult = ::gpk::packFilter(dispatchOutput, ::gpk::vcu0_t{input}, (::gpk::PACK_FILTER)iFilter);
		GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_DISPATCH_RESULT, dispatchResult != 3);
		GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_DISPATCH_OUTPUT, dispatchOutput.size() != 4 || dispatchOutput[0] != 0x1FF || dispatchOutput[1] != 1 || dispatchOutput[2] != 127 || dispatchOutput[3] != 255);
	}

	::gpk::apod<::gpk::u1_t> invalidOutput = {};
	if_fail_fe(invalidOutput.push_back(0x1FF));
	cnst ::gpk::err_t invalidResult = ::gpk::packFilter(invalidOutput, ::gpk::vcu0_t{input}, (::gpk::PACK_FILTER)0xFF);
	GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_INVALID_RESULT, invalidResult >= 0);
	GPK_TEST_CHECK(errors, PACK_FILTER_TEST_RESULT_INVALID_OUTPUT, invalidOutput.size() != 1 || invalidOutput[0] != 0x1FF);
	return 0;
}
