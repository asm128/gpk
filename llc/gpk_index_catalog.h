#include "gpk_index.h"

#ifndef GPK_INDEX_CATALOG_H_261109
#define GPK_INDEX_CATALOG_H_261109

nmsp gpk
{
	struct SItemViews {
		u2_t			ItemIndex			= 0;
		vcst_t			URL					= {};
		vcst_t			Name				= {};
		vcst_t			Title				= {};
		vcst_t			Text				= {};
		vcst_t			ImageHRef			= {};
		vcst_t			ImageSrc			= {};
		vcst_t			ImageTitle			= {};
		vcst_t			ImageAlt			= {};
		vcst_t			MapURL				= {};
		au2_t			Barrio				= {};
		aobj<vcst_t>	Phones				= {};
		aobj<vcst_t>	Addresses			= {};
		aobj<vcst_t>	WPs					= {};
	};

	GDEFINE_ENUM_TYPE(AD_SHOP_CATEGORY, u0_t);
	GDEFINE_ENUM_VALUE(AD_SHOP_CATEGORY, tours, 0);
	GDEFINE_ENUM_VALUE(AD_SHOP_CATEGORY, meals, 1);
	GDEFINE_ENUM_VALUE(AD_SHOP_CATEGORY, shops, 2);
	GDEFINE_ENUM_VALUE(AD_SHOP_CATEGORY, shows, 3);

	err_t	getItemViews		(SJSONReader & content, cnst AD_SHOP_CATEGORY category, vcst_t /*lang*/, aobj<SItemViews> & indicesToDisplay);

	struct SBarrio {
		vcst_t	Name				= {};
		vcst_t	Area				= {};
		vcst_t	Population			= {};
		vcst_t	Commune				= {};
	};

	err_t		getWordDict			(SWordDictData & wordData, aobj<SWordIndices> & wordSet, cnst SItemViews & item, u2_t iItem, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);
	stin err_t	getWordDict			(aobj<SWordIndices> & wordSet, cnst SItemViews & item, u2_t iItem, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS) { SWordDictData tempCache; rtrn getWordDict(tempCache, wordSet, item, iItem, exceptions); }
	err_t		getWordDict			(SWordDictData & wordData, aobj<SWordIndices> & wordSet, view<cnst SItemViews> items, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);
	stin err_t	getWordDict			(aobj<SWordIndices> & wordSet, view<cnst SItemViews> items, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS) { SWordDictData tempCache; rtrn getWordDict(tempCache, wordSet, items, exceptions); }
	err_t		getWordList			(aobj<string> & itemWords, cnst SItemViews & item, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);
	err_t		outputSearchScript	(string & output, view<cnst SItemViews> indicesToDisplay, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);



} // namespace

#endif // 
