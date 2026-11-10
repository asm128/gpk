#include "gpk_log.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

using namespace gpk;

static ::gpk::err_t					cutMain						(::gpk::SRuntimeValues & runtimeValues)	{
	const ::gpk::SCommandLineArgs	& args							= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:"
		"\n- `%.*s <file to split>`"
		"\n- `%.*s -j -- <file to join>`"
		, GPK_VIEW_ARGS(args.ProgramName)
		, GPK_VIEW_ARGS(args.ProgramName)
		);
	const ::gpk::vcst_t				fileName							= args.Positionals[0];
	const ::gpk::b8_c					join								= 0 <= ::gpk::argsOptionIndex(args, "j");
	if(join) {
		gpk::err_t							countJoined						;
		if_fail_fef(countJoined = ::gpk::fileJoin(fileName), "Failed to join file. Disk full? File name: %.*s.", GPK_VIEW_ARGS(fileName));
		info_printf("%u parts joined.", countJoined);
	}
	else {
		::gpk::u2_t						countParts						;
		if_fail_fef(countParts = ::gpk::fileSplit(fileName, 10*1024*1024), "Failed to split file: %.*s.", GPK_VIEW_ARGS(fileName));
		info_printf("File split into %u parts.", countParts);
	}
	for(::gpk::u2_t iArg = 1; iArg < args.Positionals.size(); ++iArg)
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));
	return 0;
}

GPK_OS_ENTRY_POINT(::cutMain);
