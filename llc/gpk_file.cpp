#include "gpk_file.h"
#include "gpk_cstdio.h"
#include "gpk_cstring.h"
#include "gpk_path.h"
#include "gpk_string_compose.h"

#include <new>

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

gpk::err_t		gpk::with			(vcst_t filepath, vcst_t mode, const gpk::function<gpk::err_t(FILE*&)> & funcFile)	{ 
	FILE					* fp				= {};
	gpk_necs(gpk::fopen_s(&fp, filepath, mode)); if_null_ve(-1, fp);
	gpk::err_t	result;
	if_fail_ef(result = funcFile(fp), "filepath:'%s'.", filepath.begin());
	if(fp)
		fclose(fp);
	return result;
}

stxp gpk::vcst_t	GPK_OPEN_MODE_READ		= GPK_CXS("rb");
stxp gpk::vcst_t	GPK_OPEN_MODE_WRITE		= GPK_CXS("wb");
stxp gpk::vcst_t	GPK_OPEN_MODE_APPEND	= GPK_CXS("ab+");

#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(GPK_ESP8266)
stin	fs::FS&		getSoCFileSystem	()	{ rtrn GPK_SOC_FILESYSTEM_INSTANCE; }
sttc  	gpk::err_t	socPath  			(gpk::asc_t & fixed, gpk::vcsc_t path) {
	gpk::vcsc_t			trimmd;
	if_fail_fe(gpk::ltrim(trimmd, path, "/ \t\n"));
	rtrn gpk::append_strings(fixed, '/', trimmd);
}
#		define FIX_SOC_PATH(_pathView)                      {   \
	if_zero_fe(_pathView.size());                           	\
	asc_t      _fixedPath;                            			\
	if('/' != _pathView[0]) {                               	\
		if_fail_fe(::socPath(_fixedPath, _pathView)); 			\
		_pathView = _fixedPath;                       			\
	}															\
}
#	endif
#endif // GPK_ARDUINO

#define gpk_file_info_printf(...) do {} while(0) // info_printf

