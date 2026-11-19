#include "test_blitter_suites.h"

#include "gpk_file.h"
#include "gpk_json.h"

GDEFINE_ENUM_TYPE(TEST_SEASON_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, INPUT_WRITE	, 0, "The season input fixture could not be written.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, PROCESS_LAUNCH	, 1, "The season frontend could not be launched.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, PROCESS_EXIT	, 2, "The season frontend rejected a valid JSON-array fixture.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLOCK_READ	, 3, "A season output block could not be read.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLOCK_PARSE	, 4, "A season output block is not valid JSON.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLOCK_ROOT	, 5, "A season output block root is not a JSON array.");
GDEFINE_ENUM_VALUED(TEST_SEASON_RESULT, BLOCK_COUNT	, 6, "A season output block contains the wrong number of records.");

::gpk::err_t test_season(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	stxp ::gpk::vcst_t inputName = "season_input.json";
	stxp ::gpk::vcst_t inputJSON = "[{\"id\":1},{\"id\":2},{\"id\":3},{\"id\":4},{\"id\":5}]";
	cnst ::gpk::err_t writeResult = ::gpk::fileFromMemory(outputFolder, inputName, inputJSON.bytes());
	GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_INPUT_WRITE, ::gpk::failed(writeResult), "Result:%i.", writeResult);

	int exitCode = -1;
	cnst ::gpk::err_t launchResult = ::testBlitterRunFrontend("season", outputFolder, "season_input.json 2 0", exitCode);
	GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_PROCESS_LAUNCH, ::gpk::failed(launchResult), "Result:%i.", launchResult);
	GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_PROCESS_EXIT, 0 != exitCode, "Exit code:%i.", exitCode);

	stxp ::gpk::vcst_t blockNames[] =
		{ "season_input.2.db/season_input.0.sson"
		, "season_input.2.db/season_input.1.sson"
		, "season_input.2.db/season_input.2.sson"
		};
	stxp ::gpk::u2_t expectedCounts[] = {2, 2, 1};
	for(::gpk::u2_t iBlock = 0; iBlock < ::gpk::size(blockNames); ++iBlock) {
		::gpk::au0_t outputBytes;
		cnst ::gpk::err_t readResult = ::gpk::fileToMemory(outputFolder, blockNames[iBlock], outputBytes);
		GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_BLOCK_READ, ::gpk::failed(readResult), "Block:%u, result:%i.", iBlock, readResult);

		::gpk::SJSONReader reader;
		cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, ::gpk::vcst_t{(cnst ::gpk::sc_t*)outputBytes.begin(), outputBytes.size()});
		GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_BLOCK_PARSE, ::gpk::failed(parseResult), "Block:%u, result:%i, bytes:%u.", iBlock, parseResult, outputBytes.size());
		GPK_TEST_REQUIREF(errors, TEST_SEASON_RESULT_BLOCK_ROOT, 0 == reader.Tree.size() || ::gpk::JSON_TYPE_ARRAY != reader.Token[0].Type, "Block:%u, tree:%u.", iBlock, reader.Tree.size());
		GPK_TEST_CHECKF(errors, TEST_SEASON_RESULT_BLOCK_COUNT, expectedCounts[iBlock] != (::gpk::u2_t)::gpk::jsonArraySize(*reader.Tree[0]), "Block:%u, records:%u, expected:%u.", iBlock, ::gpk::jsonArraySize(*reader.Tree[0]), expectedCounts[iBlock]);
	}
	rtrn 0;
}
