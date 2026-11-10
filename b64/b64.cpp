#include "gpk_base64.h"
#include "gpk_file.h"
#include "gpk_runtime.h"

sttc gpk::err_t	b64Main(::gpk::SRuntimeValues & runtimeValues) {
	cnst ::gpk::SCommandLineArgs	& args			= runtimeValues.EntryPointArgs;
	if_true_vef(-1, 1 > args.Positionals.size(), "Usage:"
		"\n- `%.*s -f -- <file to encode>  [<output filename>]` "
		"\n- `%.*s <text to encode> [<output filename>]`"
		, GPK_VIEW_ARGS(args.ProgramName)
		, GPK_VIEW_ARGS(args.ProgramName)
		);
	::gpk::vcsc_t		source			= args.Positionals[0];
	::gpk::vcst_t		filename		= (args.Positionals.size() < 2) ? ::gpk::vcst_t{} : args.Positionals[1];
	::gpk::b8_c			fromFile		= 0 <= ::gpk::argsOptionIndex(args, "f");
	::gpk::b8_c			decode			= 0 <= ::gpk::argsOptionIndex(args, "d");
	::gpk::as0_t		bytes;
	if(fromFile) {
		if_fail_fe(::gpk::fileToMemory(source, bytes));
		source		= bytes;
	}
	//
	::gpk::string		output			= {};
	if_fail_fef(::gpk::base64(source, output, decode), "source=\"\"\"\n%.*s\n\"\"\" decode=%s.", GPK_VIEW_ARGS(source), ::gpk::b82sc(decode));
	if_fail_fe(::gpk::log_print(output));
	//
	if(filename.size()) {
		if_fail_fe(::gpk::fileFromMemory(filename, output));
	}
	return 0;
}

GPK_OS_ENTRY_POINT(::b64Main);
