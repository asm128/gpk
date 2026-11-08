#include "gpk_array.h"
#include "gpk_enum.h"
#include "gpk_json.h"
#include "gpk_string.h"

#ifndef SHIP_DATA_H_928734
#define SHIP_DATA_H_928734

nmsp gpk
{
	stxp vcst_c		INDEX_EXCLUSION_DEFAULTS[]	=
		{ "the", "and", "it", "to", "an", "by", "was", "is", "in", "as", "for", "of", "up", "a"
		, "un", "con", "del", "por", "que", "es", "en", "de", "la", "el", "para", "los", "las"
		};

	struct SItemViews {
		u2_t		ItemIndex			= 0;
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
	struct SWordIndices {
		string	Text				= {};
		au2_t	Indices				= {};
	};
	err_t	getWordDict			(aobj<SWordIndices> & wordSet, cnst SItemViews & item, u2_t iItem, aobj<string> & cacheItemWords, view<vcst_c> exceptions = INDEX_EXCLUSION_DEFAULTS);
	err_t	getWordList			(aobj<string> & itemWords, cnst SItemViews & item, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);
	err_t	outputSearchScript	(string & output, view<cnst SItemViews> indicesToDisplay, view<vcst_c> indexExceptions = INDEX_EXCLUSION_DEFAULTS);
} // nmsp 

#endif // SHIP_DATA_H_928734