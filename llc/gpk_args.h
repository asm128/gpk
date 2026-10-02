#include "gpk_array_obj.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"
#include "gpk_keyval.h"

#ifndef GPK_ARGS_H_23627
#define GPK_ARGS_H_23627

namespace gpk
{
	stct SCommandLineArgs {
		vcst_t					ProgramName = {};
		aobj<keyval<vcst_t>>	Options		= {};
		aobj<vcst_t>			Positionals	= {};
		aobj<vcst_t>			Environment	= {};
	};

	err_t			argsParse			(SCommandLineArgs & output, view<vcst_t> argv, view<vcst_t> envp = {});
	err_t			argsParse			(SCommandLineArgs & output, int argc, char ** argv, char ** envp = 0);
	err_t			argsOptionValue		(const SCommandLineArgs & input, vcst_t key, vcst_t & output);
	stin	err_t	argsOptionIndex		(const SCommandLineArgs & input, vcst_t key)	{ 
		return input.Options.find([&key](const kvvcst_t<vcst_t> & option) { 
			return option.Key == key; 
		}); 
	}
	err_t			viewsFromEnvp		(aobj<vcst_t> & outputViews, char * envp[]);
	err_t			viewsFromArgv		(aobj<vcst_t> & outputViews, u2_t argc, char * argv[]);
}

#endif // GPK_ARGS_H_23627
