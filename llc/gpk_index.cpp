#include "gpk_index.h"

//#include "ntl_front.h"
//#include "ntl_session.h"

#include "gpk_cgi_app_impl_v2.h"

#include "gpk_parse.h"
#include "gpk_keyval_view.h"
#include "gpk_stdstring.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_N2();

usng nmsp gpk;

//GPK_CGI_JSON_APP_IMPL();

nmsp gpk
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
} // nmsp


gpk::err_t	gpk::wordListNormalize			(aobj<string> & itemWords, view<cnst vcst_t> upper, view<vcst_c> exceptions) {
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

gpk::err_t	gpk::getWordList				(aobj<string> & itemWords, view<cnst vcst_t> upper, view<vcst_c> exceptions) {
	rtrn wordListNormalize(itemWords, upper, exceptions);
}

gpk::err_t	gpk::wordDictAppend				(aobj<SWordIndices> & wordSet, u2_t iItem, view<cnst string> itemWords) {
	for(u2_t iItemWord = 0; iItemWord < itemWords.size(); ++iItemWord) {
		vcst_t			itemWord			= itemWords[iItemWord];
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

gpk::err_t	gpk::wordDictResolve	(view<cnst SWordIndices> wordSet, vcst_t input, u2_t itemCount, au2_t & itemIndices, view<vcst_c> exceptions) {
	if_fail_fe(itemIndices.clear());
	aobj<vcst_t>		inputWords						= {};
	aobj<string>		normalizedWords					= {};
	au0_t				matchedItems						= {};
	if_fail_fe(split(input, ' ', inputWords));
	if_fail_fe(wordListNormalize(normalizedWords, inputWords, exceptions));
	if_fail_fe(matchedItems.resize(itemCount));
	for(u2_t iItem = 0; iItem < matchedItems.size(); ++iItem)
		matchedItems[iItem] = 0;
	for(u2_t iInputWord = 0; iInputWord < normalizedWords.size(); ++iInputWord) {
		cnst vcst_t		inputWord						= normalizedWords[iInputWord];
		if(0 == inputWord.size())
			continue;
		for(u2_t iWord = 0; iWord < wordSet.size(); ++iWord) {
			cnst SWordIndices & indexedWord					= wordSet[iWord];
			if(0 > indexedWord.Text.find(inputWord))
				continue;
			for(u2_t iPosting = 0; iPosting < indexedWord.Indices.size(); ++iPosting) {
				cnst u2_t		itemIndex						= indexedWord.Indices[iPosting];
				if_true_fef(itemIndex >= itemCount, "Invalid item index: %u. Item count: %u.", itemIndex, itemCount);
				matchedItems[itemIndex] = 1;
			}
		}
	}
	for(u2_t iItem = 0; iItem < matchedItems.size(); ++iItem)
		if(matchedItems[iItem])
			if_fail_fe(itemIndices.push_back(iItem));
	rtrn (err_t)itemIndices.size();
}

//
//tatic err_t								htmlBoardGenerate					(view<const SItemViews> indicesToDisplay, vcst_t fontSize, vcst_t /*title*/, vcst_t lang, string & output)	{
//	append_strings(output, "\n<table style=\"width:100%;height:100%;text-align:center;font-size:");
//	output.append(fontSize);
//	append_strings(output, "px;\" >");
//	append_strings(output, "\n<tr style=\"\" >");
//	append_strings(output, "\n<td style=\"width:100%;font-weight:bold; vertical-align:top;\">");
//	//::ntl::htmlTag("h2", title, {}, output);
//	append_strings(output, "\n</td>");
//	append_strings(output, "\n</tr>");
//	append_strings(output, "\n<tr style=\"\" >");
//	append_strings(output, "\n<td style=\"width:100%;font-weight:bold; vertical-align:top;\">");
//
//		append_strings(output, "\n<table style=\"width:100%;text-align:center;border-style:solid;border-width:2px;font-size:");
//		output.append(fontSize);
//		append_strings(output, "px;\" >");
//		string								base64Id;
//		char													tempIntStr[256]				= {};
//		for(s2_t iItem = 0, countItems = indicesToDisplay.size(); iItem < countItems; ++iItem) {
//			const SItemViews										& views						= indicesToDisplay[iItem];
//			base64Id.clear();
//			sprintf_s(tempIntStr, "%u", iItem);
//			append_strings(output, "\n<tr id=\"");
//			append_strings(output, tempIntStr);
//			append_strings(output, "\">");
//			append_strings(output, "\n<td style=\"width:100%;text-align:left;vertical-align:top;\">");
//			//
//				append_strings(output, "\n<table style=\"background-color:white;width:100%;height:100%;min-height:100%;text-align:center;border-style:solid;border-width:2px;border-radius:8px;font-size:");
//				output.append(fontSize);
//				append_strings(output, "px;\" >");
//				append_strings(output, "\n<tr>");
//				append_strings(output, "\n<td style=\"background-color:lightgrey;border-radius:8px;text-align:left;vertical-align:top;\">");
//				//
//				string pTitle;
//				//::ntl::htmlTag("h3", views.Title, "style=\"text-align:left;\"", pTitle);
//				//::ntl::htmlHRefLink({pTitle.begin(), pTitle.size()}, views.URL, "style=\"text-decoration:none;font-weight:bold;\"", output);
//				append_strings(output, "\n</td>");
//				append_strings(output, "\n</tr>");
//				append_strings(output, "\n<tr>");
//				append_strings(output, "\n<td style=\"vertical-align:top;height:100%;text-align:left;font-size:");
//				output.append(fontSize);
//				append_strings(output, "px;\">");
//				//::ntl::htmlTag("h3", views.Text, "style=\" font-weight:normal;text-align:left;\"", output);
//				//
//				append_strings(output, "\n</td>");
//				append_strings(output, "\n</tr>");
//				append_strings(output, "\n</table>");
//			//
//			append_strings(output, "\n</td>");
//			append_strings(output, "\n<td style=\"height:100%;text-align:left;vertical-align:top;\">");
//
//				append_strings(output, "\n<table style=\"\" >");
//				append_strings(output, "\n<tr>");
//				append_strings(output, "\n<td style=\"text-align:left;vertical-align:top;width:100%;\">");
//				append_strings(output, "<a target=\"blank\" title=\"");
//				output.append(views.ImageTitle);
//				append_strings(output, "\" href=\"");
//				output.append(views.ImageHRef);
//				append_strings(output, "\" class=\"image\" ><img style=\"width:320px;\" decoding=\"async\" elementtiming=\"thumbnail\" alt=\"");
//				output.append(views.ImageAlt);
//				append_strings(output, "\" src=\"");
//				output.append(views.ImageSrc);
//				append_strings(output, "\" ></a>");
//				append_strings(output, "\n</td>");
//				append_strings(output, "\n</tr><tr>");
//				append_strings(output, "\n<td style=\"vertical-align:top;\">");
//
//				// ----- Image info table
//					append_strings(output, "\n<table style=\"background-color:white;width:100%;height:100%;text-align:center;border-style:solid;border-width:2px;border-radius:8px;\" font-size:");
//					output.append(fontSize);
//					append_strings(output, "px;\">");
//					if(views.MapURL.size()) {
//						append_strings(output, "\n<tr>");
//						append_strings(output, "\n<td style=\"text-align:center;vertical-align:top;font-size:");
//						output.append(fontSize);
//						append_strings(output, "px;\">");
//						append_strings(output, "\n<a target=\"blank\" style=\"margin:4px;\" href=\"");
//						output.append(views.MapURL);
//						append_strings(output, "\" >");
//						if(lang == "es")
//							append_strings(output, "Localización");
//						else
//							append_strings(output, "Location");
//						append_strings(output, "\n</a>");
//						append_strings(output, "\n</td>");
//						append_strings(output, "\n</tr>");
//					}
//					// ----- Addresses
//					if(views.Addresses.size()) {
//						append_strings(output, "\n<tr style=\"height:100%;\">");
//						append_strings(output, "\n<td style=\"text-align:center;vertical-align:top;border-style:solid;border-top-width:1px;font-size:");
//						output.append(fontSize);
//						append_strings(output, "px;\" >");
//						for(u2_t iPhone = 0, countAddr = views.Addresses.size(); iPhone < countAddr; ++iPhone) {
//							output.append(views.Addresses[iPhone]);
//							append_strings(output, "\n<br />");
//						}
//						append_strings(output, "\n</td>");
//						append_strings(output, "\n</tr>");
//					}
//
//					// ----- Phones
//					if(views.Phones.size() || views.WPs.size()) {
//						append_strings(output, "\n<tr style=\"height:100%;\">");
//						append_strings(output, "\n<td style=\"text-align:center;vertical-align:top;border-style:solid;border-top-width:1px;font-size:");
//						output.append(fontSize);
//						append_strings(output, "px;\" >");
//						for(u2_t iPhone = 0, countPhones = views.Phones.size(); iPhone < countPhones; ++iPhone) {
//							output.append(views.Phones[iPhone]);
//							append_strings(output, "\n<br />");
//						}
//						for(u2_t iPhone = 0, countWPs	 = views.WPs	.size(); iPhone < countWPs		; ++iPhone) {
//							vcst_t									textPhone						= views.WPs[iPhone];
//							append_strings(output, "\n<a target=\"blank\" href=\"https://wa.me/");
//							output.append(textPhone);
//							append_strings(output, "\" >");
//							output.append(textPhone);
//							append_strings(output, "</a>");
//							append_strings(output, "<br />");
//						}
//
//						//
//						append_strings(output, "\n</td>");
//						append_strings(output, "\n</tr>");
//					}
//					append_strings(output, "\n</table>");
//
//				append_strings(output, "\n</td>");
//				append_strings(output, "\n</tr>");
//				append_strings(output, "\n</table>");
//
//
//			append_strings(output, "\n</td>");
//			append_strings(output, "\n</tr>");
//		}
//		append_strings(output, "\n</table>");
//
//	append_strings(output, "\n</td>");
//	append_strings(output, "\n</tr>");
//	append_strings(output, "\n</table>");
//
//	return 0;
//

//static	err_t								pageCatalog					(vcst_t contentFileName, vcst_t /*pathScript*/, n2s2_t screenSize, const vcst_t & /*pathStyles*/, const AD_SHOP_CATEGORY category, vcst_t title, vcst_t lang, string & output) {
//
//	append_strings(output, "\n<html>");
//	// --- Head
//	append_strings(output, "\n<head>");
//	append_strings(output, "\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.5\" />");
//	append_strings(output, "\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />");
//	string								fileStyle			;
//	//::ntl::httpPath(pathStyles, "blankstyle", "css", fileStyle);
//	//::ntl::htmlHeaderTitle		(title, output);
//	//::ntl::htmlHeaderStyleLink	({fileStyle.begin(), fileStyle.size()}, output);
//	append_strings(output, "\n</head>");
//
//	// --- Body
//	append_strings(output, 
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
//		sprintf_s(fontSize, "%u", screenSize.x ? screenSize.x / 55 : 24);
//	else
//		sprintf_s(fontSize, "%u", screenSize.y ? screenSize.y / 44 : 24);
//
//	append_strings(output, 
//		"\n<div style=\"background-color:#ffffff;position:sticky;left:0;top:0;\">"
//		"\n<table style=\"width:100%;height:100%;text-align:center;\">"
//		"\n<tr style=\"\" >"
//		"\n<td style=\"font-weight:bold;vertical-align:center;font-size:"
//		);
//	append_strings(output, fontSize);
//	append_strings(output, "px;\" >");
//
//	const vcst_t							textSearch							= (lang == "es") ? "Buscar" : "Search";
//	//::ntl::htmlTag("h5", textSearch, {}, output);
//
//	append_strings(output, 
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
//	append_strings(output, "\n<td style=\"font-weight:bold;font-size:");
//	append_strings(output, fontSize);
//	append_strings(output, "px;\" >");
//	append_strings(output, "<h5>Barrio:<h5>");
//	append_strings(output, "</td>");
//	append_strings(output, 
//		"\n<td>"
//		"<select id=\"selectBarrio\" onchange=\"var searchBar = document.getElementById('searchBar'); if(searchBar.value.length > 0) obeSearch(searchBar.value); else clearSearch();\" style=\"font-size:"
//		);
//	append_strings(output, fontSize);
//	append_strings(output, "px;\" >");
//
//	SJSONFile										jsonBarrios;
//	jsonFileRead(jsonBarrios, "barrios.json");
//	const err_t									countBarrios					= jsonArraySize(*jsonBarrios.Reader[0]);
//	char													tempBarrioOption[256]			= {};
//	vcst_t								barrioName;
//	if(lang == "es")
//		append_strings(output, "\n<option value=\"-1\" style=\"\">Todos</option>");
//	else
//		append_strings(output, "\n<option value=\"-1\" style=\"\">All</option>");
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
//		sprintf_s(tempBarrioOption, "\n<option value=\"%u\" style=\"\">", iBarrio);
//		append_strings(output, tempBarrioOption);
//		output.append(viewsBarrio[iBarrio].Name);
//		append_strings(output, "</option>");
//	}
//
//	append_strings(output, 
//		"</select>"
//		"\n</td>"
//		);
//#endif
//
//	append_strings(output, 
//		"\n</tr>"
//		"\n</table>"
//		"\n</div>"
//		);
//
//	::htmlBoardGenerate(indicesToDisplay, fontSize}, title, lang, output);
//
//	append_strings(output, "\n<script charset=\"iso-8859-1\">");
//	::outputSearchScript(indicesToDisplay, output);
//	append_strings(output, "\n</script>");
//
//	string								fileScriptMenu;
//	//::ntl::httpPath(pathScript, "filter", "js", fileScriptMenu);
//	//::ntl::htmlHeaderScriptFile({fileScriptMenu.begin(), fileScriptMenu.size()}, output);
//
//	append_strings(output, 
//		"\n</td>"
//		"\n</tr>"
//		"\n</table>"
//		"\n</body>"
//		);
//
//	append_strings(output, "\n</html>");
//	return 0;
//}
//
