#include "gpk_storage.h"
#include "gpk_io.h"
#include "gpk_string.h"

#include <new>

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <dirent.h>
#endif

int64_t								gpk::fileSize					(const ::gpk::vcc	& fileNameSrc)								{
	FILE									* fp							= 0;
	ree_if(0 != fopen_s(&fp, fileNameSrc.begin(), "rb"), "Failed to open file: %s.", fileNameSrc.begin());
	ree_if(0 == fp, "Failed to open file: %s.", fileNameSrc.begin());
#if defined(GPK_WINDOWS)
	if(0 != _fseeki64(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize						= _ftelli64(fp);
#else
	if(0 != fseek(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize						= ftell(fp);
#endif
	if(0 > fileSize)
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
	fclose(fp);
	return fileSize;
}

#if defined(GPK_ANDROID) || defined(GPK_LINUX)
//#	include <sys/types.h>
//#	include <unistd.h>
#	include <sys/stat.h>
#endif

::gpk::error_t						gpk::pathCreate				(const ::gpk::vcc& pathName, const char separator) {
	rww_if(0 == pathName.begin(), "%s.", "pathName is null.");
	char									folder[1024]				= {};
	int32_t									offsetBar					= -1;
	do {
		++offsetBar;
		offsetBar							= ::gpk::find(separator, pathName, offsetBar);
		if(0 == offsetBar) {
			if(offsetBar < 0 || offsetBar == (int32_t)pathName.size() - 1)
				break;
			continue;
		}
		gpk_necall(strncpy_s(folder, pathName.begin(), (offsetBar < 0) ? pathName.size() : offsetBar), "String buffer overflow? Path size: %u.", pathName.size());
		if(0 == strcmp(".", folder))
			continue;
#if defined(GPK_WINDOWS)
		if(!CreateDirectoryA(folder, NULL)) {
			DWORD									err							= GetLastError();
			ree_if(err != ERROR_ALREADY_EXISTS, "Failed to create directory: %s. hr: (%u)", folder, err);
		}
#else
		struct stat st = {0};
		if (stat(folder, &st) == -1) {
			mkdir(folder, 0700);
		}
#endif
		if(offsetBar < 0 || offsetBar == (int32_t)pathName.size() - 1)
			break;
	} while(true);
	return 0;
}

// This function is useful for splitting files smaller than 4gb very quick.
static ::gpk::error_t				fileSplitSmall					(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %u.", fileNameSrc.begin(), sizePartMax);
	::gpk::apod<byte_t>				fileInMemory;
	gpk_necall(::gpk::fileToMemory(fileNameSrc, fileInMemory), "Failed to load file: \"%s\".", fileNameSrc);

	// -- Write parts to disk.
	uint32_t								countParts						= fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	char									fileNameDst	[1024]				= {};
	uint32_t								iPart							= 0;
	for(; iPart < countParts; ++iPart) {
		const uint32_t							offsetPart						= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, ::gpk::size(fileNameDst) - 2, "%s.%.2u", fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE									* fpDest						= 0;
		ree_if(0 != fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		uint32_t								countBytes						= (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax;
		ree_if(countBytes != fwrite(&fileInMemory[offsetPart], 1, countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// This function is useful for splitting files smaller than 4gb very quick.
static ::gpk::error_t				fileSplitLarge					(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %u.", fileNameSrc.begin(), sizePartMax);
	int64_t									sizeFile						= ::gpk::fileSize(fileNameSrc);
	ree_if(errored(sizeFile), "Failed to open file %s.", fileNameSrc.begin());
	FILE									* fp							= 0;
	ree_if(0 == fp, "%s", "Files larger than 3gb still not supported.");
	ree_if(0 != fopen_s(&fp, fileNameSrc.begin(), "rb"), "Failed to open file: %s.", fileNameSrc.begin());
	ree_if(0 == fp, "Failed to open file: %s.", fileNameSrc.begin());

	::gpk::apod<byte_t>				partInMemory;
	gpk_necall(partInMemory.resize(sizePartMax), "Failed to allocate buffer for file part. Out of memory? File part size: %u.", sizePartMax);

	// -- Write parts to disk.
	uint32_t								countParts						= (uint32_t)(sizeFile / sizePartMax + one_if(sizeFile % sizePartMax));
	char									fileNameDst	[1024]				= {};
	uint32_t								iPart							= 0;
	for(; iPart < countParts; ++iPart) { //
		const uint32_t							offsetPart						= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, ::gpk::size(fileNameDst) - 2, "%s.%.2u", fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE									* fpDest						= 0;
		ree_if(0 != fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		int64_t									countBytes						= (iPart == countParts - 1) ? sizeFile - offsetPart : sizePartMax;
		ree_if(countBytes != (int64_t)fwrite(partInMemory.begin(), 1, (uint32_t)countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// Splits a file into file.## parts.
::gpk::error_t						gpk::fileSplit					(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
	// -- Get file size to determine which algorithm to use.
	// -- For files smaller than 3gb, we use a fast algorithm that loads the entire file in memory.
	// -- For files of, or larger than, 3gb, we use a fast algorithm that loads chunks of 1gb in memory for writing the parts.
	static constexpr	const uint32_t		gigabyte						= 1024U*1024U*1024U;
	static constexpr	const uint32_t		sizeSmallFileMax				= 3U * gigabyte;
	const int64_t							sizeFile						= ::gpk::fileSize(fileNameSrc);
	ree_if(-1 == sizeFile, "Failed to get size for file: '%s'.", fileNameSrc.begin());
	return (sizeSmallFileMax > sizeFile) ? ::fileSplitSmall(fileNameSrc, sizePartMax) : ::fileSplitLarge(fileNameSrc, sizePartMax);
}

// Joins a file split into file.## parts.
::gpk::error_t						gpk::fileJoin					(const ::gpk::vcc	& fileNameDst)	{
	char									fileNameSrc	[1024]				= {};
	uint32_t								iFile							= 0;
	gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", fileNameDst.begin(), iFile++), "File name too large: %s.", fileNameDst.begin());
	FILE									* fpDest						= 0;
	::gpk::apod<char_t>					finalPathName					= ::gpk::toString(fileNameDst);
	fopen_s(&fpDest, finalPathName.begin(), "wb");
	ree_if(0 == fpDest, "Failed to create file: %s.", finalPathName.begin());
	::gpk::apod<byte_t>					fileInMemory					= {};
	// Load each .split part and write it to the destionation file.
	while(0 == ::gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file.
		ree_if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %u. File name: %s.", fileInMemory.size(), fileNameSrc);
		gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", finalPathName.begin(), iFile++), "File name too large: %s.", finalPathName.begin());
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iFile - 1;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::SPathContents& input, ::gpk::aobj<::gpk::vcc>& output, const ::gpk::vcc extension)					{
	for(uint32_t iFile   = 0; iFile   < input.Files		.size(); ++iFile	) {
		const ::gpk::vcc						& fileName				= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necall(output.push_back(fileName), "%s", "Out of memory?");
	}

	for(uint32_t iFolder = 0; iFolder < input.Folders	.size(); ++iFolder	)
		gpk_necall(::gpk::pathList(input.Folders	[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::SPathContents& input, ::gpk::aobj<::gpk::apod<char_t>>& output, const ::gpk::vcc extension)					{
	for(uint32_t iFile   = 0; iFile   < input.Files		.size(); ++iFile	) {
		const ::gpk::vcc						& fileName				= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necall(output.push_back(fileName), "%s", "Out of memory?");
	}
	for(uint32_t iFolder = 0; iFolder < input.Folders	.size(); ++iFolder	)
		gpk_necall(::gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::vcc& pathToList, ::gpk::aobj<::gpk::apod<char_t>>& output, bool listFolders, const ::gpk::vcc extension)	{
	static constexpr const char														curDir	[]							= ".";
	static constexpr const char														parDir	[]							= "..";
	char																			bufferFormat[36];
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/*.*", ::gpk::toString(pathToList).size());
	char																			sPath	[4096];
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, ::gpk::toString(pathToList).begin()), "%s", "Path too long?");
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA																fdFile								= {};
	HANDLE																			hFind								= NULL;
	hFind																		= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", pathToList.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/%%s", pathToList.size());
		int32_t																			lenPath								= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, pathToList.begin(), fdFile.cFileName);
		if((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && false == listFolders)
			continue;

		const ::gpk::vcs viewPath = sPath;
		if(0 == extension.size() || (extension.size() < viewPath.size() && 0 == strncmp(viewPath.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), viewPath.size())))) {
			verbose_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::vcs{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
		}
		//output.push_back(0)
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
    DIR																				* dir								= 0;
    dirent																			* drnt								= 0;
    dir																			= opendir(pathToList.begin());
    while ((drnt = readdir(dir)) != NULL) {
		::gpk::apod<char_t>														name							= ::gpk::vcs{drnt->d_name, (uint32_t)-1};
        if (name != curDir && name != parDir) {
			if(drnt->d_type == DT_DIR && false == listFolders)
				continue;
			int32_t																			lenPath								= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", pathToList.begin(), drnt->d_name);
			info_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::view_array<const char_t>{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
        }
	}
#endif
	return 0;
}

		::gpk::error_t														gpk::pathList						(const ::gpk::vcc& pathToList, ::gpk::SPathContents& pathContents, const gpk::vcc extension)						{
	char																			sPath[4096];
	char																			bufferFormat[36];
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/*.*", pathToList.size());
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, pathToList.begin()), "%s", "Path too long?");
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
		//_CrtCheckMemory();
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/%%s", pathToList.size());
		int32_t																			lenPath								= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, pathToList.begin(), fdFile.cFileName);
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			::gpk::error_t																	newFolderIndex						= pathContents.Folders.push_back({});
			gpk_necall(newFolderIndex, "%s", "Out of memory?");
			gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unknown error!");
			verbose_printf("Directory: %s.", sPath);
		}
		else {
			int32_t indexFile;
			gpk_necall(indexFile = pathContents.Files.push_back(::gpk::view_array<const char_t>{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			//pathContents.Files[indexFile].push_back(0);
			verbose_printf("File %u: %s.", indexFile, sPath);
		}
		//_CrtCheckMemory();
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
    DIR																				* dir	= nullptr;
    struct dirent																	* drnt	= nullptr;
    dir																			= opendir(pathToList.begin());
    while ((drnt = readdir(dir)) != NULL) {
		::gpk::apod<char_t>														name							= ::gpk::vcs{drnt->d_name, (uint32_t)-1};
        if (name != curDir && name != parDir) {
			int32_t																			lenPath								= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", pathToList.begin(), drnt->d_name);
			if(drnt->d_type == DT_DIR) {
				::gpk::error_t																	newFolderIndex						= pathContents.Folders.push_back({});
				gpk_necall(newFolderIndex, "%s", "Out of memory?");
				gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unkown error!");
				info_printf("Directory: %s.", sPath);
			}
			else {
				gpk_necall(pathContents.Files.push_back(::gpk::view_const_byte{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
				info_printf("File: %s.", sPath);
			}
		}
	}
#endif
    return 0;
}

		::gpk::error_t									gpk::fileToMemory									(const ::gpk::vcc& fileName, ::gpk::apod<byte_t>& fileInMemory)		{
	FILE														* fp												= 0;
	int32_t														fileErr												= fopen_s(&fp, ::gpk::toString(fileName).begin(), "rb");
	rvw_if(fileErr > 0 ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", ::gpk::toString(fileName).begin());
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
			error_printf("fread() failed! file: '%s'.", ::gpk::toString(fileName).begin());
			result													= -1;
		}
	}
	fclose(fp);
	return result;
}

		::gpk::error_t									gpk::fileFromMemory									(const ::gpk::vcc& fileName, const ::gpk::view_const_byte& fileInMemory)	{
	FILE														* fp												= 0;
	ree_if(0 != fopen_s(&fp, ::gpk::toString(fileName).begin(), "wb"), "Failed to create file for writing: %s.", ::gpk::toString(fileName).begin());
	ree_if(0 == fp, "Failed to create file for writing: %s.", ::gpk::toString(fileName).begin());
	::gpk::error_t												result												= 0;
	if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fp)) {
		error_printf("Failed to write file. Disk full? File size: %u.", fileInMemory.size());
		result													= -1;
	}
	fclose(fp);
	return result;
}

		::gpk::error_t									gpk::pathNameCompose								(::gpk::vcc path, ::gpk::vcc fileName, ::gpk::apod<char_t> & out_composed)		{
	if(path.size()) {
		for(uint32_t iChar = 0; iChar < path.size(); ++iChar) {
			const char curChar = path[iChar];
			if(curChar == '\\' && iChar < (path.size() - 1) && '\\' == path[iChar + 1])
				++iChar;
			out_composed.push_back(curChar);
		}
		if('\\' != path[path.size() - 1] && '/' != path[path.size() - 1])
			out_composed.push_back('/');
	}

	if(fileName.size())
		for(uint32_t iChar = '\\' == fileName[0] ? 1 : 0; iChar < fileName.size(); ++iChar) {
			const char curChar = fileName[iChar];
			if(curChar == '\\' && iChar < (fileName.size() - 1) && '\\' == fileName[iChar + 1])
				++iChar;
			out_composed.push_back(curChar);
		}
	out_composed.push_back(0);
	out_composed.resize(out_composed.size() - 1);
	return 0;
}

		::gpk::error_t									gpk::findLastSlash									(const ::gpk::vcc & path)		{
	int32_t														indexOfStartOfFileName0								= ::gpk::rfind('\\', path);
	int32_t														indexOfStartOfFileName1								= ::gpk::rfind('/', path);
	return
		(-1 == indexOfStartOfFileName1) ? indexOfStartOfFileName0 :
		(-1 == indexOfStartOfFileName0) ? indexOfStartOfFileName1 :
		::gpk::max(indexOfStartOfFileName0, indexOfStartOfFileName1)
		;
}
