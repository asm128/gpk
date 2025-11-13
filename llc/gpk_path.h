#include "gpk_array.h"

#ifndef GPK_PATH_H_23627
#define GPK_PATH_H_23627

namespace gpk
{
	stct SPathContents {
		::gpk::aasc_t				Files					= {};
		::gpk::aobj<SPathContents>	Folders					= {};
	};

	err_t						pathCreate				(::gpk::vcst_c & folderName, sc_c separator = '/');	// Recursive
	err_t						pathList				(::gpk::vcst_c & pathToList, ::gpk::aasc_t & output, bool listFolders, ::gpk::vcst_c extension = {});		// Not recursive
	err_t						pathList				(const ::gpk::SPathContents & input, ::gpk::aasc_t	& output, ::gpk::vcst_c extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(const ::gpk::SPathContents & input, ::gpk::avcsc_t	& output, ::gpk::vcst_c extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(::gpk::vcst_c & pathToList, SPathContents & output, ::gpk::vcst_c extension = {});		// Recursive
	stin	err_t				pathList				(::gpk::vcst_c & pathToList, ::gpk::aachar & output, ::gpk::vcst_c extension = {}) {
		::gpk::SPathContents			tree					= {};
		s2_t							error					= ::gpk::pathList(pathToList, tree, extension);
		gpk_necs(error |= ::gpk::pathList(tree, output, extension));
		return 0;
	}	// Recursive
	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
	//err_t						pathNameCompose			(::gpk::asc_t & out_composed, ::gpk::vcst_t fileName, ::gpk::vcst_t path = {}, ::gpk::vcst_t extension = {});
	err_t						pathNameCompose			(::gpk::vcsc_c & path, ::gpk::vcsc_c & fileName, ::gpk::asc_t & out_composed);
	err_t						findLastSlash			(::gpk::vcsc_c & path);

	err_t						pathList				(::gpk::vcst_c & pathToList, ::gpk::SPathContents & outputTree, ::gpk::function<err_t(bool, vcst_c&)> onItem, gpk::vcst_c extension);
} // namespace

#endif // GPK_PATH_H_23627
