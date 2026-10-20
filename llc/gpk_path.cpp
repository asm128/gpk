#include "gpk_path.h"
#include "gpk_string.h"

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

#if !defined(GPK_ARDUINO) && !defined(GPK_ATMEL)
#	include <filesystem>
#	include <string>
#endif

stxp	::gpk::u2_t GPK_MAX_PATH = 256;

#define gpk_path_debug info_printf


//
::gpk::err_t			gpk::pathCreate				(::gpk::vcst_t pathName, sc_c separator) {
	if_zero_fw(pathName.size());
#ifndef GPK_ATMEL
	char						folder[GPK_MAX_PATH]		= {};
	s2_t						offsetBar					= -1;
	do {
		++offsetBar;
		offsetBar				= ::gpk::find(separator, pathName, offsetBar);
		if(0 == offsetBar) {
			if(offsetBar == (int32_t)pathName.size() - 1)
				break;
			continue;
		}
		if_true_fef(0 != strncpy_s(folder, pathName.begin(), (offsetBar < 0) ? pathName.size() : offsetBar), "String buffer overflow? Path size: %" GPK_FMT_U2 ".", pathName.size());
		if(0 == strcmp(".", folder))
			continue;
#if defined(GPK_WINDOWS)
		gpk_path_debug("Creating folder \"%s\".", folder);
		if(FALSE == CreateDirectoryA(folder, NULL)) {
			if_true_ci(strlen(folder) == 2 && folder[1] == ':');
			DWORD						err							= GetLastError();
			if_true_fef(err != ERROR_ALREADY_EXISTS, "Failed to create directory: %s. hr: (%" GPK_FMT_U2 ")", folder, err);
		}
#else
		struct stat st = {0};
		if (stat(folder, &st) == -1) {
			mkdir(folder, 0700);
		}
#endif
	} while(offsetBar >= 0 && offsetBar != (s2_t)pathName.size() - 1);
#endif
	return 0;
}

::gpk::err_t			gpk::findLastSlash			(::gpk::vcst_t path)		{
	int32_t						indexOfStartOfFileName0		= ::gpk::rfind('\\', path);
	int32_t						indexOfStartOfFileName1		= ::gpk::rfind('/', path);
	return
		(-1 == indexOfStartOfFileName1) ? indexOfStartOfFileName0 :
		(-1 == indexOfStartOfFileName0) ? indexOfStartOfFileName1 :
		::gpk::max(indexOfStartOfFileName0, indexOfStartOfFileName1)
		;
}
::gpk::err_t			gpk::pathStem				(::gpk::vcst_t path, ::gpk::vcst_t & stem) {
	::gpk::vcst_t filename = {};
	if_fail_fe(::gpk::pathFilename(path, filename));
	if_zero_fef(filename.size(), "Path has no filename:'%.*s'.", (int)path.size(), path.begin());
	::gpk::u2_t iEnd = filename.size();
	while(1 < iEnd)
		if('.' == filename[--iEnd])
			rtrn filename.slice(stem, 0, iEnd);
	rtrn filename.slice(stem, 0, filename.size());
}

stxp	bool			pathSeparator			(::gpk::sc_c value) { rtrn '/' == value || '\\' == value; }
stxp	bool			pathDriveLetter			(::gpk::sc_c value) { rtrn ('A' <= value && value <= 'Z') || ('a' <= value && value <= 'z'); }

::gpk::err_t			gpk::pathBegin				(::gpk::vcst_t path, ::gpk::vcst_t & output) {
	::gpk::u2_t					prefixSize				= 0;
	if(path.size() && ::pathSeparator(path[0])) {
		prefixSize							= 1;
		if(path.size() > 1 && ::pathSeparator(path[1])) {
			prefixSize						= 2;
			while(prefixSize < path.size() && false == ::pathSeparator(path[prefixSize]))
				++prefixSize;
			if(prefixSize < path.size())
				++prefixSize;
			while(prefixSize < path.size() && false == ::pathSeparator(path[prefixSize]))
				++prefixSize;
			if(prefixSize < path.size())
				++prefixSize;
		}
	}
	else {
		::gpk::vcst_t			drive					= path;
		if_fail_fe(::gpk::split(':', drive));
		if(1 == drive.size() && drive.size() < path.size() && ::pathDriveLetter(drive[0])) {
			prefixSize						= 2;
			if(prefixSize < path.size() && ::pathSeparator(path[prefixSize]))
				++prefixSize;
		}
	}
	if_fail_fe(path.slice(output, 0, prefixSize));
	rtrn output.size();
}

