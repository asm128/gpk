#include "gpk_array.h"
#include "gpk_label.h"

#ifndef GPK_STORAGE_H_2983749283
#define GPK_STORAGE_H_2983749283

namespace gpk
{
	struct SPathContents {
						::gpk::aobj<::gpk::apod<char_t>>		Files					= {};
						::gpk::aobj<::gpk::SPathContents>			Folders					= {};
	};

					::gpk::error_t									pathList				(const ::gpk::SPathContents& input, ::gpk::aobj<::gpk::apod<char_t>>& output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
					::gpk::error_t									pathList				(const ::gpk::SPathContents& input, ::gpk::aobj<::gpk::vcc>& output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
					::gpk::error_t									pathList				(const ::gpk::vcc & pathToList, SPathContents & out_Contents, const ::gpk::vcc extension = {});		// Recursive
	static inline	::gpk::error_t									pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char_t>>& output, const ::gpk::vcc extension = {}) {
		::gpk::SPathContents												tree					= {};
		int32_t error = ::gpk::pathList(pathToList, tree, extension);
		gpk_necs(error |= ::gpk::pathList(tree, output, extension));
		return 0;
	}	// Recursive
					::gpk::error_t									pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char_t>>& output, bool listFolders, const ::gpk::vcc extension = {});		// Not recursive
					::gpk::error_t									fileToMemory			(const ::gpk::vcc & fileName, ::gpk::apod<byte_t>& fileInMemory);
					::gpk::error_t									fileFromMemory			(const ::gpk::vcc & fileName, const ::gpk::vcb & fileInMemory);
					::gpk::error_t									fileJoin				(const ::gpk::vcc & fileNameDst);	// Joins a file split into file.split.## parts.
					::gpk::error_t									fileSplit				(const ::gpk::vcc & fileNameSrc, const uint32_t partSize);	// Splits a file into file.split.## parts.
					int64_t											fileSize				(const ::gpk::vcc & fileNameSrc);
					::gpk::error_t									pathCreate				(const ::gpk::vcc & folderName, const char separator = '/');	// Recursive

	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
					::gpk::error_t									pathNameCompose			(::gpk::vcc path, ::gpk::vcc fileName, ::gpk::apod<char_t> & out_composed);
					::gpk::error_t									findLastSlash			(const ::gpk::vcc & path);


} // namespace

#endif // GPK_STORAGE_H_2983749283
