#include "gpk_array.h"
#include "gpk_enum.h"
#include "gpk_json.h"
#include "gpk_string.h"

#ifndef GPK_INDEX_H_261109
#define GPK_INDEX_H_261109

nmsp gpk
{
	stxp vcst_c		INDEX_EXCLUSION_DEFAULTS[]	=
		{ "the", "and", "it", "to", "an", "by", "was", "is", "in", "as", "for", "of", "up", "a"
		, "un", "con", "del", "por", "que", "es", "en", "de", "la", "el", "para", "los", "las"
		};

	struct SWordIndices {
		string	Text				= {};
		au2_t	Indices				= {};
	};
	struct SWordDictData {
		aobj<vcst_t>	ItemViews			= {};
		aobj<string>	ItemWords			= {};
	};
	err_t	wordListNormalize	(aobj<string> & itemWords, view<cnst vcst_t> upper, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);
	err_t	getWordList			(aobj<string> & itemWords, view<cnst vcst_t> upper, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);
	err_t	wordDictAppend		(aobj<SWordIndices> & wordSet, u2_t iItem, view<cnst string> itemWords);
	err_t	wordDictResolve		(view<cnst SWordIndices> wordSet, vcst_t input, u2_t itemCount, au2_t & itemIndices, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);

	tplt<tpnm _tProject>
	err_t	getWordDict
		(SWordDictData			& wordData
		, aobj<SWordIndices>	& wordSet
		, u2_t					sourceCount
		, _tProject				&& funcGetItemViews
		, view<vcst_c>			exceptions = INDEX_EXCLUSION_DEFAULTS
		) {
		for(u2_t iSource = 0; iSource < sourceCount; ++iSource) {
			u2_t		itemIndex			= iSource;
			if_fail_fe(wordData.ItemViews.clear());
			if_fail_fe(funcGetItemViews(iSource, itemIndex, wordData.ItemViews));
			if_fail_fe(getWordList(wordData.ItemWords, wordData.ItemViews, exceptions));
			if_fail_fe(wordDictAppend(wordSet, itemIndex, wordData.ItemWords));
		}
		rtrn 0;
	}

	tplt<tpnm _tProject>
	stin err_t	getWordDict
		(aobj<SWordIndices> & wordSet
		, u2_t sourceCount
		, _tProject && project
		, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS
		) {
		SWordDictData		tempCache			= {};
		rtrn getWordDict(tempCache, wordSet, sourceCount, project, exceptions);
	}

} // nmsp 

#endif // GPK_INDEX_H_261109
