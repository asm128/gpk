#include "gpk_log.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

using namespace gpk;

// Joins a file split into file.split.## parts.
static ::gpk::err_t					joinerMain						(::gpk::SRuntimeValues & runtimeValues)	{
	const ::gpk::SCommandLineArgs	& args							= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:\n\t%.*s [filename]", GPK_VIEW_ARGS(args.ProgramName));
	const ::gpk::vcst_t				fileNameDst						= args.Positionals[0];	// First parameter is the only parameter, which is the name of the original file that has been split.
	gpk::err_t 						countJoined						;
	if_fail_fef(countJoined = ::gpk::fileJoin(fileNameDst), "Failed to join file. Disk full? File name: %.*s.", GPK_VIEW_ARGS(fileNameDst));
	info_printf("%u parts joined.", countJoined);
	for(::gpk::u2_t iArg = 1; iArg < args.Positionals.size(); ++iArg)
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));
	return 0;
}

GPK_OS_ENTRY_POINT(::joinerMain);
