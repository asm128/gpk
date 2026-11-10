#include "gpk_index_catalog.h"
#include "gpk_json_expression.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();

sttc gpk::err_t		itemWordViews				(::gpk::aobj<::gpk::vcst_t> & upper, cnst ::gpk::SItemViews & item) {
	if_fail_fe(::gpk::split(item.Text	, ' ', upper));
	if_fail_fe(::gpk::split(item.Title	, ' ', upper));
	if_fail_fe(::gpk::split(item.URL	, ' ', upper));
	if_fail_fe(::gpk::split(item.Text	, ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.Addresses	.size(); ++iAddr) if_fail_fe(::gpk::split(item.Addresses[iAddr], ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.Phones		.size(); ++iAddr) if_fail_fe(::gpk::split(item.Phones	[iAddr], ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.WPs		.size(); ++iAddr) if_fail_fe(::gpk::split(item.WPs		[iAddr], ' ', upper));
	rtrn 0;
}
gpk::err_t			gpk::getWordList				(aobj<string> & itemWords, cnst SItemViews & item, view<vcst_c> exceptions) {
	aobj<vcst_t>			upper								= {};
	if_fail_fe(::itemWordViews(upper, item));
	rtrn getWordList(itemWords, upper, exceptions);
}

gpk::err_t			gpk::getWordDict	(SWordDictData & wordData, aobj<SWordIndices> & wordSet, cnst SItemViews & item, u2_t iItem, view<vcst_c> exceptions) {
	rtrn getWordDict(wordData, wordSet, 1
		, [&item, iItem](u2_t, u2_t & itemIndex, aobj<vcst_t> & itemViews) {
			itemIndex = iItem;
			rtrn ::itemWordViews(itemViews, item);
		}
		, exceptions);
}

gpk::err_t			gpk::getWordDict	(SWordDictData & wordData, aobj<SWordIndices> & wordSet, view<cnst SItemViews> items, view<vcst_c> exceptions) {
	rtrn getWordDict(wordData, wordSet, items.size()
		, [items](u2_t iItem, u2_t & itemIndex, aobj<vcst_t> & itemViews) {
			itemIndex = iItem;
			rtrn ::itemWordViews(itemViews, items[iItem]);
		}
		, exceptions);
}

gpk::err_t			gpk::getItemViews		(SJSONReader & content, const AD_SHOP_CATEGORY category, vcst_t /*lang*/, aobj<SItemViews> & indicesToDisplay) {
	for(s2_t iItem = 0, countItems = jsonArraySize(content, 0); iItem < countItems; ++iItem) {
		SItemViews		views					= {(u2_t)iItem};
		cnst err_t		jsonIndexCurrentItem	= jsonArrayValueGet(content, 0, iItem);
		vcst_t			viewWikiCategory		= "";
		cnst err_t		jsonIndexArrayCategory	= jsonExpressionResolve("category", content, jsonIndexCurrentItem, viewWikiCategory);
		b8_t			isCategory				= false;
		for(u2_t iCat = 0, countCat = (u2_t)jsonArraySize(*content[jsonIndexArrayCategory]); iCat < countCat; ++iCat) {
			cnst err_t		jsonIndexNodeCategory	= jsonArrayValueGet(*content[jsonIndexArrayCategory], iCat);
			cnst SJSONToken	& jsonToken				= content.Token[jsonIndexNodeCategory];
			if(category == jsonToken.Value) {
				isCategory	= true;
				break;
			}
		}
		if(false == isCategory)
			continue;

		// ---- Root arrays.
		err_t		jsonIndexArrayPhone		;
		err_t		jsonIndexArrayWP		;
		err_t		jsonIndexArrayAddr		;
		err_t		jsonIndexArrayBarrios	;
		if_fail_fe(jsonIndexArrayPhone		= jsonExpressionResolve("phone"		, content, jsonIndexCurrentItem, views.MapURL));
		if_fail_fe(jsonIndexArrayWP			= jsonExpressionResolve("whatsapp"	, content, jsonIndexCurrentItem, views.MapURL));
		if_fail_fe(jsonIndexArrayAddr		= jsonExpressionResolve("address"	, content, jsonIndexCurrentItem, views.MapURL));
		if_fail_fe(jsonIndexArrayBarrios	= jsonExpressionResolve("barrio"	, content, jsonIndexCurrentItem, views.MapURL));
		
		for(s2_t iAddr		= 0, countAddr		= jsonArraySize(*content[jsonIndexArrayAddr		]); iAddr		< countAddr		; ++iAddr		)	views.Addresses	.push_back(content.View[jsonArrayValueGet(*content[jsonIndexArrayAddr	 ], iAddr	)]);
		for(s2_t iPhones	= 0, countPhones	= jsonArraySize(*content[jsonIndexArrayPhone	]); iPhones		< countPhones	; ++iPhones		)	views.Phones	.push_back(content.View[jsonArrayValueGet(*content[jsonIndexArrayPhone	 ], iPhones	)]);
		for(s2_t iWPs		= 0, countWPs		= jsonArraySize(*content[jsonIndexArrayWP		]); iWPs		< countWPs		; ++iWPs		)	views.WPs		.push_back(content.View[jsonArrayValueGet(*content[jsonIndexArrayWP		 ], iWPs		)]);
		for(s2_t iBarrios	= 0, countBarrios	= jsonArraySize(*content[jsonIndexArrayBarrios	]); iBarrios	< countBarrios	; ++iBarrios	)	views.Barrio	.push_back((u2_t)content.Token[jsonArrayValueGet(*content[jsonIndexArrayBarrios], iBarrios	)].Value);

		if_fail_fe(indicesToDisplay.push_back(views));
	}
	return 0;
}

gpk::err_t			gpk::outputSearchScript		(string & output, view<cnst SItemViews> indicesToDisplay, view<vcst_c> exceptions) {
	aobj<SWordIndices>	wordSet;

	if_fail_fe(getWordDict(wordSet, indicesToDisplay, exceptions));

	if_fail_fe(append_strings(output, "\nvar names=["));
	for(u2_t iWord = 0, countWords = wordSet.size(); iWord < countWords; ++iWord) {
		cnst SWordIndices	& element				= wordSet[iWord];
		info_printf("Word: %.*s.", GPK_VIEW_ARGS(element.Text));
		if(0 <= find('\'', {element.Text})) {
			if(0 <= find('"', {element.Text}))
				continue;
			if_fail_fe(append_strings(output, '"', element.Text, '"'));
		}
		else {
			if_fail_fe(append_strings(output, '\'', element.Text, '\''));
		}
		if(iWord < (wordSet.size() - 1))
			if_fail_fe(output.push_back(','));
	}
	if_fail_fe(append_strings(output, "];"
		, "\nvar indices=["));
	//char		tempIntStr[256]				= {};
	for(u2_t iWord = 0; iWord < wordSet.size(); ++iWord) {
		const SWordIndices		& element = wordSet[iWord];
		if_fail_fe(output.push_back('['));
		for(u2_t iArticle = 0; iArticle < element.Indices.size(); ++iArticle) {
			if_fail_fe(append_strings(output, element.Indices[iArticle]));
			if(iArticle < (element.Indices.size() - 1))
				if_fail_fe(output.push_back(','));
		}
		if_fail_fe(output.push_back(']'));
		if(iWord < (wordSet.size() - 1))
			if_fail_fe(output.push_back(','));
	}
	if_fail_fe(append_strings(output, "];"
		, "\nvar idList=["));
	for(u2_t iArticle = 0; iArticle < indicesToDisplay.size(); ++iArticle) {
		if_fail_fe(append_strings(output, iArticle));
		if(iArticle < (indicesToDisplay.size() - 1))
			if_fail_fe(output.push_back(','));
	}
	if_fail_fe(append_strings(output, "];"
		, "\nvar barrioIdMap=["));
	for(u2_t iArticle = 0; iArticle < indicesToDisplay.size(); ++iArticle) {
		const SItemViews		& ad			= indicesToDisplay[iArticle];
		if_fail_fe(output.push_back('['));
		for(u2_t iBarrio = 0; iBarrio < ad.Barrio.size(); ++iBarrio) {
			if_fail_fe(append_strings(output, ad.Barrio[iBarrio]));
			if(iBarrio < (ad.Barrio.size() - 1))
				if_fail_fe(output.push_back(','));
		}
		if_fail_fe(output.push_back(']'));
		if(iArticle < (indicesToDisplay.size() - 1))
			if_fail_fe(output.push_back(','));
	}
	if_fail_fe(append_strings(output, "];"));
	return 0;
}