sttc	::gpk::err_t	validatePathSeparators	(::gpk::vcst_t path, bool allowUNC) {
	for(::gpk::u2_t iChar = 1; iChar < path.size(); ++iChar)
		if_true_fef(::pathSeparator(path[iChar - 1]) && ::pathSeparator(path[iChar]) && false == (allowUNC && 1 == iChar)
			, "Adjacent separators at offset %" GPK_FMT_U2 ": '%.*s'."
			, iChar - 1, (int)path.size(), path.begin()
			);
	rtrn 0;
}

sttc	::gpk::err_t 	appendPathNormalized		(::gpk::vcst_t path, ::gpk::string & output) {
	::gpk::u2_t			written				= 0;
	for(::gpk::u2_t iChar = 0; iChar < path.size(); ++iChar) {
		::gpk::sc_t			curChar				= path[iChar];
		if(::pathSeparator(curChar))
			curChar					= '/';
		if_fail_fef(output.push_back(curChar), "output.size()=%" GPK_FMT_U2 ".", output.size());
		++written;
	}
	rtrn written;
}

::gpk::err_t			gpk::pathNormalize			(::gpk::vcst_t path, ::gpk::string & output, sc_c separator) {
	if_true_fef(false == ::pathSeparator(separator), "Invalid separator: '%c'.", separator);
	if_fail_fe(::validatePathSeparators(path, true));
	if(0 == path.size())
		rtrn output.clear();

	cnst bool				isUNC					= path.size() > 1 && ::pathSeparator(path[0]) && ::pathSeparator(path[1]);
	cnst bool				hasDrive				= path.size() > 1 && ::pathDriveLetter(path[0]) && ':' == path[1];
	cnst bool				isAbsolute				= isUNC || (!hasDrive && ::pathSeparator(path[0])) || (hasDrive && path.size() > 2 && ::pathSeparator(path[2]));
	::gpk::u2_t				iChar					= isUNC ? 2U : hasDrive ? 2U : isAbsolute ? 1U : 0U;
	if(hasDrive && isAbsolute)
		++iChar;

	::gpk::aobj<vcst_t>			segments;
	cnst ::gpk::u2_t		protectedSegments		= isUNC ? 2U : 0U;
	while(iChar < path.size()) {
		cnst ::gpk::u2_t		segmentStart			= iChar;
		while(iChar < path.size() && false == ::pathSeparator(path[iChar]))
			++iChar;
		::gpk::vcst_c			segment					= {path.begin() + segmentStart, iChar - segmentStart};
		if(segment == GPK_CXS(".")) {}
		else if(segment == GPK_CXS("..")) {
			if(segments.size() > protectedSegments && segments[segments.size() - 1] != GPK_CXS("..")) {
				if_fail_fe(segments.pop_back());
			}
			else if(false == isAbsolute) {
				if_fail_fe(segments.push_back(segment));
			}
		}
		else if(segment.size())
			if_fail_fe(segments.push_back(segment));
		if(iChar < path.size())
			++iChar;
	}
	if_true_fef(isUNC && segments.size() < 2, "Incomplete UNC root: '%.*s'.", (int)path.size(), path.begin());

	::gpk::string			normalized					= {};
	if(hasDrive)
		if_fail_fe(normalized.append(path.begin(), 2));
	if(isUNC) {
		if_fail_fe(normalized.push_back(separator));
		if_fail_fe(normalized.push_back(separator));
	}
	else if(isAbsolute)
		if_fail_fe(normalized.push_back(separator));
	for(::gpk::u2_t iSegment = 0; iSegment < segments.size(); ++iSegment) {
		cnst bool				driveRelativeFirst		= hasDrive && false == isAbsolute && 0 == iSegment;
		if(normalized.size() && normalized[normalized.size() - 1] != separator && false == driveRelativeFirst)
			if_fail_fe(normalized.push_back(separator));
		if_fail_fe(normalized.append(segments[iSegment]));
	}
	if(isUNC && segments.size() == protectedSegments && normalized[normalized.size() - 1] != separator) {
		if_fail_fe(normalized.push_back(separator));
	}
	else if(false == normalized.size() && path.size()) {
		if_fail_fe(normalized.push_back('.'));
	}
	output						= normalized;
	rtrn output.size();
}

