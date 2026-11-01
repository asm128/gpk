#include "gpk_base64.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

stxp	const char	sign	[]	= "MjAwOS0yMDE5IChjKSBUaGUgRHJhZ29u";

static ::gpk::err_t b64DecodeMain(::gpk::SRuntimeValues & runtimeValues) {
	const ::gpk::SCommandLineArgs	& args			= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "usage: %.*s [filename]", GPK_VIEW_ARGS(args.ProgramName));
	::gpk::vcst_t				in_filename		= args.Positionals[0];

	::gpk::au0_t				in_b64			= {};
	::gpk::au0_t				out_data		= {};
	if_fail_fef(gpk::fileToMemory(in_filename, in_b64), "Failed to load file: %s", in_filename.begin());
	if_fail_fef(gpk::base64Decode(in_b64, out_data), "Failed to encode file contents! %s.", "Out of memory?");

	::gpk::apod<char>		out_filename	= in_filename;
	out_filename.append(".b64.decoded");
	if_fail_fef(gpk::fileFromMemory({out_filename.begin(), out_filename.size()}, out_data), "Failed to save file: %s", out_filename.begin());
	return 0;
}

GPK_OS_ENTRY_POINT(::b64DecodeMain);
