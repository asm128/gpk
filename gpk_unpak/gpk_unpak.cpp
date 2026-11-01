#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_deflate.h"

int								main							(int argc, char ** argv)						{
	if_true_vef(-1, 2 > argc, "Usage:\n\t%s [input file name] [output folder (optional)]", argv[0]);
	if_true_vef(-1, 65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);
	::gpk::apod<char>					namePathDst						= {};
	::gpk::vcst_t							nameFileSrc						= {argv[0], (uint32_t)-1};
	if(2 < argc)
		namePathDst						= ::gpk::toString({argv[2], (uint32_t)-1});
	else if(2 == argc) {
		namePathDst						= ::gpk::toString({argv[1], (uint32_t)-1});
		::gpk::error_t						indexSequence					= ::gpk::rfind('.', nameFileSrc);
		if(-1 != indexSequence) {
			namePathDst[indexSequence]		= '\\';
			namePathDst[indexSequence + 1]	= 0;
		}
	}
	if_fail_fef(gpk::folderUnpackToDisk({namePathDst.begin(), namePathDst.size()}, nameFileSrc), "Failed to unpack file '%s' to folder '%s'.", ::gpk::toString(nameFileSrc).begin(), namePathDst.begin());
	for(int32_t iArg = 3; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);

	return 0;
}

