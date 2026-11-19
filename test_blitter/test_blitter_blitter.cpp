#include "test_blitter_suites.h"

#include "blitter.h"

GDEFINE_ENUM_TYPE(TEST_BLITTER_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, TABLE_NAME_RESULT	, 0, "tableFileName() rejected valid input.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, TABLE_NAME_VALUE	, 1, "tableFileName() selected the wrong extension.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, BLOCK_NAME_RESULT	, 2, "blockFileName() rejected valid input.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, BLOCK_NAME_VALUE	, 3, "blockFileName() selected the wrong extension.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, FOLDER_NAME_RESULT	, 4, "tableFolderName() rejected valid input.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, FOLDER_NAME_VALUE	, 5, "tableFolderName() produced the wrong folder name.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, FOLDER_INIT_RESULT	, 6, "tableFolderInit() rejected a writable test folder.");
GDEFINE_ENUM_VALUED(TEST_BLITTER_RESULT, FOLDER_INIT_VALUE	, 7, "tableFolderInit() produced the wrong database path.");

::gpk::err_t test_blitter(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	struct SNameCase {
		::blt::DATABASE_HOST Host;
		bool Encrypted;
		::gpk::vcst_t Extension;
	};
	stxp SNameCase cases[] =
		{ {::blt::DATABASE_HOST_LOCAL	, false, "sson"}
		, {::blt::DATABASE_HOST_DEFLATE	, false, "zson"}
		, {::blt::DATABASE_HOST_LOCAL	, true , "cson"}
		, {::blt::DATABASE_HOST_DEFLATE	, true , "czon"}
		};

	for(cnst SNameCase & testCase : cases) {
		::gpk::string expectedTable = ::gpk::vcst_t{"catalog."};
		if_fail_fe(expectedTable.append(testCase.Extension));
		::gpk::string tableName;
		cnst ::gpk::err_t tableResult = ::blt::tableFileName(tableName, testCase.Host, testCase.Encrypted, "catalog");
		GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_TABLE_NAME_RESULT, ::gpk::failed(tableResult), "Host:%u, encrypted:%u, result:%i.", testCase.Host, testCase.Encrypted, tableResult);
		GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_TABLE_NAME_VALUE, tableName != expectedTable, "Actual:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(tableName), GPK_VIEW_ARGS(expectedTable));

		::gpk::string expectedBlock = ::gpk::vcst_t{"catalog.7."};
		if_fail_fe(expectedBlock.append(testCase.Extension));
		::gpk::string blockName;
		cnst ::gpk::err_t blockResult = ::blt::blockFileName(blockName, "catalog", testCase.Encrypted, testCase.Host, 7);
		GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_BLOCK_NAME_RESULT, ::gpk::failed(blockResult), "Host:%u, encrypted:%u, result:%i.", testCase.Host, testCase.Encrypted, blockResult);
		GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_BLOCK_NAME_VALUE, blockName != expectedBlock, "Actual:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(blockName), GPK_VIEW_ARGS(expectedBlock));
	}

	::gpk::string folderName;
	cnst ::gpk::err_t folderResult = ::blt::tableFolderName(folderName, "catalog", 64);
	GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_FOLDER_NAME_RESULT, ::gpk::failed(folderResult), "Result:%i.", folderResult);
	GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_FOLDER_NAME_VALUE, folderName != ::gpk::vcst_t{"catalog.64.db"}, "Actual:'%.*s'.", GPK_VIEW_ARGS(folderName));

	::gpk::string expectedFolder = outputFolder;
	if(expectedFolder.size() && '/' != expectedFolder[expectedFolder.size() - 1] && '\\' != expectedFolder[expectedFolder.size() - 1])
		if_fail_fe(expectedFolder.push_back('/'));
	if_fail_fe(expectedFolder.append(::gpk::vcst_t{"catalog.64.db/"}));
	::gpk::string initializedFolder;
	cnst ::gpk::err_t initResult = ::blt::tableFolderInit(initializedFolder, outputFolder, "catalog", 64);
	GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_FOLDER_INIT_RESULT, ::gpk::failed(initResult), "Result:%i.", initResult);
	GPK_TEST_CHECKF(errors, TEST_BLITTER_RESULT_FOLDER_INIT_VALUE, initializedFolder != expectedFolder, "Actual:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(initializedFolder), GPK_VIEW_ARGS(expectedFolder));
	rtrn 0;
}
