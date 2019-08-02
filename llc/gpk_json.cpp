#include "gpk_json.h"
#include "gpk_find.h"
#include "gpk_storage.h"

#define json_info_printf // info_printf

::gpk::error_t												gpk::jsonFileRead					(::gpk::SJSONFile & file, const ::gpk::view_const_string & filename) {
	info_printf("Loading json file: %s.", filename.begin());
	gpk_necall(::gpk::fileToMemory(filename, file.Bytes), "Failed to load file: '%s'", filename.begin());
	return ::gpk::jsonParse(file.Reader, {file.Bytes.begin(), file.Bytes.size()});
}

::gpk::error_t												gpk::jsonArraySplit					(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view_array<::gpk::view_const_string> & jsonViews, const uint32_t blockSize, ::gpk::array_obj<::gpk::array_pod<char_t>> & outputJsons)		{
	const uint32_t													remainder							= jsonArrayToSplit.Children.size() % blockSize;
	const uint32_t													countParts							= jsonArrayToSplit.Children.size() / blockSize + one_if(remainder);
	gpk_necall(outputJsons.resize(countParts), "%s", "Out of memory?");
	uint32_t														iSourceRecord						= 0;
	for(uint32_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		::gpk::array_pod<char_t>										& outputJson						= outputJsons[iPart];
		gpk_necall(outputJson.push_back('['), "%s", "Out of memory?");
		for(uint32_t iPartRecord = 0, countPartRecords = (remainder && iPart == countParts - 1) ? remainder : blockSize
			; iPartRecord < countPartRecords
			; ++iPartRecord) {
			gpk_necall(::gpk::jsonWrite(jsonArrayToSplit.Children[iSourceRecord++], jsonViews, outputJson), "%s", "Unknown error!");;
			if(iPartRecord < countPartRecords - 1)
				gpk_necall(outputJson.push_back(','), "%s", "Out of memory?");
		}
		gpk_necall(outputJson.push_back(']'), "%s", "Out of memory?");
	}
	return 0;
}

