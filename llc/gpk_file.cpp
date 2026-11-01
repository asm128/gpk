#include "gpk_file.h"
#include "gpk_path.h"

#include "gpk_string.h"
#include "gpk_cstdio.h"
#include "gpk_string_compose.h"

#include <new>
#ifndef GPK_ARDUINO
#	include <sys/stat.h>
#endif

#ifdef GPK_WINDOWS
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <dirent.h>
#	include <unistd.h>
#	include <fcntl.h> // Definition of AT_* constants
#elif defined(GPK_ARDUINO)
#	ifdef GPK_ESP8266
#		include <LittleFS.h>
#		include "umm_malloc/umm_malloc.h"
#		define 	GPK_SOC_FILESYSTEM_INSTANCE LittleFS
#	elif defined(GPK_ESP32)
#		include <SPIFFS.h>
#		define 	GPK_SOC_FILESYSTEM_INSTANCE SPIFFS
#	endif // GPK_ESP32
#endif // GPK_ARDUINO

GPK_USING_APOD();
GPK_USING_VIEW();
GPK_USING_TYPEINT();

usng gpk::one_if;

gpk::err_t		gpk::with			(vcst_t filepath, vcst_t mode, const function<err_t(FILE*&)> & funcFile)	{
	FILE				* fp				= {};
	if_fail_fe(gpk::fopen_s(fp, filepath, mode));
	if_null_ve(-1, fp);
	err_t				result;
	if_fail_ef(result = funcFile(fp), "filepath:'%.*s'.", (int)filepath.size(), filepath.begin());
	if(fp)
		fclose(fp);
	return result;
}

stxp gpk::vcst_t	GPK_OPEN_MODE_READ		= "rb";
stxp gpk::vcst_t	GPK_OPEN_MODE_WRITE		= "wb";
stxp gpk::vcst_t	GPK_OPEN_MODE_APPEND	= "ab+";

#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(GPK_ESP8266)
stin	fs::FS&		getSoCFileSystem	()	{ rtrn GPK_SOC_FILESYSTEM_INSTANCE; }
sttc  	gpk::err_t	socPath  			(gpk::string & fixed, gpk::vcst_t path) {
	gpk::vcst_t			trimmd				= {};
	if_fail_fe(gpk::ltrim(trimmd, path, "/ \t\n"));
	rtrn gpk::append_strings(fixed, '/', trimmd);
}
#		define FIX_SOC_PATH(_pathView)                      {   \
	if_zero_fe(_pathView.size());                           	\
	string      _fixedPath;                            			\
	if('/' != _pathView[0]) {                               	\
		if_fail_fe(::socPath(_fixedPath, _pathView)); 			\
		_pathView = _fixedPath;                       			\
	}															\
}
#	endif
#endif // GPK_ARDUINO

#define gpk_file_info_printf(...) do {} while(0) // info_printf

s3_t	gpk::	fileSize			(gpk::vcst_t fileName)			{
#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(GPK_ESP8266)
	FIX_SOC_PATH(fileName);
	File				fp						= getSoCFileSystem().open(fileName.begin(), GPK_OPEN_MODE_READ);
	if_true_fef(!fp, "Cannot open file: %.*s.", (int)fileName.size(), fileName.begin());
	u2_c				fileSize				= (::gpk::u2_t)fp.size();
#	endif // GPK_ESP32
#else
	::FILE				* fp					= 0;
	if_fail_fef(gpk::fopen_s(fp, fileName, ::GPK_OPEN_MODE_READ), "Failed to open '%.*s'.", (int)fileName.size(), fileName.begin());
	if_true_fef(0 == fp, "Failed to open '%.*s'.", (int)fileName.size(), fileName.begin());
	if(0 != gpk::fseek(fp, 0, FSEEK_END)) {
		error_printf("Unknown error reading '%.*s'.", (int)fileName.size(), fileName.begin());
		fclose(fp);
		return -1;
	}
	const s3_t			fileSize				= gpk::ftell(fp);
	if(0 > fileSize)
		error_printf("Unknown error reading '%.*s'.", (int)fileName.size(), fileName.begin());
	fclose(fp);
#endif // GPK_ARDUINO
	return fileSize;
}

