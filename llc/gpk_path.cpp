#include "gpk_path.h"

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <dirent.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <unistd.h>
#elif !defined(GPK_ATMEL)
//#	include <dirent.h>
#	include <sys/stat.h>
////#	include <sys/types.h>
////#	include <unistd.h>
#endif

stxp	uint32_t GPK_MAX_PATH = 256;
//
::gpk::err_t			gpk::pathCreate				(::gpk::vcst_c & pathName, sc_c separator) {
	if_zero_fw(pathName.size());
#ifndef GPK_ATMEL
	char						folder[GPK_MAX_PATH]		= {};
	s2_t						offsetBar					= -1;
	do {
		++offsetBar;
		offsetBar				= ::gpk::find(separator, pathName, offsetBar);
		if(0 == offsetBar) {
			if(offsetBar < 0 || offsetBar == (int32_t)pathName.size() - 1)
				break;
			continue;
		}
		ree_if(0 == strncpy_s(folder, pathName.begin(), (offsetBar < 0) ? pathName.size() : offsetBar), "String buffer overflow? Path size: %" GPK_FMT_U2 ".", pathName.size());
		if(0 == strcmp(".", folder))
			continue;
#if defined(GPK_WINDOWS)
		if(!CreateDirectoryA(folder, NULL)) {
			DWORD						err							= GetLastError();
			ree_if(err != ERROR_ALREADY_EXISTS, "Failed to create directory: %s. hr: (%" GPK_FMT_U2 ")", folder, err);
		}
#else
		struct stat st = {0};
		if (stat(folder, &st) == -1) {
			mkdir(folder, 0700);
		}
#endif
		if(offsetBar < 0 || offsetBar == (s2_t)pathName.size() - 1)
			break;
	} while(true);
#endif
	return 0;
}

::gpk::err_t			gpk::findLastSlash			(::gpk::vcsc_c & path)		{
	int32_t						indexOfStartOfFileName0		= ::gpk::rfind('\\', path);
	int32_t						indexOfStartOfFileName1		= ::gpk::rfind('/', path);
	return
		(-1 == indexOfStartOfFileName1) ? indexOfStartOfFileName0 :
		(-1 == indexOfStartOfFileName0) ? indexOfStartOfFileName1 :
		::gpk::max(indexOfStartOfFileName0, indexOfStartOfFileName1)
		;
}
//
::gpk::err_t			gpk::pathNameCompose		(::gpk::vcsc_c & path, ::gpk::vcsc_c & fileName, ::gpk::asc_t & out_composed)		{
	if(path.size()) {
		for(uint32_t iChar = 0; iChar < path.size(); ++iChar) {
			const char					curChar						= path[iChar];
			if(iChar < (path.size() - 1)) {
				const char					nxtChar						= path[iChar + 1];
				if('\\' == curChar && '\\' == nxtChar)
					continue;//++iChar;
			}
			out_composed.push_back(curChar);
		}
		if('\\' != path[path.size() - 1] && '/' != path[path.size() - 1])
			out_composed.push_back('/');
	}
	if(fileName.size()) {
		for(uint32_t iChar = ('\\' == fileName[0]) ? 1 : 0; iChar < fileName.size(); ++iChar) {
			const char					curChar						= fileName[iChar];
			if(iChar < (fileName.size() - 1)) {
				const char					nxtChar						= fileName[iChar + 1];
				if('\\' == curChar && '\\' == nxtChar)
					continue; //++iChar;
			}
			out_composed.push_back(curChar);
		}
	}
	return out_composed.size();
}