::gpk::error_t												gpk::jsonWrite						(const ::gpk::SJSONNode* node, const ::gpk::view_array<::gpk::view_const_string> & jsonViews, ::gpk::array_pod<char_t> & output)			{
	switch(node->Object->Type) {
	case ::gpk::JSON_TYPE_NULL			:
	case ::gpk::JSON_TYPE_NUMBER		:
	case ::gpk::JSON_TYPE_BOOL			:
		gpk_necall(output.append(jsonViews[node->ObjectIndex]), "%s", "Out of memory?");
		break;
	case ::gpk::JSON_TYPE_STRING		:
		gpk_necall(output.push_back('"'), "%s", "Out of memory?");
		gpk_necall(output.append(jsonViews[node->ObjectIndex]), "%s", "Out of memory?");
		gpk_necall(output.push_back('"'), "%s", "Out of memory?");
		break;
	case ::gpk::JSON_TYPE_OBJECT		:
		gpk_necall(output.push_back('{'), "%s", "Out of memory?");
		for(uint32_t iChildren = 0; iChildren < node->Children.size(); iChildren += 2) {
			gpk_necall(::gpk::jsonWrite(node->Children[iChildren + 0], jsonViews, output), "%s", "Unknown error!");;
			gpk_necall(output.push_back(':'), "%s", "Out of memory?");
			gpk_necall(::gpk::jsonWrite(node->Children[iChildren + 1], jsonViews, output), "%s", "Unknown error!");;
			if(iChildren < node->Children.size() - 2)
				gpk_necall(output.push_back(','), "%s", "Out of memory?");
		}
		gpk_necall(output.push_back('}'), "%s", "Out of memory?");
		break;
	case ::gpk::JSON_TYPE_ARRAY			:
		gpk_necall(output.push_back('['), "%s", "Out of memory?");
		for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren) {
			gpk_necall(::gpk::jsonWrite(node->Children[iChildren], jsonViews, output), "%s", "Unknown error!");;
			if(iChildren < node->Children.size() - 1)
				gpk_necall(output.push_back(','), "%s", "Out of memory?");
		}
		gpk_necall(output.push_back(']'), "%s", "Out of memory?");
	}
	return 0;
}

			::gpk::error_t									jsonTreeRebuild										(::gpk::array_pod<::gpk::SJSONType>& in_object, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>> & out_nodes)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>				& tree												= out_nodes;
	gpk_necall(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::ptr_obj<::gpk::SJSONNode>								& nodeCurrent										= tree[iObject];
		nodeCurrent->Object											= &in_object[iObject];
		nodeCurrent->Parent											= ((uint32_t)nodeCurrent->Object->ParentIndex < tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Object->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex									= iObject;
	}

	// -- Assign the children to every object of the hierarchy
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(uint32_t iOther = 0; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SJSONNode>						& nodeOther											= tree[iOther];
			if(((uint32_t)nodeOther->Object->ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "%s", "Failed to push tree node. Out of memory?");
		}
	}

	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		::gpk::ptr_obj<::gpk::SJSONNode>								& nodeCurrent										= tree[iObject];
		if(::gpk::JSON_TYPE_ARRAY	!= nodeCurrent->Object->Type
		 &&::gpk::JSON_TYPE_OBJECT	!= nodeCurrent->Object->Type
		)
			continue;
		::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>				newChildren;
		gpk_necall(newChildren.resize(nodeCurrent->Children.size()), "%s", "Out of memory?");
		for(uint32_t iChild = 0, countChild = newChildren.size(); iChild < countChild; ++iChild)
			newChildren[iChild]											= nodeCurrent->Children[iChild]->Children[0];
		nodeCurrent->Children										= newChildren;
	}
	return 0;
}

			::gpk::error_t									jsonCloseElement									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, uint32_t indexChar) {
	ree_if(object.size() <= (uint32_t)stateReader.IndexCurrentElement, "Invalid parser state. Cannot close element: %i.", stateReader.IndexCurrentElement);
	::gpk::SJSONType												* closing											= 0;
	closing														= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	closing->Span.End											= (uint32_t)indexChar + 1;
	const ::gpk::view_const_string									labelType											= ::gpk::get_value_label(closing->Type);
	const char														* labelText											= labelType.begin();
	(void)labelText;
	json_info_printf("%s closed. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i. End: %i.", labelText, stateReader.IndexCurrentElement, stateReader.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End);
	stateReader.IndexCurrentElement								= closing->ParentIndex;
	--stateReader.NestLevel;
	stateReader.CurrentElement									= ((uint32_t)stateReader.IndexCurrentElement < object.size()) ? &object[stateReader.IndexCurrentElement] : 0;
	if(0 == stateReader.NestLevel)
		stateReader.DoneReading										= true;
	return 0;	// Need to report that a list has been exited
}

			::gpk::error_t									jsonCloseElement									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, uint32_t indexChar, ::gpk::JSON_TYPE jsonType) {
	const ::gpk::SJSONType											* open												= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	ree_if(jsonType != open->Type, "Invalid object type: open: %u (%s). closing: %u (%s).", open->Type, ::gpk::get_value_label(open->Type).begin(), jsonType, ::gpk::get_value_label(jsonType).begin());
	return ::jsonCloseElement(stateReader, object, indexChar);
}

			::gpk::error_t									jsonTestAndCloseValue								(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_VALUE == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		return ::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar);
	}
	return 1;
}

#define seterr_break_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		error_printf(format, __VA_ARGS__);		\
		break;									\
	}

