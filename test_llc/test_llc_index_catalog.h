#include "gpk_index_catalog.h"

#ifndef TEST_LLC_INDEX_CATALOG_H
#define TEST_LLC_INDEX_CATALOG_H

stct SIndexCatalog {
	::gpk::SJSONFile					Source					= {};
	::gpk::aobj<::gpk::SItemViews>		Item					= {};
	::gpk::aobj<::gpk::SWordIndices>	Word					= {};
	::gpk::SWordDictData				WordData				= {};

	::gpk::err_t	Load			(::gpk::vcst_t fileName, ::gpk::AD_SHOP_CATEGORY category, ::gpk::vcst_t language);
	::gpk::err_t	Resolve			(::gpk::vcst_t input, ::gpk::au2_t & itemIndices) cnst;
	::gpk::err_t	SourceValue		(::gpk::u2_t itemIndex, ::gpk::vcst_t expression, ::gpk::vcst_t & output) cnst;
};

#endif // TEST_LLC_INDEX_CATALOG_H
