#include "gpk_index.h"
#include "gpk_path.h"

#ifndef GPK_INDEX_PATH_H_261109
#define GPK_INDEX_PATH_H_261109

namespace gpk
{
	stxp vcst_c	INDEX_PATH_INCLUSIONS_DEFAULT[]	= {".cpp", ".h", ".txt", ".log"};

	struct SPathIndexData {
		SWordDictData	WordData			= {};
		string			RelativePath		= {};
		string			FilePath			= {};
		string			FileContents		= {};
		bool			IndexPath			= true;
	};

	// reverseMap copies tree.Name views; tree.Char owns their storage and must outlive the map.
	err_t	pathNodePath		(string & output, cnst SPathTree & tree, cnst SPathNode & node);
	err_t	pathWordList		(aobj<string> & itemWords, aobj<vcst_t> & upper, cnst SPathTree & tree, cnst SPathNode & node, view<vcst_c> exceptions);
	err_t	getWordDict			(SPathIndexData & indexData, aobj<SWordIndices> & wordSet, aobj<vcst_t> & reverseMap, cnst SPathTree & tree, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);
	stin err_t	getWordDict		(aobj<SWordIndices> & wordSet, aobj<vcst_t> & reverseMap, cnst SPathTree & tree, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS) { SPathIndexData tempCache; rtrn getWordDict(tempCache, wordSet, reverseMap, tree, exceptions); }
}


#endif // GPK_INDEX_PATH_H_261109