gpk::err_t	gpk::fileLastWriteTime	(vcst_t fileName, s3_t & modified) {
#ifdef GPK_WINDOWS
	struct _stat64	fileStatus	= {};
	if(_stat64(fileName.begin(), &fileStatus))
		rtrn 1;
	modified = (s3_t)fileStatus.st_mtime;
#elif defined(GPK_ARDUINO)
	(void)fileName;
	(void)modified;
	rtrn 1;
#else
	struct stat		fileStatus	= {};
	if(stat(fileName.begin(), &fileStatus))
		rtrn 1;
	modified = (s3_t)fileStatus.st_mtime;
#endif
	rtrn 0;
}

// Retain the whole-file fast path for files below the streaming threshold.
sttc gpk::err_t fileSplitSmall(gpk::vcst_t fileNameSrc, u2_c sizePartMax) {
	gpk::as0_t			fileInMemory;
	if_fail_fe(gpk::fileToMemory(fileNameSrc, fileInMemory));
	cnst u2_t			countParts = fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	if_true_fe(countParts > 0x7FFFFFFFU);
	gpk::string			fileNameDst;
	for(u2_t iPart = 0; iPart < countParts; ++iPart) {
		cnst u2_t		offsetPart = sizePartMax * iPart;
		fileNameDst.clear();
		if_fail_fe(gpk::append_strings(fileNameDst, fileNameSrc, '.', gpk::str(iPart)));
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst.begin());
		cnst u2_t		countBytes = gpk::min(sizePartMax, fileInMemory.size() - offsetPart);
		if_fail_fe(gpk::fileFromMemory(fileNameDst, gpk::vcs0_t{fileInMemory.begin() + offsetPart, countBytes}));
	}
	return (gpk::err_t)countParts;
}

// Stream large files without retaining a complete part in memory.
sttc gpk::err_t fileSplitLarge(gpk::vcst_t fileNameSrc, u2_c sizePartMax, u3_c sizeFile) {
	gpk::au0_t			buffer;
	cnst u3_t			countParts = sizeFile / sizePartMax + one_if(sizeFile % sizePartMax);
	gpk::string			fileNameDst;
	u3_t				offset = 0;
	for(u2_t iPart = 0; iPart < countParts; ++iPart) {
		fileNameDst.clear();
		if_fail_fe(gpk::append_strings(fileNameDst, fileNameSrc, '.', gpk::str(iPart)));
		cnst u3_t		partSize = gpk::min(sizeFile - offset, (u3_t)sizePartMax);
		u3_t			partOffset = 0;
		while(partOffset < partSize) {
			cnst u2_t	count = (u2_t)gpk::min(partSize - partOffset, (u3_t)(1024U * 1024U));
			if_fail_fe(gpk::fileToMemory(fileNameSrc, buffer, count, offset));
			if_true_fef(buffer.size() != count, "Short read at offset %" GPK_FMT_U3 ": '%.*s'.", offset, GPK_VIEW_ARGS(fileNameSrc));
			if_fail_fe(gpk::fileFromMemory(fileNameDst, buffer, partOffset != 0));
			partOffset += count;
			offset += count;
		}
	}
	return (gpk::err_t)countParts;
}

