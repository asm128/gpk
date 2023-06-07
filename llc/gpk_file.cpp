#include "gpk_file.h"
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

int64_t								gpk::fileSize				(const ::gpk::vcc	& fileNameSrc)								{
	FILE									* fp						= 0;
	ree_if(0 != fopen_s(&fp, fileNameSrc.begin(), "rb"), "Failed to open file: %s.", fileNameSrc.begin());
	ree_if(0 == fp, "Failed to open file: %s.", fileNameSrc.begin());
#if defined(GPK_WINDOWS)
	if(0 != _fseeki64(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize					= _ftelli64(fp);
#else
	if(0 != fseek(fp, 0, SEEK_END)) {
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
		fclose(fp);
		return -1;
	}
	const int64_t							fileSize					= ftell(fp);
#endif
	if(0 > fileSize)
		error_printf("%s", "Unknown error reading file: '%s'.", fileNameSrc.begin());
	fclose(fp);
	return fileSize;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	::gpk::error_t	fileSplitSmall				(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
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
		ree_if(0 != fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		uint32_t								countBytes					= (iPart == countParts - 1) ? fileInMemory.size() - offsetPart : sizePartMax;
		ree_if(countBytes != fwrite(&fileInMemory[offsetPart], 1, countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// This function is useful for splitting files smaller than 4gb very quick.
static	::gpk::error_t	fileSplitLarge				(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
	ree_if(0 == sizePartMax, "Invalid part size: %u.", fileNameSrc.begin(), sizePartMax);
	int64_t						sizeFile					= ::gpk::fileSize(fileNameSrc);
	ree_if(errored(sizeFile), "Failed to open file %s.", fileNameSrc.begin());
	FILE						* fp						= 0;
	ree_if(0 == fp, "%s", "Files larger than 3gb still not supported.");
	ree_if(0 != fopen_s(&fp, fileNameSrc.begin(), "rb"), "Failed to open file: %s.", fileNameSrc.begin());
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
		ree_if(0 != fopen_s(&fpDest, fileNameDst, "wb"), "Failed to open file: %s.", fileNameDst);
		ree_if(0 == fpDest, "Failed to create file: %s.", fileNameDst);
		int64_t									countBytes					= (iPart == countParts - 1) ? sizeFile - offsetPart : sizePartMax;
		ree_if(countBytes != (int64_t)fwrite(partInMemory.begin(), 1, (uint32_t)countBytes, fpDest), "Failed to write part %u of %u bytes to disk. Disk full?", iPart, countBytes);
		fclose(fpDest);
	}
	return countParts;
}

// Splits a file into file.## parts.
::gpk::error_t			gpk::fileSplit				(const ::gpk::vcc	& fileNameSrc, const uint32_t sizePartMax) {
	// -- Get file size to determine which algorithm to use.
	// -- For files smaller than 3gb, we use a fast algorithm that loads the entire file in memory.
	// -- For files of, or larger than, 3gb, we use a fast algorithm that loads chunks of 1gb in memory for writing the parts.
	stacxpr	const uint32_t					gigabyte					= 1024U*1024U*1024U;
	stacxpr	const uint32_t					sizeSmallFileMax			= 3U * gigabyte;
	const int64_t							sizeFile					= ::gpk::fileSize(fileNameSrc);
	ree_if(-1 == sizeFile, "Failed to get size for file: '%s'.", fileNameSrc.begin());
	return (sizeSmallFileMax > sizeFile) ? ::fileSplitSmall(fileNameSrc, sizePartMax) : ::fileSplitLarge(fileNameSrc, sizePartMax);
}

// Joins a file split into file.## parts.
::gpk::error_t			gpk::fileJoin				(const ::gpk::vcc	& fileNameDst)	{
	char									fileNameSrc	[1024]			= {};
	uint32_t								iFile						= 0;
	gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", fileNameDst.begin(), iFile++), "File name too large: %s.", fileNameDst.begin());
	FILE									* fpDest					= 0;
	::gpk::apod<char>						finalPathName				= ::gpk::toString(fileNameDst);
	fopen_s(&fpDest, finalPathName.begin(), "wb");
	ree_if(0 == fpDest, "Failed to create file: %s.", finalPathName.begin());
	::gpk::apod<int8_t>						fileInMemory				= {};
	// Load each .split part and write it to the destionation file.
	while(0 == ::gpk::fileToMemory(fileNameSrc, fileInMemory)) {	// Load first part and write it to the joined file.
		ree_if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fpDest), "Write operation failed. Disk full? File size: %u. File name: %s.", fileInMemory.size(), fileNameSrc);
		gpk_necall(snprintf(fileNameSrc, ::gpk::size(fileNameSrc) - 2, "%s.%.2u", finalPathName.begin(), iFile++), "File name too large: %s.", finalPathName.begin());
		fileInMemory.clear();
	}
	fclose(fpDest);
	return iFile - 1;
}

::gpk::error_t			gpk::fileToMemory			(const ::gpk::vcc & fileName, ::gpk::au8 & fileInMemory)		{
	FILE									* fp						= 0;
	int32_t									fileErr						= fopen_s(&fp, ::gpk::toString(fileName).begin(), "rb");
	rvw_if((fileErr > 0) ? -fileErr : fileErr, 0 != fileErr || 0 == fp, "Cannot open file: %s.", ::gpk::toString(fileName).begin());
	fseek(fp, 0, SEEK_END);
	int32_t									fileSize					= (int32_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	::gpk::error_t							result						= 0;
	if errored(fileInMemory.resize(fileSize)) {
		error_printf("File too large? : %llu.", (uint64_t)fileSize);
		result								= -1;
	}
	else {
		if(fileSize != (int32_t)fread(fileInMemory.begin(), sizeof(uint8_t), fileSize, fp)) {
			error_printf("fread() failed! file: '%s'.", ::gpk::toString(fileName).begin());
			result								= -1;
		}
	}
	fclose(fp);
	return result;
}

::gpk::error_t			gpk::fileFromMemory			(const ::gpk::vcc & fileName, const ::gpk::vcu8 & fileInMemory)	{
	FILE									* fp						= 0;
	ree_if(0 != fopen_s(&fp, ::gpk::toString(fileName).begin(), "wb"), "Failed to create file for writing: %s.", ::gpk::toString(fileName).begin());
	ree_if(0 == fp, "Failed to create file for writing: %s.", ::gpk::toString(fileName).begin());
	::gpk::error_t							result						= 0;
	if(fileInMemory.size() != fwrite(fileInMemory.begin(), 1, fileInMemory.size(), fp)) {
		error_printf("Failed to write file. Disk full? File size: %u.", fileInMemory.size());
		result								= -1;
	}
	fclose(fp);
	return result;
}
