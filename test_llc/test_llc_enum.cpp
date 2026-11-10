#include "gpk_enum.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(ENUM_TEST_COMMAND, ::gpk::u0_t);
GDEFINE_ENUM_VALUE(ENUM_TEST_COMMAND, Join, 0);
GDEFINE_ENUM_VALUE(ENUM_TEST_COMMAND, Quit, 1);

GDEFINE_ENUM_TYPE(ENUM_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, SLICE_RESULT		, 0, "The command prefix could not be removed.");
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, SPLIT_RESULT		, 1, "The command name and arguments could not be split.");
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, COMMAND_NAME		, 2, "The parsed command name is not 'join'.");
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, COMMAND_ARGS		, 3, "The parsed command arguments are not 'abc'.");
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, REGISTERED_LABEL	, 4, "The registered command label is not 'Join'.");
GDEFINE_ENUM_VALUED(ENUM_TEST_RESULT, LOWERCASE_LOOKUP	, 5, "The lowercase command did not resolve to Join.");

::gpk::err_t test_enum(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	::gpk::vcsc_t inputLine = ::gpk::vcst_t{".join abc"};
	cnst ::gpk::err_t sliceResult = inputLine.slice(inputLine, 1);
	GPK_TEST_REQUIRE(errors, ENUM_TEST_RESULT_SLICE_RESULT, ::gpk::failed(sliceResult));

	::gpk::vcsc_t commandName, commandArgs;
	cnst ::gpk::err_t splitResult = ::gpk::split(::gpk::vcst_t{" "}, inputLine, commandName, commandArgs);
	GPK_TEST_REQUIRE(errors, ENUM_TEST_RESULT_SPLIT_RESULT, ::gpk::failed(splitResult));
	GPK_TEST_CHECK(errors, ENUM_TEST_RESULT_COMMAND_NAME, commandName != ::gpk::vcst_t{"join"});
	GPK_TEST_CHECK(errors, ENUM_TEST_RESULT_COMMAND_ARGS, commandArgs != ::gpk::vcst_t{"abc"});
	GPK_TEST_CHECK(errors, ENUM_TEST_RESULT_REGISTERED_LABEL, ::gpk::get_value_namev(ENUM_TEST_COMMAND_Join) != ::gpk::vcst_t{"Join"});

	cnst ENUM_TEST_COMMAND command = ::gpk::get_value<ENUM_TEST_COMMAND>(commandName);
	GPK_TEST_CHECK(errors, ENUM_TEST_RESULT_LOWERCASE_LOOKUP, command != ENUM_TEST_COMMAND_Join);
	rtrn 0;
}
