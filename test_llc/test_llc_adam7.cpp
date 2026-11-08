#include "gpk_adam7.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(ADAM7_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SCALE_RESULT	, 1, "adam7ScaleIndex() rejected a valid pass.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SCALE_MULTIPLIER	, 2, "adam7ScaleIndex() returned the wrong multiplier.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SCALE_BASE		, 3, "adam7ScaleIndex() returned the wrong base.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SCALE_INVALID		, 4, "adam7ScaleIndex() accepted an invalid pass.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SIZE_RESULT		, 5, "adam7Sizes() failed for valid storage.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SIZE_STORAGE		, 6, "adam7Sizes() accepted insufficient pass storage.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SIZE_WIDTH		, 7, "adam7Sizes() returned the wrong pass width.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SIZE_HEIGHT		, 8, "adam7Sizes() returned the wrong pass height.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, SIZE_COVERAGE		, 9, "Adam7 pass sizes do not cover every source pixel exactly once.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, INTERLACE_RESULT	, 10, "adam7Interlace() failed for valid passes.");
GDEFINE_ENUM_VALUED(ADAM7_TEST_RESULT, INTERLACE_CELL	, 11, "adam7Interlace() placed a texel in the wrong cell.");

stxp ::gpk::n2u2_t ADAM7_MULTIPLIER[] = {{8, 8}, {8, 8}, {4, 8}, {4, 4}, {2, 4}, {2, 2}, {1, 2}};
stxp ::gpk::n2u2_t ADAM7_BASE      [] = {{0, 0}, {4, 0}, {0, 4}, {2, 0}, {0, 2}, {1, 0}, {0, 1}};
stxp ::gpk::n2u2_t ADAM7_SIZE_8X8  [] = {{1, 1}, {1, 1}, {2, 1}, {2, 2}, {4, 2}, {4, 4}, {8, 4}};

sttc ::gpk::err_t testAdam7Scale(::gpk::ATestError & errors) {
	for(::gpk::u2_t iPass = 0; iPass < ::gpk::size(ADAM7_MULTIPLIER); ++iPass) {
		::gpk::n2u2_t multiplier = {}, base = {};
		cnst ::gpk::err_t result = ::gpk::adam7ScaleIndex(iPass, multiplier, base);
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_RESULT, ::gpk::failed(result), "Pass:%u, result:%i.", iPass, result);
		if(::gpk::failed(result))
			continue;
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_MULTIPLIER, multiplier.x != ADAM7_MULTIPLIER[iPass].x, "Pass:%u, x:%u, expected:%u.", iPass, multiplier.x, ADAM7_MULTIPLIER[iPass].x);
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_MULTIPLIER, multiplier.y != ADAM7_MULTIPLIER[iPass].y, "Pass:%u, y:%u, expected:%u.", iPass, multiplier.y, ADAM7_MULTIPLIER[iPass].y);
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_BASE, base.x != ADAM7_BASE[iPass].x, "Pass:%u, x:%u, expected:%u.", iPass, base.x, ADAM7_BASE[iPass].x);
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_BASE, base.y != ADAM7_BASE[iPass].y, "Pass:%u, y:%u, expected:%u.", iPass, base.y, ADAM7_BASE[iPass].y);
	}
	::gpk::n2u2_t multiplier = {}, base = {};
	GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_INVALID, 0 <= ::gpk::adam7ScaleIndex(-1, multiplier, base), "%s", "Pass -1 was accepted.");
	GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SCALE_INVALID, 0 <= ::gpk::adam7ScaleIndex( 7, multiplier, base), "%s", "Pass 7 was accepted.");
	rtrn 0;
}

