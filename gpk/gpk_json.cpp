#include "gpk_json.h"

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

						::gpk::error_t									jsonParseKeyword									(const ::gpk::view_const_string& token, ::gpk::JSON_TYPE jsonType, ::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	ree_if(token.size() > jsonAsString.size() - stateParser.IndexCurrentChar, "End of stream while parsing token: %s.", token.begin());
	ree_if(0 != strncmp(token.begin(), &jsonAsString[stateParser.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%s'.", token.begin());
	info_printf("JSON token found: %s.", token.begin());
	::gpk::SJSONType															boolElement											= {stateParser.IndexCurrentElement, jsonType, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + token.size()}};
	gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", object.size());
	stateParser.IndexCurrentChar											+= token.size() - 1;
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

						::gpk::error_t									parseJsonNumber										(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	(void)object;
	char																		bufferFormat[1024];
	const uint32_t																offset												= stateParser.IndexCurrentChar;
	uint32_t																	index												= offset;

	char																		charCurrent											= jsonAsString[index];
	bool																		isNegative											= false;
	bool																		isFloat												= false;
	if(index < jsonAsString.size() && jsonAsString[index] == '+') 
		++index;
	if(index < jsonAsString.size() && jsonAsString[index] == '-') {
		isNegative																= true;
		++index;
	}
	if(index < jsonAsString.size() && jsonAsString[index] == '+') 
		++index;
	if(index < jsonAsString.size() && jsonAsString[index] == '.') {
		isFloat																	= true;
		++index;
	}
	charCurrent																= jsonAsString[index];
	ree_if(index < jsonAsString.size() && (charCurrent != '0' && (charCurrent < '1' || charCurrent > '9')), "Character '%c' at index %i is not a number.", charCurrent, index); 
	const uint32_t																sizeNum												= lengthJsonNumber(index, jsonAsString);
	::gpk::SJSONType															currentElement										= {stateParser.IndexCurrentElement, ::gpk::JSON_TYPE_NUMBER, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + sizeNum + (index - stateParser.IndexCurrentChar)}};
	gpk_necall(object.push_back(currentElement), "%s", "Out of memory?"); 
	gpk_necall(sprintf_s(bufferFormat, "Number found: %%%u.%us. Length: %u. Negative: %s. Float: %s.", sizeNum, sizeNum, sizeNum
		, isNegative	? "true" : "false"
		, isFloat		? "true" : "false"
	), "Failed to parse json number. Number too long.");
	info_printf(bufferFormat, &jsonAsString[index]);
	stateParser.IndexCurrentChar											+= sizeNum + (index - offset) - 1;
	charCurrent																= jsonAsString[stateParser.IndexCurrentChar+1];
	ree_if(charCurrent != ' ' 
		&& charCurrent != '\t' 
		&& charCurrent != '\n' 
		&& charCurrent != '\r' 
		&& charCurrent != '\n' 
		&& charCurrent != ']' 
		&& charCurrent != '}' 
		&& charCurrent != ','
		, "Invalid number termination: '%c'.", charCurrent)
	return 0;
}

						::gpk::error_t									jsonCloseElement									(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, uint32_t indexChar) {
	ree_if(object.size() <= (uint32_t)stateParser.IndexCurrentElement, "Invalid parser state. Cannot close element: %i.", stateParser.IndexCurrentElement);
	::gpk::SJSONType															* closing											= 0;
	closing																	= stateParser.CurrentElement; //&object[stateParser.IndexCurrentElement]; 
	closing->Span.End														= (uint32_t)indexChar + 1; 
	const ::gpk::view_const_string												labelType											= ::gpk::get_value_label(closing->Type);
	info_printf("%s closed. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i. End: %i.", labelType.begin(), stateParser.IndexCurrentElement, stateParser.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End); 
	stateParser.IndexCurrentElement											= closing->ParentIndex; 
	--stateParser.NestLevel; 
	stateParser.CurrentElement												= ((uint32_t)stateParser.IndexCurrentElement < object.size()) ? &object[stateParser.IndexCurrentElement] : 0;
	return 0;	// Need to report that a list has been exited
}

						::gpk::error_t									jsonCloseElement									(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, uint32_t indexChar, ::gpk::JSON_TYPE jsonType) {
	const ::gpk::SJSONType														* open												= stateParser.CurrentElement; //&object[stateParser.IndexCurrentElement];
	ree_if(jsonType != open->Type, "Invalid object type: open: %u (%s). closing: %u (%s).", open->Type, ::gpk::get_value_label(open->Type).begin(), jsonType, ::gpk::get_value_label(jsonType).begin());
	return ::jsonCloseElement(stateParser, object, indexChar);
}

						::gpk::error_t									jsonTestAndCloseValue								(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object) {
	if(stateParser.CurrentElement && ::gpk::JSON_TYPE_VALUE == stateParser.CurrentElement->Type) {
		::gpk::error_t																errVal												= ::jsonCloseElement(stateParser, object, stateParser.IndexCurrentChar); 
		stateParser.ExpectingSeparator											= true;
		return errVal;
	}
	return 1;
}

						::gpk::error_t									jsonTestAndCloseKey									(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object) {
	if(stateParser.CurrentElement && ::gpk::JSON_TYPE_KEY == stateParser.CurrentElement->Type) {
		::gpk::error_t																errVal												= ::jsonCloseElement(stateParser, object, stateParser.IndexCurrentChar); 
		stateParser.ExpectingSeparator											= true;
		return errVal;
	}
	return 1;
}

						::gpk::error_t									jsonCloseOrDiscardIfEmptyKeyOrVal					(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t																errVal												= 0;
	if(object[object.size() - 1].Type == containerType) { 
		info_printf("Discarding empty container element at index %i (%s). Level: %i", object.size() - 1, ::gpk::get_value_label(containerType).begin(), stateParser.NestLevel);
		object.pop_back(0); 
		--stateParser.IndexCurrentElement; 
		--stateParser.NestLevel; 
		if((uint32_t)stateParser.IndexCurrentElement < object.size())
			stateParser.CurrentElement												= &object[stateParser.IndexCurrentElement];
	} 
	else {
		info_printf("Closing container at index %i (%s).", object.size() - 1, ::gpk::get_value_label(containerType).begin());
			 if(::gpk::JSON_TYPE_VALUE	== containerType) errVal = ::jsonTestAndCloseValue	(stateParser, object); 
		else if(::gpk::JSON_TYPE_KEY	== containerType) errVal = ::jsonTestAndCloseKey	(stateParser, object); 
	}
	return errVal;
}
						::gpk::error_t									jsonCloseContainer									(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t																errVal												= 0;
	gpk_necall(::jsonCloseOrDiscardIfEmptyKeyOrVal(stateParser, object, (::gpk::JSON_TYPE_ARRAY == containerType) ? ::gpk::JSON_TYPE_VALUE : ::gpk::JSON_TYPE_KEY), "Failed to close container at index %i (%s).", stateParser.IndexCurrentElement, ::gpk::get_value_label(containerType).begin());
	errVal																	= ::jsonCloseElement(stateParser, object, stateParser.IndexCurrentChar, containerType); 
	stateParser.ExpectingSeparator											= false; 
	if(stateParser.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		::jsonTestAndCloseValue(stateParser, object);
	return 0;
}

						::gpk::error_t									jsonOpenElement										(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, ::gpk::JSON_TYPE jsonType, uint32_t indexChar) {
	::gpk::SJSONType															currentElement										= {stateParser.IndexCurrentElement, jsonType, {indexChar, indexChar}};
	gpk_necall(stateParser.IndexCurrentElement = object.push_back(currentElement), "Failed to push element. %s", "Out of memory?");
	stateParser.CurrentElement												= &object[stateParser.IndexCurrentElement];
	const ::gpk::view_const_string												labelType											= ::gpk::get_value_label(currentElement.Type);
	++stateParser.NestLevel; 
	info_printf("%s open. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i.", labelType.begin(), stateParser.IndexCurrentElement, stateParser.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin); 
	return 0;
}

						::gpk::error_t									jsonParseDocumentCharacter							(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															currentElement										= {};
	const uint32_t																sizeRemaining										= jsonAsString.size() - stateParser.IndexCurrentChar;
	static const ::gpk::view_const_string										tokenFalse											= "false"	;
	static const ::gpk::view_const_string										tokenTrue											= "true"	;
	static const ::gpk::view_const_string										tokenNull											= "null"	;
	::gpk::error_t																errVal												= 0;

#define GPK_JSON_EXPECTS_SEPARATOR()													\
	if (stateParser.ExpectingSeparator) { 												\
		errVal																	= -1; 	\
		error_printf("Expected separator, found '%c'.", stateParser.CharCurrent); 		\
		break; 																			\
	}

	switch(stateParser.CharCurrent) {
	case ' '	: case '\t'	: case '\r'	: case '\n'	: // Skip these characters without error.
		break;	
	default: // Fallback for every character that is not recognized by the parser.
		error_printf("Invalid character at index %i. '%c'", stateParser.IndexCurrentChar, stateParser.CharCurrent);
		GPK_JSON_EXPECTS_SEPARATOR();
		errVal																	= -1;
		break;
	case 'f'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenFalse	, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString); ::jsonTestAndCloseValue(stateParser, object); break; 
	case 't'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateParser, object, jsonAsString); ::jsonTestAndCloseValue(stateParser, object); break; 
	case 'n'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_NULL, stateParser, object, jsonAsString); ::jsonTestAndCloseValue(stateParser, object); break; 
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.'	: case '-'	: case '+'	: //case 'x'	: // parse int or float accordingly
		GPK_JSON_EXPECTS_SEPARATOR();
		be_if(stateParser.Escaping, "Invalid character found at index %u: %c.", stateParser.IndexCurrentChar, stateParser.CharCurrent);	// Set an error or something and skip this character.
		if errored(::parseJsonNumber(stateParser, object, jsonAsString)) {
			errVal																	= -1;
			error_printf("Error parsing JSON number.");
		}
		::jsonTestAndCloseValue(stateParser, object); 
		break;
	case ','	: 
		if(false == stateParser.ExpectingSeparator) {
			errVal																	= -1; 
			error_printf("Separator found when not expected at index %i.", stateParser.IndexCurrentChar); 
			break;
		}
		if(::gpk::JSON_TYPE_OBJECT == stateParser.CurrentElement->Type) {
			if errored(::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_KEY, stateParser.IndexCurrentChar + 1)) {
				errVal																	= -1; 
				error_printf("Failed to open element at index %i.", object.size()); 
			}
		}
		else if(::gpk::JSON_TYPE_ARRAY == stateParser.CurrentElement->Type) {	// Test 
			if errored(::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_VALUE, stateParser.IndexCurrentChar + 1)) {
				errVal																	= -1; 
				error_printf("Failed to open element at index %i.", object.size()); 
			}
		}
		stateParser.ExpectingSeparator											= false;
		break;	
	case ':'	: 
		if(false == stateParser.ExpectingSeparator && ::gpk::JSON_TYPE_KEY != stateParser.CurrentElement->Type) {	// Test if we should be expecting this separator or not.
			errVal																	= -1; 
			error_printf("Separator found when not expected at index %i.", stateParser.IndexCurrentChar); 
			break;
		}
		stateParser.ExpectingSeparator											= false;
		if errored(::jsonCloseElement(stateParser, object, stateParser.IndexCurrentChar - 1, ::gpk::JSON_TYPE_KEY	)) { // close the key before the : character
			errVal = -1; 
			error_printf("Failed to close key. %s", "Unknown error."); 
			break; 
		}
		if errored(::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_VALUE, stateParser.IndexCurrentChar + 1)) { // open the value after the : character
			errVal = -1; 
			error_printf("Failed to open element at index %i.", 
			object.size()); 
		} 
		break;	
	case ']'	: errVal = ::jsonCloseContainer(stateParser, object, ::gpk::JSON_TYPE_ARRAY	); break; 
	case '}'	: errVal = ::jsonCloseContainer(stateParser, object, ::gpk::JSON_TYPE_OBJECT); break; 
	case '{'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_OBJECT	, stateParser.IndexCurrentChar); if(0 == errVal) { errVal = jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_KEY	, stateParser.IndexCurrentChar + 1); }break;
	case '['	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_ARRAY	, stateParser.IndexCurrentChar); if(0 == errVal) { errVal = jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_VALUE	, stateParser.IndexCurrentChar + 1); }break;
	case '"'	: GPK_JSON_EXPECTS_SEPARATOR();
		if errored(::jsonOpenElement(stateParser, object, ::gpk::JSON_TYPE_STRING, stateParser.IndexCurrentChar + 1)) {
			errVal																	= -1; 
			error_printf("Failed to open element at index %i.", object.size());
		}
		stateParser.InsideString												= true;
		break;	
	}
	stateParser.Escaping													= false;
	return errVal;
}

						::gpk::error_t									jsonParseStringCharacter							(::gpk::SJSONReaderState& stateParser, ::gpk::array_obj<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType															currentElement										= {};
	::gpk::error_t																errVal												= 0;
	switch(stateParser.CharCurrent) {
	default:
		if(stateParser.CharCurrent < 0x20 || stateParser.CharCurrent > 0xFE) {
			errVal																	= -1;
			error_printf("Invalid character: %i (%u) '%c'.", stateParser.CharCurrent, (uchar_t)stateParser.CharCurrent, stateParser.CharCurrent);
		}
		break;
	case 'u':
		if(false == stateParser.Escaping) 
			break;
		stateParser.IndexCurrentChar											+= 1;	// skip the u to get the next 4 digits.
		if(jsonAsString.size() - stateParser.IndexCurrentChar < 4) {
			errVal																	= -1;
			error_printf("End of stream during unicode code point parsing. JSON length: %s. Current index: %u.", jsonAsString.size(), stateParser.IndexCurrentChar);
		}
		info_printf("Unicode code point found: %4.4s", &jsonAsString[stateParser.IndexCurrentChar]);
		currentElement															= {stateParser.IndexCurrentElement, ::gpk::JSON_TYPE_CODEPOINT, {stateParser.IndexCurrentChar, stateParser.IndexCurrentChar + 4}};
		if errored(object.push_back(currentElement)) {
			errVal																	= -1;
			error_printf("%s", "Out of memory?"); 
		}
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
			if errored(::jsonCloseElement(stateParser, object, stateParser.IndexCurrentChar - 1, ::gpk::JSON_TYPE_STRING)) {
				errVal																	= -1;
				error_printf("Failed to close string elment. %s", "Unknown error."); 
			}
			stateParser.InsideString												= false;
			::jsonTestAndCloseValue(stateParser, object); 
		}
	}
	stateParser.Escaping													= false;
	return errVal;
}

						::gpk::error_t									gpk::jsonParseStep									(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	reader.StateRead.CharCurrent										= jsonAsString[reader.StateRead.IndexCurrentChar];
	return (reader.StateRead.InsideString)
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Object, jsonAsString) 
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Object, jsonAsString)
		;
}

						::gpk::error_t									gpk::jsonParse										(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	SJSONReaderState															& stateParser										= reader.StateRead;
	for(stateParser.IndexCurrentChar = 0; stateParser.IndexCurrentChar < (int32_t)jsonAsString.size(); ++stateParser.IndexCurrentChar)
		if errored(::gpk::jsonParseStep(reader, jsonAsString)) {
			const bool																validElement										= (uint32_t)reader.StateRead.IndexCurrentElement < reader.Object.size();
			const ::gpk::SJSONType													* currentElement									= validElement ? &reader.Object[reader.StateRead.IndexCurrentElement] : 0;
			error_printf("Error during read step. Malformed JSON?"
				"\nPosition  : %i." 
				"\nCharacter : '%c' (0x%x)."
				"\nElement   : %i."
				"\nString    : %s."
				"\nEscaping  : %s."
				"\nNestLevel : %i."
				"\nParent    : %i."
				"\nType      : %i (%s)."
				"\nOffset    : %i."
				, reader.StateRead.IndexCurrentChar
				, reader.StateRead.CharCurrent, reader.StateRead.CharCurrent
				, reader.StateRead.IndexCurrentElement 
				, reader.StateRead.InsideString ? "true" : "false"
				, reader.StateRead.Escaping		? "true" : "false"
				, reader.StateRead.NestLevel
				, validElement ? currentElement->ParentIndex							: -1
				, validElement ? currentElement->Type									: -1
				, validElement ? ::gpk::get_value_label(currentElement->Type).begin()	: "N/A"
				, validElement ? currentElement->Span.Begin								: -1
			);
			return -1;
		}
	ree_if(stateParser.NestLevel, "Nest level: %i (Needs to be zero).", stateParser.NestLevel);
	ree_if(0 == reader.Tree.create(), "If this fail then maybe it's because of %s.", "running out of memory or memory corrupted.");
	for(uint32_t iView = 0; iView < reader.Object.size(); ++iView) {
		::gpk::SJSONType															& currentElement									= reader.Object[iView];
		gpk_necall(reader.View.push_back({&jsonAsString[currentElement.Span.Begin], currentElement.Span.End - currentElement.Span.Begin}), "Failed to push view! Out of memory? View count: %u. Index: %i.", reader.View.size(), iView);
	}
	return ::jsonTreeRebuild(reader.Object, *reader.Tree);
}
