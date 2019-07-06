#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_deflate.h"

int										main						(int argc, char ** argv)	{
	ree_if(2 > argc, "Usage:\n\t%s [input folder name] [output file name (optional)]", argv[0]);
	ree_if(65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);

	char										nameFileDst[1024*2]			= {};	
	if(2 < argc)	// compose filename from folder name or just use the second argument.
		gpk_necall(sprintf_s(nameFileDst, "%s", argv[2]), "%s", "Buffer overflow.");
	else
		gpk_necall(sprintf_s(nameFileDst, "%s.gpk", argv[1]), "%s", "Buffer overflow.");

	const ::gpk::view_const_string				nameFolderSrc				= {argv[1], (uint32_t)strlen(argv[1])};;
	gpk_necall(::gpk::folderPackToDisk(nameFileDst, nameFolderSrc), "Failed to pack folder '%s' to file '%s'.", nameFolderSrc.begin(), nameFileDst);
	info_printf("Successfully written '%s'?", nameFileDst);
	for(int32_t iArg = 3; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	return 0; 
}