#define seterr_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		error_printf(format, __VA_ARGS__);		\
	}

			::gpk::error_t									jsonParseStringCharacter							(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	::gpk::SJSONType												currentElement										= {};
	::gpk::error_t													errVal												= 0;
	switch(stateReader.CharCurrent) {
	default:
		seterr_break_if(stateReader.CharCurrent < 0x20 || ((uchar_t)stateReader.CharCurrent) > 0xFE, "Invalid character: %i (%u) '%c'.", stateReader.CharCurrent, (uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		seterr_break_if(stateReader.Escaping, "Cannot escape character: %i (%u) '%c'.", stateReader.CharCurrent, (uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		break;
	case 'b': case 'f': case 'n': case 'r': case 't':
		break;	// these characters are both valid as part of the string and as escapable characters.
	case 'u':
		if(false == stateReader.Escaping)
			break;
		stateReader.IndexCurrentChar								+= 1;	// skip the u to get the next 4 digits.
		seterr_break_if(jsonAsString.size() - stateReader.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %s. Current index: %u.", jsonAsString.size(), stateReader.IndexCurrentChar);
		json_info_printf("Unicode code point found: %4.4s", &jsonAsString[stateReader.IndexCurrentChar]);
		currentElement												= {stateReader.IndexCurrentElement, ::gpk::JSON_TYPE_CODEPOINT, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + 4}};
		seterr_if(errored(object.push_back(currentElement)), "%s", "Out of memory?");
		stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
		stateReader.IndexCurrentChar								+= 3;	// Parse unicode code point
		break;
	case '\\'	:
		if(false == stateReader.Escaping) {
			stateReader.Escaping										= true;
			return 0;
		}
		break;
	case '"'	:
		if(false == stateReader.Escaping) {
			seterr_if(errored(::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_STRING)), "Failed to close string elment. %s", "Unknown error.");
			stateReader.InsideString									= false;
			::jsonTestAndCloseValue(stateReader, object);
			stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		}
	}
	stateReader.Escaping										= false;
	return errVal;
}

			::gpk::error_t									jsonParseKeyword									(const ::gpk::view_const_string& token, ::gpk::JSON_TYPE jsonType, ::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	ree_if(token.size() > jsonAsString.size() - stateReader.IndexCurrentChar, "End of stream while parsing token: %s.", token.begin());
	ree_if(0 != strncmp(token.begin(), &jsonAsString[stateReader.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%s'.", token.begin());
	json_info_printf("JSON token found: %s.", token.begin());
	::gpk::SJSONType												boolElement											= {stateReader.IndexCurrentElement, jsonType, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + token.size()}};
	gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", object.size());
	stateReader.IndexCurrentChar								+= token.size() - 1;
	stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
	return 0;
}

			::gpk::error_t									lengthJsonNumber									(uint32_t indexCurrentChar, const ::gpk::view_const_string& jsonAsString)	{
	const uint32_t													offset												= indexCurrentChar;
	char															charCurrent											= jsonAsString[indexCurrentChar];
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

			::gpk::error_t									parseJsonNumber										(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	char															bufferFormat[1024];
	const uint32_t													offset												= stateReader.IndexCurrentChar;
	uint32_t														index												= offset;

	char															charCurrent											= jsonAsString[index];
	bool															isNegative											= false;
	bool															isFloat												= false;
	if(index < jsonAsString.size() && jsonAsString[index] == '+')
		++index;
	if(index < jsonAsString.size() && jsonAsString[index] == '-') {
		isNegative													= true;
		++index;
	}
	if(index < jsonAsString.size() && jsonAsString[index] == '+')
		++index;
	if(index < jsonAsString.size() && jsonAsString[index] == '.') {
		isFloat														= true;
		++index;
	}
	charCurrent													= jsonAsString[index];
	ree_if(index < jsonAsString.size() && (charCurrent != '0' && (charCurrent < '1' || charCurrent > '9')), "Character '%c' at index %i is not a number.", charCurrent, index);
	const uint32_t													sizeNum												= lengthJsonNumber(index, jsonAsString);
	::gpk::SJSONType												currentElement										= {stateReader.IndexCurrentElement, ::gpk::JSON_TYPE_NUMBER, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + sizeNum + (index - stateReader.IndexCurrentChar)}};
	gpk_necall(object.push_back(currentElement), "%s", "Out of memory?");
	stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
	gpk_necall(sprintf_s(bufferFormat, "Number found: %%%u.%us. Length: %u. Negative: %s. Float: %s.", sizeNum, sizeNum, sizeNum
		, isNegative	? "true" : "false"
		, isFloat		? "true" : "false"
	), "%s", "Failed to parse json number. Number too long.");
	json_info_printf(bufferFormat, &jsonAsString[index]);
	stateReader.IndexCurrentChar								+= sizeNum + (index - offset) - 1;
	charCurrent													= jsonAsString[stateReader.IndexCurrentChar+1];
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

			::gpk::error_t									jsonTestAndCloseKey									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		return ::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar);
	}
	return 1;
}

			::gpk::error_t									jsonCloseOrDiscardIfEmptyKeyOrVal					(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t													errVal												= 0;
	if(object[object.size() - 1].Type == containerType) {
		json_info_printf("Discarding empty container element at index %i (%s). Level: %i", object.size() - 1, ::gpk::get_value_label(containerType).begin(), stateReader.NestLevel);
		stateReader.IndexCurrentElement								= object[object.size() - 1].ParentIndex;
		object.pop_back(0);
		--stateReader.NestLevel;
		if((uint32_t)stateReader.IndexCurrentElement < object.size())
			stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
	}
	else {
		json_info_printf("Closing container at index %i (%s).", object.size() - 1, ::gpk::get_value_label(containerType).begin());
			 if(::gpk::JSON_TYPE_VALUE	== containerType) errVal		= ::jsonTestAndCloseValue	(stateReader, object);
		else if(::gpk::JSON_TYPE_KEY	== containerType) errVal		= ::jsonTestAndCloseKey	(stateReader, object);
	}
	return errVal;
}
			::gpk::error_t									jsonCloseContainer									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t													errVal												= 0;
	gpk_necall(::jsonCloseOrDiscardIfEmptyKeyOrVal(stateReader, object, (::gpk::JSON_TYPE_ARRAY == containerType) ? ::gpk::JSON_TYPE_VALUE : ::gpk::JSON_TYPE_KEY), "Failed to close container at index %i (%s).", stateReader.IndexCurrentElement, ::gpk::get_value_label(containerType).begin());
	errVal														= ::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar, containerType);
	stateReader.ExpectingSeparator								= false;
	if(stateReader.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		::jsonTestAndCloseValue(stateReader, object);
	return 0;
}

			::gpk::error_t									jsonOpenElement										(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, ::gpk::JSON_TYPE jsonType, uint32_t indexChar) {
	::gpk::SJSONType												currentElement										= {stateReader.IndexCurrentElement, jsonType, {indexChar, indexChar}};
	gpk_necall(stateReader.IndexCurrentElement = object.push_back(currentElement), "Failed to push element. %s", "Out of memory?");
	stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
	const ::gpk::view_const_string									labelType											= ::gpk::get_value_label(currentElement.Type);
	++stateReader.NestLevel;
	json_info_printf("%s open. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i.", labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin);
	return 0;
}

			::gpk::error_t									jsonParseDocumentCharacter							(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONType>& object, const ::gpk::view_const_string& jsonAsString)	{
	static const ::gpk::view_const_string							tokenFalse											= "false"	;
	static const ::gpk::view_const_string							tokenTrue											= "true"	;
	static const ::gpk::view_const_string							tokenNull											= "null"	;
	::gpk::error_t													errVal												= 0;
	::gpk::error_t													nextMatch											= -1;
#define GPK_JSON_EXPECTS_SEPARATOR()													\
	if (stateReader.ExpectingSeparator) { 												\
		errVal														= -1; 				\
		error_printf("Expected separator, found '%c'.", stateReader.CharCurrent); 		\
		break; 																			\
	}

	switch(stateReader.CharCurrent) {
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		break;
	default: // Fallback error for every character that is not recognized by the parser.
		GPK_JSON_EXPECTS_SEPARATOR();
		errVal														= -1;
		error_printf("Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		break;
	case '/'	:
		seterr_break_if(stateReader.IndexCurrentChar >= jsonAsString.size() || '/' != jsonAsString[stateReader.IndexCurrentChar + 1], "Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		if(-1 != (nextMatch = ::gpk::find('\n', jsonAsString, stateReader.IndexCurrentChar)))
			stateReader.IndexCurrentChar								= nextMatch - 1;
		else
			stateReader.IndexCurrentChar								= jsonAsString.size() - 1;
	break;
	case 'f'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenFalse	, ::gpk::JSON_TYPE_BOOL, stateReader, object, jsonAsString); ::jsonTestAndCloseValue(stateReader, object); break;
	case 't'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateReader, object, jsonAsString); ::jsonTestAndCloseValue(stateReader, object); break;
	case 'n'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_NULL, stateReader, object, jsonAsString); ::jsonTestAndCloseValue(stateReader, object); break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.'	: case '-'	: case '+'	: //case 'x'	: // parse int or float accordingly
		GPK_JSON_EXPECTS_SEPARATOR();
		seterr_break_if(stateReader.Escaping, "Invalid character found at index %u: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);	// Set an error or something and skip this character.
		seterr_break_if(::gpk::JSON_TYPE_VALUE != stateReader.CurrentElement->Type, "%s", "Number found outside a value.");
		seterr_break_if(errored(::parseJsonNumber(stateReader, object, jsonAsString)), "%s", "Error parsing JSON number.");
		::jsonTestAndCloseValue(stateReader, object);
		break;
	case ','	:
		seterr_break_if(false == stateReader.ExpectingSeparator, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar);
		if(::gpk::JSON_TYPE_OBJECT == stateReader.CurrentElement->Type)
			errVal														= ::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_KEY, stateReader.IndexCurrentChar + 1);
		else if(::gpk::JSON_TYPE_ARRAY == stateReader.CurrentElement->Type) // Test
			errVal														= ::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1);
		stateReader.ExpectingSeparator								= false;
		break;
	case ':'	:
		seterr_break_if(false == stateReader.ExpectingSeparator && ::gpk::JSON_TYPE_KEY != stateReader.CurrentElement->Type, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar); // Test if we should be expecting this separator or not.
		stateReader.ExpectingSeparator								= false;
		seterr_break_if(errored(::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_KEY) ), "Failed to close key. %s", "Unknown error.");	// close the key before the : character
		seterr_break_if(errored(::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1)), "Failed to open element at index %i.", object.size()); // open the value after the : character
		break;
	case ']'	: errVal = ::jsonCloseContainer(stateReader, object, ::gpk::JSON_TYPE_ARRAY	); break;
	case '}'	: seterr_break_if(::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type && object.size() - 1 != (uint32_t)stateReader.IndexCurrentElement, "Invalid format: %s", "Keys cannot be left without a value."); errVal = ::jsonCloseContainer(stateReader, object, ::gpk::JSON_TYPE_OBJECT); break;
	case '{'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_OBJECT	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_KEY	, stateReader.IndexCurrentChar + 1); } break;
	case '['	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_ARRAY	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_VALUE	, stateReader.IndexCurrentChar + 1); } break;
	case '"'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, object, ::gpk::JSON_TYPE_STRING	, stateReader.IndexCurrentChar + 1);	// skip the " character in order to set the begin the string
		stateReader.InsideString									= true;
		break;
	}
	stateReader.Escaping										= false;
	return errVal;
}

			::gpk::error_t									gpk::jsonParseStep									(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	reader.StateRead.CharCurrent								= jsonAsString[reader.StateRead.IndexCurrentChar];
	::gpk::error_t													errVal												= (reader.StateRead.InsideString)
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Object, jsonAsString)
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Object, jsonAsString)
		;
	if errored(errVal) {
		const bool														validElement										= (uint32_t)reader.StateRead.IndexCurrentElement < reader.Object.size();
		const ::gpk::SJSONType											* currentElement									= validElement ? &reader.Object[reader.StateRead.IndexCurrentElement] : 0;
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
	}
	return errVal;
}

			::gpk::error_t									gpk::jsonParse										(::gpk::SJSONReader& reader, const ::gpk::view_const_string& jsonAsString)	{
	SJSONReaderState												& stateReader										= reader.StateRead;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < (int32_t)jsonAsString.size(); ++stateReader.IndexCurrentChar) {
		gpk_necall(::gpk::jsonParseStep(reader, jsonAsString), "%s", "Parse step failed.");
		bi_if(reader.StateRead.DoneReading, "%i json characters read.", stateReader.IndexCurrentChar + 1);
	}
	ree_if(stateReader.NestLevel, "Nest level: %i (Needs to be zero).", stateReader.NestLevel);
	for(uint32_t iView = 0; iView < reader.Object.size(); ++iView) {
		::gpk::SJSONType												& currentElement									= reader.Object[iView];
		gpk_necall(reader.View.push_back({&jsonAsString[currentElement.Span.Begin], currentElement.Span.End - currentElement.Span.Begin}), "Failed to push view! Out of memory? View count: %u. Index: %i.", reader.View.size(), iView);
	}
	return ::jsonTreeRebuild(reader.Object, reader.Tree);
}

			::gpk::error_t									gpk::jsonObjectKeyList								(const ::gpk::SJSONNode& node_object, const ::gpk::view_array<::gpk::view_const_string>& views, ::gpk::array_obj<int32_t> & indices, ::gpk::array_obj<::gpk::view_const_string> & keys)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Object->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Object->Type, ::gpk::get_value_label(node_object.Object->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Object->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::view_const_string									& view												= views[node->ObjectIndex];
		gpk_necall(indices	.push_back(node->ObjectIndex)	, "Failed! %s", "Too many nodes?");
		gpk_necall(keys		.push_back(view)				, "Failed! %s", "Too many nodes?");
	}
	return 0;
}

			::gpk::error_t									gpk::jsonObjectValueGet								(const ::gpk::SJSONNode& node_object, const ::gpk::view_array<::gpk::view_const_string>& views, const ::gpk::view_const_string& key)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Object->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Object->Type, ::gpk::get_value_label(node_object.Object->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Object->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Object->Type, ::gpk::get_value_label(node->Object->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::view_const_string									& view												= views[node->ObjectIndex];
		if(key == view)
			return (::gpk::error_t)node->ObjectIndex + 2; // one for value and other for the actual element
	}
	return -1;
}

			::gpk::error_t									gpk::jsonArrayValueGet								(const ::gpk::SJSONNode& tree, uint32_t index)				{
	ree_if(::gpk::JSON_TYPE_ARRAY != tree.Object->Type, "Invalid node type: %i (%s). Only arrays are allowed to be accessed by index.", tree.Object->Type, ::gpk::get_value_label(tree.Object->Type).begin());
	ree_if(index >= tree.Children.size(), "Index out of range: %i. Max index: %i.", index, tree.Children.size() - 1);
	const ::gpk::SJSONNode											* node												= tree.Children[index];	// Get the
	ree_if(0 == node, "Invalid or corrupt tree. %s", "Nodes cannot be null.");
	return node->ObjectIndex;
}

