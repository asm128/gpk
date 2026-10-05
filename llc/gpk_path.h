#include "gpk_array.h"

#ifndef GPK_PATH_H_23627
#define GPK_PATH_H_23627

namespace gpk
{
	stct SPathContents {
		aasc_t				Files					= {};
		aobj<SPathContents>	Folders					= {};
	};

	err_t						pathCreate				(vcst_c & folderName, sc_c separator = '/');	// Recursive
	err_t						pathList				(vcst_c & pathToList, aasc_t & output, bool listFolders, vcst_c extension = {});		// Not recursive
	err_t						pathList				(const SPathContents & input, aasc_t	& output, vcst_c extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(const SPathContents & input, avcsc_t	& output, vcst_c extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(vcst_c & pathToList, SPathContents & output, vcst_c extension = {});		// Recursive
	stin	err_t				pathList				(vcst_c & pathToList, aachar & output, vcst_c extension = {}) {
		SPathContents			tree					= {};
		s2_t							error					= pathList(pathToList, tree, extension);
		gpk_necs(error |= pathList(tree, output, extension));
		return 0;
	}	// Recursive
	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
	//err_t						pathNameCompose			(asc_t & out_composed, vcst_t fileName, vcst_t path = {}, vcst_t extension = {});
	err_t						pathNameCompose			(vcsc_c & path, vcsc_c & fileName, asc_t & out_composed);
	err_t						pathNormalize			(vcsc_c & path, asc_t & output, sc_c separator = '/');
	err_t						pathAbsolute			(vcsc_c & path, asc_t & output, sc_c separator = '/');
	err_t						pathBegin				(vcsc_c & path, vcsc_t & output);
	err_t						findLastSlash			(vcsc_c & path);
	stin err_t					pathDirectory			(vcsc_t path, vcsc_t & directory) {
		cnst err_t iSlash = findLastSlash(path);
		rtrn path.slice(directory, 0, 0 <= iSlash ? (u2_t)iSlash + 1 : 0);
	}
	stin err_t					pathFilename			(vcsc_t path, vcsc_t & filename) {
		cnst err_t iSlash = findLastSlash(path);
		rtrn path.slice(filename, 0 <= iSlash ? (u2_t)iSlash + 1 : 0);
	}
	err_t						pathStem				(vcsc_t path, vcsc_t & stem);

	err_t						pathList				(vcst_c & pathToList, SPathContents & outputTree, function<err_t(bool, vcst_c&)> onItem, gpk::vcst_c extension);
} // namespace

#endif // GPK_PATH_H_23627
