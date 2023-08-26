#include "gpk_path.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include "gpk_string.h"
#	include <dirent.h>
#	include <sys/stat.h>
//#	include <sys/types.h>
//#	include <unistd.h>
#else
#	include "gpk_string.h"
#	include <dirent.h>
#	include <sys/stat.h>
//#	include <sys/types.h>
//#	include <unistd.h>
#endif

::gpk::error_t			gpk::findLastSlash			(const ::gpk::vcc & path)		{
	int32_t						indexOfStartOfFileName0		= ::gpk::rfind('\\', path);
	int32_t						indexOfStartOfFileName1		= ::gpk::rfind('/', path);
	return
		(-1 == indexOfStartOfFileName1) ? indexOfStartOfFileName0 :
		(-1 == indexOfStartOfFileName0) ? indexOfStartOfFileName1 :
		::gpk::max(indexOfStartOfFileName0, indexOfStartOfFileName1)
		;
}

::gpk::error_t			gpk::pathNameCompose		(const ::gpk::vcc & path, const ::gpk::vcc & fileName, ::gpk::apod<char> & out_composed)		{
	if(path.size()) {
		for(uint32_t iChar = 0; iChar < path.size(); ++iChar) {
			const char					curChar						= path[iChar];
			if(curChar == '\\' && iChar < (path.size() - 1) && '\\' == path[iChar + 1])
				++iChar;
			out_composed.push_back(curChar);
		}
		if('\\' != path[path.size() - 1] && '/' != path[path.size() - 1])
			out_composed.push_back('/');
	}

	if(fileName.size()) {
		for(uint32_t iChar = ('\\' == fileName[0]) ? 1 : 0; iChar < fileName.size(); ++iChar) {
			const char					curChar						= fileName[iChar];
			if(curChar == '\\' && iChar < (fileName.size() - 1) && '\\' == fileName[iChar + 1])
				++iChar;
			out_composed.push_back(curChar);
		}
	}
	out_composed.push_back(0);
	return out_composed.resize(out_composed.size() - 1);
}

::gpk::error_t			gpk::pathCreate				(const ::gpk::vcc & pathName, const char separator) {
	rww_if(0 == pathName.begin(), "%s.", "pathName is null.");
	char						folder[1024]				= {};
	int32_t						offsetBar					= -1;
	do {
		++offsetBar;
		offsetBar				= ::gpk::find(separator, pathName, offsetBar);
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
			DWORD						err							= GetLastError();
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

::gpk::error_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::vcc> & output, const ::gpk::vcc extension)					{
	for(uint32_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		const ::gpk::vcc			& fileName					= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necall(output.push_back(fileName), "output.size(): %i. fileName: '%s'.", output.size(), ::gpk::toString(fileName).begin());
	}

	for(uint32_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder)
		gpk_necall(::gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

::gpk::error_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::apod<char>> & output, const ::gpk::vcc extension)					{
	for(uint32_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		const ::gpk::vcc			& fileName					= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necall(output.push_back(fileName), "output.size(): %i. fileName: '%s'.", output.size(), ::gpk::toString(fileName).begin());
	}
	for(uint32_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder	)
		gpk_necall(::gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

::gpk::error_t			gpk::pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char>> & output, bool listFolders, const ::gpk::vcc extension)	{
	stacxpr	char				curDir	[]					= ".";
	stacxpr	char				parDir	[]					= "..";

	::gpk::achar				withoutTrailingSlash		= ::gpk::toString((pathToList.size() - 1 > (uint32_t)::gpk::findLastSlash(pathToList)) ? pathToList : ::gpk::vcc{pathToList.begin(), pathToList.size() - 1});

	char						bufferFormat[16]			=  {};
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/*.*", withoutTrailingSlash.size());

	char						sPath	[4096]				= {};
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin()), "bufferFormat: '%s'. withoutTrailingSlash: '%s'", bufferFormat, withoutTrailingSlash.begin());

#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= NULL;
	hFind					= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", withoutTrailingSlash.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/%%s", withoutTrailingSlash.size());
		int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin(), fdFile.cFileName);
		if((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && false == listFolders)
			continue;

		const ::gpk::vcs			viewPath					= sPath;
		if(0 == extension.size() || (extension.size() < viewPath.size() && 0 == strncmp(viewPath.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), viewPath.size())))) {
			verbose_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::vcs{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
		}
		//output.push_back(0)
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	DIR							* dir						= 0;
	dirent						* drnt						= 0;
	dir						= opendir(withoutTrailingSlash.begin());
	while ((drnt = readdir(dir)) != NULL) {
		::gpk::achar				name						= ::gpk::vcs{drnt->d_name, (uint32_t)-1};
		if (name != curDir && name != parDir) {
			if(drnt->d_type == DT_DIR && false == listFolders)
				continue;
			int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", withoutTrailingSlash.begin(), drnt->d_name);
			info_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::vcc{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
		}
	}
#endif
	return 0;
}

::gpk::error_t			gpk::pathList				(const ::gpk::vcc & pathToList, ::gpk::SPathContents & pathContents, const gpk::vcc extension)						{
	char						sPath[4096]					= {};
	char						bufferFormat[36]			= {};

	::gpk::achar				withoutTrailingSlash		= ::gpk::toString(pathToList);
	if(withoutTrailingSlash.size() && withoutTrailingSlash.size() - 1 == (uint32_t)::gpk::findLastSlash(withoutTrailingSlash))
		withoutTrailingSlash.resize(withoutTrailingSlash.size () -1);

	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/*.*", withoutTrailingSlash.size());
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin()), "%s", "Path too long?");
	stacxpr	const char			curDir []					= ".";
	stacxpr	const char			parDir []					= "..";
#if defined(GPK_WINDOWS)
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= NULL;
	hFind					= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", withoutTrailingSlash.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		//_CrtCheckMemory();
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%us/%%s", withoutTrailingSlash.size());
		int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin(), fdFile.cFileName);
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			::gpk::error_t				newFolderIndex				= pathContents.Folders.push_back({});
			gpk_necall(newFolderIndex, "%s", "Out of memory?");
			gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unknown error!");
			verbose_printf("Directory: %s.", sPath);
		}
		else {
			int32_t						indexFile;
			gpk_necall(indexFile = pathContents.Files.push_back(::gpk::view<const char>{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			//pathContents.Files[indexFile].push_back(0);
			verbose_printf("File %u: %s.", indexFile, sPath);
		}
		//_CrtCheckMemory();
	}
	while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	DIR							* dir						= nullptr;
	struct dirent				* drnt						= nullptr;
	dir						= opendir(withoutTrailingSlash.begin());
	while ((drnt = readdir(dir)) != NULL) {
		::gpk::apod<char>			name						= ::gpk::vcs{drnt->d_name, (uint32_t)-1};
		if (name != curDir && name != parDir) {
			int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", withoutTrailingSlash.begin(), drnt->d_name);
			if(drnt->d_type == DT_DIR) {
				::gpk::error_t				newFolderIndex				= pathContents.Folders.push_back({});
				gpk_necall(newFolderIndex, "%s", "Out of memory?");
				gpk_necall(::gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unkown error!");
				info_printf("Directory: %s.", sPath);
			}
			else {
				gpk_necall(pathContents.Files.push_back(::gpk::vcc{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
				info_printf("File: %s.", sPath);
			}
		}
	}
#endif
    return 0;
}