s3_t	gpk::	fileSize			(gpk::vcst_t usfileName)			{
	gpk::asc_t			fileName					= gpk::toString(usfileName);
#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(GPK_ESP8266)
	FIX_SOC_PATH(fileName);
	File				fp						= getSoCFileSystem().open(fileName.begin(), GPK_OPEN_MODE_READ);
	ree_if(!fp, "Cannot open file: %s.", fileName.begin());
	u2_c				fileSize				= (uint32_t)fp.size();
#	endif // GPK_ESP32
#else
	::FILE				* fp					= 0;
	gpk_necall(gpk::fopen_s(&fp, fileName, ::GPK_OPEN_MODE_READ), "Failed to open '%s'.", fileName.begin());
	ree_if(0 == fp, "Failed to open '%s'.", fileName.begin());
	if(0 != gpk::fseek(fp, 0, FSEEK_END)) {
		error_printf("%s", "Unknown error reading '%s'.", fileName.begin());
		fclose(fp);
		return -1;
	}
	const s3_t			fileSize				= gpk::ftell(fp);
	if(0 > fileSize)
		error_printf("%s", "Unknown error reading '%s'.", fileName.begin());
	fclose(fp);
#endif // GPK_ARDUINO
	return fileSize;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	gpk::err_t	fileSplitSmall	(gpk::vcst_t fileNameSrc, u2_c sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %" GPK_FMT_U2 ".", fileNameSrc.begin(), sizePartMax);
	gpk::apod<int8_t>				fileInMemory;
	gpk_necall(gpk::fileToMemory(fileNameSrc, fileInMemory), "Failed to load '%s'.", fileNameSrc);

	// -- Write parts to disk.
	uint32_t			countParts			= fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	char				fileNameDst	[1024]	= {};
	uint32_t			iPart				= 0;
	for(; iPart < countParts; ++iPart) {
		u2_c					offsetPart					= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, gpk::size(fileNameDst) - 2, "%s.%.2" GPK_FMT_U2, fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %" GPK_FMT_U2 ": '%s'.", iPart, fileNameDst);
		FILE				* fpDest					= 0;
		ree_if(0 != gpk::fopen_s(&fpDest, fileNameDst, GPK_OPEN_MODE_WRITE), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		uint32_t			countBytes					= (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax;
		ree_if(countBytes != fwrite(&fileInMemory[offsetPart], 1, countBytes, fpDest), "Failed to write part %" GPK_FMT_U2 " of %" GPK_FMT_U2 " bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	gpk::err_t	fileSplitLarge				(gpk::vcst_t fileNameSrc, u2_c sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %" GPK_FMT_U2 ".", fileNameSrc.begin(), sizePartMax);
	s3_t				sizeFile;
	if_fail_vef(-1, sizeFile = gpk::fileSize(fileNameSrc), "Failed to open file %s.", fileNameSrc.begin());
	FILE				* fp				= 0;
	ree_if(0 == fp, "%s", "Files larger than 3gb still not supported.");
	ree_if(0 != gpk::fopen_s(&fp, {fileNameSrc}, GPK_OPEN_MODE_READ), "Failed to open '%s'.", fileNameSrc.begin());
	ree_if(0 == fp, "Failed to open '%s'.", fileNameSrc.begin());

	gpk::as0_t					partInMemory;
	gpk_necall(partInMemory.resize(sizePartMax), "Failed to allocate buffer for file part. Out of memory? File part size: %" GPK_FMT_U2 ".", sizePartMax);

	// -- Write parts to disk.
	uint32_t			countParts					= (uint32_t)(sizeFile / sizePartMax + one_if(sizeFile % sizePartMax));
	char				fileNameDst	[1024]			= {};
	uint32_t			iPart				= 0;
	for(; iPart < countParts; ++iPart) { //
		u2_c					offsetPart					= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, gpk::size(fileNameDst) - 2, "%s.%.2" GPK_FMT_U2, fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %" GPK_FMT_U2 ": '%s'.", iPart, fileNameDst);
		FILE				* fpDest					= 0;
		ree_if(0 != gpk::fopen_s(&fpDest, fileNameDst, GPK_OPEN_MODE_WRITE), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		s3_t				countBytes					= (iPart == countParts - 1) ? sizeFile - offsetPart : sizePartMax;
		ree_if(countBytes != (s3_t)fwrite(partInMemory.begin(), 1, (uint32_t)countBytes, fpDest), "Failed to write part %" GPK_FMT_U2 " of %" GPK_FMT_U2 " bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// Splits a file into file.## parts.
gpk::err_t	gpk::	fileSplit				(vcst_t fileNameSrc, u2_c sizePartMax) {
	// -- Get file size to determine which algorithm to use.
	// -- For files smaller than 3gb, we use a fast algorithm that loads the entire file in memory.
	// -- For files of, or larger than, 3gb, we use a fast algorithm that loads chunks of 1gb in memory for writing the parts.
	stxp	u2_c		gigabyte					= 1024U*1024U*1024U;
	stxp	u2_c		sizeSmallFileMax			= 3U * gigabyte;
	const s3_t				sizeFile					= gpk::fileSize(fileNameSrc);
	ree_if(-1 == sizeFile, "Failed to get size for file: '%s'.", fileNameSrc.begin());
	return (sizeSmallFileMax > sizeFile) ? ::fileSplitSmall(fileNameSrc, sizePartMax) : ::fileSplitLarge(fileNameSrc, sizePartMax);
}
// Joins a file split into file.## parts.
gpk::err_t	gpk::	fileJoin				(vcst_t fileNameDst)	{
	char				fileNameSrc	[1024]			= {};
	uint32_t					iFile				= 0;
	gpk_necall(snprintf(fileNameSrc, gpk::size(fileNameSrc) - 2, "%s.%.2" GPK_FMT_U2, str(fileNameDst).begin(), ++iFile), "File name too large: %s.", fileNameDst.begin());
	::FILE				* fpDest					= 0;
	gpk::apod<char>			finalPathName				= gpk::toString(fileNameDst);
	gpk_necall(gpk::fopen_s(&fpDest, finalPathName, GPK_OPEN_MODE_WRITE), "%s", finalPathName.begin());
	ree_if(0 == fpDest, "Failed to create file: %s.", finalPathName.begin());
	gpk::apod<int8_t>			fileInMemory				= {};
	// Load each .split part and write it to the destionation file.
	while(0 == gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file.
		ree_if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %" GPK_FMT_U2 ". File name: %s.", fileInMemory.size(), fileNameSrc);
		gpk_necall(snprintf(fileNameSrc, gpk::size(fileNameSrc) - 2, "%s.%.2" GPK_FMT_U2, finalPathName.begin(), ++iFile), "File name too large: %s.", finalPathName.begin());
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iFile - 1;
}

#define GPK_DEBUG_FILE_CONTENTS

gpk::err_t	gpk::	fileToMemory			(vcst_t usfileName, gpk::au0_t & fileInMemory, uint32_t maxSize, uint64_t offset)		{
	gpk::asc_t			fileName					= gpk::toString(usfileName);
	gpk_file_info_printf("Loading '%s'.", fileName.begin());

#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	File				fp					= getSoCFileSystem().open(fileName.begin(), GPK_OPEN_MODE_READ);
	ree_if(!fp, "Cannot open file: %s.", fileName.begin());
	u2_c				fileSize					= (uint32_t)fp.size();
	fileInMemory.clear();
#		ifdef GPK_ESP32
	gpk_necall(fileInMemory.resize(fileSize), "Too large to load in memory? File size: %" GPK_FMT_S2 ". Available memory: %" GPK_FMT_S2 ".", fp.size(), heap_caps_get_free_size(MALLOC_CAP_8BIT));
#		else
	gpk_necall(fileInMemory.resize(fileSize), "Too large to load in memory? File size: %" GPK_FMT_S2 ". Available memory: %" GPK_FMT_S2 ".", fp.size(), umm_free_heap_size_lw());
#		endif // GPK_ESP32
	rees_if(fileSize != (uint32_t)fp.readBytes((char*)fileInMemory.begin(), fileSize));
	fp.close();
#	endif // GPK_ESP32 || GPK_ESP8266
	gpk::err_t	result				= 0;
#else // !GPK_ARDUINO
	::FILE				* fp				= 0;
	const int32_t				fileErr				= gpk::fopen_s(&fp, fileName, ::GPK_OPEN_MODE_READ);
	rvw_if((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", fileName.begin());
	fseek(fp, 0, SEEK_END);
	const s3_t				fileSize					= gpk::ftell(fp);
	fail_if_lt3s(fileSize, 0);
	fail_if_ge3u(offset, fileSize);
	u2_c				maxRead				= (uint32_t)min((s3_t)maxSize, fileSize - (s3_t)offset);
	fail_if_ne3u(0, gpk::fseek(fp, offset, FSEEK_SET));
	gpk::err_t	result				= 0;
	if_failed(fileInMemory.resize(maxRead)) {
		error_printf("File too large? : %llu.", (uint64_t)fileSize);
		result					= -1;
	}
	else {
		if(fileSize != (int32_t)fread(fileInMemory.begin(), szof(uint8_t), fileSize, fp)) {
			error_printf("fread() failed! file: '%s'.", fileName.begin());
			result					= -1;
		}
	}
	fclose(fp);
#endif // GPK_ARDUINO

#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("'%s' loaded successfully. Size: %" GPK_FMT_U2 ":\n%s\n", fileName.begin(), fileInMemory.size(), fileInMemory.size() ? fileInMemory.begin() : (const uint8_t*)"");
#else
	gpk_file_info_printf("'%s' loaded successfully. Size: %" GPK_FMT_U2 ".", fileName.begin(), fileInMemory.size());
#endif // GPK_DEBUG_FILE_CONTENTS
	return result;
}

gpk::err_t	gpk::	fileFromMemory			(vcst_t usfileName, vcu0_c & fileInMemory, bool append)	{
	gpk::asc_t			fileName					= gpk::toString(usfileName);
#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("%s '%s':\n%s\n", append ? "Appending to" : "Writing", fileName.begin(), fileInMemory.size() ? fileInMemory.begin() : (const uint8_t*)"");
#else
	gpk_file_info_printf("%s '%s'.", append ? "Appending to" : "Writing", fileName.begin());
#endif // GPK_DEBUG_FILE_CONTENTS

	gpk::err_t	result				= 0;
#ifdef GPK_ARDUINO
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	File 				fp	 				= ::getSoCFileSystem().open(fileName.begin(), append ? GPK_OPEN_MODE_APPEND : GPK_OPEN_MODE_WRITE);
	ree_if(!fp, "Failed to open '%s'.", fileName.begin());
	ree_if(!fp.write(fileInMemory.begin(), fileInMemory.size()), "Failed to write to '%s'.", fileName.begin());
#	endif // GPK_ESP32 || GPK_ESP8266
#else // !GPK_ARDUINO
	::FILE				* fp				= 0;
	const int32_t				fileErr				= gpk::fopen_s(&fp, fileName, append ? GPK_OPEN_MODE_APPEND : GPK_OPEN_MODE_WRITE);
	rvw_if((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Failed to create '%s' for %s.", fileName.begin(), append ? "appending" : "writing");
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
	gpk::asc_t	fileName					= gpk::toString(usfileName);
	gpk_file_info_printf("Deleting '%s'.", fileName.begin());
#ifdef GPK_WINDOWS
	ree_if(FALSE == DeleteFileA(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#elif defined(GPK_ANDROID)
#elif defined(GPK_ARDUINO)
#	if defined(GPK_ESP32) || defined(ESP8266)
	FIX_SOC_PATH(fileName);
	ree_if(0 == ::getSoCFileSystem().remove(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#	elif defined(GPK_ATMEL)
#	endif // GPK_ESP32 || GPK_ESP8266
#else
	gpk_necall(unlink(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#endif
	gpk_file_info_printf("'%s' deleted successfully.", fileName.begin());
	return 0;
}
gpk::err_t	gpk::	fileToMemory	(vcst_t folderPath, vcst_t fileName, gpk::au0_t & fileBytes, uint32_t maxSize, uint64_t offset) {
	gpk::asc_t			filePath			= {};
	gpk_necall(gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	gpk_necall(gpk::fileToMemory({filePath}, fileBytes, maxSize, offset), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	return 0;
}
gpk::err_t	gpk::	fileFromMemory	(vcst_t folderPath, vcst_t fileName, vcu0_c & fileInMemory, bool append) {
	gpk::asc_t			filePath			= {}; 
	gpk_necall(gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	gpk_necall(gpk::fileFromMemory({filePath}, fileInMemory, append), "folderPath: '%s', fileName: '%s', append: %s.", folderPath.begin(), fileName.begin(), gpk::bool2char(append));
	return 0;
}
