#include "gpk_json.h"
#include "gpk_eval.h"

						::gpk::error_t									jsonTreeRebuild										(::gpk::array_obj<::gpk::SJSONType>& object, ::gpk::SJSONNode& jsonRoot)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>							tree;
	gpk_necall(tree.resize(object.size()), "Out of memory? Object count: %u.", object.size());
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::ptr_obj<::gpk::SJSONNode>											& nodeCurrent										= tree[iObject];
		nodeCurrent->Object														= &object[iObject];
		nodeCurrent->Parent														= ::gpk::in_range((uint32_t)nodeCurrent->Object->ParentIndex, 0U, tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Object->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex												= iObject;
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
		charCurrent																= jsonAsString[index];
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
	::gpk::SJSONType															currentElement										= {stateParser.IndexCurrentElement, ::gpk::JSON_TYPE_INT, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + sizeNum + (index - stateParser.IndexCurrentChar)}};
	gpk_necall(object.push_back(currentElement), "%s", "Out of memory?"); 
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
	::gpk::error_t																errVal												= 0;

	switch(stateParser.CharCurrent) {
	default:
		break;
	case 'f'	: if(errored(jsonParseKeyword(tokenFalse, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString))) {	error_printf("Failed to parse token: %s.", tokenFalse	.begin()); errVal = -1; break; }
	case 't'	: if(errored(jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString))) {	error_printf("Failed to parse token: %s.", tokenTrue	.begin()); errVal = -1; break; }
	case 'n'	: if(errored(jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_NULL, stateParser, object, jsonAsString))) {	error_printf("Failed to parse token: %s.", tokenNull	.begin()); errVal = -1; break; }
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.'	: case 'x'	: case '-'	: case '+'	: // parse int or float accordingly
		be_if(stateParser.Escaping, "Invalid character found at index %u: %c.", stateParser.IndexCurrentChar, stateParser.CharCurrent);	// Set an error or something and skip this character.
		if(errored(parseJsonNumber(stateParser, object, jsonAsString))) {
			errVal																= -1;
			error_printf("Error parsing JSON number.");
		}
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
	return errVal;
}

						::gpk::error_t									jsonParseStringCharacter							(::gpk::SJSONParserState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															currentElement										= {};
	switch(stateParser.CharCurrent) {
	default:
		break;
	case 'u':
		if(false == stateParser.Escaping) 
			break;
		stateParser.IndexCurrentChar											+= 1;	// skip the u to get the next 4 digits.
		ree_if(jsonAsString.size() - stateParser.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %s. Current index: %u.", jsonAsString.size(), stateParser.IndexCurrentChar);
		info_printf("Unicode code point found: %4.4s", &jsonAsString[stateParser.IndexCurrentChar]);
		currentElement															= {stateParser.IndexCurrentElement, ::gpk::JSON_TYPE_CODEPOINT, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + 4}};
		gpk_necall(object.push_back(currentElement), "%s", "Out of memory?"); 
		stateParser.IndexCurrentChar											+= 3;	// Parse unicode code point
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

						::gpk::error_t									gpk::jsonParseStep									(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	reader.StateOfParser.CharCurrent										= jsonAsString[reader.StateOfParser.IndexCurrentChar];
	return (reader.StateOfParser.InsideString)
		? ::jsonParseStringCharacter	(reader.StateOfParser, reader.Object, jsonAsString) 
		: ::jsonParseDocumentCharacter	(reader.StateOfParser, reader.Object, jsonAsString)
		;
}

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	SJSONParserState															& stateParser										= reader.StateOfParser;
	for(stateParser.IndexCurrentChar = 0; stateParser.IndexCurrentChar < (int32_t)jsonAsString.size(); ++stateParser.IndexCurrentChar)
		error_if(errored(::gpk::jsonParseStep(reader, jsonAsString)), "Error during read step."
			"\nPosition  : %i." 
			"\nCharacter : %c."
			"\nElement   : %i."
			"\nString    : %s."
			"\nEscaping  : %s."
			"\nNestLevel : %i."
			, reader.StateOfParser.IndexCurrentChar
			, reader.StateOfParser.CharCurrent
			, reader.StateOfParser.IndexCurrentElement 
			, reader.StateOfParser.InsideString ? "true" : "false"
			, reader.StateOfParser.Escaping		? "true" : "false"
			, reader.StateOfParser.NestLevel
		)
	info_printf("Nest level: %u (Needs to be zero).", stateParser.NestLevel);
	ree_if(errored(reader.Tree.create()), "If this fail then maybe it's because of %s.", "running out of memory or memory corrupted.");
	return ::jsonTreeRebuild(reader.Object, *reader.Tree);
}
