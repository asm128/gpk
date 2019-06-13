#include "gpk_json.h"
#include "gpk_eval.h"

						::gpk::error_t									jsonTreeRebuild										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonRoot)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>							tree;
	gpk_necall(tree.resize(document.Object.size()), "Out of memory? Object count: %u.", document.Object.size());
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		tree[iObject]->Object													= &document.Object[iObject];
		tree[iObject]->Parent													= ::gpk::in_range((uint32_t)tree[iObject]->Object->ParentIndex, 0U, tree.size()) ? (gpk::SJSONNode*)tree[tree[iObject]->Object->ParentIndex] : 0;
		tree[iObject]->ObjectIndex												= iObject;
	}
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(uint32_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SJSONNode>										& nodeOther												= tree[iOther];
			if(((uint32_t)nodeOther->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "Failed to push tree node. Out of memory?");
		}
	}
	jsonRoot																= {};
	jsonRoot.Children														= tree;
	return 0;
}

#pragma pack(push, 1)
struct SJSONParserState {
	uint32_t	IndexCurrentChar	= 0;
	int32_t		IndexCurrentElement	= -1;
	int32_t		NestLevel			= 0;
	char		CharCurrent			= 0;
	bool		Escaping			= 0;
	bool		InsideString		= 0;
};
#pragma pack(pop)

						::gpk::error_t									jsonParseKeyword									(const ::gpk::view_const_string& token, ::gpk::JSON_TYPE jsonType, ::SJSONParserState& stateParser, ::gpk::SJSONDocument& document, ::gpk::view_const_string jsonAsString)	{
	if(jsonAsString.size() - stateParser.IndexCurrentChar >= token.size() && 0 == strncmp(token.begin(), &jsonAsString[stateParser.IndexCurrentChar], token.size())) { 
		info_printf("JSON token found: %s.", token.begin());
		::gpk::SJSONType															boolElement										= {stateParser.IndexCurrentElement, jsonType, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + token.size()}};
		gpk_necall(document.Object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", document.Object.size());
		stateParser.IndexCurrentChar											+= token.size() - 1;
	} 
	return 0;
}

						::gpk::error_t									jsonParseDocumentCharacter							(::SJSONParserState& stateParser, ::gpk::SJSONDocument& document, ::gpk::view_const_string jsonAsString)	{
	::gpk::SJSONType															* temp												= 0;
	::gpk::SJSONType															currentElement										= {};
	const uint32_t																sizeRemaining										= jsonAsString.size() - stateParser.IndexCurrentChar;
	static const ::gpk::view_const_string										tokenFalse											= "false";
	static const ::gpk::view_const_string										tokenTrue											= "true";
	static const ::gpk::view_const_string										tokenNull											= "null";

	switch(stateParser.CharCurrent) {
	default:
		break;
	case 'f'	: jsonParseKeyword(tokenFalse	, ::gpk::JSON_TYPE_BOOL, stateParser, document, jsonAsString); break;
	case 't'	: jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateParser, document, jsonAsString); break;
	case 'n'	: jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_BOOL, stateParser, document, jsonAsString); break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
		if(stateParser.Escaping) {
			break;
		}
	case '.'	:	// parse int or float accordingly
		break;
	case ' '	:
	case '\t'	: case '\r'	: case '\n'	: 
		break;	// These separator characters mean nothing in json.
	case ':'	: break;	// Need to report that we've switched from element name to element value
	case ','	: break;	// Need to report that we've switched from an element to the next
	case '{'	: 
	case '['	: 
	case '"'	: 
		++stateParser.NestLevel; 
		info_printf("opening at index %.2u a level %u %s. Parent index: %u.", document.Object.size(), stateParser.NestLevel, 
			( '{' == stateParser.CharCurrent ? "object" 
			: '[' == stateParser.CharCurrent ? "array" 
			: "string"
			), stateParser.IndexCurrentElement); 
		currentElement																= {stateParser.IndexCurrentElement, 
			( '{' == stateParser.CharCurrent ? ::gpk::JSON_TYPE_OBJECT 
			: '[' == stateParser.CharCurrent ? ::gpk::JSON_TYPE_ARRAY
			: ::gpk::JSON_TYPE_STRING
			), {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar}};
		stateParser.IndexCurrentElement												= document.Object.push_back(currentElement); 
		if(stateParser.CharCurrent == '"')
			stateParser.InsideString													= true;
		break;	// Need to report that a block has been entered
	case '}'	: 
	case ']'	: 
		temp																		= &document.Object[stateParser.IndexCurrentElement]; 
		info_printf("closing at index %.2u a level %u %s. Parent index: %u.", stateParser.IndexCurrentElement, stateParser.NestLevel, '{' == stateParser.CharCurrent ? "object" : "array", temp->ParentIndex); 
		--stateParser.NestLevel; 
		temp->Span.End																= stateParser.IndexCurrentChar + 1; 
		stateParser.IndexCurrentElement												= temp->ParentIndex; 
		break;	// Need to report that a list has been exited
	}
	stateParser.Escaping													= false;
	return 0;
}

						::gpk::error_t									jsonParseStringCharacter							(SJSONParserState& stateParser, ::gpk::SJSONDocument& document)	{
	switch(stateParser.CharCurrent) {
	default:
		break;
	case '\\'	: 
		if(false == stateParser.Escaping) {
			stateParser.Escaping													= true;
			return 0;
		}
		break;
	case '"'	: 
		if(false == stateParser.Escaping) {
			::gpk::SJSONType														* temp												= &document.Object[stateParser.IndexCurrentElement]; 
			info_printf("Closing at index %.2u a level %u string. Parent index: %u.", stateParser.IndexCurrentElement, stateParser.NestLevel, temp->ParentIndex); 
			--stateParser.NestLevel; 
			temp->Span.End															= stateParser.IndexCurrentChar + 1; 
			stateParser.IndexCurrentElement											= temp->ParentIndex; 
			stateParser.InsideString												= false;
		}
	}
	stateParser.Escaping													= false;
	return 0;
}

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONDocument& document, ::gpk::SJSONNode& jsonTree, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															currentElement										= {};
	currentElement.ParentIndex												= -1;
	currentElement.Span														= {0, jsonAsString.size()};
	currentElement.Type														= ::gpk::JSON_TYPE_UNKNOWN;

	SJSONParserState															stateParser											= {};
	stateParser.IndexCurrentElement											= document.Object.push_back(currentElement);

	for(stateParser.IndexCurrentChar = 0; stateParser.IndexCurrentChar < (int32_t)jsonAsString.size(); ++stateParser.IndexCurrentChar) {
		stateParser.CharCurrent													= jsonAsString[stateParser.IndexCurrentChar];
		if(stateParser.InsideString) 
			::jsonParseStringCharacter(stateParser, document);
		else 
			::jsonParseDocumentCharacter(stateParser, document, jsonAsString);
	}
	info_printf("Nest level: %u (Needs to be zero)", stateParser.NestLevel);
	return ::jsonTreeRebuild(document, jsonTree);
}
