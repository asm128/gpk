#include "gpk_index.h"

//#include "ntl_front.h"
//#include "ntl_session.h"

#include "gpk_cgi_app_impl_v2.h"

#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_keyval_view.h"
#include "gpk_stdstring.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_N2();

using namespace gpk;

//GPK_CGI_JSON_APP_IMPL();

namespace gpk
{
	stxp vcst_c		DEFAULT_LANGUAGE	= "es";
	stxp vcst_c		DEFAULT_CATEGORIES	=  R"CATEGORIES(
{ "en":
	{ "categories"	:
		[ "Tourism"	
		, "Meals"		
		, "Shopping"	
		, "Shows"		
		]
	}
, "es":
	{ "categories"	:
		[ "Turismo"	
		, "Comidas"	
		, "Comercios"	
		, "Espectáculos"
		]
	}
}
)CATEGORIES"; 
} // namespace

//sttc	err_t	pageCatalog						(vcst_t contentFileName, vcst_t pathScript, n2u2_c sizeScreen, vcst_t pathStyles, const AD_SHOP_CATEGORY category, vcst_t title, vcst_t lang, string & output);

err_t			gpk_cgi_generate_output			(SCGIRuntimeValues & runtimeValues, string & output)	{
	SHTTPAPIRequest		requestReceived					= {};
	bool				isCGIEnviron					= httpRequestInit(requestReceived, runtimeValues, true);
	vcst_t				cookie;
	vcst_t				sessionFileName;				;
	string				sessionFileContents				= {};
	aobj<kv<vcst_t>>	cookieValues;
	if (isCGIEnviron) {
		if_fail_fe(output.append_string("Content-type: text/html\r\nCache-control: no-cache\r\n"));
		if_fail_fe(output.append_string("\r\n"));
		gpk::find("HTTP_COOKIE", view<cnst kv<vcst_t>>{runtimeValues.RuntimeValues.EntryPointArgs.Environment}, cookie);
		if(0 == cookie.size()) {
			output.append_string("{ \"status\" : 403, \"description\" :\"forbidden - The server currently doesn't accept requests to this frame without a cookie set.\" }\r\n");
			return 1;
			//::sessionInitialize(requestReceived, cookie, sessionFileName, digested);
			//if_fail_fef(output.append_string("\r\nSet-Cookie: "));
			//if_fail_fef(output.append(cookie));
		}
		else {
			if_fail_fe(append_strings(output, "\r\n\r\n"));
			//if_fail_fef(::ntl::sessionFileLoad(cookie, sessionFileContents, cookieValues), "%s", "Failed to load session!");
		}
		//if_fail_fef(output.append_string("\r\nSet-Cookie: "));
		//if_fail_fef(output.append(cookie));

		stxp vcst_c		methodsValid	[]	= {"GET", "POST"};
		if(-1 == gpk::keyValVerify(runtimeValues.RuntimeValues.EntryPointArgs.Environment, "REQUEST_METHOD", methodsValid)) {
			output.append_string("{ \"status\" : 403, \"description\" :\"forbidden\" }\r\n");
			return 1;
		}
	}
	AD_SHOP_CATEGORY		category						= AD_SHOP_CATEGORY_shops;//{}get_value<::AD_SHOP_CATEGORY>(qsArgs.Module);
	{
		SJSONReader categoryML;
		if_fail_fef(jsonParse(categoryML, gpk::DEFAULT_CATEGORIES), "\n%.*s", GPK_VIEW_ARGS(DEFAULT_CATEGORIES));
		vcst_t	qsArgsLanguage	= gpk::DEFAULT_LANGUAGE;
		vcst_t	categoryName	= {};
		{
			vcst_t	rootNode;
			s2_t	indexRoot;
			if_fail_fe(indexRoot = jsonExpressionResolve(qsArgsLanguage, categoryML, category, categoryName));
		}
	}
	// switch(category) {
	// case ::AD_SHOP_CATEGORY_tours: title = (qsArgsLanguage == "es") ? vcst_t("Turismo"		) : vcst_t("Tourism"	); break;
	// case ::AD_SHOP_CATEGORY_meals: title = (qsArgsLanguage == "es") ? vcst_t("Comidas"		) : vcst_t("Meals"		); break;
	// case ::AD_SHOP_CATEGORY_shops: title = (qsArgsLanguage == "es") ? vcst_t("Comercios"	) : vcst_t("Shopping"	); break;
	// case ::AD_SHOP_CATEGORY_shows: title = (qsArgsLanguage == "es") ? vcst_t("Espectáculos"	) : vcst_t("Shows"		); break;
	// default: break;
	// }

	n2<vcst_t>							renderTargetMetrics	= {"", ""};
	n2u2_t								sizeScreen						= {};
	parseIntegerDecimal(renderTargetMetrics.x, sizeScreen.x);
	parseIntegerDecimal(renderTargetMetrics.y, sizeScreen.y);

	//::pageCatalog("ads.json", programState.Path.Script, sizeScreen, programState.Path.Style, category, title, qsArgs.Language.size() ? qsArgs.Language : "es", output);

	if(output.size()) {
		OutputDebugStringA(output.begin());
		OutputDebugStringA("\n");
	}
	return 0;
}

