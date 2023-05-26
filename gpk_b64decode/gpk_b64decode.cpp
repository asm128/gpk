#define GPK_STDOUT_LOG_ENABLED

#include "gpk_base64.h"
#include "gpk_file.h"

static constexpr const char sign []= "MjAwOS0yMDE5IChjKSBUaGUgRHJhZ29u";

int main(int argc, char** argv) {
	ree_if(2 > argc, "usage: %s [filename]", argv[0]);
	::gpk::vcs				in_filename		= {argv[1], (uint32_t)-1};

	::gpk::au8				in_b64			= {};
	::gpk::au8				out_data		= {};
	gpk_necall(::gpk::fileToMemory(in_filename, in_b64), "Failed to load file: %s", in_filename.begin());
	gpk_necall(::gpk::base64Decode(in_b64, out_data), "Failed to encode file contents! %s.", "Out of memory?");

	::gpk::apod<char>		out_filename	= in_filename;
	out_filename.append(".b64.decoded");
	gpk_necall(::gpk::fileFromMemory({out_filename.begin(), out_filename.size()}, out_data), "Failed to save file: %s", out_filename.begin());
	return 0;
}
