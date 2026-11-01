#include "gpk_string.h"
#include "gpk_array_obj.h"
#include "gpk_keyval.h"

#ifndef GPK_ARGS_H_23627
#define GPK_ARGS_H_23627

namespace gpk
{
	stct SCommandLineArgs {
		vcst_t					ProgramName = {};
		aobj<keyval<vcst_t>>	Options		= {};
		aobj<vcst_t>			Positionals	= {};
		aobj<kv<vcst_t>>		Environment	= {};
	};

	err_t			argsParse					(SCommandLineArgs & output, view<cnst vcst_t> argv, view<cnst kv<vcst_t>> envp = {});
	err_t			argsParse					(SCommandLineArgs & output, int argc, char ** argv, char ** envp = 0);
	err_t			argsOptionValue				(cnst SCommandLineArgs & input, vcst_t key, vcst_t & output);
	err_t			argsOptionValues			(cnst SCommandLineArgs & input, vcst_t key, aobj<vcst_t> & output);
	stin	err_t	argsOptionIndex				(cnst SCommandLineArgs & input, vcst_t key, u2_t offset = 0)	{ rtrn input.Options.find([&key](cnst kvvcst_t<vcst_t> & option) { rtrn option.Key == key; }, offset); }
	err_t			viewsFromEnvp				(aobj<kv<vcst_t>> & outputViews, char * envp[]);
	err_t			viewsFromArgv				(aobj<vcst_t> & outputViews, u2_t argc, char * argv[]);
	err_t			environmentBlockViews		(vcs0_t environmentBlock, aobj<kv<vcst_t>> & out_Views);
	err_t			environmentBlock			(as0_t & environmentBlock);
	err_t			environmentBlock			(as0_t & environmentBlock, view<cnst kv<vcst_t>> inputEnvp = {});
}

#endif // GPK_ARGS_H_23627
