#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_file.h"

// Splits a file into file.split.## parts.
int									main							(int argc, char ** argv)		{
	if_true_fef(2 > argc, "Usage:\n\t%s [filename]", argv[0]);
	if_true_fef(65535 < argc, "Invalid parameter count: %u.", (::gpk::u2_t)argc);
	::gpk::vcsc_c							fileNameSrc						= {argv[1], (::gpk::u2_t)-1};	// First parameter is the only parameter, which is the name of the source file to be split.
	::gpk::u2_c								countParts						= ::gpk::fileSplit(fileNameSrc, 10*1024*1024);
	if_true_fef(gpk::failed(countParts), "Failed to split file: %s.", fileNameSrc.begin());
	info_printf("File split into %u parts.", countParts);
	for(::gpk::s2_t iArg = 2; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	return 0;
}
