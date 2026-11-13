#include "gpk_string_compose.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(STRING_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, EMPTY				,  0, "A default string is not empty.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, LITERAL_SIZE		,  1, "Literal construction produced the wrong size.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, LITERAL_CONTENT		,  2, "Literal construction produced the wrong content.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, TERMINATOR			,  3, "The string terminator was not preserved.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, VIEW_CONVERSIONS		,  4, "String view conversions changed the pointer or count.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, POINTER_CONVERSIONS	,  5, "String pointer conversions changed the pointer.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, COUNTED_VIEW			,  6, "Counted-view construction produced the wrong string.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, COPY_INDEPENDENCE		,  7, "A copied string shares mutable storage with its source.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, COMPARISON			,  8, "String comparison disagrees with its literal or view.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, CAMEL_CASE_RESULT		,  9, "camelCase() rejected valid input.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, CAMEL_CASE_TEXT		, 10, "camelCase() produced unexpected text.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, CAMEL_CASE_APPEND_RESULT, 11, "camelCase() rejected valid appended input.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, CAMEL_CASE_APPEND_TEXT	, 12, "camelCase() did not append the converted text.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, STR_U0				, 13, "str(u0_t) does not expose exactly its formatted digits.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, STR_U0_APPEND_RESULT	, 14, "append_strings() rejected a valid u0_t.");
GDEFINE_ENUM_VALUED(STRING_TEST_RESULT, STR_U0_APPEND_TEXT	, 15, "append_strings() appended the wrong u0_t text.");

::gpk::err_t test_string(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::string empty;
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_EMPTY, 0 != empty.size() || empty.begin() != empty.end());

	::gpk::string literal = "alpha";
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_LITERAL_SIZE, 5 != literal.size());
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_LITERAL_CONTENT, literal != "alpha");
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_TERMINATOR, 0 != literal.begin()[literal.size()]);

	cnst ::gpk::string & constLiteral = literal;
	cnst ::gpk::vcst_t constView = constLiteral;
	::gpk::vstr_t mutableView = literal;
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_VIEW_CONVERSIONS
		, constView.begin() != literal.begin() || constView.size() != literal.size()
		|| mutableView.begin() != literal.begin() || mutableView.size() != literal.size()
		);
	cnst ::gpk::sc_c * constPointer = constLiteral.begin();
	::gpk::sc_t * mutablePointer = literal.begin();
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_POINTER_CONVERSIONS, constPointer != literal.begin() || mutablePointer != literal.begin());

	cnst ::gpk::vcst_t countedSource = {"bravo-tail", 5};
	::gpk::string counted = countedSource;
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_COUNTED_VIEW, counted != "bravo" || 5 != counted.size());

	::gpk::string copy = literal;
	copy.begin()[0] = 'A';
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_COPY_INDEPENDENCE, copy != "Alpha" || literal != "alpha" || copy.begin() == literal.begin());
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_COMPARISON, false == (literal == ::gpk::vcst_t{"alpha"}) || false == (literal != "beta"));

	::gpk::asc_t camelCased;
	cnst ::gpk::err_t camelResult = ::gpk::camelCase("hello_world-test", camelCased);
	GPK_TEST_REQUIRE(errors, STRING_TEST_RESULT_CAMEL_CASE_RESULT, ::gpk::failed(camelResult));
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_CAMEL_CASE_TEXT, ::gpk::vcst_t{camelCased} != "HelloWorldTest");

	::gpk::asc_t appended = ::gpk::vcst_t{"Prefix"};
	cnst ::gpk::err_t appendResult = ::gpk::camelCase("next_value", appended);
	GPK_TEST_REQUIRE(errors, STRING_TEST_RESULT_CAMEL_CASE_APPEND_RESULT, ::gpk::failed(appendResult));
	GPK_TEST_CHECK(errors, STRING_TEST_RESULT_CAMEL_CASE_APPEND_TEXT, ::gpk::vcst_t{appended} != "PrefixNextValue");

	for(::gpk::u2_t iValue = 0; iValue <= 0xFF; ++iValue) {
		::gpk::sc_t expected[5] = {};
		cnst ::gpk::err_t expectedLength = ::gpk::sprintf_s(expected, "%u", iValue);
		cnst auto actualStorage = ::gpk::str((::gpk::u0_t)iValue);
		cnst ::gpk::vcst_t actual = actualStorage;
		GPK_TEST_CHECKF(errors, STRING_TEST_RESULT_STR_U0
			, ::gpk::failed(expectedLength)
			|| actualStorage.Count != (::gpk::u2_t)expectedLength
			|| actualStorage.size() != (::gpk::u2_t)expectedLength
			|| actual.size() != (::gpk::u2_t)expectedLength
			|| 0 != ::memcmp(actual.begin(), expected, expectedLength)
			|| 0 != actualStorage.Storage[expectedLength]
			, "value:%u count:%u actual-size:%u expected-size:%i"
			, iValue, actualStorage.Count, actual.size(), expectedLength
			);

		::gpk::asc_t composed;
		cnst ::gpk::err_t composeResult = ::gpk::append_strings(composed, ::gpk::str((::gpk::u0_t)iValue), "x");
		GPK_TEST_CHECKF(errors, STRING_TEST_RESULT_STR_U0_APPEND_RESULT, ::gpk::failed(composeResult), "value:%u result:%i", iValue, composeResult);
		expected[expectedLength] = 'x';
		expected[expectedLength + 1] = 0;
		GPK_TEST_CHECKF(errors, STRING_TEST_RESULT_STR_U0_APPEND_TEXT
			, composed.size() != (::gpk::u2_t)expectedLength + 1
			|| 0 != ::memcmp(composed.begin(), expected, expectedLength + 1)
			, "value:%u actual-size:%u expected-size:%i"
			, iValue, composed.size(), expectedLength + 1
			);
	}
	rtrn 0;
}
