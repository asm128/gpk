#include "gpk_base64.h"
#include "gpk_file.h"

stxp	gpk::vcst_c		sign	= "MjAwOS0yMDE5IChjKSBUaGUgRHJhZ29u";

int main(int argc, char** argv) {
	if_true_vef(-1, 2 > argc, "usage: %s [filename]", argv[0]);
	::gpk::vcst_t		in_filename		= {argv[1], (uint32_t)-1};
	::gpk::au0_t		in_data			= {};
	::gpk::au0_t		out_b64			= {};
	if_fail_fe(gpk::fileToMemory(in_filename, in_data));
	if_fail_fef(gpk::base64Encode(in_data, out_b64), "Failed to encode file contents! %.*s.", GPK_VIEW_ARGS(in_filename));

	::gpk::string		out_filename	= in_filename;
	out_filename.append(".b64");
	if_fail_fe(gpk::fileFromMemory(out_filename, out_b64));
	return 0;
}
