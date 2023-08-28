#include "gpk_file.h"
#include "gpk_io.h"
#include "gpk_string.h"
#include "gpk_path.h"

#include <new>

#if defined(GPK_WINDOWS)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <Windows.h>
#elif defined(GPK_ANDROID) || defined(GPK_LINUX)
#	include <dirent.h>
#elif defined(GPK_ESP32)
#	include <SPIFFS.h>
#endif


#define gpk_file_info_printf info_printf

int64_t								gpk::fileSize				(::gpk::vcs fileName)								{
	FILE									* fp						= 0;
	ree_if(0 != ::gpk::fopen_s(&fp, fileName.begin(), "rb"), "Failed to open file: %s.", fileName.begin());
	ree_if(0 == fp, "Failed to open file: %s.", fileName.begin());
#if defined(GPK_WINDOWS)
	if(0 != _fseeki64(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileName.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize					= _ftelli64(fp);
#else
	if(0 != fseek(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileName.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize					= ftell(fp);
#endif
	if(0 > fileSize)
		error_printf("%s", "Unknown error reading file: '%s'.", fileName.begin());
	fclose(fp);
	return fileSize;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	::gpk::error_t	fileSplitSmall				(::gpk::vcs fileNameSrc, const uint32_t sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %u.", fileNameSrc.begin(), sizePartMax);
	::gpk::apod<int8_t>						fileInMemory;
	gpk_necall(::gpk::fileToMemory(fileNameSrc, fileInMemory), "Failed to load file: \"%s\".", fileNameSrc);

	// -- Write parts to disk.
	uint32_t								countParts					= fileInMemory.size() / sizePartMax + one_if(fileInMemory.size() % sizePartMax);
	char									fileNameDst	[1024]			= {};
	uint32_t								iPart						= 0;
	for(; iPart < countParts; ++iPart) {
		const uint32_t							offsetPart					= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, ::gpk::size(fileNameDst) - 2, "%s.%.2u", fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE									* fpDest					= 0;
		ree_if(0 != ::gpk::fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		uint32_t								countBytes					= (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax;
		ree_if(countBytes != fwrite(&fileInMemory[offsetPart], 1, countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	::gpk::error_t	fileSplitLarge				(::gpk::vcs fileNameSrc, const uint32_t sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %u.", fileNameSrc.begin(), sizePartMax);
	int64_t						sizeFile					= ::gpk::fileSize(fileNameSrc);
	ree_if(errored(sizeFile), "Failed to open file %s.", fileNameSrc.begin());
	FILE						* fp						= 0;
	ree_if(0 == fp, "%s", "Files larger than 3gb still not supported.");
	ree_if(0 != ::gpk::fopen_s(&fp, fileNameSrc.begin(), "rb"), "Failed to open file: %s.", fileNameSrc.begin());
	ree_if(0 == fp, "Failed to open file: %s.", fileNameSrc.begin());

	::gpk::ai8					partInMemory;
	gpk_necall(partInMemory.resize(sizePartMax), "Failed to allocate buffer for file part. Out of memory? File part size: %u.", sizePartMax);

	// -- Write parts to disk.
	uint32_t								countParts					= (uint32_t)(sizeFile / sizePartMax + one_if(sizeFile % sizePartMax));
	char									fileNameDst	[1024]			= {};
	uint32_t								iPart						= 0;
	for(; iPart < countParts; ++iPart) { //
		const uint32_t							offsetPart					= sizePartMax * iPart;
		gpk_necall(snprintf(fileNameDst, ::gpk::size(fileNameDst) - 2, "%s.%.2u", fileNameSrc.begin(), iPart), "File name too large: %s.", fileNameSrc.begin());
		info_printf("Creating part %u: '%s'.", iPart, fileNameDst);
		FILE									* fpDest					= 0;
		ree_if(0 != ::gpk::fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		int64_t									countBytes					= (iPart == countParts - 1) ? sizeFile - offsetPart : sizePartMax;
		ree_if(countBytes != (int64_t)fwrite(partInMemory.begin(), 1, (uint32_t)countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// Splits a file into file.## parts.
::gpk::error_t			gpk::fileSplit				(::gpk::vcs fileNameSrc, const uint32_t sizePartMax) {
	// -- Get file size to determine which algorithm to use.
	// -- For files smaller than 3gb, we use a fast algorithm that loads the entire file in memory.
	// -- For files of, or larger than, 3gb, we use a fast algorithm that loads chunks of 1gb in memory for writing the parts.
	stacxpr	const uint32_t		gigabyte					= 1024U*1024U*1024U;
	stacxpr	const uint32_t		sizeSmallFileMax			= 3U * gigabyte;
	const int64_t				sizeFile					= ::gpk::fileSize(fileNameSrc);
	ree_if(-1 == sizeFile, "Failed to get size for file: '%s'.", fileNameSrc.begin());
	return (sizeSmallFileMax > sizeFile) ? ::fileSplitSmall(fileNameSrc, sizePartMax) : ::fileSplitLarge(fileNameSrc, sizePartMax);
}

// Joins a file split into file.## parts.
::gpk::error_t			gpk::fileJoin				(::gpk::vcs fileNameDst)	{
	char						fileNameSrc	[1024]			= {};
	uint32_t					iFile						= 0;
	gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", fileNameDst.begin(), iFile++), "File name too large: %s.", fileNameDst.begin());
	FILE						* fpDest					= 0;
	::gpk::apod<char>			finalPathName				= ::gpk::toString(fileNameDst);
	::gpk::fopen_s(&fpDest, finalPathName.begin(), "wb");
	ree_if(0 == fpDest, "Failed to create file: %s.", finalPathName.begin());
	::gpk::apod<int8_t>			fileInMemory				= {};
	// Load each .split part and write it to the destionation file.
	while(0 == ::gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file.
		ree_if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %u. File name: %s.", fileInMemory.size(), fileNameSrc);
		gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", finalPathName.begin(), iFile++), "File name too large: %s.", finalPathName.begin());
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iFile - 1;
}


#define GPK_DEBUG_FILE_CONTENTS

::gpk::error_t			gpk::fileToMemory			(::gpk::vcs usfileName, ::gpk::au8 & fileInMemory)		{
	const ::gpk::achar			fileName					= ::gpk::toString(usfileName);
	gpk_file_info_printf("Loading '%s'.", fileName.begin());

	::gpk::error_t				result						= 0;
#ifdef GPK_ESP32
	File						fp							= SPIFFS.open(fileName.begin());
	ree_if(!fp, "Cannot open file: %s.", fileName.begin());
	const int32_t				fileSize					= (int32_t)fp.size();
	fileInMemory.clear();
	gpk_necs(fileInMemory.resize(fileSize), "Too large to load in memory? File size: %i. Available memory: %i.", fp.size(), heap_caps_get_free_size(MALLOC_CAP_8BIT));
	rees_if(fileSize != fp.readBytes((char*)fileInMemory.begin(), fileSize));
	fp.close();
#else
	FILE						* fp						= 0;
	const int32_t				fileErr						= ::gpk::fopen_s(&fp, fileName.begin(), "rb");
	rvw_if((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", fileName.begin());
	fseek(fp, 0, SEEK_END);
	const int32_t				fileSize					= (int32_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (errored(fileInMemory.resize(fileSize))) {
		error_printf("File too large? : %llu.", (uint64_t)fileSize);
		result					= -1;
	}
	else {
		if(fileSize != (int32_t)fread(fileInMemory.begin(), sizeof(uint8_t), fileSize, fp)) {
			error_printf("fread() failed! file: '%s'.", fileName.begin());
			result					= -1;
		}
	}
	fclose(fp);
#endif

#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("'%s' loaded successfully. Size: %u:\n%s\n", fileName.begin(), fileInMemory.size(), ::gpk::toString(fileInMemory).begin());
#else
	gpk_file_info_printf("'%s' loaded successfully. Size: %u.", fileName.begin(), fileInMemory.size());
#endif
	return result;
}

::gpk::error_t			gpk::fileFromMemory			(::gpk::vcs usfileName, const ::gpk::vcu8 & fileInMemory, bool append)	{
	const ::gpk::achar			fileName					= ::gpk::toString(usfileName);
#ifdef GPK_DEBUG_FILE_CONTENTS
	gpk_file_info_printf("%s '%s':\n%s\n", append ? "Appending to" : "Writing", fileName.begin(), ::gpk::toString(fileInMemory).begin());
#else
	gpk_file_info_printf("%s '%s'.", append ? "Appending to" : "Writing", fileName.begin());
#endif

	::gpk::error_t				result						= 0;
#ifdef GPK_ESP32
	File 						fp	 						= SPIFFS.open(fileName.begin(), append ? FILE_APPEND : FILE_WRITE);
	ree_if(!fp, "Failed to open '%s'.", fileName.begin());
	ree_if(!fp.write(fileInMemory.begin(), fileInMemory.size()), "Failed to write to '%s'.", fileName.begin());
#else
	FILE						* fp						= 0;
	const int32_t				fileErr						= ::gpk::fopen_s(&fp, fileName.begin(), append ? "ab" : "wb");
	rvw_if((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Failed to create '%s' for %s.", fileName.begin(), append ? "appending" : "writing");
	if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fp)) {
		error_printf("Failed to write '%s'. Disk full? File size: %u.", fileName.begin(), fileInMemory.size());
		result						= -1;
	}
	fclose(fp);
#endif

	gpk_file_info_printf("'%s' written successfully.", fileName.begin());
	return result;
}

::gpk::error_t			gpk::fileDelete				(::gpk::vcs usfileName)	{
	const ::gpk::achar			fileName					= ::gpk::toString(usfileName);
	gpk_file_info_printf("Deleting '%s'.", fileName.begin());

#ifdef GPK_ESP32
	ree_if(0 == SPIFFS.remove(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#elif defined(GPK_WINDOWS)
	ree_if(FALSE == DeleteFileA(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#elif defined(GPK_ANDROID)
#elif defined(GPK_ATMEL)
#else
	gpk_necall(unlink(fileName.begin()), "Failed to delete '%s'.", fileName.begin());
#endif

	gpk_file_info_printf("'%s' deleted successfully.", fileName.begin());
	return 0;
}

::gpk::error_t			gpk::fileToMemory	(::gpk::vcs folderPath, ::gpk::vcs fileName, ::gpk::au8 & fileBytes) {
	::gpk::achar			filePath			= {};
	gpk_necall(::gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	gpk_necall(::gpk::fileToMemory({filePath}, fileBytes), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	return 0;
}


::gpk::error_t			gpk::fileFromMemory	(::gpk::vcs folderPath, ::gpk::vcs fileName, const ::gpk::vcu8 & fileInMemory, bool append) {
	::gpk::achar			filePath			= {}; 
	gpk_necall(::gpk::pathNameCompose(folderPath, fileName, filePath), "folderPath: '%s', fileName: '%s'.", folderPath.begin(), fileName.begin());
	gpk_necall(::gpk::fileFromMemory({filePath}, fileInMemory, append), "folderPath: '%s', fileName: '%s', append: %s.", folderPath.begin(), fileName.begin(), ::gpk::bool2char(append));
	return 0;
}

