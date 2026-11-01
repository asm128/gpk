#include "gpk_file_pack.h"

int								main							(int argc, char ** argv)						{
	if_true_vef(-1, 2 > argc, "Usage:\n\t%s [input file name] [output folder (optional)]", argv[0]);
	if_true_vef(-1, 65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);
	::gpk::apod<char>					namePathDst						= {};
	::gpk::vcst_t							nameFileSrc						= {argv[0], (uint32_t)-1};
	if(2 < argc) {
		namePathDst						= {argv[2], (uint32_t)-1};
	}
	else if(2 == argc) {
		namePathDst						= {argv[1], (uint32_t)-1};
		::gpk::err_t						indexSequence					= ::gpk::rfind('.', nameFileSrc);
		if(-1 != indexSequence) {
			namePathDst[indexSequence]		= '\\';
			namePathDst[indexSequence + 1]	= 0;
		}
	}
	if_fail_fef(gpk::folderUnpackToDisk(namePathDst, nameFileSrc), "Failed to unpack file '%.*s' to folder '%.*s'.", GPK_VIEW_ARGS(nameFileSrc), GPK_VIEW_ARGS(namePathDst));
	for(int32_t iArg = 3; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);

	return 0;
}

