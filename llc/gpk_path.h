#include "gpk_array_ptr.h"
#include "gpk_string.h"

#ifndef GPK_PATH_H_23627
#define GPK_PATH_H_23627

namespace gpk
{
	stct SPathInfo {
		u0_t					NestLevel	: 7;
		u0_t					Folder		: 1;
	};
	stct SPathNode {
		cnst SPathInfo			* Info				= 0;
		SPathNode				* Parent			= 0;
		apobj<SPathNode>		Children			= {};
		u2_t					ObjectIndex			= (u2_t)-1;
	};
	stct SPathTree {
		string					Root;	// Scan root exactly as supplied by the caller.
		apod<sc_t>				Char;	// store here file names one after the other no null-terminated strings. The path for each file is not stored as it is assumed the concatenation of the parent entries.
		aobj<vcst_t>			Name;	// construct here views from each file name. This avoids allocating each file name as a string
		apod<SPathInfo>			Info;	// this tells whether an entry is a folder and its nest level. An entry with a higher nest level than the previous entry is a child of that entry. Likewise, an entry with a lower nest level than the previous determines the end of the branch and the consequent level of its sibling/parent
		apobj<SPathNode>		Tree;	// the tree is constructed from the SPathInfo data as described above
		
		cnst pobj<SPathNode>&	oper[]	(u2_t index)	cnst	{ rtrn Tree[index]; }
		cnst pobj<SPathNode>*	begin	()				cnst	{ rtrn Tree.begin(); }
		cnst pobj<SPathNode>*	end		()				cnst	{ rtrn Tree.end(); }
		err_t					Reset	()						{ rtrn ::gpk::clear(Name, Info); }
	};

	err_t						pathList				(SPathTree & output, vcst_t pathToList, function<err_t(bool, vcst_c&)> filter);
	err_t						pathList				(SPathTree & output, vcst_t pathToList, vcst_t extension = {});
	//err_t						pathList				(const SPathTree & input, aobj<string> & output, vcst_t extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"

	stct SPathContents {
		aobj<string>			Files					= {};
		aobj<SPathContents>		Folders					= {};
	};
	err_t						pathCreate				(vcst_t folderName, sc_c separator = '/');	// Recursive
	err_t						pathList				(const SPathContents & input, aobj<string> & output, vcst_t extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(const SPathContents & input, aobj<vcst_t> & output, vcst_t extension = {});	// recursively walk over a pathcontents hierarchy and store all the file names into "output"
	err_t						pathList				(vcst_t pathToList, SPathContents & output, vcst_t extension = {});		// Recursive
	stin	err_t				pathList				(vcst_t pathToList, aobj<string> & output, vcst_t extension = {}) {
		SPathContents			tree					= {};
		s2_t							error					= pathList(pathToList, tree, extension);
		if_fail_fe(error |= pathList(tree, output, extension));
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
