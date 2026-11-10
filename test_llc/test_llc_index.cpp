#include "gpk_index.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(INDEX_CORE_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, NORMALIZE_RESULT	, 0, "wordListNormalize() rejected valid input.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, NORMALIZE_COUNT	, 1, "wordListNormalize() returned an unexpected word count.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, NORMALIZE_VALUE	, 2, "wordListNormalize() returned an unexpected normalized word.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, APPEND_RESULT		, 3, "wordDictAppend() rejected valid input.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, APPEND_WORD		, 4, "wordDictAppend() did not create an expected dictionary word.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, APPEND_POSTING	, 5, "wordDictAppend() did not create the expected unique posting.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, LIST_RESULT		, 6, "getWordList() rejected a valid item.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, LIST_WORD		, 7, "getWordList() did not expose an expected item word.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, BUILD_RESULT		, 8, "getWordDict() rejected a valid item.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, RESOLVE_RESULT	, 9, "wordDictResolve() returned an unexpected result count.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, RESOLVE_ITEM		, 10, "wordDictResolve() returned an unexpected item order.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, SCRIPT_RESULT	, 11, "outputSearchScript() rejected a valid index.");
GDEFINE_ENUM_VALUED(INDEX_CORE_TEST_RESULT, SCRIPT_DATA		, 12, "outputSearchScript() omitted index data.");

sttc cnst ::gpk::SWordIndices * indexWord(cnst ::gpk::aobj<::gpk::SWordIndices> & wordSet, ::gpk::vcst_t word) {
	for(::gpk::u2_t iWord = 0; iWord < wordSet.size(); ++iWord)
		if(wordSet[iWord].Text == word)
			rtrn &wordSet[iWord];
	rtrn {};
}

sttc bool indexWordHas(cnst ::gpk::aobj<::gpk::SWordIndices> & wordSet, ::gpk::vcst_t word, ::gpk::u2_t item) {
	cnst ::gpk::SWordIndices * indexed = ::indexWord(wordSet, word);
	rtrn indexed && 0 <= ::gpk::find(item, {indexed->Indices.begin(), indexed->Indices.size()});
}

sttc ::gpk::err_t testIndexNormalizeAndAppend(::gpk::ATestError & errors) {
	stxp ::gpk::vcst_c inputWords[] = {"ALPHA", "the", "Beta"};
	::gpk::aobj<::gpk::string> normalized = {};
	cnst ::gpk::err_t normalizeResult = ::gpk::wordListNormalize(normalized, inputWords);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_NORMALIZE_RESULT, ::gpk::failed(normalizeResult), "Result:%i.", normalizeResult);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_NORMALIZE_COUNT, normalized.size() != ::gpk::size(inputWords), "Count:%u.", normalized.size());
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_NORMALIZE_VALUE, normalized[0] != "alpha");
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_NORMALIZE_VALUE, normalized[1].size());
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_NORMALIZE_VALUE, normalized[2] != "beta");

	if_fail_fe(normalized.push_back("alpha"));
	::gpk::aobj<::gpk::SWordIndices> wordSet = {};
	cnst ::gpk::err_t appendResult = ::gpk::wordDictAppend(wordSet, 7, normalized);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_APPEND_RESULT, ::gpk::failed(appendResult), "Result:%i.", appendResult);
	cnst ::gpk::SWordIndices * alpha = ::indexWord(wordSet, "alpha");
	cnst ::gpk::SWordIndices * beta = ::indexWord(wordSet, "beta");
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_APPEND_WORD, !alpha || !beta, "Words:%u.", wordSet.size());
	GPK_TEST_CHECKF(errors, INDEX_CORE_TEST_RESULT_APPEND_POSTING, 1 != alpha->Indices.size() || 7 != alpha->Indices[0], "Alpha postings:%u.", alpha->Indices.size());
	GPK_TEST_CHECKF(errors, INDEX_CORE_TEST_RESULT_APPEND_POSTING, 1 != beta->Indices.size() || 7 != beta->Indices[0], "Beta postings:%u.", beta->Indices.size());
	rtrn 0;
}

