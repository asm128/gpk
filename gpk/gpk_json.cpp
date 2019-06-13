#include "gpk_json.h"
#include "gpk_eval.h"

						::gpk::error_t									jsonTreeRebuild										(::gpk::array_obj<::gpk::SJSONType>& object, ::gpk::SJSONNode& jsonRoot)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>							tree;
	gpk_necall(tree.resize(object.size()), "Out of memory? Object count: %u.", object.size());
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		tree[iObject]->Object													= &object[iObject];
		tree[iObject]->Parent													= ::gpk::in_range((uint32_t)tree[iObject]->Object->ParentIndex, 0U, tree.size()) ? (gpk::SJSONNode*)tree[tree[iObject]->Object->ParentIndex] : 0;
		tree[iObject]->ObjectIndex												= iObject;
	}
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(uint32_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SJSONNode>										& nodeOther											= tree[iOther];
			if(((uint32_t)nodeOther->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "Failed to push tree node. Out of memory?");
		}
	}
	jsonRoot																= {};
	jsonRoot.Children														= tree;
	return 0;
}

						::gpk::error_t									jsonParseKeyword									(const ::gpk::view_const_string& token, ::gpk::JSON_TYPE jsonType, ::gpk::SJSONParserState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	if(jsonAsString.size() - stateParser.IndexCurrentChar >= token.size() && 0 == strncmp(token.begin(), &jsonAsString[stateParser.IndexCurrentChar], token.size())) { 
		info_printf("JSON token found: %s.", token.begin());
		::gpk::SJSONType															boolElement											= {stateParser.IndexCurrentElement, jsonType, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + token.size()}};
		gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", object.size());
		stateParser.IndexCurrentChar											+= token.size() - 1;
	} 
	return 0;
}

						::gpk::error_t									lengthJsonNumber									(uint32_t indexCurrentChar, const ::gpk::view_const_string& jsonAsString)	{
	const uint32_t																offset												= indexCurrentChar;
	char																		charCurrent											= jsonAsString[indexCurrentChar];
	while(indexCurrentChar < jsonAsString.size() && 
		( ( charCurrent >= '1' && charCurrent <= '9') 
		 || charCurrent == '0' 
		 || charCurrent == '.' 
		 )
		) {
		++indexCurrentChar;
		charCurrent													= jsonAsString[indexCurrentChar];
	}
	return indexCurrentChar - offset;
}

						::gpk::error_t									parseJsonNumber										(::gpk::SJSONParserState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	(void)object;
	char																		bufferFormat[1024];
	const uint32_t																offset												= stateParser.IndexCurrentChar;
	uint32_t																	index												= offset;

	char																		charCurrent											= jsonAsString[index];
	uint32_t																	countSignPlus										= 0;
	uint32_t																	countSignMinus										= 0;
	while(index < jsonAsString.size() && 
		 ( charCurrent == '-'
		|| charCurrent == '+'
		)) {
		if(charCurrent == '-')
			++countSignMinus;
		else
			++countSignPlus;

		++index;
		charCurrent													= jsonAsString[index];
	}
	const bool																	isNegative											= 0 != (countSignMinus % 2);
	bool																		isHex												= false;
	bool																		isFloat												= false;
	if(index < jsonAsString.size() && jsonAsString[index] == '.') {
		isFloat																	= true;
		++index;
	}
	else if(index < jsonAsString.size() && jsonAsString[index] == 'x') {
		isHex																	= true;
		++index;
	}
	const uint32_t																sizeNum												= lengthJsonNumber(index, jsonAsString);
	//uint32_t 
	gpk_necall(sprintf_s(bufferFormat, "Number found: %%%u.%us. Length: %u. Negative: %s. Hex: %s. Float: %s.", sizeNum, sizeNum, sizeNum
		, isNegative	? "true" : "false"
		, isHex			? "true" : "false"
		, isFloat		? "true" : "false"
	), "Failed to parse json number. Number too long.");
	info_printf(bufferFormat, &jsonAsString[index]);
	stateParser.IndexCurrentChar											+= sizeNum + (index - offset) - 1;
	return 0;
}
						::gpk::error_t									jsonParseDocumentCharacter							(::gpk::SJSONParserState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															* temp												= 0;
	::gpk::SJSONType															currentElement										= {};
	const uint32_t																sizeRemaining										= jsonAsString.size() - stateParser.IndexCurrentChar;
	static const ::gpk::view_const_string										tokenFalse											= "false";
	static const ::gpk::view_const_string										tokenTrue											= "true";
	static const ::gpk::view_const_string										tokenNull											= "null";

	switch(stateParser.CharCurrent) {
	default:
		break;
	case 'f'	: e_if(errored(jsonParseKeyword(tokenFalse	, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString)), "Failed to parse token: %s.", tokenFalse.begin()); break;
	case 't'	: e_if(errored(jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString)), "Failed to parse token: %s.", tokenTrue	.begin()); break;
	case 'n'	: e_if(errored(jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_NULL, stateParser, object, jsonAsString)), "Failed to parse token: %s.", tokenNull	.begin()); break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.'	: case 'x'	: case '-'	: case '+'	: // parse int or float accordingly
		be_if(stateParser.Escaping, "Invalid character found at index %u: %c.", stateParser.IndexCurrentChar, stateParser.CharCurrent);	// Set an error or something and skip this character.
		parseJsonNumber(stateParser, object, jsonAsString);
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
		info_printf("opening at index %.2u a level %u %s. Parent index: %u.", object.size(), stateParser.NestLevel, 
			( '{' == stateParser.CharCurrent ? "object" 
			: '[' == stateParser.CharCurrent ? "array" 
			: "string"
			), stateParser.IndexCurrentElement); 
		currentElement																= {stateParser.IndexCurrentElement, 
			( '{' == stateParser.CharCurrent ? ::gpk::JSON_TYPE_OBJECT 
			: '[' == stateParser.CharCurrent ? ::gpk::JSON_TYPE_ARRAY
			: ::gpk::JSON_TYPE_STRING
			), {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar}};
		stateParser.IndexCurrentElement												= object.push_back(currentElement); 
		if(stateParser.CharCurrent == '"')
			stateParser.InsideString													= true;
		break;	// Need to report that a block has been entered
	case '}'	: 
	case ']'	: 
		temp																		= &object[stateParser.IndexCurrentElement]; 
		info_printf("closing at index %.2u a level %u %s. Parent index: %u.", stateParser.IndexCurrentElement, stateParser.NestLevel, '{' == stateParser.CharCurrent ? "object" : "array", temp->ParentIndex); 
		--stateParser.NestLevel; 
		temp->Span.End																= stateParser.IndexCurrentChar + 1; 
		stateParser.IndexCurrentElement												= temp->ParentIndex; 
		break;	// Need to report that a list has been exited
	}
	stateParser.Escaping													= false;
	return 0;
}

						::gpk::error_t									jsonParseStringCharacter							(::gpk::SJSONParserState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	switch(stateParser.CharCurrent) {
	default:
		break;
	case 'u':
		if(false == stateParser.Escaping) 
			break;
		stateParser.IndexCurrentChar											+= 1;	// skip the u to get the next 4 digits.
		ree_if(jsonAsString.size() - stateParser.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %s. Current index: %u.", jsonAsString.size(), stateParser.IndexCurrentChar);
		info_printf("Unicode code point found: %4.4s", &jsonAsString[stateParser.IndexCurrentChar]);
		// Parse unicode code point
		stateParser.IndexCurrentChar											+= 3;
		break;
	case '\\'	: 
		if(false == stateParser.Escaping) {
			stateParser.Escaping													= true;
			return 0;
		}
		break;
	case '"'	: 
		if(false == stateParser.Escaping) {
			::gpk::SJSONType														* temp												= &object[stateParser.IndexCurrentElement]; 
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

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															currentElement										= {};
	currentElement.ParentIndex												= -1;
	currentElement.Span														= {0, jsonAsString.size()};
	currentElement.Type														= ::gpk::JSON_TYPE_UNKNOWN;

	SJSONParserState															& stateParser										= reader.StateOfParser;
	stateParser.IndexCurrentElement											= reader.Object.push_back(currentElement);
	for(stateParser.IndexCurrentChar = 0; stateParser.IndexCurrentChar < (int32_t)jsonAsString.size(); ++stateParser.IndexCurrentChar) {
		stateParser.CharCurrent													= jsonAsString[stateParser.IndexCurrentChar];
		if(stateParser.InsideString) 
			::jsonParseStringCharacter(stateParser, reader.Object, jsonAsString);
		else 
			::jsonParseDocumentCharacter(stateParser, reader.Object, jsonAsString);
	}
	info_printf("Nest level: %u (Needs to be zero)", stateParser.NestLevel);
	reader.Tree.create();
	return ::jsonTreeRebuild(reader.Object, *reader.Tree);
}
