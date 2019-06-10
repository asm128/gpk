#include "gpk_storage.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <dirent.h>
#	include <string>
#	include <iostream>
#endif

::gpk::error_t						gpk::pathCreate				(const view_const_string & pathName) {
	char									folder[1024]				= {};
	const char								* end						= strchr(pathName.begin(), '\\');
	const char								* path						= pathName.begin();
	while(0 != end) {
		strncpy_s(folder, path, end - path + 1);
		if(!CreateDirectoryA(folder, NULL)) {
			DWORD								err							= GetLastError();
			ree_if(err != ERROR_ALREADY_EXISTS, "Failed to create directory: %s. hr: (%u)", folder, err);
		}
		end									= strchr(++end, L'\\');
	}
	return 0;
}

// Splits a file into file.split.## parts.
::gpk::error_t						gpk::fileSplit					(const ::gpk::view_const_string	& fileNameSrc, const uint32_t sizePartMax) {
	::gpk::array_pod<byte_t>				fileInMemory;
	gpk_necall(::gpk::fileToMemory(fileNameSrc, fileInMemory), "Failed to load file: \"%s\".", fileNameSrc);

	// -- Write parts to disk.
	uint32_t								countParts						= fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	char									fileNameDst	[1024]				= {};
	uint32_t								iPart							= 0;
	for(; iPart < countParts; ++iPart) {
		const uint32_t							offsetPart						= sizePartMax * iPart;
		sprintf_s(fileNameDst, "%s.split.%.2u", fileNameSrc.begin(), iPart);
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE									* fpDest						= 0;
		fopen_s(&fpDest, fileNameDst, "wb");
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		uint32_t								countBytes						= (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax;
		ree_if(countBytes != fwrite(&fileInMemory[offsetPart], 1, countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// Joins a file split into file.split.## parts.
::gpk::error_t						gpk::fileJoin					(const ::gpk::view_const_string	& fileNameDst)	{
	char									fileNameSrc	[1024]				= {};
	
	// Load each .split part and write it to the destionation file.
	uint32_t								iFile							= 0;
	sprintf_s(fileNameSrc, "%s.split.%.2u", fileNameDst.begin(), iFile++);
	FILE									* fpDest						= 0;
	fopen_s(&fpDest, fileNameDst.begin(), "wb");
	ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst.begin());
	::gpk::array_pod<byte_t>					fileInMemory					= {};
	while(0 == ::gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file. 
		ree_if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %u. File name: %s.", fileInMemory.size(), fileNameSrc);
		sprintf_s(fileNameSrc, "%s.split.%.2u", fileNameDst.begin(), iFile++);
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iFile - 1;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::view_const_string>& output)					{
	for(uint32_t iFile   = 0; iFile   < input.Files		.size(); ++iFile	) 
		gpk_necall(output.push_back({input.Files[iFile].begin(), input.Files[iFile].size()}), "%s", "Out of memory?");
	for(uint32_t iFolder = 0; iFolder < input.Folders	.size(); ++iFolder	) 
		gpk_necall(::gpk::pathList(input.Folders	[iFolder], output), "%s", "Unknown error!");
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::SPathContents& input, ::gpk::array_obj<::gpk::array_pod<char_t>>& output)					{
	for(uint32_t iFile   = 0; iFile   < input.Files		.size(); ++iFile	) 
		gpk_necall(output.push_back(input.Files[iFile]), "%s", "Out of memory?");
	for(uint32_t iFolder = 0; iFolder < input.Folders	.size(); ++iFolder	) 
		gpk_necall(::gpk::pathList(input.Folders[iFolder], output), "%s", "Unknown error!");
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::view_const_string & pathToList, ::gpk::array_obj<::gpk::array_pod<char_t>>& output, bool listFolders)	{
	static constexpr const char														curDir	[]							= ".";
	static constexpr const char														parDir	[]							= "..";
	char																			sPath	[4096];
	gpk_necall(sprintf_s(sPath, "%s\\*.*", pathToList.begin()), "%s", "Path too long?");
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA																fdFile								= {};
	HANDLE																			hFind								= NULL;
	hFind																		= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", pathToList.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		int32_t																			lenPath							= sprintf_s(sPath, "%s\\%s", pathToList.begin(), fdFile.cFileName);
		if((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && false == listFolders)
			continue;
		info_printf("Path: %s.", sPath);
		gpk_necall(output.push_back(::gpk::view_array<const char_t>{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
    DIR																				* dir								= 0;
    dirent																			* drnt								= 0;
    dir																			= opendir(pathToList.begin());
    while ((drnt = readdir(dir)) != NULL) {
        ::std::string																	name								(drnt->d_name);
        if (name != curDir && name != parDir) {
			if(drnt->d_type == DT_DIR && false == listFolders)
				continue;
			int32_t																			lenPath								= sprintf_s(sPath, "%s//%s", pathToList.begin(), drnt->d_name);
			info_printf("Path: %s.", sPath);
			gpk_necall(output.push_back({sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
        }
	}
#endif
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::view_const_string & pathToList, ::gpk::SPathContents& pathContents)						{
	char																			sPath[4096];
	gpk_necall(sprintf_s(sPath, "%s\\*.*", pathToList.begin()), "%s", "Path too long?");
	static constexpr const char														curDir []							= ".";
	static constexpr const char														parDir []							= "..";
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA																fdFile								= {};
	HANDLE																			hFind								= NULL;
	hFind																		= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", pathToList.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		int32_t																			lenPath								= sprintf_s(sPath, "%s\\%s", pathToList.begin(), fdFile.cFileName);
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			::gpk::error_t																	newFolderIndex						= pathContents.Folders.push_back({});
			gpk_necall(newFolderIndex, "%s", "Out of memory?");
			gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex]), "%s", "Unknown error!");
			info_printf("Directory: %s.", sPath);
		}
		else {
			int32_t indexFile;
			gpk_necall(indexFile = pathContents.Files.push_back(::gpk::view_array<const char_t>{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			pathContents.Files[indexFile].push_back(0);
			info_printf("File %u: %s.", indexFile, sPath);
		}
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
    DIR																				* dir;
    struct dirent																	* drnt;
    dir																			= opendir(pathToList.begin());
    while ((drnt = readdir(dir)) != NULL) {
        ::std::string																	name								(drnt->d_name);
        if (name != curDir && name != parDir) {
			int32_t																			lenPath								= sprintf_s(sPath, "%s//%s", pathToList.begin(), drnt->d_name);
			if(drnt->d_type == DT_DIR) {
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
	}
#endif
    return 0;
}

		::gpk::error_t									gpk::fileToMemory									(const ::gpk::view_const_string& fileName, ::gpk::array_pod<byte_t>& fileInMemory)		{
	FILE														* fp												= 0;
	int32_t														fileErr												= fopen_s(&fp, fileName.begin(), "rb");
	rve_if(fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", fileName.begin());

	fseek(fp, 0, SEEK_END);
	int32_t														fileSize											= (int32_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	::gpk::error_t												result												= 0;
	if errored(fileInMemory.resize(fileSize)) {
		error_printf("File too large? : %llu.", (uint64_t)fileSize);
		result													= -1;
	}
	else {
		if(fileSize != (int32_t)fread(fileInMemory.begin(), sizeof(ubyte_t), fileSize, fp)) {
			error_printf("fread() failed! file: '%s'.", fileName.begin());
			result													= -1;
		}
	}
	fclose(fp);
	return result;
}

		::gpk::error_t									gpk::fileFromMemory									(const ::gpk::view_const_string& fileName, const ::gpk::array_pod<byte_t>& fileInMemory)	{
	FILE														* fp												= 0;
	ree_if(0 != fopen_s(&fp, fileName.begin(), "wb"), "Failed to create file for writing: %s.", fileName.begin());
	::gpk::error_t												result												= 0;
	if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fp)) {
		error_printf("Failed to write file. Disk full? File size: %u.", fileInMemory.size());
		result													= -1;
	}
	fclose(fp);
	return result;
}
