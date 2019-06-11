#include "gpk_json.h"
#include "gpk_eval.h"

						::gpk::error_t									jsonTreeRebuild										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonRoot)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>							tree;
	gpk_necall(tree.resize(document.Object.size()), "Out of memory?");
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		tree[iObject]->Object													= &document.Object[iObject];
		tree[iObject]->Parent													= ::gpk::in_range((uint32_t)tree[iObject]->Object->ParentIndex, 0U, tree.size()) ? (gpk::SJSONNode*)tree[tree[iObject]->Object->ParentIndex] : 0;
		tree[iObject]->ObjectIndex												= iObject;
	}
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		for(uint32_t iOther = 0; iOther < tree.size(); ++iOther) {
			if(((uint32_t)tree[iOther]->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(tree[iOther]), "Failed to push tree node. Out of memory?");
		}
	}
	jsonRoot																= {};
	jsonRoot.Children														= tree;
	return 0;
}

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const char* _jsonAsString, uint32_t jsonLength)	{
	::gpk::view_const_string													jsonAsString										= {_jsonAsString, jsonLength};
	::gpk::SJSONType															currentElement										= {};
	currentElement.ParentIndex												= -1;
	currentElement.Span														= {0, jsonLength};
	currentElement.Type														= ::gpk::JSON_TYPE_UNKNOWN;

	int32_t																		elementIndexCurrent									= document.Object.push_back(currentElement);
	uint32_t																	nestLevel											= 0;
	bool																		escaping											= false;
	bool																		insideString										= false;
	for(uint32_t iDocChar = 0; iDocChar < jsonLength; ++iDocChar) {
		::gpk::SJSONType															* temp												= 0;
		if(insideString) {
			switch(jsonAsString[iDocChar]) {
			default:
				break;
			case '\\'	: 
				if(false == escaping) {
					escaping																= true;
					continue;
				}
				break;
			case '"'	: 
				if(false == escaping) {
					temp																= &document.Object[elementIndexCurrent]; 
					info_printf("closing at index %.2u a level %u string. Parent index: %u.", elementIndexCurrent, nestLevel, temp->ParentIndex); 
					--nestLevel; 
					temp->Span.End														= iDocChar + 1; 
					elementIndexCurrent													= temp->ParentIndex; 
					insideString														= false;
				}
			}
		}
		else {
			switch(jsonAsString[iDocChar]) {
			default:
				break;
			case 'f'	: 
				if(jsonLength - iDocChar >= 5 && 0 == strncmp("false", &jsonAsString[iDocChar], 5)) { info_printf("false");
					::gpk::SJSONType															boolElement										= {elementIndexCurrent, ::gpk::JSON_TYPE_BOOL, {iDocChar, iDocChar + 5}};
					document.Object.push_back(boolElement);
				} 
				break;
			case 't'	: 
				if(jsonLength - iDocChar >= 4 && 0 == strncmp("true", &jsonAsString[iDocChar], 4)) { info_printf("true");
					::gpk::SJSONType															boolElement										= {elementIndexCurrent, ::gpk::JSON_TYPE_BOOL, {iDocChar, iDocChar + 4}};
					document.Object.push_back(boolElement);
				} 
				break;
			case 'n'	: 
				if(jsonLength - iDocChar >= 4 && 0 == strncmp("null", &jsonAsString[iDocChar], 4)) { info_printf("null");
					::gpk::SJSONType															boolElement										= {elementIndexCurrent, ::gpk::JSON_TYPE_NULL, {iDocChar, iDocChar + 4}};
					document.Object.push_back(boolElement);
				} 
				break;
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
				if(escaping) {
					break;
				}
			case '.'	:	// parse int or float accordingly
				break;
			case ' '	:
			case '\t'	:
			case '\r'	:
			case '\n'	: break;	// These separator characters mean nothing in json.
			case ':'	: break;	// Need to report that we've switched from element name to element value
			case ','	: break;	// Need to report that we've switched from an element to the next
			case '{'	: 
				++nestLevel; 
				info_printf("opening at index %.2u a level %u object. Parent index: %u.", document.Object.size(), nestLevel, elementIndexCurrent); 
				currentElement																= {elementIndexCurrent, ::gpk::JSON_TYPE_OBJECT, {iDocChar, iDocChar}};
				elementIndexCurrent															= document.Object.push_back(currentElement); 
				break;	// Need to report that a block has been entered
			case '['	: ++nestLevel; 
				info_printf("opening at index %.2u a level %u array. Parent index: %u.", document.Object.size(), nestLevel, elementIndexCurrent); 
				currentElement																= {elementIndexCurrent, ::gpk::JSON_TYPE_ARRAY, {iDocChar, iDocChar}};
				elementIndexCurrent															= document.Object.push_back(currentElement); 
				break;	// Need to report that a list has been entered
			case '}'	: 
				temp																		= &document.Object[elementIndexCurrent]; 
				info_printf("closing at index %.2u a level %u object. Parent index: %u.", elementIndexCurrent, nestLevel, temp->ParentIndex); 
				--nestLevel; 
				temp->Span.End																= iDocChar + 1; 
				elementIndexCurrent															= temp->ParentIndex; 
				break;	// Need to report that a block has been exited
			case ']'	: 
				temp																		= &document.Object[elementIndexCurrent]; 
				info_printf("closing at index %.2u a level %u array. Parent index: %u.", elementIndexCurrent, nestLevel, temp->ParentIndex); 
				--nestLevel; 
				temp->Span.End																= iDocChar + 1; 
				elementIndexCurrent															= temp->ParentIndex; 
				break;	// Need to report that a list has been exited
			case '"'	: 
				++nestLevel; 
				info_printf("opening at index %.2u a level %u string. Parent index: %u.", document.Object.size(), nestLevel, elementIndexCurrent); 
				currentElement																= {elementIndexCurrent, ::gpk::JSON_TYPE_STRING, {iDocChar, iDocChar}};
				elementIndexCurrent															= document.Object.push_back(currentElement); 
				insideString																= true;
				break;	// Need to report that a string has been entered
			}
		}
		escaping								= false;
	}
	info_printf("Nest level: %u (Needs to be zero)", nestLevel);
	return ::jsonTreeRebuild(document, jsonTree);
}
