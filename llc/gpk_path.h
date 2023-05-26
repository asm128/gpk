#include "gpk_array.h"

#ifndef GPK_PATH_H_2983749283
#define GPK_PATH_H_2983749283

namespace gpk
{
	struct SPathContents {
		::gpk::aobj<::gpk::apod<char>>		Files					= {};
		::gpk::aobj<::gpk::SPathContents>	Folders					= {};
	};

	::gpk::error_t						pathCreate				(const ::gpk::vcc & folderName, const char separator = '/');	// Recursive
	::gpk::error_t						pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char>> & output, bool listFolders, const ::gpk::vcc extension = {});		// Not recursive
	::gpk::error_t						pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::apod<char>> & output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	::gpk::error_t						pathList				(const ::gpk::SPathContents & input, ::gpk::aobj<::gpk::vcc> & output, const ::gpk::vcc extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	::gpk::error_t						pathList				(const ::gpk::vcc & pathToList, SPathContents & out_Contents, const ::gpk::vcc extension = {});		// Recursive
	stainli	::gpk::error_t				pathList				(const ::gpk::vcc & pathToList, ::gpk::aobj<::gpk::apod<char>>& output, const ::gpk::vcc extension = {}) {
		::gpk::SPathContents					tree					= {};
		int32_t									error					= ::gpk::pathList(pathToList, tree, extension);
		gpk_necs(error |= ::gpk::pathList(tree, output, extension));
		return 0;
	}	// Recursive

	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
	::gpk::error_t						pathNameCompose			(const ::gpk::vcc & path, const ::gpk::vcc & fileName, ::gpk::apod<char> & out_composed);
	::gpk::error_t						findLastSlash			(const ::gpk::vcc & path);

} // namespace

#endif // GPK_PATH_H_2983749283
