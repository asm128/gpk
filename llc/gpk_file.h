#include "gpk_array_pod.h"

#ifndef GPK_FILE_H_23627
#define GPK_FILE_H_23627

namespace gpk
{
	::gpk::error_t			fileJoin		(::gpk::vcs fileNameDst);								// Joins a file split into file.split.## parts.
	::gpk::error_t			fileSplit		(::gpk::vcs fileNameSrc, const uint32_t partSize);		// Splits a file into file.split.## parts.
	int64_t					fileSize		(::gpk::vcs fileName);
	::gpk::error_t			fileDelete		(::gpk::vcs fileName);
	::gpk::error_t			fileFromMemory	(::gpk::vcs fileName, const ::gpk::vcu8 & fileInMemory, bool append = false);
	::gpk::error_t			fileToMemory	(::gpk::vcs fileName, ::gpk::au8 & fileInMemory);
	::gpk::error_t			fileToMemory	(::gpk::vcs folderPath, ::gpk::vcs fileName, ::gpk::au8 & fileBytes);
	::gpk::error_t			fileFromMemory	(::gpk::vcs folderPath, ::gpk::vcs fileName, const ::gpk::vcu8 & fileInMemory, bool append = false);

	stainli	::gpk::error_t	fileToMemory	(::gpk::vcs fileName, ::gpk::ai8 & fileInMemory)								{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t	fileFromMemory	(::gpk::vcs fileName, const ::gpk::vci8 & fileInMemory, bool append = false)	{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory, append);; }
	stainli	::gpk::error_t	fileToMemory	(::gpk::vcs fileName, ::gpk::ac & fileInMemory)									{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t	fileFromMemory	(::gpk::vcs fileName, const ::gpk::vcc & fileInMemory, bool append = false)		{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory, append);; }
} // namespace

#endif // GPK_FILE_H_23627