// Splits a file into file.## parts; failure retains completed/partial output.
gpk::err_t gpk::fileSplit(vcst_t fileNameSrc, u2_c sizePartMax) {
	if_zero_fef(sizePartMax, "Invalid part size for '%.*s'.", GPK_VIEW_ARGS(fileNameSrc));
	stxp u2_c			sizeSmallFileMax = 3U * 1024U * 1024U * 1024U;
	cnst gpk::string		fileName = fileNameSrc;
	cnst s3_t			sizeFile = gpk::fileSize(fileName);
	if_fail_fef(sizeFile, "Failed to get size for '%.*s'.", GPK_VIEW_ARGS(fileNameSrc));
	if(0 == sizeFile)
		return 0;
	if_true_fef((u3_t)sizeFile / sizePartMax + one_if(sizeFile % sizePartMax) > 0x7FFFFFFFU, "Too many parts for '%.*s'.", GPK_VIEW_ARGS(fileNameSrc));
	return (sizeSmallFileMax > sizeFile) ? ::fileSplitSmall(fileName, sizePartMax) : ::fileSplitLarge(fileName, sizePartMax, (u3_t)sizeFile);
}
// Joins a file split into file.## parts.
gpk::err_t	gpk::	fileJoin				(vcst_t fileNameDst)	{
	string					fileNameSrc				= {};
	::gpk::u2_t				iPart					= 0;
	if_fail_fe(gpk::append_strings(fileNameSrc, fileNameDst, '.', gpk::str(iPart)));
	::FILE					* fpDest					= 0;
	gpk::string				finalPathName				= fileNameDst;
	if_fail_fef(gpk::fopen_s(fpDest, finalPathName, GPK_OPEN_MODE_WRITE), "%s", finalPathName.begin());
	if_true_fef(0 == fpDest, "Failed to create file: %s.", finalPathName.begin());
	gpk::apod<s0_t>			fileInMemory				= {};
	// Load each .split part and write it to the destionation file.
	while(0 == gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file.
		if_true_fef(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %" GPK_FMT_U2 ". File name: %s.", fileInMemory.size(), fileNameSrc.begin());
		fileNameSrc.clear();
		if_fail_fe(gpk::append_strings(fileNameSrc, fileNameDst, '.', gpk::str(++iPart)));
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iPart;
}

#define GPK_DEBUG_FILE_CONTENTS

gpk::err_t	gpk::	fileToMemory			(vcst_t usfileName, gpk::au0_t & fileInMemory, ::gpk::u2_t maxSize, u3_t offset)		{
	gpk::string			fileName					= usfileName;
	gpk_file_info_printf("Loading '%s'.", fileName.begin());

#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	File				fp					= getSoCFileSystem().open(fileName.begin(), GPK_OPEN_MODE_READ);
	if_true_fef(!fp, "Cannot open file: %s.", fileName.begin());
	u2_c				fileSize					= (::gpk::u2_t)fp.size();
	fileInMemory.clear();
#		ifdef GPK_ESP32
	if_fail_fef(fileInMemory.resize(fileSize), "Too large to load in memory? File size: %" GPK_FMT_S2 ". Available memory: %" GPK_FMT_S2 ".", fp.size(), heap_caps_get_free_size(MALLOC_CAP_8BIT));
#		else
	if_fail_fef(fileInMemory.resize(fileSize), "Too large to load in memory? File size: %" GPK_FMT_S2 ". Available memory: %" GPK_FMT_S2 ".", fp.size(), umm_free_heap_size_lw());
#		endif // GPK_ESP32
	if_true_fe(fileSize != (::gpk::u2_t)fp.readBytes((char*)fileInMemory.begin(), fileSize));
	fp.close();
#	endif // GPK_ESP32 || GPK_ESP8266
	gpk::err_t	result				= 0;
#else // !GPK_ARDUINO
	::FILE				* fp				= 0;
	const s2_t				fileErr				= gpk::fopen_s(fp, fileName, ::GPK_OPEN_MODE_READ);
	if_true_vef((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", fileName.begin());
	fseek(fp, 0, SEEK_END);
	const s3_t				fileSize					= gpk::ftell(fp);
	gpk_fail_if_lt(GPK_FMT_LT_S3, fileSize, 0);
	gpk_fail_if_gt(GPK_FMT_GT_U3, offset, (u3_t)fileSize);
	u2_c				maxRead				= (::gpk::u2_t)min((s3_t)maxSize, fileSize - (s3_t)offset);
	gpk_fail_if_ne(GPK_FMT_NE_S3, 0, gpk::fseek(fp, offset, FSEEK_SET));
	gpk::err_t	result				= 0;
	if_failed(fileInMemory.resize(maxRead)) {
		error_printf("File too large? : %llu.", (u3_t)fileSize);
		result					= -1;
	}
	else {
		if(maxRead != fread(fileInMemory.begin(), szof(u0_t), maxRead, fp)) {
			error_printf("fread() failed! file: '%s'.", fileName.begin());
			result					= -1;
		}
	}
	fclose(fp);
#endif // GPK_ARDUINO

#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("'%s' loaded successfully. Size: %" GPK_FMT_U2 ":\n%.*s\n", fileName.begin(), fileInMemory.size(), (int)fileInMemory.size(), (cnst char*)fileInMemory.begin());
#else
	gpk_file_info_printf("'%s' loaded successfully. Size: %" GPK_FMT_U2 ".", fileName.begin(), fileInMemory.size());
#endif // GPK_DEBUG_FILE_CONTENTS
	return result;
}

gpk::err_t	gpk::	fileFromMemory			(vcst_t usfileName, vcu0_t fileInMemory, bool append)	{
	gpk::string			fileName					= usfileName;
#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("%s '%s':\n%.*s\n", append ? "Appending to" : "Writing", fileName.begin(), (int)fileInMemory.size(), (cnst char*)fileInMemory.begin());
#else
	gpk_file_info_printf("%s '%s'.", append ? "Appending to" : "Writing", fileName.begin());
#endif // GPK_DEBUG_FILE_CONTENTS

	gpk::err_t	result				= 0;
#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	File 				fp	 				= ::getSoCFileSystem().open(fileName.begin(), append ? GPK_OPEN_MODE_APPEND : GPK_OPEN_MODE_WRITE);
	if_true_fef(!fp, "Failed to open '%s'.", fileName.begin());
	if_true_fef(!fp.write(fileInMemory.begin(), fileInMemory.size()), "Failed to write to '%s'.", fileName.begin());
#	endif // GPK_ESP32 || GPK_ESP8266
#else // !GPK_ARDUINO
	::FILE				* fp				= 0;
	const s2_t				fileErr				= gpk::fopen_s(fp, fileName, append ? GPK_OPEN_MODE_APPEND : GPK_OPEN_MODE_WRITE);
	if_true_vwf((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Failed to create '%s' for %s.", fileName.begin(), append ? "appending" : "writing");
	if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fp)) {
		error_printf("Failed to write '%s'. Disk full? File size: %" GPK_FMT_U2 ".", fileName.begin(), fileInMemory.size());
		result				= -1;
	}
	fclose(fp);
#endif // GPK_ARDUINO
	gpk_file_info_printf("'%s' written successfully.", fileName.begin());
	return result;
}

gpk::err_t	gpk::	fileDelete				(vcst_t usfileName)	{
	gpk::string 			fileName				= usfileName;
	gpk_file_info_printf("Deleting '%s'.", fileName.begin());
#ifdef GPK_WINDOWS
	if_true_fef(FALSE == DeleteFileA(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#elif defined(GPK_ANDROID)
#elif defined(GPK_ARDUINO)
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	if_true_fef(0 == ::getSoCFileSystem().remove(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#	elif defined(GPK_ATMEL)
#	endif // GPK_ESP32 || GPK_ESP8266
#else
	if_fail_fef(unlink(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#endif
	gpk_file_info_printf("'%s' deleted successfully.", fileName.begin());
	return 0;
}
gpk::err_t	gpk::fileToMemory		(vcst_t folderPath, vcst_t fileName, gpk::au0_t & fileBytes, ::gpk::u2_t maxSize, u3_t offset) {
	gpk::string			filePath			= {};
	if_fail_fef(gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%.*s', fileName: '%.*s'.", (int)folderPath.size(), folderPath.begin(), (int)fileName.size(), fileName.begin());
	if_fail_fef(gpk::fileToMemory(filePath, fileBytes, maxSize, offset), "folderPath: '%.*s', fileName: '%.*s'.", (int)folderPath.size(), folderPath.begin(), (int)fileName.size(), fileName.begin());
	return 0;
}
gpk::err_t	gpk::fileFromMemory		(vcst_t folderPath, vcst_t fileName, vcu0_c fileInMemory, bool append) {
	gpk::string			filePath			= {};
	if_fail_fef(gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%.*s', fileName: '%.*s'.", (int)folderPath.size(), folderPath.begin(), (int)fileName.size(), fileName.begin());
	if_fail_fef(gpk::fileFromMemory(filePath, fileInMemory, append), "folderPath: '%.*s', fileName: '%.*s', append: %s.", (int)folderPath.size(), folderPath.begin(), (int)fileName.size(), fileName.begin(), gpk::b82sc(append));
	return 0;
}