sttc ::gpk::err_t testIndexBuildAndResolve(::gpk::ATestError & errors) {
	::gpk::SItemViews items[] = {{}, {}, {}};
	items[0].Text = "Alpha orchard common";
	items[1].Text = "Betamax relay common";
	items[2].Text = "Alphabet soup common";
	::gpk::aobj<::gpk::string> itemWords = {};
	cnst ::gpk::err_t listResult = ::gpk::getWordList(itemWords, items[0]);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_LIST_RESULT, ::gpk::failed(listResult), "Result:%i.", listResult);
	bool foundAlpha = false;
	bool foundOrchard = false;
	for(::gpk::u2_t iWord = 0; iWord < itemWords.size(); ++iWord) {
		foundAlpha |= itemWords[iWord] == "alpha";
		foundOrchard |= itemWords[iWord] == "orchard";
	}
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_LIST_WORD, false == foundAlpha);
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_LIST_WORD, false == foundOrchard);

	::gpk::aobj<::gpk::SWordIndices> wordSet = {};
	for(::gpk::u2_t iItem = 0; iItem < ::gpk::size(items); ++iItem) {
		cnst ::gpk::err_t buildResult = ::gpk::getWordDict(wordSet, items[iItem], iItem, itemWords);
		GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_BUILD_RESULT, ::gpk::failed(buildResult), "Item:%u, result:%i.", iItem, buildResult);
	}
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_APPEND_WORD, false == ::indexWordHas(wordSet, "alpha", 0));
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_APPEND_POSTING, false == ::indexWordHas(wordSet, "relay", 1));

	::gpk::au2_t resolved = {};
	cnst ::gpk::err_t sharedResult = ::gpk::wordDictResolve(wordSet, "COMMON", ::gpk::size(items), resolved);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_RESOLVE_RESULT, sharedResult != 3 || resolved.size() != 3, "Result:%i, count:%u.", sharedResult, resolved.size());
	for(::gpk::u2_t iItem = 0; iItem < resolved.size(); ++iItem)
		GPK_TEST_CHECKF(errors, INDEX_CORE_TEST_RESULT_RESOLVE_ITEM, resolved[iItem] != iItem, "Position:%u, item:%u.", iItem, resolved[iItem]);
	cnst ::gpk::err_t resolveResult = ::gpk::wordDictResolve(wordSet, "PHA MAX", ::gpk::size(items), resolved);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_RESOLVE_RESULT, resolveResult != 3 || resolved.size() != 3, "Result:%i, count:%u.", resolveResult, resolved.size());
	for(::gpk::u2_t iItem = 0; iItem < resolved.size(); ++iItem)
		GPK_TEST_CHECKF(errors, INDEX_CORE_TEST_RESULT_RESOLVE_ITEM, resolved[iItem] != iItem, "Position:%u, item:%u.", iItem, resolved[iItem]);
	cnst ::gpk::err_t emptyResult = ::gpk::wordDictResolve(wordSet, "", ::gpk::size(items), resolved);
	GPK_TEST_CHECKF(errors, INDEX_CORE_TEST_RESULT_RESOLVE_RESULT, emptyResult || resolved.size(), "Result:%i, count:%u.", emptyResult, resolved.size());

	::gpk::string script = {};
	cnst ::gpk::err_t scriptResult = ::gpk::outputSearchScript(script, items);
	GPK_TEST_REQUIREF(errors, INDEX_CORE_TEST_RESULT_SCRIPT_RESULT, ::gpk::failed(scriptResult), "Result:%i.", scriptResult);
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_SCRIPT_DATA, 0 > script.find(::gpk::vcst_t{"var names=["}));
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_SCRIPT_DATA, 0 > script.find(::gpk::vcst_t{"var indices=["}));
	GPK_TEST_CHECK(errors, INDEX_CORE_TEST_RESULT_SCRIPT_DATA, 0 > script.find(::gpk::vcst_t{"var idList=["}));
	rtrn 0;
}

::gpk::err_t test_index(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	if_fail_fe(::testIndexNormalizeAndAppend(errors));
	if_fail_fe(::testIndexBuildAndResolve(errors));
	rtrn 0;
}
