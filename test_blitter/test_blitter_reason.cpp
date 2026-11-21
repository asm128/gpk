#include "test_blitter_suites.h"

#include "blitter.h"
#include "gpk_file.h"
#include "gpk_json.h"

GDEFINE_ENUM_TYPE(TEST_REASON_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, INPUT_WRITE	, 0, "The reason input fixture could not be written.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, PROCESS_LAUNCH	, 1, "The reason frontend could not be launched.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, PROCESS_EXIT	, 2, "The reason frontend rejected a valid JSON-array fixture.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, BLITTER_LOAD	, 3, "Blitter could not load the table written by reason.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, BLOCK_AVAILABLE	, 4, "Blitter did not retain the loaded reason table block.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, JSON_WRITE	, 5, "The JSON loaded by Blitter could not be serialized.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, JSON_CONTENT	, 6, "The JSON loaded by Blitter differs from the reason input.");

::gpk::err_t test_reason(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	stxp ::gpk::vcst_t inputJSON = "[{\"id\":1},{\"id\":2},{\"id\":3}]";
	stxp ::gpk::vcst_t encryptionKey = "0123456789ABCDEF0123456789ABCDEF";
	struct SModeCase {
		::gpk::vcst_t Name;
		::gpk::vcst_t Database;
		::gpk::vcst_t InputFile;
		::gpk::vcst_t Arguments;
		::blt::DATABASE_HOST Host;
		::gpk::vcst_t Key;
	};
	stxp SModeCase modes[] =
		{ {"plain"              , "reason_plain"    , "reason_plain.json"    , "reason_plain.json 0"                         , ::blt::DATABASE_HOST_LOCAL  , {}}
		, {"deflated"           , "reason_deflated" , "reason_deflated.json" , "reason_deflated.json 1"                      , ::blt::DATABASE_HOST_DEFLATE, {}}
		, {"encrypted"          , "reason_encrypted", "reason_encrypted.json", "reason_encrypted.json 0 0123456789ABCDEF0123456789ABCDEF" , ::blt::DATABASE_HOST_LOCAL  , encryptionKey}
		, {"encrypted+deflated" , "reason_secure"   , "reason_secure.json"   , "reason_secure.json 1 0123456789ABCDEF0123456789ABCDEF"    , ::blt::DATABASE_HOST_DEFLATE, encryptionKey}
		};

	::gpk::SJSONReader sourceReader;
	if_fail_fe(::gpk::jsonParse(sourceReader, inputJSON));
	::gpk::string expectedJSON;
	if_fail_fe(::gpk::jsonWrite(sourceReader.Tree[0], sourceReader.View, expectedJSON));

	for(cnst SModeCase & mode : modes) {
		cnst ::gpk::err_t writeResult = ::gpk::fileFromMemory(outputFolder, mode.InputFile, inputJSON.bytes());
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_INPUT_WRITE, ::gpk::failed(writeResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), writeResult);
		if(::gpk::failed(writeResult))
			continue;

		int exitCode = -1;
		cnst ::gpk::err_t launchResult = ::testBlitterRunFrontend("reason", outputFolder, mode.Arguments, exitCode);
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_PROCESS_LAUNCH, ::gpk::failed(launchResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), launchResult);
		if(::gpk::failed(launchResult))
			continue;
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_PROCESS_EXIT, 0 != exitCode, "Mode:%.*s, exit code:%i.", GPK_VIEW_ARGS(mode.Name), exitCode);
		if(0 != exitCode)
			continue;

		::gpk::SLoadCache loadCache;
		::blt::TNamedBlitterDB database = {};
		database.Key = mode.Database;
		database.Val.HostType = mode.Host;
		database.Val.EncryptionKey = mode.Key;
		cnst ::gpk::err_t loadResult = ::blt::tableFileLoad(loadCache, database, outputFolder);
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_BLITTER_LOAD, ::gpk::failed(loadResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), loadResult);
		if(::gpk::failed(loadResult))
			continue;

		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_BLOCK_AVAILABLE, 0 == database.Val.Blocks.size() || 0 == database.Val.Blocks[0], "Mode:%.*s, blocks:%u.", GPK_VIEW_ARGS(mode.Name), database.Val.Blocks.size());
		if(0 == database.Val.Blocks.size() || 0 == database.Val.Blocks[0])
			continue;

		::gpk::string recoveredJSON;
		cnst ::gpk::SJSONReader & recoveredReader = database.Val.Blocks[0]->Reader;
		cnst ::gpk::err_t jsonResult = ::gpk::jsonWrite(recoveredReader.Tree[0], recoveredReader.View, recoveredJSON);
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_JSON_WRITE, ::gpk::failed(jsonResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), jsonResult);
		if(::gpk::failed(jsonResult))
			continue;
		GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_JSON_CONTENT, recoveredJSON != expectedJSON, "Mode:%.*s, recovered:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(mode.Name), GPK_VIEW_ARGS(recoveredJSON), GPK_VIEW_ARGS(expectedJSON));
	}
	rtrn 0;
}