// Get the codepoint from 4 hex characters of an unicode escape sequence
//	For input "\u c1 c2 c3 c4" the codepoint is:
//		(c1 * 0x1000)	+ (c2 * 0x0100)	+ (c3 * 0x0010) + c4
// aka:	(c1 << 12)		+ (c2 << 8)		+ (c3 << 4)		+ (c4 << 0)
//
//	Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f' must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp.
//	The conversion is done by subtracting the offset (0x30, 0x37, and 0x57) between the ASCII value of the character and the desired integer value.
//	Returns codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or non-hex character)
static		::gpk::error_t									decodeUnicodeEscapeSequence							(::gpk::view_const_string input, uint32_t& ret_unicode)		{
	ree_if(input.size() < 4, "Invalid escape sequence: %s.", input.begin());
	int																unicode												= 0;
	for (int index = 0; index < 4; ++index) {
		char															c													= input[index];
		unicode														*= 16;
		if (c >= '0' && c <= '9')
			unicode														+= c - '0';
		else if (c >= 'a' && c <= 'f')
			unicode														+= c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			unicode														+= c - 'A' + 10;
		else
			return -1;
	}
	ret_unicode													= static_cast<unsigned int>(unicode);
	return true;
}

// this function only makes sense after reading the first `\u`
::gpk::error_t									jsonToCodePoint										(::gpk::view_const_string input, uint32_t& unicode)			{
	gpk_necall(::decodeUnicodeEscapeSequence(input, unicode), "Invalid escape sequence: %s.", input.begin());
	if (unicode < 0xD800 || unicode > 0xDBFF)
		return 0;
	// surrogate pairs
	ree_if(input.size() < 7, "%s", "expecting a \\u token for the second half of the surrogate pair");
	ree_if(input[4] != '\\' || input[5] == 'u', "%s", "expecting a \\u token for the second half of the surrogate pair");
	uint32_t														surrogatePair;
	gpk_necall(::decodeUnicodeEscapeSequence({&input[6], 4U}, surrogatePair), "Invalid escape sequence: %s.", input.begin());
	unicode														= 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
	return 0;
}