gpk::err_t	gpk::getItemViews		(SJSONReader & content, const AD_SHOP_CATEGORY category, vcst_t /*lang*/, aobj<SItemViews> & indicesToDisplay) {
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

gpk::err_t			gpk::getWordList				(aobj<string> & itemWords, cnst SItemViews & item, view<vcst_c> exceptions) {
	aobj<vcst_t>			upper								= {};
	if_fail_fe(split(item.Text	, ' ', upper));
	if_fail_fe(split(item.Title	, ' ', upper));
	if_fail_fe(split(item.URL	, ' ', upper));
	if_fail_fe(split(item.Text	, ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.Addresses	.size(); ++iAddr) if_fail_fe(split(item.Addresses[iAddr], ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.Phones		.size(); ++iAddr) if_fail_fe(split(item.Phones	[iAddr], ' ', upper));
	for(u2_t iAddr = 0; iAddr < item.WPs		.size(); ++iAddr) if_fail_fe(split(item.WPs		[iAddr], ' ', upper));

	if_fail_fe(itemWords.resize(upper.size()));
	cnst kv<vcst_t>			replaceDict	[]						=
		{	{ "áéíóúàèìòùäëïöüãõñçºª"
			, "aeiouaeiouaeiouaoncoa"
			}
		};
	string					fixedWord;
	for(u2_t iWord = 0, countWords = upper.size(); iWord < countWords; ++iWord) {
		vcst_t					originalWord						= upper[iWord];
		trim(originalWord, originalWord);
		string					& finalWord							= itemWords[iWord];
		finalWord.clear();
		if_fail_fe(append_strings(finalWord, originalWord));
		tolower(finalWord);
		if(0 <= exceptions.find(finalWord)) {
			finalWord.clear();
			continue;
		}
		info_printf("finalWord: %.*s.", GPK_VIEW_ARGS(finalWord));
		fixedWord			= finalWord;
		bool					replaced		= false;
		for(u2_t iChar = 0; iChar < fixedWord.size(); ++iChar) {
			char					& wordChar		= fixedWord[iChar];
			for(u2_t iTable = 0; iTable < size(replaceDict); ++iTable) {
				cnst kv<vcst_t> &	table = replaceDict[iTable];
				for(u2_t iItem = 0; iItem < table.Key.size(); ++iItem)
					if(wordChar == table.Key[iItem]) {
						wordChar			= table.Val[iItem];
						replaced			= true;
					}
			}
		}
		if(replaced) {
			if_fail_fe(itemWords.push_back(fixedWord));
			info_printf("fixedWord: %.*s.", GPK_VIEW_ARGS(fixedWord));
		}
	}
	return 0;
}
gpk::err_t	gpk::getWordDict	(aobj<SWordIndices> & wordSet, cnst SItemViews & item, u2_t iItem, aobj<string> & cacheItemWords, view<vcst_c> exceptions) {
	if_fail_fe(gpk::getWordList(cacheItemWords, item, exceptions));
	for(u2_t iItemWord = 0; iItemWord < cacheItemWords.size(); ++iItemWord) {
		vcst_t			itemWord			= cacheItemWords[iItemWord];
		info_printf("itemWord: %.*s.", GPK_VIEW_ARGS(itemWord));
		if(1 >= itemWord.size())
			continue;
		bool			bFound				= false;
		if(',' == itemWord[itemWord.size() - 1] || '.' == itemWord[itemWord.size() - 1]) {
			if_fail_fe(itemWord.slice(itemWord, 0, itemWord.size()-1));
			if(0 >= itemWord.size())
				continue;
		}
		for(u2_t iWordInSet = 0; iWordInSet < wordSet.size(); ++iWordInSet) {
			SWordIndices	& dictWord			= wordSet[iWordInSet];
			if(itemWord != dictWord.Text)
				continue;
			bFound		= true;
			if(0 > find(iItem, {dictWord.Indices.begin(), dictWord.Indices.size()}))
				if_fail_fe(dictWord.Indices.push_back(iItem));
		}
		if(false == bFound)
			if_fail_fe(wordSet[wordSet.push_back({itemWord})].Indices.push_back(iItem));
	}
	return 0;
}

gpk::err_t	gpk::outputSearchScript		(string & output, view<cnst SItemViews> indicesToDisplay, view<vcst_c> exceptions) {
	aobj<SWordIndices>	wordSet;
	aobj<string>		cacheItemWords;

	for(u2_t iItem = 0, countItems = indicesToDisplay.size(); iItem < countItems; ++iItem)
		if_fail_fe(getWordDict(wordSet, indicesToDisplay[iItem], iItem, cacheItemWords, exceptions));

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
			if_fail_fe(append_strings(output, str(element.Indices[iArticle])));
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
//
//tatic err_t								htmlBoardGenerate					(view<const SItemViews> indicesToDisplay, vcst_t fontSize, vcst_t /*title*/, vcst_t lang, string & output)	{
//	output.append_string("\n<table style=\"width:100%;height:100%;text-align:center;font-size:");
//	output.append(fontSize);
//	output.append_string("px;\" >");
//	output.append_string("\n<tr style=\"\" >");
//	output.append_string("\n<td style=\"width:100%;font-weight:bold; vertical-align:top;\">");
//	//::ntl::htmlTag("h2", title, {}, output);
//	output.append_string("\n</td>");
//	output.append_string("\n</tr>");
//	output.append_string("\n<tr style=\"\" >");
//	output.append_string("\n<td style=\"width:100%;font-weight:bold; vertical-align:top;\">");
//
//		output.append_string("\n<table style=\"width:100%;text-align:center;border-style:solid;border-width:2px;font-size:");
//		output.append(fontSize);
//		output.append_string("px;\" >");
//		string								base64Id;
//		char													tempIntStr[256]				= {};
//		for(s2_t iItem = 0, countItems = indicesToDisplay.size(); iItem < countItems; ++iItem) {
//			const SItemViews										& views						= indicesToDisplay[iItem];
//			base64Id.clear();
//			::gpk::sprintf_s(tempIntStr, "%u", iItem);
//			output.append_string("\n<tr id=\"");
//			output.append_string(tempIntStr);
//			output.append_string("\">");
//			output.append_string("\n<td style=\"width:100%;text-align:left;vertical-align:top;\">");
//			//
//				output.append_string("\n<table style=\"background-color:white;width:100%;height:100%;min-height:100%;text-align:center;border-style:solid;border-width:2px;border-radius:8px;font-size:");
//				output.append(fontSize);
//				output.append_string("px;\" >");
//				output.append_string("\n<tr>");
//				output.append_string("\n<td style=\"background-color:lightgrey;border-radius:8px;text-align:left;vertical-align:top;\">");
//				//
//				string pTitle;
//				//::ntl::htmlTag("h3", views.Title, "style=\"text-align:left;\"", pTitle);
//				//::ntl::htmlHRefLink({pTitle.begin(), pTitle.size()}, views.URL, "style=\"text-decoration:none;font-weight:bold;\"", output);
//				output.append_string("\n</td>");
//				output.append_string("\n</tr>");
//				output.append_string("\n<tr>");
//				output.append_string("\n<td style=\"vertical-align:top;height:100%;text-align:left;font-size:");
//				output.append(fontSize);
//				output.append_string("px;\">");
//				//::ntl::htmlTag("h3", views.Text, "style=\" font-weight:normal;text-align:left;\"", output);
//				//
//				output.append_string("\n</td>");
//				output.append_string("\n</tr>");
//				output.append_string("\n</table>");
//			//
//			output.append_string("\n</td>");
//			output.append_string("\n<td style=\"height:100%;text-align:left;vertical-align:top;\">");
//
//				output.append_string("\n<table style=\"\" >");
//				output.append_string("\n<tr>");
//				output.append_string("\n<td style=\"text-align:left;vertical-align:top;width:100%;\">");
//				output.append_string("<a target=\"blank\" title=\"");
//				output.append(views.ImageTitle);
//				output.append_string("\" href=\"");
//				output.append(views.ImageHRef);
//				output.append_string("\" class=\"image\" ><img style=\"width:320px;\" decoding=\"async\" elementtiming=\"thumbnail\" alt=\"");
//				output.append(views.ImageAlt);
//				output.append_string("\" src=\"");
//				output.append(views.ImageSrc);
//				output.append_string("\" ></a>");
//				output.append_string("\n</td>");
//				output.append_string("\n</tr><tr>");
//				output.append_string("\n<td style=\"vertical-align:top;\">");
//
//				// ----- Image info table
//					output.append_string("\n<table style=\"background-color:white;width:100%;height:100%;text-align:center;border-style:solid;border-width:2px;border-radius:8px;\" font-size:");
//					output.append(fontSize);
//					output.append_string("px;\">");
//					if(views.MapURL.size()) {
//						output.append_string("\n<tr>");
//						output.append_string("\n<td style=\"text-align:center;vertical-align:top;font-size:");
//						output.append(fontSize);
//						output.append_string("px;\">");
//						output.append_string("\n<a target=\"blank\" style=\"margin:4px;\" href=\"");
//						output.append(views.MapURL);
//						output.append_string("\" >");
//						if(lang == "es")
//							output.append_string("Localización");
//						else
//							output.append_string("Location");
//						output.append_string("\n</a>");
//						output.append_string("\n</td>");
//						output.append_string("\n</tr>");
//					}
//					// ----- Addresses
//					if(views.Addresses.size()) {
//						output.append_string("\n<tr style=\"height:100%;\">");
//						output.append_string("\n<td style=\"text-align:center;vertical-align:top;border-style:solid;border-top-width:1px;font-size:");
//						output.append(fontSize);
//						output.append_string("px;\" >");
//						for(u2_t iPhone = 0, countAddr = views.Addresses.size(); iPhone < countAddr; ++iPhone) {
//							output.append(views.Addresses[iPhone]);
//							output.append_string("\n<br />");
//						}
//						output.append_string("\n</td>");
//						output.append_string("\n</tr>");
//					}
//
//					// ----- Phones
//					if(views.Phones.size() || views.WPs.size()) {
//						output.append_string("\n<tr style=\"height:100%;\">");
//						output.append_string("\n<td style=\"text-align:center;vertical-align:top;border-style:solid;border-top-width:1px;font-size:");
//						output.append(fontSize);
//						output.append_string("px;\" >");
//						for(u2_t iPhone = 0, countPhones = views.Phones.size(); iPhone < countPhones; ++iPhone) {
//							output.append(views.Phones[iPhone]);
//							output.append_string("\n<br />");
//						}
//						for(u2_t iPhone = 0, countWPs	 = views.WPs	.size(); iPhone < countWPs		; ++iPhone) {
//							vcst_t									textPhone						= views.WPs[iPhone];
//							output.append_string("\n<a target=\"blank\" href=\"https://wa.me/");
//							output.append(textPhone);
//							output.append_string("\" >");
//							output.append(textPhone);
//							output.append_string("</a>");
//							output.append_string("<br />");
//						}
//
//						//
//						output.append_string("\n</td>");
//						output.append_string("\n</tr>");
//					}
//					output.append_string("\n</table>");
//
//				output.append_string("\n</td>");
//				output.append_string("\n</tr>");
//				output.append_string("\n</table>");
//
//
//			output.append_string("\n</td>");
//			output.append_string("\n</tr>");
//		}
//		output.append_string("\n</table>");
//
//	output.append_string("\n</td>");
//	output.append_string("\n</tr>");
//	output.append_string("\n</table>");
//
//	return 0;
//

//static	err_t								pageCatalog					(vcst_t contentFileName, vcst_t /*pathScript*/, n2s2_t screenSize, const vcst_t & /*pathStyles*/, const AD_SHOP_CATEGORY category, vcst_t title, vcst_t lang, string & output) {
//
//	output.append_string("\n<html>");
//	// --- Head
//	output.append_string("\n<head>");
//	output.append_string("\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.5\" />");
//	output.append_string("\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />");
//	string								fileStyle			;
//	//::ntl::httpPath(pathStyles, "blankstyle", "css", fileStyle);
//	//::ntl::htmlHeaderTitle		(title, output);
//	//::ntl::htmlHeaderStyleLink	({fileStyle.begin(), fileStyle.size()}, output);
//	output.append_string("\n</head>");
//
//	// --- Body
//	output.append_string(
//		"\n<body style=\"width:100%;height:100%;font-family:Arial;\">"
//		"\n<table style=\"width:100%;height:100%;text-align:center;\">"
//		"\n<tr style=\"\" >"
//		"\n<td style=\"vertical-align:top;\">"
//		);
//
//	//---------------------
//	SJSONFile										config								= {};
//	if_fail_fef(jsonFileRead(config, contentFileName), "Failed to load configuration file: %s.", contentFileName);
//	array_obj<::SItemViews>							indicesToDisplay					= {};
//	::getItemViews(config.Reader, category, lang, indicesToDisplay);
//
//	char										fontSize	[32]					= {};
//	if(screenSize.x > screenSize.y)
//		::gpk::sprintf_s(fontSize, "%u", screenSize.x ? screenSize.x / 55 : 24);
//	else
//		::gpk::sprintf_s(fontSize, "%u", screenSize.y ? screenSize.y / 44 : 24);
//
//	output.append_string(
//		"\n<div style=\"background-color:#ffffff;position:sticky;left:0;top:0;\">"
//		"\n<table style=\"width:100%;height:100%;text-align:center;\">"
//		"\n<tr style=\"\" >"
//		"\n<td style=\"font-weight:bold;vertical-align:center;font-size:"
//		);
//	output.append_string(fontSize);
//	output.append_string("px;\" >");
//
//	const vcst_t							textSearch							= (lang == "es") ? "Buscar" : "Search";
//	//::ntl::htmlTag("h5", textSearch, {}, output);
//
//	output.append_string(
//		"\n</td>"
//		"\n<td style=\"position:sticky;left:0px;top:0px;sticky;font-size:24px;font-weight:bold;width:20px;text-align:left;vertical-align:top;\">"
//	//"\n<img src=\"/obelisco/image/blank.png\"/>"
//		"\n</td>"
//		"\n<td style=\"position:sticky;left:0px;top:0px;width:100%;text-align:left;vertical-align:top;\">"
//		"<input id=\"searchBar\" oninput=\"if(this.value.length > 0) obeSearch(this.value); else clearSearch();\" style=\"position:sticky;height:100%;font-size:24px;border-width:1px;width:50%;text-align:left;border-style:solid;border-radius:8px;\" type=\"text\"></input>"
//		"\n</td>"
//		);
//
//#define ENABLE_CATALOG_BARRIO
//#if defined(ENABLE_CATALOG_BARRIO)
//	output.append_string("\n<td style=\"font-weight:bold;font-size:");
//	output.append_string(fontSize);
//	output.append_string("px;\" >");
//	output.append_string("<h5>Barrio:<h5>");
//	output.append_string("</td>");
//	output.append_string(
//		"\n<td>"
//		"<select id=\"selectBarrio\" onchange=\"var searchBar = document.getElementById('searchBar'); if(searchBar.value.length > 0) obeSearch(searchBar.value); else clearSearch();\" style=\"font-size:"
//		);
//	output.append_string(fontSize);
//	output.append_string("px;\" >");
//
//	SJSONFile										jsonBarrios;
//	jsonFileRead(jsonBarrios, "barrios.json");
//	const err_t									countBarrios					= jsonArraySize(*jsonBarrios.Reader[0]);
//	char													tempBarrioOption[256]			= {};
//	vcst_t								barrioName;
//	if(lang == "es")
//		output.append_string("\n<option value=\"-1\" style=\"\">Todos</option>");
//	else
//		output.append_string("\n<option value=\"-1\" style=\"\">All</option>");
//
//	;;gpk::array_obj<::SBarrio>								viewsBarrio;
//	for(s2_t iBarrio = 0; iBarrio < countBarrios; ++iBarrio) {
//		s2_t													indexBarrioNode					= jsonArrayValueGet(*jsonBarrios.Reader[0], iBarrio);
//		s2_t													indexBarrioName					= jsonObjectValueGet(*jsonBarrios.Reader[indexBarrioNode], jsonBarrios.Reader.View, "name"		);
//		s2_t													indexBarrioArea					= jsonObjectValueGet(*jsonBarrios.Reader[indexBarrioNode], jsonBarrios.Reader.View, "area"		);
//		s2_t													indexBarrioPopulation			= jsonObjectValueGet(*jsonBarrios.Reader[indexBarrioNode], jsonBarrios.Reader.View, "population"	);
//		s2_t													indexBarrioCommune				= jsonObjectValueGet(*jsonBarrios.Reader[indexBarrioNode], jsonBarrios.Reader.View, "commune"	);
//		::SBarrio												barrio;
//		barrio.Name											= jsonBarrios.Reader.View[indexBarrioName		];
//		barrio.Area											= jsonBarrios.Reader.View[indexBarrioArea		];
//		barrio.Population									= jsonBarrios.Reader.View[indexBarrioPopulation	];
//		barrio.Commune										= jsonBarrios.Reader.View[indexBarrioCommune	];
//		viewsBarrio.push_back(barrio);
//	}
//
//	for(u2_t iBarrio = 0; iBarrio < viewsBarrio.size(); ++iBarrio) {
//		::gpk::sprintf_s(tempBarrioOption, "\n<option value=\"%u\" style=\"\">", iBarrio);
//		output.append_string(tempBarrioOption);
//		output.append(viewsBarrio[iBarrio].Name);
//		output.append_string("</option>");
//	}
//
//	output.append_string(
//		"</select>"
//		"\n</td>"
//		);
//#endif
//
//	output.append_string(
//		"\n</tr>"
//		"\n</table>"
//		"\n</div>"
//		);
//
//	::htmlBoardGenerate(indicesToDisplay, fontSize}, title, lang, output);
//
//	output.append_string("\n<script charset=\"iso-8859-1\">");
//	::outputSearchScript(indicesToDisplay, output);
//	output.append_string("\n</script>");
//
//	string								fileScriptMenu;
//	//::ntl::httpPath(pathScript, "filter", "js", fileScriptMenu);
//	//::ntl::htmlHeaderScriptFile({fileScriptMenu.begin(), fileScriptMenu.size()}, output);
//
//	output.append_string(
//		"\n</td>"
//		"\n</tr>"
//		"\n</table>"
//		"\n</body>"
//		);
//
//	output.append_string("\n</html>");
//	return 0;
//}
//