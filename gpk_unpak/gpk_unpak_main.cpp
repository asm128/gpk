#include "gpk_file_pack.h"
#include "gpk_runtime.h"

static ::gpk::err_t				unpakMain						(::gpk::SRuntimeValues & runtimeValues)	{
	const ::gpk::SCommandLineArgs	& args							= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:\n\t%.*s [input file name] [output folder (optional)]", GPK_VIEW_ARGS(args.ProgramName));
	::gpk::apod<char>					namePathDst						= {};
	::gpk::vcst_t							nameFileSrc						= args.ProgramName;
	if(1 < args.Positionals.size()) {
		namePathDst						= args.Positionals[1];
	}
	else if(1 == args.Positionals.size()) {
		namePathDst						= args.Positionals[0];
		::gpk::err_t						indexSequence					= ::gpk::rfind('.', nameFileSrc);
		if(-1 != indexSequence) {
			namePathDst[indexSequence]		= '\\';
			namePathDst[indexSequence + 1]	= 0;
		}
	}
	if_fail_fef(gpk::folderUnpackToDisk(namePathDst, nameFileSrc), "Failed to unpack file '%.*s' to folder '%.*s'.", GPK_VIEW_ARGS(nameFileSrc), GPK_VIEW_ARGS(namePathDst));
	for(::gpk::u2_t iArg = 2; iArg < args.Positionals.size(); ++iArg)
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));

	return 0;
}

GPK_OS_ENTRY_POINT(::unpakMain);

