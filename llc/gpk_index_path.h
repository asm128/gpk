#include "gpk_index.h"
#include "gpk_path.h"

#ifndef GPK_INDEX_PATH_H_261109
#define GPK_INDEX_PATH_H_261109

namespace gpk
{
	// reverseMap copies tree.Name views; tree.Char owns their storage and must outlive the map.
	err_t	pathWordList		(aobj<string> & itemWords, aobj<vcst_t> & upper, cnst SPathTree & tree, cnst SPathNode & node, view<vcst_c> exceptions);
	err_t	getWordDict			(aobj<SWordIndices> & wordSet, aobj<vcst_t> & reverseMap, cnst SPathTree & tree, aobj<string> & cacheItemWords, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);
}


#endif GPK_INDEX_PATH_H_261109