::gpk::err_t			gpk::pathAbsolute			(::gpk::vcst_t path, ::gpk::string & output, sc_c separator) {
	if_zero_fef(path.size(), "%s", "Empty path.");
	if_true_fef(false == ::pathSeparator(separator), "Invalid separator: '%c'.", separator);
	if_fail_fe(::validatePathSeparators(path, true));
#if !defined(GPK_ARDUINO) && !defined(GPK_ATMEL)
	try {
		cnst ::std::string		input					= {path.begin(), path.size()};
		cnst ::std::string		absolute				= ::std::filesystem::absolute(input).generic_string();
		::gpk::vcst_c			absoluteView			= {absolute.data(), (::gpk::u2_t)absolute.size()};
		rtrn ::gpk::pathNormalize(absoluteView, output, separator);
	}
	catch(cnst ::std::filesystem::filesystem_error & exception) {
		error_printf("Failed to resolve absolute path '%.*s': %s.", (int)path.size(), path.begin(), exception.what());
		rtrn -1;
	}
#else
	(void)output;
	(void)separator;
	error_printf("Absolute path resolution is unavailable on this platform: '%.*s'.", (int)path.size(), path.begin());
	rtrn -1;
#endif
}
//
::gpk::err_t			gpk::pathNameCompose		(::gpk::vcst_t path, ::gpk::vcst_t fileName, ::gpk::string & out_composed)		{
	if_fail_fe(::validatePathSeparators(path, true));
	if_fail_fe(::validatePathSeparators(fileName, 0 == path.size()));
	::gpk::u2_t				pathLength				= 0;
	if(path.size())
		if_fail_fe(pathLength = ::appendPathNormalized(path, out_composed));
	if(fileName.size()) {
		::gpk::u2_t				fileOffset				= 0;
		if(pathLength) {
			while(fileOffset < fileName.size() && ::pathSeparator(fileName[fileOffset]))
				++fileOffset;
			if('/' != out_composed[out_composed.size() - 1] && fileOffset < fileName.size())
				if_fail_fef(out_composed.push_back('/'), "out_composed.size()=%" GPK_FMT_U2 ".", out_composed.size());
		}
		::gpk::vcst_c			fileToAppend				= {fileName.begin() + fileOffset, fileName.size() - fileOffset};
		if_fail_fe(::appendPathNormalized(fileToAppend, out_composed));
	}
	rtrn out_composed.size();
}

::gpk::err_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<vcst_t> & output, ::gpk::vcst_c extension)					{
	gpk_path_debug("extension=\"%.*s\"", (int)extension.size(), extension.begin());
	for(::gpk::u2_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		::gpk::vcst_c			& fileName					= input.Files[iFile];
		if(extension.size() && extension.size() >= fileName.size())
			continue;
		::gpk::vcst_t			fileExtension				= {};
		if(extension.size())
			if_fail_fe(fileName.slice(fileExtension, fileName.size() - extension.size()));
		if(0 == extension.size() || fileExtension == extension) {
			gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
			gpk_necs(output.push_back(fileName));
		}
	}
	for(::gpk::u2_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder)
		if_fail_fef(gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

::gpk::err_t			gpk::pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<string> & output, ::gpk::vcst_c extension)					{
	gpk_path_debug("extension=\"%.*s\"", (int)extension.size(), extension.begin());
	for(::gpk::u2_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		::gpk::vcst_c			& fileName					= input.Files[iFile];
		gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
		if(extension.size() && extension.size() >= fileName.size())
			continue;
		::gpk::vcst_t			fileExtension				= {};
		if(extension.size())
			if_fail_fe(fileName.slice(fileExtension, fileName.size() - extension.size()));
		if(0 == extension.size() || fileExtension == extension) {
			gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
			if_fail_fef(output.push_back(fileName), "fileName=%.*s, output.size()=%" GPK_FMT_U2, (int)fileName.size(), fileName.begin(), output.size());
		}
	}
	for(::gpk::u2_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder	) {
		const ::gpk::SPathContents	& childPath	= input.Folders[iFolder];
		if_fail_fef(gpk::pathList(childPath, output, extension), "output.size()=%" GPK_FMT_U2 ", extension=%.*s", output.size(), (int)extension.size(), extension.begin());
	}
	return 0;
}

//gpk::err_t		listDirContents		(gpk::vcst_t targetWildcard, gpk::aobj<string> & filenames, gpk::aobj<string> & dirnames) {
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

sttc ::gpk::err_t		pathListEntry			(::gpk::vcst_t pathToList, ::gpk::SPathContents & pathContents, const ::gpk::function<::gpk::err_t(bool, ::gpk::vcst_c&)> & onItem, ::gpk::vcst_c extension, ::gpk::vcst_t entryName, bool isFolder) {
	if(0 == strcmp(entryName.begin(), curDir) || 0 == strcmp(entryName.begin(), parDir))
		rtrn 0;
	::gpk::string				sPath						= {};
	if_fail_fe(::gpk::pathNameCompose(pathToList, entryName, sPath));
	if(false == isFolder && extension.size()) {
		::gpk::vcst_t			fileExtension				= {};
		if(extension.size() >= sPath.size())
			rtrn 0;
		if_fail_fe(sPath.slice(fileExtension, sPath.size() - extension.size()));
		if(fileExtension != extension)
			rtrn 0;
	}
	::gpk::err_t				action						= 0;
	if_fail_fef(action = onItem(isFolder, sPath), "'%s'", sPath.begin());
	if(action & 1)
		rtrn 0;
	if(isFolder) {
		::gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
		gpk_necs(newFolderIndex);
		gpk_necall(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], onItem, extension), "'%s'", sPath.begin());
		verbose_printf("Directory: %s.", sPath.begin());
	}
	else {
		int32_t						indexFile;
		gpk_necall(indexFile = pathContents.Files.push_back(sPath), "%s", "Failed to push path to output list");
		verbose_printf("File %" GPK_FMT_U2 ": %s.", indexFile, sPath.begin());
	}
	rtrn 0;
}

