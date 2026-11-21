#include "gpk_json_core.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(JSON_CORE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, OK				, 0, "All JSON core tests passed.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, TOKEN_TYPE		, 1, "SJSONToken does not preserve its declared fields.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, TOKEN_NEXT		, 2, "Implicit JSON token traversal returned the wrong sibling.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, ARRAY_SIZE		, 3, "Implicit JSON array traversal returned the wrong size.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, OBJECT_KEY_COUNT	, 4, "Implicit JSON object traversal returned the wrong key count.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, OBJECT_VALUE		, 5, "Implicit JSON object traversal returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, ARRAY_VALUE		, 6, "Implicit JSON array traversal returned the wrong value.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, ARRAY_FOREACH		, 7, "Implicit JSON array enumeration returned the wrong values.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, OBJECT_FOREACH	, 8, "Implicit JSON object enumeration returned the wrong pairs.");
GDEFINE_ENUM_VALUED(JSON_CORE_TEST_RESULT, COMPARE			, 9, "Implicit JSON comparison returned the wrong result.");

stxp ::gpk::sc_t JSON_CORE_SOURCE[] = "{\"values\":[1,2,3],\"flag\":true}";
stxp ::gpk::SJSONToken JSON_CORE_TOKENS[] =
	{ {-1, ::gpk::JSON_TYPE_OBJECT , { 0, 30}, {}}
	, {-1, ::gpk::JSON_TYPE_KEY    , { 1,  9}, {}}
	, {-1, ::gpk::JSON_TYPE_STRING , { 2,  8}, {}}
	, {-1, ::gpk::JSON_TYPE_VALUE  , {10, 17}, {}}
	, {-1, ::gpk::JSON_TYPE_ARRAY  , {10, 17}, {}}
	, {-1, ::gpk::JSON_TYPE_VALUE  , {11, 12}, {}}
	, {-1, ::gpk::JSON_TYPE_INTEGER, {11, 12}, 1 }
	, {-1, ::gpk::JSON_TYPE_VALUE  , {13, 14}, {}}
	, {-1, ::gpk::JSON_TYPE_INTEGER, {13, 14}, 2 }
	, {-1, ::gpk::JSON_TYPE_VALUE  , {15, 16}, {}}
	, {-1, ::gpk::JSON_TYPE_INTEGER, {15, 16}, 3 }
	, {-1, ::gpk::JSON_TYPE_KEY    , {18, 24}, {}}
	, {-1, ::gpk::JSON_TYPE_STRING , {19, 23}, {}}
	, {-1, ::gpk::JSON_TYPE_VALUE  , {25, 29}, {}}
	, {-1, ::gpk::JSON_TYPE_BOOLEAN, {25, 29}, 1 }
	};
stxp ::gpk::vcst_t JSON_CORE_VIEWS[] =
	{ {}, {}, "values", {}, {}, {}, "1", {}, "2", {}, "3", {}, "flag", {}, "true" };
stxp ::gpk::view<cnst ::gpk::SJSONToken> JSON_CORE_TOKEN_VIEW = JSON_CORE_TOKENS;
stxp ::gpk::view<::gpk::vcst_c> JSON_CORE_VIEW_LIST = JSON_CORE_VIEWS;
static_assert(JSON_CORE_TOKEN_VIEW[0].Type == ::gpk::JSON_TYPE_OBJECT);
static_assert(::gpk::jsonObjectValueGet(JSON_CORE_TOKEN_VIEW, JSON_CORE_VIEW_LIST, 0, "values") == 4);
static_assert(::gpk::jsonObjectValueGet(JSON_CORE_TOKEN_VIEW, JSON_CORE_VIEW_LIST, 0, "flag") == 14);
static_assert(::gpk::jsonObjectValueGet(JSON_CORE_TOKEN_VIEW, JSON_CORE_VIEW_LIST, 0, "missing") == -1);

::gpk::err_t test_json_core(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t) {
	cnst ::gpk::SJSONToken token = {-1, ::gpk::JSON_TYPE_STRING, {2, 7}, 11};
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_TOKEN_TYPE
		, token.ParentIndex != -1 || token.Type != ::gpk::JSON_TYPE_STRING || token.Span.Begin != 2 || token.Span.End != 7 || token.Value != 11
		);
	cnst ::gpk::view<cnst ::gpk::SJSONToken> tokens = JSON_CORE_TOKENS;
	cnst ::gpk::vcst_t source = JSON_CORE_SOURCE;
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_TOKEN_NEXT, ::gpk::jsonTokenNext(tokens, 3) != 11);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_ARRAY_SIZE, ::gpk::jsonArraySize(tokens, 4) != 3);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_OBJECT_KEY_COUNT, ::gpk::jsonObjectKeyCount(tokens, 0) != 2);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_OBJECT_VALUE, ::gpk::jsonObjectValueGet(tokens, source, 0, "values") != 4);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_OBJECT_VALUE, ::gpk::jsonObjectValueGet(tokens, source, 0, "flag") != 14);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_OBJECT_VALUE, 0 <= ::gpk::jsonObjectValueGet(tokens, source, 0, "missing"));
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_ARRAY_VALUE, ::gpk::jsonArrayValueGet(tokens, 4, 1) != 8);

	stxp ::gpk::u2_t expectedValues[] = {6, 8, 10};
	::gpk::u2_t valueCount = 0;
	cnst ::gpk::err_t arrayResult = ::gpk::jsonArrayForEach(tokens, 4, [&valueCount](::gpk::u2_t iValue) -> ::gpk::err_t {
		if(valueCount >= ::gpk::size(expectedValues) || iValue != expectedValues[valueCount++])
			rtrn -1;
		rtrn 0;
		});
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_ARRAY_FOREACH, arrayResult != 3 || valueCount != 3);

	stxp ::gpk::u2_t expectedKeys[] = {2, 12};
	stxp ::gpk::u2_t expectedObjectValues[] = {4, 14};
	::gpk::u2_t pairCount = 0;
	cnst ::gpk::err_t objectResult = ::gpk::jsonObjectForEach(tokens, 0, [&pairCount](::gpk::u2_t iKey, ::gpk::u2_t iValue) -> ::gpk::err_t {
		if(pairCount >= ::gpk::size(expectedKeys) || iKey != expectedKeys[pairCount] || iValue != expectedObjectValues[pairCount])
			rtrn -1;
		++pairCount;
		rtrn 0;
		});
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_OBJECT_FOREACH, objectResult != 2 || pairCount != 2);
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_COMPARE, 1 != ::gpk::jsonCompare(tokens, source, 0, tokens, source, 0));
	GPK_TEST_CHECK(errors, JSON_CORE_TEST_RESULT_COMPARE, 0 != ::gpk::jsonCompareNumber(tokens, source, 6, tokens, source, 8));
	rtrn 0;
}
