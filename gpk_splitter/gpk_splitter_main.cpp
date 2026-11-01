#include "gpk_log.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

using namespace gpk;

// Splits a file into file.split.## parts.
static ::gpk::err_t					splitterMain					(::gpk::SRuntimeValues & runtimeValues)	{
	const ::gpk::SCommandLineArgs	& args							= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:\n\t%.*s [filename]", GPK_VIEW_ARGS(args.ProgramName));
	::gpk::vcst_t							fileNameSrc						= args.Positionals[0];	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::u2_t								countParts						;
	if_fail_fef(countParts = ::gpk::fileSplit(fileNameSrc, 10*1024*1024), "Failed to split file: %.*s.", GPK_VIEW_ARGS(fileNameSrc));
	info_printf("File split into %u parts.", countParts);
	for(::gpk::u2_t iArg = 1; iArg < args.Positionals.size(); ++iArg) {
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));
	}
	return 0;
}

GPK_OS_ENTRY_POINT(::splitterMain);