sttc ::gpk::err_t		pathListNative			(::gpk::vcst_t pathToList, ::gpk::SPathContents & pathContents, const ::gpk::function<::gpk::err_t(bool, ::gpk::vcst_c&)> & onItem, ::gpk::vcst_c extension) {
	::gpk::err_t				result						= 0;
#ifdef GPK_WINDOWS
	::gpk::string				sPath						= {};
	if_fail_fef(::gpk::pathNameCompose(pathToList, GPK_CXS("*.*"), sPath), "Invalid path: '%.*s'.", (int)pathToList.size(), pathToList.begin());
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= FindFirstFile(sPath.begin(), &fdFile);
	if_true_fef(hFind == INVALID_HANDLE_VALUE, "Path not found: [%.*s].", (int)pathToList.size(), pathToList.begin());
	do {
		cnst ::gpk::vcst_t		entryName						= {fdFile.cFileName, (::gpk::u2_t)-1};
		if_fail_bef(result = ::pathListEntry(pathToList, pathContents, onItem, extension, entryName, 0 != (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)), "'%.*s'", (int)entryName.size(), entryName.begin());
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	DIR							* dir							= opendir(pathToList.begin());
	if_true_fef(0 == dir, "Path not found: [%.*s].", (int)pathToList.size(), pathToList.begin());
	struct dirent				* drnt						= nullptr;
	while((drnt = readdir(dir))) {
		cnst ::gpk::vcst_t		entryName						= {drnt->d_name, (::gpk::u2_t)-1};
		if_fail_bef(result = ::pathListEntry(pathToList, pathContents, onItem, extension, entryName, drnt->d_type == DT_DIR), "'%.*s'", (int)entryName.size(), entryName.begin());
	}
	closedir(dir);
#else
	(void)pathToList;
	(void)pathContents;
	(void)onItem;
	(void)extension;
#endif
	if_fail_fe(result);
	rtrn 0;
}

::gpk::err_t			gpk::pathList				(::gpk::vcst_t pathToList, ::gpk::SPathContents & pathContents, ::gpk::vcst_c extension)						{
	rtrn ::gpk::pathList(pathToList, pathContents, [](::gpk::b8_t, ::gpk::vcst_c &) { rtrn 0; }, extension);
}

::gpk::err_t			gpk::pathList				(::gpk::vcst_t pathToList, ::gpk::SPathContents & pathContents, ::gpk::function<err_t(bool, vcst_c&)> onItem, ::gpk::vcst_c extension)						{
	::gpk::vcst_t				pathBegin;
	if_fail_fe(::gpk::pathBegin(pathToList, pathBegin));
	cnst bool					removeTrailingSlash		= pathToList.size() > pathBegin.size() && ::pathSeparator(pathToList[pathToList.size() - 1]);
	::gpk::string				withoutTrailingSlash		= removeTrailingSlash ? ::gpk::vcst_t{pathToList.begin(), pathToList.size() - 1} : pathToList;
	rtrn ::pathListNative(withoutTrailingSlash, pathContents, onItem, extension);
}


