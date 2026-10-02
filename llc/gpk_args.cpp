#include "gpk_args.h"
#include "gpk_enum.h"

gpk::err_t			gpk::viewsFromEnvp(gpk::aobj<vcst_t> & outputViews, char * envp[]) {
	if_null_fe(envp);
	u2_t iVar = 0;
	for (; envp[iVar]; ++iVar) {
		if_fail_fe(outputViews.push_back({envp[iVar], (u2_t)-1}));
	}
	rtrn iVar;
}

gpk::err_t			gpk::viewsFromArgv(gpk::aobj<vcst_t> & outputViews, u2_t argc, char * argv[]) {
	if_zero_vi(0, argc);
	if_null_fe(argv);
	u2_t iArg = 0;
	for(; iArg < argc; ++iArg) {
		if_fail_fe(outputViews.push_back({argv[iArg], (u2_t)-1}));
	}
	rtrn iArg;
}
gpk::err_t			gpk::argsOptionValue	(const SCommandLineArgs & input, vcst_t key, vcst_t & output)	{
	err_c 					optionIndex			= argsOptionIndex(input, key);
	if(0 <= optionIndex) { // success!
		output = input.Options[optionIndex].Val;
		return optionIndex;
	}
	gpk::string				possibleKeys		= {};
	for(u2_t iKey = 0; iKey < input.Options.size(); ++iKey) {
		if_fail_fe(gpk::append_strings(possibleKeys, iKey ? str(", ") : str(""), '"', input.Options[iKey].Key, '"'));
	}
	warning_printf("{%s} contains no [\"%.*s\"]", possibleKeys.begin(), key.size(), key.begin());
	return -1;
}

namespace gpk 
{
	GDEFINE_ENUM_TYPE(ARGS_STATE, gpk::u0_t);
	GDEFINE_ENUM_VALUE(ARGS_STATE, ARGUMENT		, 0);
	GDEFINE_ENUM_VALUE(ARGS_STATE, OPTION_VALUE	, 1);
	GDEFINE_ENUM_VALUE(ARGS_STATE, POSITIONAL	, 2);
} // namespace

sttc ::gpk::err_t	argsOptionName			(::gpk::vcst_c & argument, ::gpk::kvvcst_t<::gpk::vcst_t> & option) {
	if_zero_fw(argument.size());
	::gpk::b8_c				isDoubleDash		= argument.size() > 1 && '-' == argument[1];
	::gpk::u0_c				prefixLen			= isDoubleDash ? 2U : 1U;
	::gpk::u2_t				iChar				= prefixLen;
	for(; iChar < argument.size() && argument[iChar] != '='; ++iChar) 
		continue;

	option.Key			= {&argument[prefixLen], iChar - prefixLen};

	gpk::b8_c				hasValue			= iChar < argument.size(); // If we found an '=' character, then the option has a value.
	if(not hasValue) 
		return 0;

	option.Val			=  {&argument[iChar + 1], argument.size() - iChar - 1};
	rtrn option.Val.size();
}


::gpk::err_t		gpk::argsParse			(SCommandLineArgs & output, int argc, char ** argv, char ** envp) {
	gpk::aobj<gpk::vcst_t> arguments, envvars;											
	if_fail_fe(viewsFromArgv(arguments, argc, argv));
	if(envp)
		if_fail_fe(viewsFromEnvp(envvars, envp));
	return argsParse(output, arguments, envvars);
}

::gpk::err_t		gpk::argsParse		(::gpk::SCommandLineArgs & output, ::gpk::view<vcst_t> argv, ::gpk::view<vcst_t> envp) {
	output.Environment	= envp;
	if_zero_vw(0, argv.size()); // Exit early if no argv: nothing to do

	output.ProgramName	= argv[0];

	ARGS_STATE				state				= ARGS_STATE_ARGUMENT;
	kvvcst_t<vcst_t>		option				= {};
	for(u2_t iArg = 1; iArg < argv.size(); ++iArg) {
		vcst_t					argument			= {argv[iArg], (u2_t)-1};
		if_zero_cwf(argument.size(), "iArg:(%" GPK_FMT_U2  ")", iArg);
		if(state != ARGS_STATE_ARGUMENT) {
				 if(state == ARGS_STATE_POSITIONAL)		{ if_fail_fe(output.Positionals.push_back(argument)); }
			else if(state != ARGS_STATE_OPTION_VALUE)	{ warning_printf("Unrecognized state! 0x%X(%s)", (u2_t)state, gpk::get_value_namep(state)); }
			else { // state == ARGS_STATE_OPTION_VALUE, obviously
				option.Val			= argument; // The value of the current option is the next argument
				if_fail_fe(output.Options.push_back(option));
				state				= ARGS_STATE_ARGUMENT;
			}
			continue;
		}
		if(argument[0] != '-') {
			if_fail_fe(output.Positionals.push_back(argument));
			continue;
		}
		if(argument == vcsc_t{"--", 2}) {
			state				= ARGS_STATE_POSITIONAL;
			continue;
		}
		err_t				hasValue;
		if_fail_fe(hasValue = argsOptionName(argument, option));
		if(not hasValue) 
			state				= ARGS_STATE_OPTION_VALUE;
		else {
			if_fail_fe(output.Options.push_back(option));
			option				= {};
		}
	}
	if(state == ARGS_STATE_OPTION_VALUE) {
		if_fail_fe(output.Options.push_back(option));
	}
	rtrn output.Options.size() + output.Positionals.size();
}
