#include "gpk_array.h"
#include "gpk_string.h"

#ifndef GPK_PATH_H_23627
#define GPK_PATH_H_23627

namespace gpk
{
	stct SPathContents {
		aobj<string>			Files					= {};
		aobj<SPathContents>	Folders					= {};
	};

	err_t						pathCreate				(vcst_t folderName, sc_c separator = '/');	// Recursive
	err_t						pathList				(const SPathContents & input, aobj<string> & output, vcst_t extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(const SPathContents & input, aobj<vcst_t> & output, vcst_t extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(vcst_t pathToList, SPathContents & output, vcst_t extension = {});		// Recursive
	stin	err_t				pathList				(vcst_t pathToList, aobj<string> & output, vcst_t extension = {}) {
		SPathContents			tree					= {};
		s2_t							error					= pathList(pathToList, tree, extension);
		gpk_necs(error |= pathList(tree, output, extension));
		return 0;
	}	// Recursive
	// this function was ceated in order to work around the problem of the JSON system returning pointers to the original string, without having the opportunity of processing escaped path slashes.
	//err_t						pathNameCompose			(string & out_composed, vcst_t fileName, vcst_t path = {}, vcst_t extension = {});
	err_t						pathNameCompose			(vcst_t path, vcst_t fileName, string & out_composed);
	err_t						pathNormalize			(vcst_t path, string & output, sc_c separator = '/');
	err_t						pathAbsolute			(vcst_t path, string & output, sc_c separator = '/');
	err_t						pathBegin				(vcst_t path, vcst_t & output);
	err_t						findLastSlash			(vcst_t path);
	stin err_t					pathDirectory			(vcst_t path, vcst_t & directory) {
		cnst err_t iSlash = findLastSlash(path);
		rtrn path.slice(directory, 0, 0 <= iSlash ? (u2_t)iSlash + 1 : 0);
	}
	stin err_t					pathFilename			(vcst_t path, vcst_t & filename) {
		cnst err_t iSlash = findLastSlash(path);
		rtrn path.slice(filename, 0 <= iSlash ? (u2_t)iSlash + 1 : 0);
	}
	err_t						pathStem				(vcst_t path, vcst_t & stem);

	err_t						pathList				(vcst_t pathToList, SPathContents & outputTree, function<err_t(bool, vcst_c&)> onItem, gpk::vcst_c extension);
} // namespace

#endif // GPK_PATH_H_23627