sttc ::gpk::err_t testAdam7Sizes(::gpk::ATestError & errors) {
	::gpk::n2u2_t insufficient[6] = {};
	::gpk::view<::gpk::n2u2_t> insufficientView = {insufficient};
	GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SIZE_STORAGE, 0 <= ::gpk::adam7Sizes(insufficientView, {8, 8}), "%s", "Six pass sizes were accepted.");

	for(::gpk::u2_t height = 1; height <= 17; ++height) {
		for(::gpk::u2_t width = 1; width <= 17; ++width) {
			::gpk::n2u2_t passSize[7] = {};
			::gpk::view<::gpk::n2u2_t> passSizeView = {passSize};
			cnst ::gpk::n2u2_t imageSize = {width, height};
			cnst ::gpk::err_t result = ::gpk::adam7Sizes(passSizeView, imageSize);
			GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SIZE_RESULT, ::gpk::failed(result) , "Image:%ux%u, result:%i." , width, height, result );
			if(::gpk::failed(result))
				continue;
			::gpk::u2_t covered = {};
			for(::gpk::u2_t iPass = 0; iPass < passSizeView.size(); ++iPass)
				covered += passSize[iPass].x * passSize[iPass].y;
			GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SIZE_COVERAGE, covered != width * height , "Image:%ux%u, covered:%u, expected:%u." , width, height, covered, width * height );
		}
	}

	::gpk::n2u2_t passSize[7] = {};
	::gpk::view<::gpk::n2u2_t> passSizeView = {passSize};
	if_fail_fe(::gpk::adam7Sizes(passSizeView, {8, 8}));
	for(::gpk::u2_t iPass = 0; iPass < passSizeView.size(); ++iPass) {
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SIZE_WIDTH, passSize[iPass].x != ADAM7_SIZE_8X8[iPass].x , "Pass:%u, width:%u, expected:%u.", iPass, passSize[iPass].x, ADAM7_SIZE_8X8[iPass].x);
		GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_SIZE_HEIGHT, passSize[iPass].y != ADAM7_SIZE_8X8[iPass].y , "Pass:%u, height:%u, expected:%u.", iPass, passSize[iPass].y, ADAM7_SIZE_8X8[iPass].y);
	}
	rtrn 0;
}

sttc ::gpk::err_t testAdam7Interlace(::gpk::ATestError & errors) {
	::gpk::imgu2_t passes[7] = {};
	for(::gpk::u2_t iPass = 0; iPass < ::gpk::size(passes); ++iPass) {
		if_fail_fe(passes[iPass].resize(ADAM7_SIZE_8X8[iPass]));
		for(::gpk::u2_t y = 0; y < passes[iPass].metrics().y; ++y) {
			for(::gpk::u2_t x = 0; x < passes[iPass].metrics().x; ++x) {
				cnst ::gpk::n2u2_t target =
					{ x * ADAM7_MULTIPLIER[iPass].x + ADAM7_BASE[iPass].x
					, y * ADAM7_MULTIPLIER[iPass].y + ADAM7_BASE[iPass].y
					};
				passes[iPass][y][x] = 1 + target.y * 8 + target.x;
			}
		}
	}
	::gpk::imgu2_t output = {};
	cnst ::gpk::n2u2_t outputSize = {8, 8};
	if_fail_fe(output.resize(outputSize, 0U));
	::gpk::view<::gpk::imgu2_t> passView = {passes};
	cnst ::gpk::err_t result = ::gpk::adam7Interlace(passView, output.View);
	GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_INTERLACE_RESULT, ::gpk::failed(result), "Result:%i.", result);
	if(::gpk::failed(result))
		rtrn 0;
	for(::gpk::u2_t y = 0; y < output.metrics().y; ++y) {
		for(::gpk::u2_t x = 0; x < output.metrics().x; ++x) {
			cnst ::gpk::u2_t expected = 1 + y * output.metrics().x + x;
			GPK_TEST_CHECKF(errors, ADAM7_TEST_RESULT_INTERLACE_CELL, output[y][x] != expected , "Cell:{%u,%u}, value:%u, expected:%u." , x, y, output[y][x], expected );
		}
	}
	rtrn 0;
}

::gpk::err_t test_adam7(STestRunnerState & , ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(::testAdam7Scale(errors));
	if_fail_fe(::testAdam7Sizes(errors));
	rtrn ::testAdam7Interlace(errors);
}
