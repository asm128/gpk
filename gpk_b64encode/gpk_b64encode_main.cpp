#include "gpk_base64.h"
#include "gpk_file.h"

stxp	const char	sign []	= "MjAwOS0yMDE5IChjKSBUaGUgRHJhZ29u";

int main(int argc, char** argv) {
	if_true_vef(-1, 2 > argc, "usage: %s [filename]", argv[0]);
	::gpk::vcst_t				in_filename		= {argv[1], (uint32_t)-1};

	::gpk::au0_t				in_data			= {};
	::gpk::au0_t				out_b64			= {};
	if_fail_fef(gpk::fileToMemory(in_filename, in_data), "Failed to load file: %s", in_filename.begin());
	if_fail_fef(gpk::base64Encode(in_data, out_b64), "Failed to encode file contents! %s.", "Out of memory?");

	::gpk::apod<char>		out_filename	= in_filename;
	out_filename.append(".b64");
	if_fail_fef(gpk::fileFromMemory({out_filename.begin(), out_filename.size()}, out_b64), "Failed to save file: %s", out_filename.begin());
	return 0;
}
