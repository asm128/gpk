#include "gpk_storage.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#endif

		::gpk::error_t														gpk::pathList						(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::label>& output)					{
	for(uint32_t iFile		= 0; iFile		< input.Files	.size(); ++iFile) 
		gpk_necall(output.push_back(input.Files[iFile]), "%s", "Out of memory?");
	for(uint32_t iFolder	= 0; iFolder	< input.Folders	.size(); ++iFolder) 
		gpk_necall(::gpk::pathList(input.Folders[iFolder], output), "%s", "Unknown error!");
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::label & pathToList, ::gpk::array_obj<::gpk::label>& output, bool listFolders)	{
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA																fdFile								= {};
	HANDLE																			hFind								= NULL;
	char																			sPath	[4096];
	gpk_necall(sprintf_s(sPath, "%s\\*.*", pathToList.begin()), "%s", "Path too long?");
	hFind																		= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", pathToList.begin());
	do if(	0 != strcmp(fdFile.cFileName,  ".")
		 &&	0 != strcmp(fdFile.cFileName, "..")
		) {
		int32_t																			lenPath							= sprintf_s(sPath, "%s\\%s", pathToList.begin(), fdFile.cFileName);
		if((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && false == listFolders) 
			continue;
		info_printf("Path: %s.", sPath);
		gpk_necall(output.push_back({sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#endif
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::label & pathToList, ::gpk::SPathContents& pathContents)						{
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA																fdFile								= {};
	HANDLE																			hFind								= NULL;
	char																			sPath[4096];
	gpk_necall(sprintf_s(sPath, "%s\\*.*", pathToList.begin()), "%s", "Path too long?");
	hFind																		= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", pathToList.begin());
	do if(	0 != strcmp(fdFile.cFileName,  ".")	
		 &&	0 != strcmp(fdFile.cFileName, "..")
		) {
		int32_t																			lenPath								= sprintf_s(sPath, "%s\\%s", pathToList.begin(), fdFile.cFileName);
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			::gpk::error_t																	newFolderIndex						= pathContents.Folders.push_back({});
			gpk_necall(newFolderIndex, "%s", "Out of memory?");
			gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex]), "%s", "Unkown error!");
			info_printf("Directory: %s.", sPath);
		}
		else {
			gpk_necall(pathContents.Files.push_back({sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			info_printf("File: %s.", sPath);
		}
	}
	while(FindNextFile(hFind, &fdFile)); 
	FindClose(hFind);
#endif
    return 0;
}
