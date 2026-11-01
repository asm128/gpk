#include "gpk_file_pack.h"
#include "gpk_array_static.h"
#include "gpk_runtime.h"

using namespace gpk;

static ::gpk::err_t						pakMain						(::gpk::SRuntimeValues & runtimeValues)	{
	const ::gpk::SCommandLineArgs	& args						= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:\n\t%.*s [input folder name] [output file name (optional)]", GPK_VIEW_ARGS(args.ProgramName));

	astatic<sc_t, 1024 * 2>	s_nameFileDst = {};
	if(1 < args.Positionals.size()) {	// compose filename from folder name or just use the second argument.
		if_fail_fe(::gpk::sprintf_s(s_nameFileDst.Storage, "%.*s", GPK_VIEW_ARGS(args.Positionals[1])));
	}
	else
		if_fail_fe(::gpk::sprintf_s(s_nameFileDst.Storage, "%.*s.gpk", GPK_VIEW_ARGS(args.Positionals[0])));
	vcst_c						nameFileDst		= s_nameFileDst.Storage;
	vcst_c						nameFolderSrc	= args.Positionals[0];
	if_fail_fef(gpk::folderPackToDisk(nameFileDst, nameFolderSrc), "Failed to pack folder '%.*s' to file '%.*s'.", GPK_VIEW_ARGS(nameFolderSrc), GPK_VIEW_ARGS(nameFileDst));
	info_printf("Successfully written '%s'?", nameFileDst);
	for(::gpk::u2_t iArg = 2; iArg < args.Positionals.size(); ++iArg)
		info_printf("Unknown parameter: %.*s.", GPK_VIEW_ARGS(args.Positionals[iArg]));
	return 0;
}

GPK_OS_ENTRY_POINT(::pakMain);
