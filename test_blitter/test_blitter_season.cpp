#include "test_blitter_suites.h"

#include "blitter.h"
#include "gpk_file.h"
#include "gpk_json.h"

GDEFINE_ENUM_TYPE(TEST_SEASON_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, INPUT_WRITE	, 0, "The season input fixture could not be written.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, PROCESS_LAUNCH	, 1, "The season frontend could not be launched.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, PROCESS_EXIT	, 2, "The season frontend rejected a valid JSON-array fixture.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLITTER_LOAD	, 3, "Blitter could not load a block written by season.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLOCK_AVAILABLE	, 4, "Blitter did not retain a loaded season block.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, JSON_WRITE	, 5, "A season block loaded by Blitter could not be serialized.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, JSON_CONTENT	, 6, "A season block loaded by Blitter differs from the expected source records.");

::gpk::err_t test_season(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	stxp ::gpk::vcst_t inputJSON = "[{\"id\":1},{\"id\":2},{\"id\":3},{\"id\":4},{\"id\":5}]";
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
		{ {"plain"              , "season_plain"    , "season_plain.json"    , "season_plain.json 2 0"                      , ::blt::DATABASE_HOST_LOCAL  , {}}
		, {"deflated"           , "season_deflated" , "season_deflated.json" , "season_deflated.json 2 1"                   , ::blt::DATABASE_HOST_DEFLATE, {}}
		, {"encrypted"          , "season_encrypted", "season_encrypted.json", "season_encrypted.json 2 0 0123456789ABCDEF0123456789ABCDEF" , ::blt::DATABASE_HOST_LOCAL  , encryptionKey}
		, {"encrypted+deflated" , "season_secure"   , "season_secure.json"   , "season_secure.json 2 1 0123456789ABCDEF0123456789ABCDEF"    , ::blt::DATABASE_HOST_DEFLATE, encryptionKey}
		};

	::gpk::SJSONReader sourceReader;
	if_fail_fe(::gpk::jsonParse(sourceReader, inputJSON));
	::gpk::aobj<::gpk::string> expectedBlocks;
	if_fail_fe(::gpk::jsonArraySplit(*sourceReader.Tree[0], sourceReader.View, 2, expectedBlocks));

	for(cnst SModeCase & mode : modes) {
		cnst ::gpk::err_t writeResult = ::gpk::fileFromMemory(outputFolder, mode.InputFile, inputJSON.bytes());
		GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_INPUT_WRITE, ::gpk::failed(writeResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), writeResult);
		if(::gpk::failed(writeResult))
			continue;

		int exitCode = -1;
		cnst ::gpk::err_t launchResult = ::testBlitterRunFrontend("season", outputFolder, mode.Arguments, exitCode);
		GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_PROCESS_LAUNCH, ::gpk::failed(launchResult), "Mode:%.*s, result:%i.", GPK_VIEW_ARGS(mode.Name), launchResult);
		if(::gpk::failed(launchResult))
			continue;
		GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_PROCESS_EXIT, 0 != exitCode, "Mode:%.*s, exit code:%i.", GPK_VIEW_ARGS(mode.Name), exitCode);
		if(0 != exitCode)
			continue;

		::gpk::SLoadCache loadCache;
		::blt::TNamedBlitterDB database = {};
		database.Key = mode.Database;
		database.Val.BlockSize = 2;
		database.Val.HostType = mode.Host;
		database.Val.EncryptionKey = mode.Key;
		for(::gpk::u2_t iBlock = 0; iBlock < expectedBlocks.size(); ++iBlock)
			if_fail_fe(database.Val.BlocksOnDisk.push_back(iBlock));
		database.Val.MaxBlockOnDisk = expectedBlocks.size() - 1;

		for(::gpk::u2_t iBlock = 0; iBlock < expectedBlocks.size(); ++iBlock) {
			cnst ::gpk::err_t loadResult = ::blt::blockFileLoad(loadCache, database, outputFolder, iBlock);
			GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_BLITTER_LOAD, ::gpk::failed(loadResult), "Mode:%.*s, block:%u, result:%i.", GPK_VIEW_ARGS(mode.Name), iBlock, loadResult);
			if(::gpk::failed(loadResult))
				continue;

			cnst ::gpk::u2_t loadedIndex = (::gpk::u2_t)loadResult;
			GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_BLOCK_AVAILABLE, loadedIndex >= database.Val.Blocks.size() || 0 == database.Val.Blocks[loadedIndex], "Mode:%.*s, block:%u, loaded index:%u, blocks:%u.", GPK_VIEW_ARGS(mode.Name), iBlock, loadedIndex, database.Val.Blocks.size());
			if(loadedIndex >= database.Val.Blocks.size() || 0 == database.Val.Blocks[loadedIndex])
				continue;

			::gpk::string recoveredJSON;
			cnst ::gpk::SJSONReader & recoveredReader = database.Val.Blocks[loadedIndex]->Reader;
			cnst ::gpk::err_t jsonResult = ::gpk::jsonWrite(recoveredReader.Tree[0], recoveredReader.View, recoveredJSON);
			GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_JSON_WRITE, ::gpk::failed(jsonResult), "Mode:%.*s, block:%u, result:%i.", GPK_VIEW_ARGS(mode.Name), iBlock, jsonResult);
			if(::gpk::failed(jsonResult))
				continue;
			GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_JSON_CONTENT, recoveredJSON != expectedBlocks[iBlock], "Mode:%.*s, block:%u, recovered:'%.*s', expected:'%.*s'.", GPK_VIEW_ARGS(mode.Name), iBlock, GPK_VIEW_ARGS(recoveredJSON), GPK_VIEW_ARGS(expectedBlocks[iBlock]));
		}
	}
	rtrn 0;
}
