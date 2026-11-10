#include "gpk_file_pack.h"
#include "gpk_array_static.h"
#include "gpk_runtime.h"
#include "gpk_string_compose.h"

sttc gpk::err_t				pakMain			(::gpk::SRuntimeValues & runtimeValues) {
	cnst ::gpk::SCommandLineArgs	& args			= runtimeValues.EntryPointArgs;
	if_zero_vef(-1, args.Positionals.size(), "Usage:"
		"\n- `%.*s <input folder name> [output file name]`"
		"\n- `%.*s -u -- <input file name> [output folder name]`"
		, GPK_VIEW_ARGS(args.ProgramName)
		, GPK_VIEW_ARGS(args.ProgramName)
		);
	::gpk::vcst_c					pathSource		= args.Positionals[0];
	::gpk::vcst_c					pathTarget		= (args.Positionals.size() > 1) ? args.Positionals[1] : ::gpk::vcst_t{};
	::gpk::b8_c						unpak			= 0 <= ::gpk::argsOptionIndex(args, "u");
	if(unpak) {
		if_fail_fe(::gpk::unpak(pathSource, pathTarget));
	}
	else
		if_fail_fe(::gpk::pak(pathSource, pathTarget));

	for(::gpk::u2_t iArg = 2; iArg < args.Positionals.size(); ++iArg)
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));
	return 0;
}

GPK_OS_ENTRY_POINT(::pakMain);
