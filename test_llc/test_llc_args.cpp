#include "gpk_args.h"
#include "gpk_log.h"

#include "test_llc_suites.h"
////
//GDEFINE_ENUM_TYPE(ARGS_TEST_RESULT, ::gpk::u0_t);
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, PARSE_RESULT		, 0, "argsParse() returned an unexpected argument count.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, PROGRAM_NAME		, 1, "argsParse() did not preserve the program name.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, ENVIRONMENT		, 2, "argsParse() did not preserve the environment views.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, OPTIONS			, 3, "argsParse() produced unexpected option records.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, POSITIONALS		, 4, "argsParse() produced unexpected positional arguments.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, OPTION_INDEX		, 5, "argsOptionIndex() did not find repeated options from an offset.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, OPTION_VALUES		, 6, "argsOptionValues() did not append every matching value in order.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, OPTION_VALUE		, 7, "argsOptionValue() did not return the sole matching option value.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, AMBIGUOUS_VALUE	, 8, "argsOptionValue() accepted a key with multiple values.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, MISSING_VALUE		, 9, "The option accessors did not report a missing key consistently.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, REUSE				, 10, "argsParse() retained state from an earlier parse.");
//GDEFINE_ENUM_VALUED(ARGS_TEST_RESULT, EMPTY_VALUE		, 11, "argsParse() did not preserve empty inline and valueless options.");
//
//sttc ::gpk::err_t testArgsParsedValues(::gpk::ATestError & errors, cnst ::gpk::SCommandLineArgs & args) {
//	cnst ::gpk::keyval<::gpk::vcst_t> expectedOptions[] =
//		{ {"include", "a"}
//		, {"include", "b"}
//		, {"flag"   , {}}
//		, {"include", "c"}
//		, {"output" , "result"}
//		};
//	GPK_TEST_REQUIREF(errors, ARGS_TEST_RESULT_OPTIONS, args.Options.size() != ::gpk::size(expectedOptions) , "Option count:%u, expected:%u." , args.Options.size(), (::gpk::u2_t)::gpk::size(expectedOptions) );
//	for(::gpk::u2_t iOption = 0; iOption < args.Options.size(); ++iOption) {
//		always_printf("ARGS option:%u expected:'%.*s'='%.*s'; actual:'%.*s'='%.*s'.", iOption, GPK_VIEW_ARGS(expectedOptions[iOption].Key), GPK_VIEW_ARGS(expectedOptions[iOption].Val), GPK_VIEW_ARGS(args.Options[iOption].Key), GPK_VIEW_ARGS(args.Options[iOption].Val));
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTIONS, args.Options[iOption].Key != expectedOptions[iOption].Key , "Option:%u key:'%.*s', expected:'%.*s'." , iOption, (int)args.Options[iOption].Key.size(), args.Options[iOption].Key.begin() , (int)expectedOptions[iOption].Key.size(), expectedOptions[iOption].Key.begin());
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTIONS, args.Options[iOption].Val != expectedOptions[iOption].Val , "Option:%u value:'%.*s', expected:'%.*s'." , iOption, (int)args.Options[iOption].Val.size(), args.Options[iOption].Val.begin() , (int)expectedOptions[iOption].Val.size(), expectedOptions[iOption].Val.begin());
//	}
//
//	cnst ::gpk::vcst_t expectedPositionals[] = {"first", "last", "--literal", ""};
//	GPK_TEST_REQUIREF(errors, ARGS_TEST_RESULT_POSITIONALS, args.Positionals.size() != ::gpk::size(expectedPositionals) , "Positional count:%u, expected:%u." , args.Positionals.size(), (::gpk::u2_t)::gpk::size(expectedPositionals) );
//	for(::gpk::u2_t iPositional = 0; iPositional < args.Positionals.size(); ++iPositional)
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_POSITIONALS, args.Positionals[iPositional] != expectedPositionals[iPositional] , "Positional:%u is '%.*s', expected '%.*s'." , iPositional, (int)args.Positionals[iPositional].size(), args.Positionals[iPositional].begin(), (int)expectedPositionals[iPositional].size(), expectedPositionals[iPositional].begin() );
//	rtrn 0;
//}
//
//sttc ::gpk::err_t testArgsAccessors(::gpk::ATestError & errors, cnst ::gpk::SCommandLineArgs & args) {
//	cnst ::gpk::err_t firstInclude	= ::gpk::argsOptionIndex(args, "include");
//	cnst ::gpk::err_t secondInclude	= ::gpk::argsOptionIndex(args, "include", 1);
//	cnst ::gpk::err_t thirdInclude	= ::gpk::argsOptionIndex(args, "include", 2);
//	cnst ::gpk::err_t afterInclude	= ::gpk::argsOptionIndex(args, "include", 4);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_INDEX, firstInclude != 0 , "First --include index:%i, expected:0.", firstInclude);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_INDEX, secondInclude != 1 , "Second --include index:%i, expected:1.", secondInclude);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_INDEX, thirdInclude != 3 , "Third --include index:%i, expected:3.", thirdInclude);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_INDEX, afterInclude != -1 , "--include index after offset 4:%i, expected:-1.", afterInclude);
//
//	::gpk::aobj<::gpk::vcst_t> values = {"seed"};
//	cnst ::gpk::err_t valueCount = ::gpk::argsOptionValues(args, "include", values);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_VALUES, valueCount != 3 , "Collected --include value count:%i, expected:3.", valueCount);
//	GPK_TEST_REQUIREF(errors, ARGS_TEST_RESULT_OPTION_VALUES, values.size() != 4 , "Collected --include output size:%u, expected:4 including the seed.", values.size());
//	cnst ::gpk::vcst_t expectedValues[] = {"seed", "a", "b", "c"};
//	for(::gpk::u2_t iValue = 0; iValue < values.size(); ++iValue)
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_VALUES, values[iValue] != expectedValues[iValue] , "Collected value:%u is '%.*s', expected '%.*s'." , iValue, (int)values[iValue].size(), values[iValue].begin(), (int)expectedValues[iValue].size(), expectedValues[iValue].begin() );
//
//	::gpk::vcst_t singleValue = {};
//	cnst ::gpk::err_t singleIndex = ::gpk::argsOptionValue(args, "output", singleValue);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_VALUE, singleIndex != 4 , "--output index:%i, expected:4.", singleIndex);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_OPTION_VALUE, singleValue != "result" , "--output value:'%.*s', expected:'result'.", GPK_VIEW_ARGS(singleValue));
//
//	::gpk::vcst_t ambiguousValue = "stale";
//	::gpk::vcst_t missingValue = "stale";
//	cnst ::gpk::err_t ambiguous = ::gpk::argsOptionValue(args, "include", ambiguousValue);
//	cnst ::gpk::err_t missing = ::gpk::argsOptionValue(args, "missing", missingValue);
//	cnst ::gpk::err_t missingCount = ::gpk::argsOptionValues(args, "missing", values);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_AMBIGUOUS_VALUE, ambiguous != -2 , "Repeated --include single-value result:%i, expected:-2.", ambiguous);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_MISSING_VALUE, missing != -1 , "Missing option single-value result:%i, expected:-1.", missing);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_MISSING_VALUE, missingCount , "Missing option collected value count:%i, expected:0.", missingCount);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_MISSING_VALUE, values.size() != 4 , "Missing option changed collected output size:%u, expected:4.", values.size());
//	rtrn 0;
//}
//
//::gpk::err_t testArgs(::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
//	::gpk::vcst_t argv[] =
//		{ "app"
//		, "first"
//		, "--include", "a", "b"
//		, "--flag"
//		, "--include", "c"
//		, "--output=result", "last"
//		, "--", "--literal", ""
//		};
//	::gpk::vcst_t envp[] = {"A=B"};
//	::gpk::SCommandLineArgs args = {};
//	cnst ::gpk::err_t parsed = ::gpk::argsParse(args, argv, envp);
//	for(::gpk::u2_t iArg = 0; iArg < ::gpk::size(argv); ++iArg)
//		always_printf("ARGS input[%u] (%u bytes): '%.*s'.", iArg, argv[iArg].size(), GPK_VIEW_ARGS(argv[iArg]));
//	always_printf("ARGS parse result:%i; program:'%.*s'; options:%u; positionals:%u.", parsed, GPK_VIEW_ARGS(args.ProgramName), args.Options.size(), args.Positionals.size());
//	for(::gpk::u2_t iArg = 0; iArg < args.Positionals.size(); ++iArg)
//		always_printf("ARGS positional[%u]:'%.*s'.", iArg, GPK_VIEW_ARGS(args.Positionals[iArg]));
//	GPK_TEST_REQUIREF(errors, ARGS_TEST_RESULT_PARSE_RESULT, 0 > parsed , "Initial argsParse() failed with result:%i.", parsed);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_PARSE_RESULT, parsed != 9 , "Parse result:%i, expected:9." , parsed );
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_PROGRAM_NAME, args.ProgramName != "app" , "Program:'%.*s', expected:'app'." , (int)args.ProgramName.size(), args.ProgramName.begin() );
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_ENVIRONMENT, args.Environment.size() != 1 , "Environment entry count:%u, expected:1.", args.Environment.size());
//	if(args.Environment.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_ENVIRONMENT, args.Environment[0] != "A=B" , "Environment entry 0:'%.*s', expected:'A=B'." , (int)args.Environment[0].size(), args.Environment[0].begin());
//	}
//	if_fail_fe(::testArgsParsedValues(errors, args));
//	if_fail_fe(::testArgsAccessors(errors, args));
//
//	::gpk::vcst_t reuseArgv[] = {"other", "only"};
//	::gpk::vcst_t reuseEnvp[] = {"C=D"};
//	cnst ::gpk::err_t reused = ::gpk::argsParse(args, reuseArgv, reuseEnvp);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, reused != 1 , "Reused parse result:%i, expected:1.", reused);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.ProgramName != "other" , "Reused program name:'%.*s', expected:'other'." , (int)args.ProgramName.size(), args.ProgramName.begin());
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Options.size() , "Reused option count:%u, expected:0.", args.Options.size());
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Positionals.size() != 1 , "Reused positional count:%u, expected:1.", args.Positionals.size());
//	if(args.Positionals.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Positionals[0] != "only" , "Reused positional 0:'%.*s', expected:'only'." , (int)args.Positionals[0].size(), args.Positionals[0].begin());
//	}
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Environment.size() != 1 , "Reused environment entry count:%u, expected:1.", args.Environment.size());
//	if(args.Environment.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Environment[0] != "C=D" , "Reused environment entry 0:'%.*s', expected:'C=D'." , (int)args.Environment[0].size(), args.Environment[0].begin());
//	}
//	cnst ::gpk::err_t cleared = ::gpk::argsParse(args, ::gpk::view<::gpk::vcst_t>{});
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, cleared , "Empty reused parse result:%i, expected:0.", cleared);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.ProgramName.size() , "Empty reused program name length:%u, expected:0.", args.ProgramName.size());
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Options.size() , "Empty reused option count:%u, expected:0.", args.Options.size());
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Positionals.size() , "Empty reused positional count:%u, expected:0.", args.Positionals.size());
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_REUSE, args.Environment.size() , "Empty reused environment count:%u, expected:0.", args.Environment.size());
//
//	::gpk::vcst_t emptyArgv[] =
//		{ "app"
//		, "--empty="
//		, "-"
//		, "--flag"
//		};
//	cnst ::gpk::err_t emptyParsed = ::gpk::argsParse(args, emptyArgv);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, emptyParsed != 3 , "Empty-value parse result:%i, expected:3.", emptyParsed);
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Options.size() != 2 , "Empty-value option count:%u, expected:2.", args.Options.size());
//	if(1 <= args.Options.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Options[0].Key != "empty" , "Empty-value option 0 key:'%.*s', expected:'empty'." , (int)args.Options[0].Key.size(), args.Options[0].Key.begin());
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Options[0].Val.size() , "Empty-value option 0 value length:%u, expected:0.", args.Options[0].Val.size());
//	}
//	if(2 <= args.Options.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Options[1].Key != "flag" , "Empty-value option 1 key:'%.*s', expected:'flag'." , (int)args.Options[1].Key.size(), args.Options[1].Key.begin());
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Options[1].Val.size() , "Empty-value option 1 value length:%u, expected:0.", args.Options[1].Val.size());
//	}
//	GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Positionals.size() != 1 , "Empty-value positional count:%u, expected:1.", args.Positionals.size());
//	if(args.Positionals.size()) {
//		GPK_TEST_CHECKF(errors, ARGS_TEST_RESULT_EMPTY_VALUE, args.Positionals[0] != "-" , "Empty-value positional 0:'%.*s', expected:'-'." , (int)args.Positionals[0].size(), args.Positionals[0].begin());
//	}
//	rtrn 0;
//}
