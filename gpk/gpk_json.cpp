#include "gpk_json.h"
#include "gpk_eval.h"

						::gpk::error_t									jsonTreeRebuild										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonRoot)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>						tree;
	gpk_necall(tree.resize(document.Object.size()), "Out of memory?");

	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		tree[iObject]->Object													= &document.Object[iObject];
		tree[iObject]->Parent													= ::gpk::in_range((uint32_t)tree[iObject]->Object->ParentIndex, 0U, tree.size()) ? (gpk::SJSONNode*)tree[tree[iObject]->Object->ParentIndex] : 0;
		tree[iObject]->ObjectIndex												= iObject;
	}

	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		for(uint32_t iOther = 0; iOther < tree.size(); ++iOther)
			if(((uint32_t)tree[iOther]->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(tree[iOther]), "Failed to push tree node. Out of memory?");
	}
	jsonRoot																= {};
	jsonRoot.Children														= tree;
	return 0;
}

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const char* jsonAsString, uint32_t jsonLength)	{
	::gpk::SJSONType															currentElement										= {};
	currentElement.ParentIndex												= -1;
	currentElement.Span														= {0, jsonLength};
	currentElement.Type														= ::gpk::JSON_TYPE_UNKNOWN;

	int32_t																		elementIndexCurrent									= document.Object.push_back(currentElement);
	uint32_t																	nestLevel											= 0;
	for(uint32_t iDocChar = 0; iDocChar < jsonLength; ++iDocChar) {
		switch(jsonAsString[iDocChar]) {
		case ' '	:
		case '\t'	:
		case '\r'	:
		case '\n'	: continue;	// These separator characters mean nothing in json.
		case ':'	: continue;	// Need to report that we've switched from element name to element value
		case ','	: continue;	// Need to report that we've switched from an element to the next
		case '{'	: ++nestLevel; info_printf("opening level %u object", nestLevel); currentElement.Type = ::gpk::JSON_TYPE_OBJECT; currentElement.Span = {iDocChar, iDocChar}; currentElement.ParentIndex = elementIndexCurrent; elementIndexCurrent = document.Object.push_back(currentElement); continue;	// Need to report that a block has been entered
		case '['	: ++nestLevel; info_printf("opening level %u array"	, nestLevel); currentElement.Type = ::gpk::JSON_TYPE_ARRAY ; currentElement.Span = {iDocChar, iDocChar}; currentElement.ParentIndex = elementIndexCurrent; elementIndexCurrent = document.Object.push_back(currentElement); continue;	// Need to report that a list has been entered
		case '}'	: info_printf("closing level %u object"	, nestLevel); --nestLevel; document.Object[elementIndexCurrent].Span.End = iDocChar + 1; elementIndexCurrent = currentElement.ParentIndex; continue;	// Need to report that a block has been exited
		case ']'	: info_printf("closing level %u array"	, nestLevel); --nestLevel; document.Object[elementIndexCurrent].Span.End = iDocChar + 1; elementIndexCurrent = currentElement.ParentIndex; continue;	// Need to report that a list has been exited
		default		:
			switch(jsonAsString[iDocChar]) {	// --- We may have a word... or a number.
			case '0'	:
			case '1'	:
			case '2'	:
			case '3'	:
			case '4'	:
			case '5'	:
			case '6'	:
			case '7'	:
			case '8'	:
			case '9'	:
			case '.'	:	// parse int or float accordingly
				break;
			default		:	// parse text
			{
				::gpk::SSlice<uint32_t>													keywordSpan											= {iDocChar, iDocChar};
				bool																	insideQuotes										= false;
				while(keywordSpan.End < jsonLength) {
					if( jsonAsString[keywordSpan.End] == '"' )
						insideQuotes															= ((keywordSpan.End > keywordSpan.Begin) && jsonAsString[keywordSpan.End - 1] == '\\') ? insideQuotes : !insideQuotes;
					else {
						if( false == insideQuotes &&
							(  jsonAsString[keywordSpan.End] == ' '
							|| jsonAsString[keywordSpan.End] == '\t'
							|| jsonAsString[keywordSpan.End] == '\r'
							|| jsonAsString[keywordSpan.End] == '\n'
							|| jsonAsString[keywordSpan.End] == ':'
							|| jsonAsString[keywordSpan.End] == ','
							|| jsonAsString[keywordSpan.End] == '{'
							|| jsonAsString[keywordSpan.End] == '}'
							|| jsonAsString[keywordSpan.End] == '['
							|| jsonAsString[keywordSpan.End] == ']'
							)
							)
							break;
					}
					++keywordSpan.End;
				}
				const uint32_t																keywordLength										= keywordSpan.End - keywordSpan.Begin;
				iDocChar																+= keywordLength;
				break;
			}
			}
			continue;
		}
	}
	info_printf("Nest level: %u (Needs to be zero)", nestLevel);
	return ::jsonTreeRebuild(document, jsonTree);
}

// [ { "Id" :"TuMami", "NombrePuesto": "Puesto", "IdExterno" : "out" }, { "Id": "TuPapi", "NombrePuesto": "Puesto 2", "IdExterno": "out2 " } ]
