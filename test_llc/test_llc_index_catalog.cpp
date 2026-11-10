#include "test_llc_index_catalog.h"

#include "gpk_file.h"
#include "gpk_json_expression.h"
#include "test_llc_suites.h"

GDEFINE_ENUM_TYPE(INDEX_CATALOG_TEST_RESULT, ::gpk::u0_t);
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, LOAD_RESULT		, 0, "The Obelisco catalog consumer could not load ads.json.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, SOURCE_BYTES		, 1, "The retained catalog source does not match the reopened file.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, ITEM_COUNT		, 2, "The shops category did not produce catalog items.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, RESOLVE_RESULT	, 3, "The native resolver did not find the indexed phone fragment.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, SOURCE_RESULT		, 4, "A resolved item could not be mapped back to its source JSON record.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, SOURCE_RECORD		, 5, "The expected onsushi source record was not resolved.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, INDEXED_VALUE		, 6, "The resolved record does not contain the indexed phone value.");
GDEFINE_ENUM_VALUED(INDEX_CATALOG_TEST_RESULT, EMPTY_RESULT		, 7, "A missing catalog term unexpectedly resolved records.");

stxp ::gpk::vcst_c INDEX_CATALOG_FILE = "D:/enterprise/programming_sciences/codebases/Axiom/bundle_galaxy_hell/gpk_data/test_llc/ads.json";

::gpk::err_t SIndexCatalog::Load(::gpk::vcst_t fileName, ::gpk::AD_SHOP_CATEGORY category, ::gpk::vcst_t language) {
	if_fail_fe(Item.clear());
	if_fail_fe(Word.clear());
	if_fail_fe(WordCache.clear());
	if_fail_fe(::gpk::jsonFileRead(Source, fileName));
	if_fail_fe(::gpk::getItemViews(Source.Reader, category, language, Item));
	for(::gpk::u2_t iItem = 0; iItem < Item.size(); ++iItem)
		if_fail_fe(::gpk::getWordDict(Word, Item[iItem], iItem, WordCache));
	rtrn 0;
}

::gpk::err_t SIndexCatalog::Resolve(::gpk::vcst_t input, ::gpk::au2_t & itemIndices) cnst {
	rtrn ::gpk::wordDictResolve(Word, input, Item.size(), itemIndices);
}

::gpk::err_t SIndexCatalog::SourceValue(::gpk::u2_t itemIndex, ::gpk::vcst_t expression, ::gpk::vcst_t & output) cnst {
	if_true_fef(itemIndex >= Item.size(), "Invalid catalog item: %u. Item count: %u.", itemIndex, Item.size());
	cnst ::gpk::err_t sourceNode = ::gpk::jsonArrayValueGet(Source.Reader, 0, Item[itemIndex].ItemIndex);
	if_fail_fe(sourceNode);
	rtrn ::gpk::jsonExpressionResolve(expression, Source.Reader, sourceNode, output);
}

::gpk::err_t test_index_catalog(STestRunnerState &, ::gpk::ATestError & errors, [[maybe_unused]] ::gpk::vcst_t outputFolder) {
	SIndexCatalog catalog = {};
	cnst ::gpk::err_t loadResult = catalog.Load(INDEX_CATALOG_FILE, ::gpk::AD_SHOP_CATEGORY_shops, "en");
	GPK_TEST_REQUIREF(errors, INDEX_CATALOG_TEST_RESULT_LOAD_RESULT, ::gpk::failed(loadResult), "Result:%i.", loadResult);
	::gpk::string reopened = {};
	cnst ::gpk::err_t reopenResult = ::gpk::fileToMemory(INDEX_CATALOG_FILE, reopened);
	GPK_TEST_REQUIREF(errors, INDEX_CATALOG_TEST_RESULT_LOAD_RESULT, ::gpk::failed(reopenResult), "Result:%i.", reopenResult);
	GPK_TEST_CHECKF(errors, INDEX_CATALOG_TEST_RESULT_SOURCE_BYTES, reopened != catalog.Source.Bytes, "Reopened:%u, retained:%u.", reopened.size(), catalog.Source.Bytes.size());
	GPK_TEST_REQUIREF(errors, INDEX_CATALOG_TEST_RESULT_ITEM_COUNT, 0 == catalog.Item.size(), "Items:%u.", catalog.Item.size());

	::gpk::au2_t resolved = {};
	cnst ::gpk::err_t resolveResult = catalog.Resolve("7518", resolved);
	GPK_TEST_REQUIREF(errors, INDEX_CATALOG_TEST_RESULT_RESOLVE_RESULT, 0 >= resolveResult || 0 == resolved.size(), "Result:%i, count:%u.", resolveResult, resolved.size());
	bool foundOnSushi = false;
	for(::gpk::u2_t iResult = 0; iResult < resolved.size(); ++iResult) {
		cnst ::gpk::u2_t itemIndex = resolved[iResult];
		::gpk::vcst_t sourceName = {};
		cnst ::gpk::err_t sourceResult = catalog.SourceValue(itemIndex, "name", sourceName);
		GPK_TEST_REQUIREF(errors, INDEX_CATALOG_TEST_RESULT_SOURCE_RESULT, ::gpk::failed(sourceResult), "Item:%u, result:%i.", itemIndex, sourceResult);
		if(sourceName != "onsushi")
			continue;
		foundOnSushi = true;
		bool foundPhone = false;
		for(::gpk::u2_t iPhone = 0; iPhone < catalog.Item[itemIndex].Phones.size(); ++iPhone)
			foundPhone |= catalog.Item[itemIndex].Phones[iPhone] == "7518-4019";
		GPK_TEST_CHECKF(errors, INDEX_CATALOG_TEST_RESULT_INDEXED_VALUE, false == foundPhone, "Item:%u, phones:%u.", itemIndex, catalog.Item[itemIndex].Phones.size());
	}
	GPK_TEST_CHECK(errors, INDEX_CATALOG_TEST_RESULT_SOURCE_RECORD, false == foundOnSushi);
	cnst ::gpk::err_t missingResult = catalog.Resolve("definitely-not-in-obelisco", resolved);
	GPK_TEST_CHECKF(errors, INDEX_CATALOG_TEST_RESULT_EMPTY_RESULT, missingResult || resolved.size(), "Result:%i, count:%u.", missingResult, resolved.size());
	rtrn 0;
}
