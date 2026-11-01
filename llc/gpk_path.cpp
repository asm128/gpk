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

GPK_USING_APOD();
GPK_USING_VIEW();
GPK_USING_TYPEINT();

usng nmsp gpk;;

stxp	u2_t GPK_MAX_PATH = 256;

#define gpk_path_debug info_printf


//
gpk::err_t			gpk::pathCreate				(vcst_t pathName, sc_c separator) {
	if_zero_fw(pathName.size());
#ifndef GPK_ATMEL
	char						folder[GPK_MAX_PATH]		= {};
	s2_t						offsetBar					= -1;
	do {
		++offsetBar;
		offsetBar				= find(separator, pathName, offsetBar);
		if(0 == offsetBar) {
			if(offsetBar == (s2_t)pathName.size() - 1)
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

gpk::err_t			gpk::findLastSlash			(vcst_t path)		{
	s2_t						indexOfStartOfFileName0		= rfind('\\', path);
	s2_t						indexOfStartOfFileName1		= rfind('/', path);
	return
		(-1 == indexOfStartOfFileName1) ? indexOfStartOfFileName0 :
		(-1 == indexOfStartOfFileName0) ? indexOfStartOfFileName1 :
		max(indexOfStartOfFileName0, indexOfStartOfFileName1)
		;
}
gpk::err_t			gpk::pathStem				(vcst_t path, vcst_t & stem) {
	vcst_t filename = {};
	if_fail_fe(pathFilename(path, filename));
	if_zero_fef(filename.size(), "Path has no filename:'%.*s'.", (int)path.size(), path.begin());
	u2_t iEnd = filename.size();
	while(1 < iEnd)
		if('.' == filename[--iEnd])
			rtrn filename.slice(stem, 0, iEnd);
	rtrn filename.slice(stem, 0, filename.size());
}

stxp	bool			pathSeparator			(sc_c value) { rtrn '/' == value || '\\' == value; }
stxp	bool			pathDriveLetter			(sc_c value) { rtrn ('A' <= value && value <= 'Z') || ('a' <= value && value <= 'z'); }

gpk::err_t			gpk::pathBegin				(vcst_t path, vcst_t & output) {
	u2_t					prefixSize				= 0;
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
		vcst_t			drive					= path;
		if_fail_fe(split(':', drive));
		if(1 == drive.size() && drive.size() < path.size() && ::pathDriveLetter(drive[0])) {
			prefixSize						= 2;
			if(prefixSize < path.size() && ::pathSeparator(path[prefixSize]))
				++prefixSize;
		}
	}
	if_fail_fe(path.slice(output, 0, prefixSize));
	rtrn output.size();
}

sttc	gpk::err_t	validatePathSeparators	(vcst_t path, bool allowUNC) {
	for(u2_t iChar = 1; iChar < path.size(); ++iChar)
		if_true_fef(::pathSeparator(path[iChar - 1]) && ::pathSeparator(path[iChar]) && false == (allowUNC && 1 == iChar)
			, "Adjacent separators at offset %" GPK_FMT_U2 ": '%.*s'."
			, iChar - 1, (int)path.size(), path.begin()
			);
	rtrn 0;
}

sttc	err_t 	appendPathNormalized		(vcst_t path, string & output) {
	u2_t			written				= 0;
	for(u2_t iChar = 0; iChar < path.size(); ++iChar) {
		sc_t			curChar				= path[iChar];
		if(::pathSeparator(curChar))
			curChar					= '/';
		if_fail_fef(output.push_back(curChar), "output.size()=%" GPK_FMT_U2 ".", output.size());
		++written;
	}
	rtrn written;
}

gpk::err_t			gpk::pathNormalize			(vcst_t path, string & output, sc_c separator) {
	if_true_fef(false == ::pathSeparator(separator), "Invalid separator: '%c'.", separator);
	if_fail_fe(::validatePathSeparators(path, true));
	if(0 == path.size())
		rtrn output.clear();

	cnst bool				isUNC					= path.size() > 1 && ::pathSeparator(path[0]) && ::pathSeparator(path[1]);
	cnst bool				hasDrive				= path.size() > 1 && ::pathDriveLetter(path[0]) && ':' == path[1];
	cnst bool				isAbsolute				= isUNC || (!hasDrive && ::pathSeparator(path[0])) || (hasDrive && path.size() > 2 && ::pathSeparator(path[2]));
	u2_t				iChar					= isUNC ? 2U : hasDrive ? 2U : isAbsolute ? 1U : 0U;
	if(hasDrive && isAbsolute)
		++iChar;

	aobj<vcst_t>			segments;
	cnst u2_t		protectedSegments		= isUNC ? 2U : 0U;
	while(iChar < path.size()) {
		cnst u2_t		segmentStart			= iChar;
		while(iChar < path.size() && false == ::pathSeparator(path[iChar]))
			++iChar;
		vcst_c			segment					= {path.begin() + segmentStart, iChar - segmentStart};
		if(segment == ".") {}
		else if(segment == "..") {
			if(segments.size() > protectedSegments && segments[segments.size() - 1] != "..") {
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

	string			normalized					= {};
	if(hasDrive)
		if_fail_fe(normalized.append(path.begin(), 2));
	if(isUNC) {
		if_fail_fe(normalized.push_back(separator));
		if_fail_fe(normalized.push_back(separator));
	}
	else if(isAbsolute)
		if_fail_fe(normalized.push_back(separator));
	for(u2_t iSegment = 0; iSegment < segments.size(); ++iSegment) {
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
//
gpk::err_t			gpk::pathAbsolute			(vcst_t path, string & output, sc_c separator) {
	if_zero_fef(path.size(), "%s", "Empty path.");
	if_true_fef(false == ::pathSeparator(separator), "Invalid separator: '%c'.", separator);
	if_fail_fe(::validatePathSeparators(path, true));
#if defined(GPK_WINDOWS)
	string			input					= path;
	cnst DWORD				required				= GetFullPathNameA(input.begin(), 0, 0, 0);
	if_zero_fef(required, "Failed to resolve absolute path '%.*s'. Error: %lu.", (int)path.size(), path.begin(), GetLastError());
	string			absolute;
	if_fail_fe(absolute.resize(required));
	cnst DWORD				written					= GetFullPathNameA(input.begin(), required, &absolute[0], 0);
	if_zero_fef(written, "Failed to resolve absolute path '%.*s'. Error: %lu.", (int)path.size(), path.begin(), GetLastError());
	if_true_fef(written >= required, "Absolute path grew while resolving '%.*s'. Required: %lu. Available: %lu.", (int)path.size(), path.begin(), written, required);
	if_fail_fe(absolute.resize(written));
	rtrn pathNormalize(absolute, output, separator);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	string			normalized;
	if_fail_fe(pathNormalize(path, normalized, '/'));
	if(normalized.size() && '/' == normalized[0])
		rtrn pathNormalize(normalized, output, separator);
	char					currentPath[GPK_MAX_PATH]	= {};
	if_null_fef(getcwd(currentPath, size(currentPath)), "Failed to obtain the current directory while resolving '%.*s'.", (int)path.size(), path.begin());
	string			absolute;
	if_fail_fe(pathNameCompose(currentPath, normalized, absolute));
	rtrn pathNormalize(absolute, output, separator);
#else
	(void)output;
	(void)separator;
	error_printf("Absolute path resolution is unavailable on this platform: '%.*s'.", (int)path.size(), path.begin());
	rtrn -1;
#endif
}
//
gpk::err_t			gpk::pathNameCompose		(vcst_t path, vcst_t fileName, string & out_composed)		{
	if_fail_fe(::validatePathSeparators(path, true));
	if_fail_fe(::validatePathSeparators(fileName, 0 == path.size()));
	u2_t				pathLength				= 0;
	if(path.size())
		if_fail_fe(pathLength = ::appendPathNormalized(path, out_composed));
	if(fileName.size()) {
		u2_t				fileOffset				= 0;
		if(pathLength) {
			while(fileOffset < fileName.size() && ::pathSeparator(fileName[fileOffset]))
				++fileOffset;
			if('/' != out_composed[out_composed.size() - 1] && fileOffset < fileName.size())
				if_fail_fef(out_composed.push_back('/'), "out_composed.size()=%" GPK_FMT_U2 ".", out_composed.size());
		}
		vcst_c			fileToAppend				= {fileName.begin() + fileOffset, fileName.size() - fileOffset};
		if_fail_fe(::appendPathNormalized(fileToAppend, out_composed));
	}
	rtrn out_composed.size();
}

gpk::err_t			gpk::pathList				(const SPathContents & input, aobj<vcst_t> & output, vcst_c extension)					{
	gpk_path_debug("extension=\"%.*s\"", (int)extension.size(), extension.begin());
	for(u2_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		vcst_c			& fileName					= input.Files[iFile];
		if(extension.size() && extension.size() >= fileName.size())
			continue;
		vcst_t			fileExtension				= {};
		if(extension.size())
			if_fail_fe(fileName.slice(fileExtension, fileName.size() - extension.size()));
		if(0 == extension.size() || fileExtension == extension) {
			gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
			if_fail_fe(output.push_back(fileName));
		}
	}
	for(u2_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder)
		if_fail_fef(gpk::pathList(input.Folders[iFolder], output, extension), "%s", "Unknown error!");
	return 0;
}

gpk::err_t			gpk::pathList				(const SPathContents & input, aobj<string> & output, vcst_c extension)					{
	gpk_path_debug("extension=\"%.*s\"", (int)extension.size(), extension.begin());
	for(u2_t iFile = 0; iFile < input.Files.size(); ++iFile) {
		vcst_c			& fileName					= input.Files[iFile];
		gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
		if(extension.size() && extension.size() >= fileName.size())
			continue;
		vcst_t			fileExtension				= {};
		if(extension.size())
			if_fail_fe(fileName.slice(fileExtension, fileName.size() - extension.size()));
		if(0 == extension.size() || fileExtension == extension) {
			gpk_path_debug("fileName=\"%.*s\"", (int)fileName.size(), fileName.begin());
			if_fail_fef(output.push_back(fileName), "fileName=%.*s, output.size()=%" GPK_FMT_U2, (int)fileName.size(), fileName.begin(), output.size());
		}
	}
	for(u2_t iFolder = 0; iFolder < input.Folders.size(); ++iFolder	) {
		const SPathContents	& childPath	= input.Folders[iFolder];
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

sttc gpk::err_t		pathListEntry			(vcst_t pathToList, gpk::SPathContents & pathContents, const gpk::function<err_t(bool, vcst_c&)> & onItem, vcst_c extension, vcst_t entryName, bool isFolder) {
	if(0 == strcmp(entryName.begin(), curDir) || 0 == strcmp(entryName.begin(), parDir))
		rtrn 0;
	gpk::string				sPath						= {};
	if_fail_fe(pathNameCompose(pathToList, entryName, sPath));
	if(false == isFolder && extension.size()) {
		vcst_t			fileExtension				= {};
		if(extension.size() >= sPath.size())
			rtrn 0;
		if_fail_fe(sPath.slice(fileExtension, sPath.size() - extension.size()));
		if(fileExtension != extension)
			rtrn 0;
	}
	gpk::err_t				action						= 0;
	if_fail_fef(action = onItem(isFolder, sPath), "'%s'", sPath.begin());
	if(action & 1)
		rtrn 0;
	if(isFolder) {
		gpk::err_t				newFolderIndex				= pathContents.Folders.push_back({});
		if_fail_fe(newFolderIndex);
		if_fail_fef(gpk::pathList(sPath, pathContents.Folders[newFolderIndex], onItem, extension), "'%s'", sPath.begin());
		verbose_printf("Directory: %s.", sPath.begin());
	}
	else {
		s2_t						indexFile;
		if_fail_fef(indexFile = pathContents.Files.push_back(sPath), "%s", "Failed to push path to output list");
		verbose_printf("File %" GPK_FMT_U2 ": %s.", indexFile, sPath.begin());
	}
	rtrn 0;
}

sttc gpk::err_t		pathListNative			(vcst_t pathToList, gpk::SPathContents & pathContents, const function<err_t(bool, vcst_c&)> & onItem, vcst_c extension) {
	err_t				result						= 0;
#ifdef GPK_WINDOWS
	gpk::string				sPath						= {};
	if_fail_fef(pathNameCompose(pathToList, "*.*", sPath), "Invalid path: '%.*s'.", (int)pathToList.size(), pathToList.begin());
	WIN32_FIND_DATAA			fdFile						= {};
	HANDLE						hFind						= FindFirstFile(sPath.begin(), &fdFile);
	if_true_fef(hFind == INVALID_HANDLE_VALUE, "Path not found: [%.*s].", (int)pathToList.size(), pathToList.begin());
	do {
		cnst vcst_t		entryName						= {fdFile.cFileName, (u2_t)-1};
		if_fail_bef(result = ::pathListEntry(pathToList, pathContents, onItem, extension, entryName, 0 != (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)), "'%.*s'", (int)entryName.size(), entryName.begin());
	}
	while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
	DIR							* dir							= opendir(pathToList.begin());
	if_true_fef(0 == dir, "Path not found: [%.*s].", (int)pathToList.size(), pathToList.begin());
	struct dirent				* drnt						= nullptr;
	while((drnt = readdir(dir))) {
		cnst vcst_t		entryName						= {drnt->d_name, (u2_t)-1};
		bool					isFolder						= drnt->d_type == DT_DIR;
		if(drnt->d_type == DT_UNKNOWN) {
			gpk::string		entryPath;
			if_fail_bef(result = pathNameCompose(pathToList, entryName, entryPath), "%s", drnt->d_name);
			struct stat			metadata						= {};
			if_fail_bef(result = lstat(entryPath.begin(), &metadata), "%s", entryPath.begin());
			isFolder											= S_ISDIR(metadata.st_mode);
		}
		if_fail_bef(result = ::pathListEntry(pathToList, pathContents, onItem, extension, entryName, isFolder), "'%.*s'", (int)entryName.size(), entryName.begin());
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

gpk::err_t			gpk::pathList				(vcst_t pathToList, SPathContents & pathContents, vcst_c extension)						{
	rtrn pathList(pathToList, pathContents, [](b8_t, vcst_c &) { rtrn 0; }, extension);
}

gpk::err_t			gpk::pathList				(vcst_t pathToList, SPathContents & pathContents, function<err_t(bool, vcst_c&)> onItem, vcst_c extension)						{
	vcst_t				pathLeft;
	if_fail_fe(pathBegin(pathToList, pathLeft));
	cnst bool					removeTrailingSlash		= pathToList.size() > pathLeft.size() && ::pathSeparator(pathToList[pathToList.size() - 1]);
	string				withoutTrailingSlash		= removeTrailingSlash ? vcst_t{pathToList.begin(), pathToList.size() - 1} : pathToList;
	rtrn ::pathListNative(withoutTrailingSlash, pathContents, onItem, extension);
}

sttc err_t		pathTreeScan			(SPathTree & output, au2_t & lengths, vcst_t path, cnst function<err_t(bool, vcst_c&)> & filter, u2_t level) {
	SPathContents unused;
	rtrn pathList(path, unused, [&](bool folder, vcst_c & entry) -> err_t {
		err_t action = 0;
		if(filter) {
			if_fail_fe(action = filter(folder, entry));
		}
		if(action & 1)
			rtrn 1;
		if_true_fef(level > 127, "Path nesting exceeds SPathInfo: '%.*s'.", GPK_VIEW_ARGS(entry));
		vcst_t name;
		if_fail_fe(pathFilename(entry, name));
		cnst u2_t oldSize = output.Char.size(), index = lengths.size();
		if_fail_fe(output.Char.append(name));
		if_true_block_log(error_printf, failed(resize(index + 1, output.Info, lengths)), {
			if_fail_e(output.Char.resize(oldSize));
			rtrn -1;
		});
		output.Info[index] = {(u0_t)level, (u0_t)folder};
		lengths[index] = name.size();
		if(folder && !(action & 2)) {
			if_fail_fe(::pathTreeScan(output, lengths, entry, filter, level + 1));
		}
		rtrn 1; // Recursion and storage are owned here, not by SPathContents.
	}, {});
}

sttc err_t		pathTreeBuild			(SPathTree & output, vcu2_t lengths) {
	if_fail_fe(resize(output.Info.size(), output.Name, output.Tree));
	SPathNode * parents[128] = {};
	u2_t offset = 0;
	for(u2_t i = 0; i < output.Info.size(); ++i) {
		output.Name[i] = {output.Char.begin() + offset, lengths[i]};
		offset += lengths[i];
		pobj<SPathNode> & node = output.Tree[i];
		if(!node) {
			if_null_fe(node.create());
		}
		node->Info = &output.Info[i];
		node->ObjectIndex = i;
		if_fail_fe(node->Children.clear());
		cnst u2_t level = node->Info->NestLevel;
		node->Parent = level ? parents[level - 1] : nullptr;
		parents[level] = node->Info->Folder ? (SPathNode*)node : nullptr;
		if(node->Parent) {
			if_fail_fe(node->Parent->Children.push_back(node));
		}
	}
	rtrn 0;
}

gpk::err_t			gpk::pathList			(SPathTree & output, vcst_t pathToList, function<err_t(bool, vcst_c&)> filter) {
	// Keep a caller-supplied view into output alive across Reset().
	output.Root = pathToList;
	if_fail_fe(output.Reset());
	if_fail_fe(output.Char.clear());
	au2_t lengths;
	cnst err_t scanned = ::pathTreeScan(output, lengths, output.Root, filter, 0);
	// Rebuild accepted entries even if scanning failed, for inspection by the caller.
	if_fail_fe(::pathTreeBuild(output, lengths));
	rtrn scanned;
}

gpk::err_t			gpk::pathList			(SPathTree & output, vcst_t pathToList, vcst_t extension) {
	rtrn pathList(output, pathToList, [&extension](bool folder, vcst_c & entry) -> err_t {
		if(folder || !extension.size())
			rtrn 0;
		vcst_t name, ending;
		if_fail_fe(pathFilename(entry, name));
		if(extension.size() >= name.size())
			rtrn 1;
		if_fail_fe(name.slice(ending, name.size() - extension.size()));
		rtrn ending == extension ? 0 : 1;
	});
}
