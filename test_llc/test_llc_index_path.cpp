#include "gpk_index_path.h"
#include "gpk_file.h"

#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(INDEX_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, FILE_WRITE		, 0, "A real index fixture file could not be written.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, SCAN_RESULT		, 1, "pathList() rejected the real fixture directory.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, SCAN_COUNT		, 2, "The extension filter retained an unexpected number of files.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, EXTENSION_SET		, 3, "The extension filter did not retain exactly the .cpp and .h files.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, BUILD_RESULT		, 4, "getWordDict() rejected the filtered source files.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, CONTENT_RESOLVE	, 5, "A file-content word did not resolve its source record.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, RESOLVED_ITEM		, 6, "A content query resolved an unexpected source record.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, FULL_PATH			, 7, "A resolved source record did not reconstruct its full path.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, REOPEN_RESULT		, 8, "A resolved source file could not be reopened.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, SOURCE_BYTES		, 9, "A reopened source file does not match its indexed fixture bytes.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, EXCLUDED_CONTENT	, 10, "Content from the excluded extension entered the index.");
GDEFINE_ENUM_VALUED(INDEX_TEST_RESULT, OPTIONAL_PATH		, 11, "Optional path indexing did not honor its selected state.");

stxp ::gpk::vcst_c	INDEX_FILE_NAMES[]		= {"index_fixture.cpp", "index_fixture.h", "index_fixture.txt"};
stxp ::gpk::vcst_c	INDEX_FILE_CONTENTS[]	= {"cpp_content_probe alpha", "header_content_probe beta", "excluded_text_probe gamma"};
stxp ::gpk::vcst_c	INDEX_CONTENT_PROBES[]	= {"cpp_content_probe", "header_content_probe"};

sttc ::gpk::err_t indexFixtureWrite(::gpk::vcst_t folder, ::gpk::vcst_t name, ::gpk::vcst_t contents, ::gpk::string & fullPath) {
	if_fail_fe(fullPath.clear());
	if_fail_fe(::gpk::pathNameCompose(folder, name, fullPath));
	rtrn ::gpk::fileFromMemory(fullPath, contents);
}

sttc bool indexExtensionAccepted(::gpk::vcst_t path) {
	::gpk::vcst_t fileName = {};
	if(::gpk::failed(::gpk::pathFilename(path, fileName)))
		rtrn false;
	::gpk::vcst_t extension = {};
	if(fileName.size() >= 4) {
		fileName.slice(extension, fileName.size() - 4);
		if(extension == ".cpp")
			rtrn true;
	}
	if(fileName.size() >= 2) {
		fileName.slice(extension, fileName.size() - 2);
		if(extension == ".h")
			rtrn true;
	}
	rtrn false;
}

