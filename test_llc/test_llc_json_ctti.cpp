#include "gpk_json_ctti.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(JSON_CTTI_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, OK				, 0, "All constexpr JSON reader tests passed.");
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, PARSE			, 1, "The constexpr JSON reader rejected a valid document.");
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, TOKEN_COUNT		, 2, "The constexpr JSON reader produced no tokens.");
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, TOKEN_SPAN		, 3, "A constexpr JSON token has an invalid source span.");
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, TOKEN_PARENT	, 4, "A constexpr JSON token has an invalid parent index.");
GDEFINE_ENUM_VALUED(JSON_CTTI_TEST_RESULT, INVALID_INPUT	, 5, "The constexpr JSON reader accepted invalid input.");

stxp ::gpk::sc_t JSON_CTTI_DOCUMENT[] = "{\"name\":\"gpk\",\"values\":[1,true,null,\"x\"],\"nested\":{\"enabled\":true}}";
stxp auto JSON_CTTI_READER = ::gpk::jsonParseCTTI(JSON_CTTI_DOCUMENT);
stxp auto JSON_CTTI_INVALID = ::gpk::jsonParseCTTI("{\"name\":}");
static_assert(JSON_CTTI_READER.Success());
static_assert(JSON_CTTI_READER.Count);
static_assert(false == JSON_CTTI_INVALID.Success());

::gpk::err_t test_json_ctti(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t) {
	GPK_TEST_REQUIRE(errors, JSON_CTTI_TEST_RESULT_PARSE, false == JSON_CTTI_READER.Success());
	GPK_TEST_REQUIRE(errors, JSON_CTTI_TEST_RESULT_TOKEN_COUNT, 0 == JSON_CTTI_READER.Count);
	cnst ::gpk::view<cnst ::gpk::SJSONToken> tokens = JSON_CTTI_READER.Tokens();
	for(::gpk::u2_t iToken = 0; iToken < tokens.size(); ++iToken) {
		cnst ::gpk::SJSONToken & token = tokens[iToken];
		GPK_TEST_CHECKF(errors, JSON_CTTI_TEST_RESULT_TOKEN_SPAN
			, token.Span.Begin > token.Span.End || token.Span.End > ::gpk::size(JSON_CTTI_DOCUMENT) - 1
			, "Token:%u, span:%u..%u.", iToken, token.Span.Begin, token.Span.End
			);
		GPK_TEST_CHECKF(errors, JSON_CTTI_TEST_RESULT_TOKEN_PARENT
			, 0 <= token.ParentIndex && (::gpk::u2_t)token.ParentIndex >= iToken
			, "Token:%u, parent:%i.", iToken, token.ParentIndex
			);
	}
	GPK_TEST_CHECK(errors, JSON_CTTI_TEST_RESULT_INVALID_INPUT, JSON_CTTI_INVALID.Success());
	rtrn 0;
}
