#include "gpk_array_pod.h"

#ifndef GPK_FILE_H_2983749283
#define GPK_FILE_H_2983749283

namespace gpk
{
	::gpk::error_t			fileJoin		(const ::gpk::vcc & fileNameDst);								// Joins a file split into file.split.## parts.
	::gpk::error_t			fileSplit		(const ::gpk::vcc & fileNameSrc, const uint32_t partSize);		// Splits a file into file.split.## parts.
	int64_t					fileSize		(const ::gpk::vcc & fileNameSrc);
	::gpk::error_t			fileToMemory	(const ::gpk::vcc & fileName, ::gpk::au8 & fileInMemory);
	::gpk::error_t			fileFromMemory	(const ::gpk::vcc & fileName, const ::gpk::vcu8 & fileInMemory);
	stainli	::gpk::error_t	fileToMemory	(const ::gpk::vcc & fileName, ::gpk::ai8 & fileInMemory)		{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t	fileFromMemory	(const ::gpk::vcc & fileName, const ::gpk::vci8 & fileInMemory)	{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory);; }
	stainli	::gpk::error_t	fileToMemory	(const ::gpk::vcc & fileName, ::gpk::ac & fileInMemory)			{ return ::gpk::fileToMemory	(fileName, *(::gpk::au8*)&fileInMemory); }
	stainli	::gpk::error_t	fileFromMemory	(const ::gpk::vcc & fileName, const ::gpk::vcc & fileInMemory)	{ return ::gpk::fileFromMemory	(fileName, *(const ::gpk::vcu8*)&fileInMemory);; }
} // namespace

#endif // GPK_FILE_H_2983749283