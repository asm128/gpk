#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_log.h"
#include "gpk_file.h"

// Joins a file split into file.split.## parts.
int									main							(int argc, char ** argv)				{
	if_true_vef(-1, 2 > argc, "Usage:\n\t%s [filename]", argv[0]);
	if_true_vef(-1, 65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);
	const ::gpk::view<sc_t>						fileNameDst						= {argv[1], (uint32_t)-1};	// First parameter is the only parameter, which is the name of the original file that has been split.
	::gpk::err_t							countJoined						= ::gpk::fileJoin(fileNameDst);
	if_true_vef(-1, ::gpk::failed(countJoined), "Failed to join file. Disk full? File name: %s.", fileNameDst.begin());
	info_printf("%u parts joined.", countJoined);
	for(int32_t iArg = 2; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	return 0;
}