::gpk::err_t test_index_path(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	::gpk::string fixturePath[::gpk::size(INDEX_FILE_NAMES)] = {};
	for(::gpk::u2_t iFile = 0; iFile < ::gpk::size(INDEX_FILE_NAMES); ++iFile) {
		cnst ::gpk::err_t writeResult = ::indexFixtureWrite(outputFolder, INDEX_FILE_NAMES[iFile], INDEX_FILE_CONTENTS[iFile], fixturePath[iFile]);
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_FILE_WRITE, ::gpk::failed(writeResult), "File:%u, result:%i.", iFile, writeResult);
	}

	::gpk::SPathTree tree = {};
	cnst ::gpk::err_t scanResult = ::gpk::pathList(tree, outputFolder, [](bool folder, ::gpk::vcst_c & entry) -> ::gpk::err_t {
		rtrn folder || false == ::indexExtensionAccepted(entry) ? 1 : 0;
	});
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_SCAN_RESULT, ::gpk::failed(scanResult), "Result:%i.", scanResult);
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_SCAN_COUNT, 2 != tree.Tree.size() || 2 != tree.Name.size(), "Nodes:%u, names:%u.", tree.Tree.size(), tree.Name.size());
	bool foundCPP = false, foundH = false, foundTXT = false;
	for(::gpk::u2_t iName = 0; iName < tree.Name.size(); ++iName) {
		foundCPP |= tree.Name[iName] == INDEX_FILE_NAMES[0];
		foundH   |= tree.Name[iName] == INDEX_FILE_NAMES[1];
		foundTXT |= tree.Name[iName] == INDEX_FILE_NAMES[2];
	}
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_EXTENSION_SET, false == foundCPP || false == foundH || foundTXT, "CPP:%u, H:%u, TXT:%u.", foundCPP, foundH, foundTXT);

	::gpk::aobj<::gpk::SWordIndices> wordSet = {};
	::gpk::aobj<::gpk::vcst_t> reverseMap = {};
	::gpk::SPathIndexData indexData = {};
	indexData.IndexPath = false;
	cnst ::gpk::err_t buildResult = ::gpk::getWordDict(indexData, wordSet, reverseMap, tree);
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_BUILD_RESULT, ::gpk::failed(buildResult), "Result:%i.", buildResult);

	for(::gpk::u2_t iFile = 0; iFile < ::gpk::size(INDEX_CONTENT_PROBES); ++iFile) {
		::gpk::au2_t resolved = {};
		cnst ::gpk::err_t resolveResult = ::gpk::wordDictResolve(wordSet, INDEX_CONTENT_PROBES[iFile], tree.Name.size(), resolved);
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_CONTENT_RESOLVE, 1 != resolveResult || 1 != resolved.size(), "File:%u, result:%i, count:%u.", iFile, resolveResult, resolved.size());
		cnst ::gpk::u2_t itemIndex = resolved[0];
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_RESOLVED_ITEM, itemIndex >= tree.Tree.size() || itemIndex >= reverseMap.size(), "File:%u, item:%u, nodes:%u, reverse:%u.", iFile, itemIndex, tree.Tree.size(), reverseMap.size());
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_RESOLVED_ITEM, reverseMap[itemIndex] != INDEX_FILE_NAMES[iFile], "File:%u, item:%u, name:'%.*s'.", iFile, itemIndex, GPK_VIEW_ARGS(reverseMap[itemIndex]));
		::gpk::string resolvedPath = {};
		cnst ::gpk::err_t pathResult = ::gpk::pathNodePath(resolvedPath, tree, *tree.Tree[itemIndex]);
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_FULL_PATH, ::gpk::failed(pathResult) || resolvedPath != fixturePath[iFile], "File:%u, result:%i, resolved:'%.*s', expected:'%.*s'.", iFile, pathResult, GPK_VIEW_ARGS(resolvedPath), GPK_VIEW_ARGS(fixturePath[iFile]));
		::gpk::string reopened = {};
		cnst ::gpk::err_t reopenResult = ::gpk::fileToMemory(resolvedPath, reopened);
		GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_REOPEN_RESULT, ::gpk::failed(reopenResult), "File:%u, result:%i.", iFile, reopenResult);
		GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_SOURCE_BYTES, reopened != INDEX_FILE_CONTENTS[iFile], "File:%u, reopened:%u, expected:%u.", iFile, reopened.size(), INDEX_FILE_CONTENTS[iFile].size());
	}
	::gpk::au2_t excluded = {};
	cnst ::gpk::err_t excludedResult = ::gpk::wordDictResolve(wordSet, "excluded_text_probe", tree.Name.size(), excluded);
	GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_EXCLUDED_CONTENT, excludedResult || excluded.size(), "Result:%i, count:%u.", excludedResult, excluded.size());
	::gpk::au2_t pathMatches = {};
	cnst ::gpk::err_t pathDisabledResult = ::gpk::wordDictResolve(wordSet, INDEX_FILE_NAMES[0], tree.Name.size(), pathMatches);
	GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_OPTIONAL_PATH, pathDisabledResult || pathMatches.size(), "Result:%i, count:%u.", pathDisabledResult, pathMatches.size());

	if_fail_fe(wordSet.clear());
	indexData.IndexPath = true;
	cnst ::gpk::err_t pathBuildResult = ::gpk::getWordDict(indexData, wordSet, reverseMap, tree);
	GPK_TEST_REQUIREF(errors, INDEX_TEST_RESULT_BUILD_RESULT, ::gpk::failed(pathBuildResult), "Result:%i.", pathBuildResult);
	cnst ::gpk::err_t pathEnabledResult = ::gpk::wordDictResolve(wordSet, INDEX_FILE_NAMES[0], tree.Name.size(), pathMatches);
	GPK_TEST_CHECKF(errors, INDEX_TEST_RESULT_OPTIONAL_PATH, 1 != pathEnabledResult || 1 != pathMatches.size(), "Result:%i, count:%u.", pathEnabledResult, pathMatches.size());
	rtrn 0;
}
