#include "test_blitter_suites.h"

#include "gpk_file.h"
#include "gpk_json.h"

GDEFINE_ENUM_TYPE(TEST_REASON_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, INPUT_WRITE	, 0, "The reason input fixture could not be written.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, PROCESS_LAUNCH	, 1, "The reason frontend could not be launched.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, PROCESS_EXIT	, 2, "The reason frontend rejected a valid JSON-array fixture.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, OUTPUT_READ	, 3, "The reason output table could not be read.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, OUTPUT_PARSE	, 4, "The reason output table is not valid JSON.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, OUTPUT_ROOT	, 5, "The reason output root is not a JSON array.");
GDEFINE_ENUM_VALUED(TEST_REASON_RESULT, RECORD_COUNT	, 6, "The reason output does not retain every input record.");

::gpk::err_t test_reason(STestRunnerState &, ::gpk::ATestError & errors, ::gpk::vcst_t outputFolder) {
	stxp ::gpk::vcst_t inputName = "reason_input.json";
	stxp ::gpk::vcst_t outputName = "reason_input.sson";
	stxp ::gpk::vcst_t inputJSON = "[{\"id\":1},{\"id\":2},{\"id\":3}]";

	cnst ::gpk::err_t writeResult = ::gpk::fileFromMemory(outputFolder, inputName, inputJSON.bytes());
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_INPUT_WRITE, ::gpk::failed(writeResult), "Result:%i.", writeResult);

	int exitCode = -1;
	cnst ::gpk::err_t launchResult = ::testBlitterRunFrontend("reason", outputFolder, "reason_input.json 0", exitCode);
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_PROCESS_LAUNCH, ::gpk::failed(launchResult), "Result:%i.", launchResult);
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_PROCESS_EXIT, 0 != exitCode, "Exit code:%i.", exitCode);

	::gpk::au0_t outputBytes;
	cnst ::gpk::err_t readResult = ::gpk::fileToMemory(outputFolder, outputName, outputBytes);
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_OUTPUT_READ, ::gpk::failed(readResult), "Result:%i.", readResult);

	::gpk::SJSONReader reader;
	cnst ::gpk::err_t parseResult = ::gpk::jsonParse(reader, ::gpk::vcst_t{(cnst ::gpk::sc_t*)outputBytes.begin(), outputBytes.size()});
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_OUTPUT_PARSE, ::gpk::failed(parseResult), "Result:%i, bytes:%u.", parseResult, outputBytes.size());
	GPK_TEST_REQUIREF(errors, TEST_REASON_RESULT_OUTPUT_ROOT, 0 == reader.Tree.size() || ::gpk::JSON_TYPE_ARRAY != reader.Token[0].Type, "Tree:%u.", reader.Tree.size());
	GPK_TEST_CHECKF(errors, TEST_REASON_RESULT_RECORD_COUNT, 3 != ::gpk::jsonArraySize(*reader.Tree[0]), "Records:%u.", ::gpk::jsonArraySize(*reader.Tree[0]));
	rtrn 0;
}