::gpk::err_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::avcsc_t & output, ::gpk::vcst_c extension)					{
	for(uint32_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		::gpk::vcsc_c			& fileName					= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necs(output.push_back(fileName));
	}
	for(uint32_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder)
		gpk_necall(gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

::gpk::err_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::aasc_t & output, ::gpk::vcst_c extension)					{
	for(uint32_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		::gpk::vcsc_c			& fileName					= input.Files[iFile];
		if(0 == extension.size() || (extension.size() < fileName.size() && 0 == strncmp(fileName.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), fileName.size()))))
			gpk_necs(output.push_back(fileName));
	}
	for(uint32_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder	)
		gpk_necall(gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

//gpk::err_t		listDirContents		(gpk::vcsc_t targetWildcard, gpk::aasc_t & filenames, gpk::aasc_t & dirnames) {
//	WIN32_FIND_DATA data = {}; 
//	HANDLE hFind;
//
//	while (INVALID_HANDLE_VALUE !=  (hFind = FindFirstFile(targetWildcard.begin(), &data))) 
//		if(0 == (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { 
//			 if_fail_fe(filenames.push_back(data.cFileName)); 
//		}
//		else if(0 != memcmp(data.cFileName,TEXT( "."), sizeof(TCHAR) * 2) && 0 != memcmp(data.cFileName, TEXT(".."), sizeof(TCHAR) * 3)) 
//			if_fail_fe(dirnames.push_back(data.cFileName));
//
//	FindClose(hFind);
//	return 0;
//}


#if !defined(GPK_ESP8266) // && !defined(GPK_ESP32) && !defined(GPK_ARDUINO) 
stxp	const char		curDir	[]					= ".";
stxp	const char		parDir	[]					= "..";
#endif

::gpk::err_t			gpk::pathList				(const ::gpk::vcst_t & pathToList, ::gpk::aasc_t & output, bool listFolders, ::gpk::vcst_c extension)	{
	::gpk::asc_t				withoutTrailingSlash		= (pathToList.size() - 1 > (uint32_t)::gpk::findLastSlash(pathToList)) ? pathToList : ::gpk::vcst_t{pathToList.begin(), pathToList.size() - 1};
	char						bufferFormat[16]			=  {};
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/*.*", withoutTrailingSlash.size());
	char						sPath	[GPK_MAX_PATH]		= {};
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin()), "bufferFormat: '%s'. withoutTrailingSlash: '%s'", bufferFormat, withoutTrailingSlash.begin());

#ifdef GPK_WINDOWS
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= NULL;
	hFind					= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", withoutTrailingSlash.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/%%s", withoutTrailingSlash.size());
		int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin(), fdFile.cFileName);
		if((fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && false == listFolders)
			continue;

		::gpk::vcst_c				viewPath					= sPath;
		if(0 == extension.size() || (extension.size() < viewPath.size() && 0 == strncmp(viewPath.end() - extension.size(), extension.begin(), ::gpk::min(extension.size(), viewPath.size())))) {
			verbose_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::vcst_t{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
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
		::gpk::asc_t				name						= ::gpk::vcst_t{drnt->d_name, (uint32_t)-1};
		if (name != curDir && name != parDir) {
			if(drnt->d_type == DT_DIR && false == listFolders)
				continue;
			int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", withoutTrailingSlash.begin(), drnt->d_name);
			info_printf("Path: %s.", sPath);
			gpk_necall(output.push_back(::gpk::vcsc_t{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list.");
		}
	}
#endif
	return 0;
}


::gpk::err_t			gpk::pathList				(::gpk::vcst_c & pathToList, ::gpk::SPathContents & pathContents, ::gpk::vcst_c extension)						{
	::gpk::asc_t				withoutTrailingSlash		= (pathToList.size() - 1 > (uint32_t)::gpk::findLastSlash(pathToList)) ? pathToList : ::gpk::vcst_t{pathToList.begin(), pathToList.size() - 1};
	char						bufferFormat[36]			= {};
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/*.*", withoutTrailingSlash.size());
	char						sPath[GPK_MAX_PATH]			= {};
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin()), "%s", "Path too long?");
#ifdef GPK_WINDOWS
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= NULL;
	hFind					= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", withoutTrailingSlash.begin());
	do if(	0 != strcmp(fdFile.cFileName, curDir)
		 &&	0 != strcmp(fdFile.cFileName, parDir)
		) {
		//_CrtCheckMemory();
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/%%s", withoutTrailingSlash.size());
		int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin(), fdFile.cFileName);
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			::gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
			gpk_necs(newFolderIndex);
			gpk_necall(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unknown error!");
			verbose_printf("Directory: %s.", sPath);
		}
		else {
			int32_t						indexFile;
			gpk_necall(indexFile = pathContents.Files.push_back(::gpk::vcst_t{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			//pathContents.Files[indexFile].push_back(0);
			verbose_printf("File %" GPK_FMT_U2 ": %s.", indexFile, sPath);
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
		::gpk::apod<char>			name						= ::gpk::vcst_t{drnt->d_name, (uint32_t)-1};
		if (name != curDir && name != parDir) {
			int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", withoutTrailingSlash.begin(), drnt->d_name);
			if(drnt->d_type == DT_DIR) {
				::gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
				gpk_necs(newFolderIndex);
				gpk_necall(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unkown error!");
				info_printf("Directory: %s.", sPath);
			}
			else {
				gpk_necall(pathContents.Files.push_back(::gpk::vcsc_t{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
				info_printf("File: %s.", sPath);
			}
		}
	}
#endif
	return 0;
}

::gpk::err_t			gpk::pathList				(::gpk::vcst_c & pathToList, ::gpk::SPathContents & pathContents, ::gpk::function<err_t(bool, vcst_c&)> onItem, ::gpk::vcst_c extension)						{
	::gpk::asc_t				withoutTrailingSlash		= (pathToList.size() - 1 > (uint32_t)::gpk::findLastSlash(pathToList)) ? pathToList : ::gpk::vcst_t{pathToList.begin(), pathToList.size() - 1};
	char						bufferFormat[36]			= {};
	snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/*.*", withoutTrailingSlash.size());
	char						sPath[GPK_MAX_PATH]			= {};
	gpk_necall(snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin()), "%s", "Path too long?");
#ifdef GPK_WINDOWS
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= NULL;
	hFind					= FindFirstFile(sPath, &fdFile);
	ree_if(hFind == INVALID_HANDLE_VALUE, "Path not found: [%s].", withoutTrailingSlash.begin());
	do if(	strcmp(fdFile.cFileName, curDir)
		 &&	strcmp(fdFile.cFileName, parDir)
		) {
		//_CrtCheckMemory();
		snprintf(bufferFormat, ::gpk::size(bufferFormat) - 2, "%%.%" GPK_FMT_U2 "s/%%s", withoutTrailingSlash.size());
		int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, bufferFormat, withoutTrailingSlash.begin(), fdFile.cFileName);
		const bool					isFolder					= fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		err_t						action						= 0;
		if_fail_fef(action = onItem(isFolder, sPath), "'%s'", sPath);
		if(action & 1)
			continue;
		if(isFolder) {
			::gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
			gpk_necs(newFolderIndex);
			gpk_necall(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], extension), "%s", "Unknown error!");
			verbose_printf("Directory: %s.", sPath);
		}
		else {
			int32_t						indexFile;
			gpk_necall(indexFile = pathContents.Files.push_back(::gpk::vcst_t{sPath, (uint32_t)lenPath}), "%s", "Failed to push path to output list");
			//pathContents.Files[indexFile].push_back(0);
			verbose_printf("File %" GPK_FMT_U2 ": %s.", indexFile, sPath);
		}
		//_CrtCheckMemory();
	}
	while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	DIR							* dir						= nullptr;
	struct dirent				* drnt						= nullptr;
	dir						= opendir(withoutTrailingSlash.begin());
	while ((drnt = readdir(dir))) {
		::gpk::apod<char>			name						= ::gpk::vcst_t{drnt->d_name, (uint32_t)-1};
		if (name != curDir && name != parDir) {
			int32_t						lenPath						= snprintf(sPath, ::gpk::size(sPath) - 2, "%s/%s", withoutTrailingSlash.begin(), drnt->d_name);
			const bool					isFolder					= drnt->d_type == DT_DIR;
			err_t						action						= 0;
			if_fail_fef(action = onItem(isFolder, sPath), "'%s'", sPath);
			if(action & 1)
				continue;
			if(isFolder) {
				::gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
				gpk_necs(newFolderIndex);
				gpk_necall(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], onItem, extension), "'%s'", sPath);
				info_printf("Directory: %s.", sPath);
			}
			else {
				gpk_necall(pathContents.Files.push_back(::gpk::vcsc_t{sPath, (uint32_t)lenPath}), "%s", sPath);
				info_printf("File: %s.", sPath);
			}
		}
	}
#endif
	return 0;
}
