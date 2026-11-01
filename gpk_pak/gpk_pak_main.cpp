#include "gpk_deflate.h"
#include "gpk_file_pack.h"

using namespace gpk;

int										main						(int argc, char ** argv)	{
	if_true_vef(-1, 2 > argc, "Usage:\n\t%s [input folder name] [output file name (optional)]", argv[0]);
	if_true_vef(-1, 65535 < argc, "Invalid parameter count: %u.", (uint32_t)argc);

	char	nameFileDst[1024*2]			= {};
	if(2 < argc) {	// compose filename from folder name or just use the second argument.
		if_fail_fe(::gpk::sprintf_s(nameFileDst, "%s", argv[2]));
	}
	else
		if_fail_fe(::gpk::sprintf_s(nameFileDst, "%s.gpk", argv[1]));

	::gpk::vcst_c				nameFolderSrc				= {argv[1], (uint32_t)strlen(argv[1])};;
	if_fail_fef(gpk::folderPackToDisk(vcst_t{nameFileDst}, nameFolderSrc), "Failed to pack folder '%.*s' to file '%.*s'.", GPK_VIEW_ARGS(nameFolderSrc), GPK_VIEW_ARGS(nameFileDst));
	info_printf("Successfully written '%s'?", nameFileDst);
	for(int32_t iArg = 3; iArg < argc; ++iArg)
		info_printf("Unknown parameter: %s.", argv[iArg]);
	return 0;
}
