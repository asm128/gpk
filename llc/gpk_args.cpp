#include "gpk_args.h"
#include "gpk_keyval_view.h"

GPK_USING_TYPEINT();

gpk::err_t		gpk::environmentBlock		(as0_t & output)	{
	aobj<kv<vcst_t>>	venv						= {};
	if_fail_fe(viewsFromEnvp(venv, environ));
	return environmentBlock(output, venv);
}
gpk::err_t		gpk::environmentBlock		(as0_t & output, view<cnst kv<vcst_t>> inputEnvp)	{
	for(cnst kv<vcst_t> & venvValue : inputEnvp) {
		if_fail_fe(output.append(venvValue.Key));
		if_fail_fe(output.push_back('='));
		if_fail_fe(output.append(venvValue.Val));
		if_fail_fe(output.push_back(0));
	}
	if_fail_fe(output.push_back(0));
	return 0;
}
gpk::err_t		gpk::environmentBlockViews	(vcs0_t environmentBlock, aobj<kv<vcst_t>> & out_Views)	{
	aobj<kv<vcst_t>>	views						= {};
	for(u2_t offsetBeginVar = 0; offsetBeginVar < environmentBlock.size(); ) {
		cnst err_t			offsetEndVar					= find('\0', environmentBlock, offsetBeginVar);
		if_true_fef(0 > offsetEndVar, "Unterminated environment variable at offset:%u.", offsetBeginVar);
		if((u2_t)offsetEndVar == offsetBeginVar)
			break;
		vcst_t					viewEnvironVar					= {};
		if_fail_fe(environmentBlock.slice(viewEnvironVar, offsetBeginVar, offsetEndVar - offsetBeginVar));
		keyval<vcst_t>	newKeyVal						= {};
		if(-1 != keyval_split(viewEnvironVar, newKeyVal)) {
			if_fail_fe(views.push_back(newKeyVal));
		}
		else {
			newKeyVal.Key			= {viewEnvironVar.begin(), viewEnvironVar.size()};
			if_fail_fe(views.push_back(newKeyVal));
		}
		offsetBeginVar				= offsetEndVar + 1;
	}
	if_fail_fe(out_Views.append(views));
	return 0;
}
gpk::err_t		gpk::viewsFromEnvp		(aobj<kv<vcst_t>> & outputViews, char * envp[]) {
	if_null_fe(envp);
	u2_t				iVar					= 0;
	for(; envp[iVar]; ++iVar) {
		const vcst_t		environmentEntry		= {envp[iVar], (u2_t)-1};
		kv<vcst_t>			keyVal					= {};
		if(-1 == keyval_split(environmentEntry, keyVal))
			keyVal.Key								= environmentEntry;
		if_fail_fe(outputViews.push_back(keyVal));
	}
	rtrn iVar;
}
gpk::err_t		gpk::viewsFromArgv		(aobj<vcst_t> & outputViews, u2_t argc, char * argv[]) {
	if_zero_vi(0, argc);
	if_null_fe(argv);
	u2_t				iArg					= 0;
	for(; iArg < argc; ++iArg) {
		if_fail_fe(outputViews.push_back({argv[iArg], (u2_t)-1}));
	}
	rtrn iArg;
}
gpk::err_t		gpk::argsOptionValue	(const SCommandLineArgs & input, vcst_t key, vcst_t & output)	{
	//output			= {};
	err_c				optionIndex				= argsOptionIndex(input, key);
	if_fail_fwf(optionIndex, "Option not found:'%.*s'.", (int)key.size(), key.begin());
	if_true_vef(-2, 0 <= argsOptionIndex(input, key, (u2_t)optionIndex + 1), "Option has multiple values:'%.*s'.", (int)key.size(), key.begin());
	output			= input.Options[optionIndex].Val;
	rtrn optionIndex;
}

gpk::err_t		gpk::argsOptionValues	(const SCommandLineArgs & input, vcst_t key, aobj<vcst_t> & output)	{
	u2_c				outputStart				= output.size();
	for(const kvvcst_t<vcst_t> & option : input.Options)
		if(option.Key == key)
			if_fail_fe(output.push_back(option.Val));
	rtrn output.size() - outputStart;
}

sttc gpk::err_t	argsOptionName			(::gpk::vcst_c & argument, ::gpk::kvvcst_t<::gpk::vcst_t> & option) {
	if_zero_fw(argument.size());
	b8_c				isDoubleDash			= argument.size() > 1 && '-' == argument[1];
	u0_c				prefixLen				= isDoubleDash ? 2U : 1U;
	u2_t				iChar					= prefixLen;
	for(; iChar < argument.size() && argument[iChar] != '='; ++iChar) 
		continue;

	if_fail_fe(argument.slice(option.Key, prefixLen, iChar - prefixLen));
	if_zero_fef(option.Key.size(), "Option has no name:'%.*s'.", (int)argument.size(), argument.begin());

	b8_c				hasValue				= iChar < argument.size(); // If we found an '=' character, then the option has a value.
	if(hasValue) {
		if_fail_fe(argument.slice(option.Val, iChar + 1));
	}
	rtrn hasValue;
}


gpk::err_t		gpk::argsParse			(SCommandLineArgs & output, int argc, char ** argv, char ** envp) {
	aobj<vcst_t>		arguments;
	aobj<kv<vcst_t>>	envvars;											
	if_fail_fe(viewsFromArgv(arguments, argc, argv));
	if(envp) {
		if_fail_fe(viewsFromEnvp(envvars, envp));
	}
	return argsParse(output, arguments, envvars);
}

gpk::err_t		gpk::argsParse			(SCommandLineArgs & output, view<cnst vcst_t> argv, view<cnst kv<vcst_t>> envp) {
	output				= {};
	output.Environment	= envp;
	if_zero_vw(0, argv.size()); // Exit early if no argv: nothing to do

	output.ProgramName				= argv[0];

	kvvcst_t<vcst_t>		option				= {};
	bool					hasOptionValues		= false;
	bool					positionalOnly		= false;
	for(u2_t iArg = 1; iArg < argv.size(); ++iArg) {
		vcst_t					argument			= argv[iArg];
		if(positionalOnly) {
			if_fail_fe(output.Positionals.push_back(argument));
			continue;
		}
		if(argument == "--") {
			if(option.Key.size() && false == hasOptionValues) {
				if_fail_fe(output.Options.push_back(option));
			}
			option				= {};
			positionalOnly		= true;
			continue;
		}
		if(1 >= argument.size() || '-' != argument[0]) {
			if(0 == option.Key.size()) {
				if_fail_fe(output.Positionals.push_back(argument));
			}
			else {
				if_fail_fe(output.Options.emplace_back(option.Key, argument));
				hasOptionValues	= true;
			}
			continue;
		}
		if(option.Key.size() && false == hasOptionValues) {
			if_fail_fe(output.Options.push_back(option));
		}
		option				= {};
		hasOptionValues		= false;
		err_t					hasValue			= {};
		if_fail_fe(hasValue = argsOptionName(argument, option));
		if(hasValue) {
			if_fail_fe(output.Options.push_back(option));
			option				= {};
		}
	}
	if(option.Key.size() && false == hasOptionValues)
		if_fail_fe(output.Options.push_back(option));
	rtrn output.Options.size() + output.Positionals.size();
}
