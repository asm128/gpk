#include "gpk_index_path.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(INDEX_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, BUILD_RESULT	, 0, "getWordDict() rejected a valid SPathTree.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, REVERSE_COUNT	, 1, "The reverse map does not contain one view per tree entry.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, REVERSE_VALUE	, 2, "A reverse-map view does not contain the tree entry name.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, REVERSE_ALIAS	, 3, "A reverse-map view does not point to the tree entry string.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, PATH_WORD		, 4, "An ancestor directory was not indexed for its descendants.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, FILE_NAME		, 5, "A file name was not indexed.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, FILE_TITLE	, 6, "A file title was not indexed.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, EXTENSION		, 7, "A file extension was not indexed.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, ENTRY_TYPE	, 8, "A filesystem entry type was not indexed.");

sttc ::gpk::err_t indexTreeBuild(::gpk::SPathTree & tree) {
	stxp ::gpk::vcst_c names[] = {"src", "network", "client.cpp", "README.md"};
	if_fail_fe(::gpk::resize(::gpk::size(names), tree.Name, tree.Info, tree.Tree));
	for(::gpk::u2_t iName = 0; iName < ::gpk::size(names); ++iName)
		if_fail_fe(tree.Char.append(names[iName]));
	::gpk::u2_t offset = 0;
	for(::gpk::u2_t iName = 0; iName < ::gpk::size(names); ++iName) {
		tree.Name[iName] = {tree.Char.begin() + offset, names[iName].size()};
		offset += names[iName].size();
		if_null_fe(tree.Tree[iName].create());
		tree.Tree[iName]->Info = &tree.Info[iName];
		tree.Tree[iName]->ObjectIndex = iName;
	}
	tree.Info[0] = {0, 1};
	tree.Info[1] = {1, 1};
	tree.Info[2] = {2, 0};
	tree.Info[3] = {1, 0};
	tree.Tree[1]->Parent = tree.Tree[0].operator->();
	tree.Tree[2]->Parent = tree.Tree[1].operator->();
	tree.Tree[3]->Parent = tree.Tree[0].operator->();
	if_fail_fe(tree.Tree[0]->Children.push_back(tree.Tree[1]));
	if_fail_fe(tree.Tree[1]->Children.push_back(tree.Tree[2]));
	if_fail_fe(tree.Tree[0]->Children.push_back(tree.Tree[3]));
	rtrn 0;
}

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

::gpk::err_t test_index_path(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::SPathTree tree = {};
	if_fail_fe(::indexTreeBuild(tree));
	::gpk::aobj<::gpk::SWordIndices> wordSet = {};
	::gpk::aobj<::gpk::vcst_t> reverseMap = {};
	::gpk::aobj<::gpk::string> cacheItemWords = {};
	cnst ::gpk::err_t result = ::gpk::getWordDict(wordSet, reverseMap, tree, cacheItemWords);
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_BUILD_RESULT, ::gpk::failed(result), "Result:%i.", result);
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_REVERSE_COUNT, reverseMap.size() != tree.Name.size(), "Reverse count:%u, tree names:%u.", reverseMap.size(), tree.Name.size());
	for(::gpk::u2_t iName = 0; iName < tree.Name.size(); ++iName) {
		GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_REVERSE_VALUE, reverseMap[iName] != tree.Name[iName], "Entry:%u reverse:'%.*s', tree:'%.*s'.", iName, GPK_VIEW_ARGS(reverseMap[iName]), GPK_VIEW_ARGS(tree.Name[iName]));
		GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_REVERSE_ALIAS, reverseMap[iName].begin() != tree.Name[iName].begin(), "Entry:%u reverse:%p, tree:%p.", iName, reverseMap[iName].begin(), tree.Name[iName].begin());
	}
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_PATH_WORD, false == ::indexWordHas(wordSet, "src", 2));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_PATH_WORD, false == ::indexWordHas(wordSet, "network", 2));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_FILE_NAME, false == ::indexWordHas(wordSet, "client.cpp", 2));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_FILE_TITLE, false == ::indexWordHas(wordSet, "client", 2));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_EXTENSION, false == ::indexWordHas(wordSet, "cpp", 2));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_ENTRY_TYPE, false == ::indexWordHas(wordSet, "folder", 1));
	GPK_TEST_CHECK(errors, INDEX_TEST_RESULT_ENTRY_TYPE, false == ::indexWordHas(wordSet, "file", 3));
	rtrn 0;
}
