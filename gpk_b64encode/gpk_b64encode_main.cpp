#include "gpk_base64.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

stxp	gpk::vcst_c		sign	= "MjAwOS0yMDE5IChjKSBUaGUgRHJhZ29u";

static ::gpk::err_t b64EncodeMain(::gpk::SRuntimeValues & runtimeValues) {
	const ::gpk::SCommandLineArgs	& args			= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "usage: %.*s [filename]", GPK_VIEW_ARGS(args.ProgramName));
	::gpk::vcst_t		in_filename		= args.Positionals[0];
	::gpk::au0_t		in_data			= {};
	::gpk::au0_t		out_b64			= {};
	if_fail_fe(gpk::fileToMemory(in_filename, in_data));
	if_fail_fef(gpk::base64Encode(in_data, out_b64), "Failed to encode file contents! %.*s.", GPK_VIEW_ARGS(in_filename));

	::gpk::string		out_filename	= in_filename;
	out_filename.append(".b64");
	if_fail_fe(gpk::fileFromMemory(out_filename, out_b64));
	return 0;
}

GPK_OS_ENTRY_POINT(::b64EncodeMain);
