#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#include "gpk_json.h"
#include "gpk_storage.h"
#include "gpk_parse.h"

#define json_info_printf(...) // info_printf
#define json_error_printf error_printf

::gpk::error_t												gpk::jsonMapToFields
(	::gpk::array_pod<int32_t>									& indicesOfFields
,	const ::gpk::view_array<const ::gpk::SJSONFieldBinding>		fields
,	const ::gpk::view_array<const ::gpk::TKeyValConstChar>		fieldMaps
) {
	for(uint32_t iMap = 0; iMap < fieldMaps.size(); ++iMap)
	for(uint32_t iField = 0; iField < fields.size(); ++iField) {
		const ::gpk::SJSONFieldBinding									& fieldToAdd						= fields[iField];
		if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
			indicesOfFields.push_back(iField);
			break;
		}
	}
	return indicesOfFields.size();
}

::gpk::error_t												gpk::jsonFieldsToMap
(	::gpk::array_pod<int32_t>									& indicesOfMaps
,	const ::gpk::view_array<const ::gpk::SJSONFieldBinding>		fields
,	const ::gpk::view_array<const ::gpk::TKeyValConstChar>		fieldMaps
) {
	uint32_t														countFields							= fields.size();
	for(uint32_t iField = 0; iField < countFields; ++iField) {
		bool															bNotAdded							= true;
		for(uint32_t iMap = 0; iMap < fieldMaps.size(); ++iMap) {
			const ::gpk::SJSONFieldBinding									& fieldToAdd						= fields[iField];
			if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
				indicesOfMaps.push_back(iMap);
				bNotAdded													= false;
				break;
			}
		}
		if(bNotAdded)
			indicesOfMaps.push_back(-1);
	}
	return indicesOfMaps.size();
}

::gpk::error_t												gpk::jsonFileRead					(::gpk::SJSONFile & file, const ::gpk::view_const_char & filename) {
	info_printf("Loading json file: %s.", filename.begin());
	gpk_necall(::gpk::fileToMemory({filename.begin(), filename.size()}, file.Bytes), "Failed to load file: '%s'", filename.begin());
	return ::gpk::jsonParse(file.Reader, file.Bytes);
}

::gpk::error_t												gpk::jsonArraySplit					(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view_array<::gpk::view_const_char> & jsonViews, const uint32_t blockSize, ::gpk::array_obj<::gpk::array_pod<char_t>> & outputJsons)		{
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

static const ::gpk::view_const_string		gpk_json_str_true  = "true";
static const ::gpk::view_const_string		gpk_json_str_false = "false";
::gpk::error_t												gpk::jsonWrite						(const ::gpk::SJSONNode* node, const ::gpk::view_array<::gpk::view_const_char> & jsonViews, ::gpk::array_pod<char_t> & output)			{
	if(node->Token->Type == ::gpk::JSON_TYPE_VALUE && node->Children.size())
		node														= node->Children[0];
	switch(node->Token->Type) {
	case ::gpk::JSON_TYPE_INTEGER		: {
		char														temp[64];
		snprintf(temp, ::gpk::size(temp) - 2, "%lli"	, node->Token->Value);
		output.append_string(temp);
	}
		break;
	case ::gpk::JSON_TYPE_DOUBLE		: {
		double														f = 0;
		memcpy(&f, &node->Token->Value, sizeof(double));
		char														temp[64];
		uint32_t													lenNum								= snprintf(temp, ::gpk::size(temp) - 2, "%.16f", f);
		while(lenNum > 0 && (temp[lenNum] == 0 || temp[lenNum] == '0')) {
			temp[lenNum] = 0;
			--lenNum;
		}
		gpk_necall(output.append_string(temp), "%s", "Out of memory?");
	}
		break;
	case ::gpk::JSON_TYPE_BOOL			:
		gpk_necall(output.append(node->Token->Value ? gpk_json_str_true : gpk_json_str_false), "%s", "Out of memory?");
		break;
	case ::gpk::JSON_TYPE_NULL			:
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

::gpk::error_t												gpk::jsonTreeRebuild								(::gpk::view_array<::gpk::SJSONToken>& in_object, ::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>> & out_nodes)								{
	::gpk::array_obj<::gpk::ptr_obj<::gpk::SJSONNode>>				& tree												= out_nodes;
	gpk_necall(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::ptr_obj<::gpk::SJSONNode>								& nodeCurrent										= tree[iObject];
		nodeCurrent->Token											= &in_object[iObject];
		nodeCurrent->Parent											= ((uint32_t)nodeCurrent->Token->ParentIndex < tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Token->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex									= iObject;
		nodeCurrent->Children.clear();
	}

	// -- Assign the children to every object of the hierarchy
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		for(uint32_t iOther = iObject + 1; iOther < countNodes; ++iOther) {
			const ::gpk::ptr_obj<::gpk::SJSONNode>						& nodeOther											= tree[iOther];
			if(((uint32_t)in_object[iOther].ParentIndex) == iObject)
				gpk_necall(tree[iObject]->Children.push_back(nodeOther), "%s", "Failed to push tree node. Out of memory?");
		}
	}

	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		::gpk::ptr_obj<::gpk::SJSONNode>								& nodeCurrent										= tree[iObject];
		if( ::gpk::JSON_TYPE_ARRAY	!= nodeCurrent->Token->Type
		 && ::gpk::JSON_TYPE_OBJECT	!= nodeCurrent->Token->Type
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

static	::gpk::error_t										jsonCloseElement									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, uint32_t indexChar) {
	ree_if(tokens.size() <= (uint32_t)stateReader.IndexCurrentElement, "Invalid parser state. Cannot close element: %i.", stateReader.IndexCurrentElement);
	::gpk::SJSONToken												* closing											= 0;
	closing														= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	closing->Span.End											= (uint32_t)indexChar + 1;
	const ::gpk::view_const_char									labelType											= ::gpk::get_value_label(closing->Type);
	const char														* labelText											= labelType.begin();
	(void)labelText;
	json_info_printf("%s closed. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i. End: %i.", labelText, stateReader.IndexCurrentElement, stateReader.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End);
	stateReader.IndexCurrentElement								= closing->ParentIndex;
	--stateReader.NestLevel;
	stateReader.CurrentElement									= ((uint32_t)stateReader.IndexCurrentElement < tokens.size()) ? &tokens[stateReader.IndexCurrentElement] : 0;
	if(0 == stateReader.NestLevel)
		stateReader.DoneReading										= true;
	return 0;	// Need to report that a list has been exited
}

static	::gpk::error_t										jsonCloseElement									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& object, uint32_t indexChar, ::gpk::JSON_TYPE jsonType) {
	const ::gpk::SJSONToken											* open												= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	ree_if(jsonType != open->Type, "Invalid object type: open: %u (%s). closing: %u (%s).", open->Type, ::gpk::get_value_label(open->Type).begin(), jsonType, ::gpk::get_value_label(jsonType).begin());
	return ::jsonCloseElement(stateReader, object, indexChar);
}

static	::gpk::error_t										jsonTestAndCloseValue								(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& object) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_VALUE == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		return ::jsonCloseElement(stateReader, object, stateReader.IndexCurrentChar);
	}
	return 1;
}

#define seterr_break_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		json_error_printf(format, __VA_ARGS__);		\
		break;									\
	}

#define seterr_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		json_error_printf(format, __VA_ARGS__);		\
	}

static	bool												isSpaceCharacter						(const char characterToTest)		{
	switch(characterToTest) {
	case ' ': case '\t': case '\r': case '\n'	: case '\f'	: case '\b'	:
		return true;
	default:
		return false;
	}
}

static	::gpk::error_t										skipToNextCharacter						(uint32_t& indexCurrentChar, const ::gpk::view_const_char& expression)		{
	while(indexCurrentChar < expression.size()) {
		if(::isSpaceCharacter(expression[indexCurrentChar]))
			++indexCurrentChar;
		else
			break;
	}
	return 0;
}

static	::gpk::error_t										jsonParseStringCharacter							(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, const ::gpk::view_const_char& jsonAsString)	{
	::gpk::SJSONToken												currentElement										= {};
	::gpk::error_t													errVal												= 0;
	switch(stateReader.CharCurrent) {
	default:
		seterr_break_if(((uchar_t)stateReader.CharCurrent) < 0x20 || ((uchar_t)stateReader.CharCurrent) > 0xFE, "Invalid character: %i (%u) '%c'.", stateReader.CharCurrent, (uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		seterr_break_if(stateReader.Escaping, "Cannot escape character: %i (%u) '%c'.", stateReader.CharCurrent, (uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		break;
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		::skipToNextCharacter(stateReader.IndexCurrentChar, jsonAsString);
		--stateReader.IndexCurrentChar;
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
		seterr_if(errored(tokens.push_back(currentElement)), "%s", "Out of memory?");
		stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
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
			seterr_if(errored(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_STRING)), "Failed to close string elment. %s", "Unknown error.");
			stateReader.InsideString									= false;
			::jsonTestAndCloseValue(stateReader, tokens);
			stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		}
	}
	stateReader.Escaping										= false;
	return errVal;
}

static	::gpk::error_t										jsonParseKeyword									(const ::gpk::view_const_char& token, ::gpk::JSON_TYPE jsonType, ::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& object, const ::gpk::view_const_char& jsonAsString)	{
	ree_if(token.size() > jsonAsString.size() - stateReader.IndexCurrentChar, "End of stream while parsing token: %s.", token.begin());
	ree_if(0 != strncmp(token.begin(), &jsonAsString[stateReader.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%s'.", token.begin());
	json_info_printf("JSON token found: %s.", token.begin());
	::gpk::SJSONToken												boolElement											= {stateReader.IndexCurrentElement, jsonType, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + token.size()}};
	gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", object.size());
	stateReader.IndexCurrentChar								+= token.size() - 1;
	stateReader.CurrentElement									= &object[stateReader.IndexCurrentElement];
	return 0;
}

static	::gpk::error_t										lengthJsonNumber									(uint32_t indexCurrentChar, const ::gpk::view_const_char& jsonAsString)	{
	const uint32_t													offset												= indexCurrentChar;
	char															charCurrent											= jsonAsString[indexCurrentChar];
	while(indexCurrentChar < jsonAsString.size() &&
		( ( charCurrent >= '1' && charCurrent <= '9')
		 || charCurrent == '0'
		 || charCurrent == '.'
		 )
		) {
		++indexCurrentChar;
		if(indexCurrentChar >= jsonAsString.size())
			break;
		charCurrent													= jsonAsString[indexCurrentChar];
	}
	return indexCurrentChar - offset;
}

static	::gpk::error_t										parseJsonNumber										(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, const ::gpk::view_const_char& jsonAsString)	{
	const uint32_t													offset												= stateReader.IndexCurrentChar;
	uint32_t														index												= offset;

	char															charCurrent											= jsonAsString[index];
	bool															isNegative											= false;
	bool															isFloat												= false;
	(void)isNegative;
	(void)isFloat	;
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
	::gpk::SJSONToken												currentElement										= {stateReader.IndexCurrentElement, ::gpk::JSON_TYPE_INTEGER, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + sizeNum + (index - stateReader.IndexCurrentChar)}};
	::gpk::view_const_char											numString											= {};
	gpk_necall(jsonAsString.slice(numString, index, sizeNum), "%s", "");
	::gpk::error_t													intCount											= ::gpk::parseIntegerDecimal(numString, &currentElement.Value);
	if(intCount < (int32_t)sizeNum) {
		currentElement.Type											= ::gpk::JSON_TYPE_DOUBLE;
		double															finalValue											= (double)currentElement.Value;
		//info_printf("Integer part: %f.", finalValue);
		++intCount;	// Skip dot.
		const uint32_t													offsetStart											= currentElement.Span.Begin + sizeNum - (sizeNum - intCount);
		const uint32_t													lenDec												= sizeNum - intCount;
		if(lenDec) {
			double															decValue											= 0;
			if(numString[0] == '.')
				gpk_necall(jsonAsString.slice(numString, offsetStart + 1, lenDec - 1), "%s", "");
			else
				gpk_necall(jsonAsString.slice(numString, offsetStart, lenDec), "%s", "");
			const ::gpk::error_t											decCount											= ::gpk::parseIntegerDecimal(numString, &decValue);
			ree_if(errored(decCount), "%s", "Unknown error.");
			decValue													/= ::gpk::powui(10, decCount);
			//info_printf("Decimal part: %f.", decValue);
			finalValue													+= decValue;
		}
		memcpy((double*)&currentElement.Value, &finalValue, sizeof(uint64_t));
	}
	else {
		//info_printf("Integer part: %i.", currentElement.Value);
	}
	gpk_necall(tokens.push_back(currentElement), "%s", "Out of memory?");
	stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
	//info_printf("Number found: %s. Length: %u. Negative: %s. Float: %s."
	//	, ::gpk::toString({&jsonAsString[index], sizeNum}).begin()
	//	, sizeNum
	//	, isNegative	? "true" : "false"
	//	, isFloat		? "true" : "false"
	//);
	stateReader.IndexCurrentChar								+= sizeNum + (index - offset) - 1;
	if(stateReader.IndexCurrentChar + 1 < jsonAsString.size() - 1) {
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
	}
	return 0;
}

static	::gpk::error_t									jsonTestAndCloseKey									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator								= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		return ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar);
	}
	return 1;
}

static	::gpk::error_t									jsonCloseOrDiscardIfEmptyKeyOrVal					(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t													errVal												= 0;
	if(tokens[tokens.size() - 1].Type == containerType) {
		json_info_printf("Discarding empty container element at index %i (%s). Level: %i", tokens.size() - 1, ::gpk::get_value_label(containerType).begin(), stateReader.NestLevel);
		stateReader.IndexCurrentElement								= tokens[tokens.size() - 1].ParentIndex;
		tokens.pop_back(0);
		--stateReader.NestLevel;
		if((uint32_t)stateReader.IndexCurrentElement < tokens.size())
			stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
	}
	else {
		json_info_printf("Closing container at index %i (%s).", tokens.size() - 1, ::gpk::get_value_label(containerType).begin());
			 if(::gpk::JSON_TYPE_VALUE	== containerType) errVal = ::jsonTestAndCloseValue	(stateReader, tokens);
		else if(::gpk::JSON_TYPE_KEY	== containerType) errVal = ::jsonTestAndCloseKey	(stateReader, tokens);
	}
	return errVal;
}
static	::gpk::error_t									jsonCloseContainer									(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t													errVal												= 0;
	gpk_necall(::jsonCloseOrDiscardIfEmptyKeyOrVal(stateReader, tokens, (::gpk::JSON_TYPE_ARRAY == containerType) ? ::gpk::JSON_TYPE_VALUE : ::gpk::JSON_TYPE_KEY), "Failed to close container at index %i (%s).", stateReader.IndexCurrentElement, ::gpk::get_value_label(containerType).begin());
	errVal														= ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar, containerType);
	(void)errVal;
	stateReader.ExpectingSeparator								= false;
	if(stateReader.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		::jsonTestAndCloseValue(stateReader, tokens);
	return 0;
}

static	::gpk::error_t									jsonOpenElement										(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, ::gpk::JSON_TYPE jsonType, uint32_t indexChar) {
	::gpk::SJSONToken												currentElement										= {stateReader.IndexCurrentElement, jsonType, {indexChar, indexChar}};
	gpk_necall(stateReader.IndexCurrentElement = tokens.push_back(currentElement), "Failed to push element. %s", "Out of memory?");
	stateReader.CurrentElement									= &tokens[stateReader.IndexCurrentElement];
	const ::gpk::view_const_char									labelType											= ::gpk::get_value_label(currentElement.Type);
	(void)labelType;
	++stateReader.NestLevel;
	json_info_printf("%s open. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i.", labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin);
	return 0;
}

static	::gpk::error_t									jsonParseDocumentCharacter							(::gpk::SJSONReaderState& stateReader, ::gpk::array_pod<::gpk::SJSONToken>& tokens, const ::gpk::view_const_char& jsonAsString)	{
	static const ::gpk::view_const_string							tokenFalse											= "false"	;
	static const ::gpk::view_const_string							tokenTrue											= "true"	;
	static const ::gpk::view_const_string							tokenNull											= "null"	;
	::gpk::error_t													errVal												= 0;
	::gpk::error_t													nextMatch											= -1;
#define GPK_JSON_EXPECTS_SEPARATOR()													\
	if (stateReader.ExpectingSeparator) { 												\
		errVal														= -1; 				\
		json_error_printf("Expected separator, found '%c'.", stateReader.CharCurrent); 		\
		break; 																			\
	}

#define test_first_position()	ree_if(0 == stateReader.CharCurrent, "Character found at invalid position. Index: %u. Character: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);

	switch(stateReader.CharCurrent) {
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		break;
	default: // Fallback error for every character that is not recognized by the parser.
		GPK_JSON_EXPECTS_SEPARATOR();
		errVal														= -1;
		json_error_printf("Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		break;
	case '/'	:
		seterr_break_if(stateReader.IndexCurrentChar >= jsonAsString.size() || '/' != jsonAsString[stateReader.IndexCurrentChar + 1], "Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		if(-1 != (nextMatch = ::gpk::find('\n', jsonAsString, stateReader.IndexCurrentChar)))
			stateReader.IndexCurrentChar								= nextMatch - 1;
		else
			stateReader.IndexCurrentChar								= jsonAsString.size() - 1;
	break;
	case 'f'	: GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(tokenFalse	, ::gpk::JSON_TYPE_BOOL, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 't'	: GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(tokenTrue	, ::gpk::JSON_TYPE_BOOL, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 'n'	: GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(tokenNull	, ::gpk::JSON_TYPE_NULL, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case '0'	: case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.'	: case '-'	: case '+'	: //case 'x'	: // parse int or float accordingly
		GPK_JSON_EXPECTS_SEPARATOR();
		if(-1 == stateReader.IndexCurrentElement)
			ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size());
		seterr_break_if(stateReader.Escaping, "Invalid character found at index %u: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);	// Set an error or something and skip this character.
		seterr_break_if(::gpk::JSON_TYPE_VALUE != stateReader.CurrentElement->Type, "%s", "Number found outside a value.");
		seterr_break_if(errored(::parseJsonNumber(stateReader, tokens, jsonAsString)), "%s", "Error parsing JSON number.");
		::jsonTestAndCloseValue(stateReader, tokens);
		break;
	case ','	:
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar);
		if(::gpk::JSON_TYPE_OBJECT == stateReader.CurrentElement->Type)
			errVal														= ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_KEY, stateReader.IndexCurrentChar + 1);
		else if(::gpk::JSON_TYPE_ARRAY == stateReader.CurrentElement->Type) // Test
			errVal														= ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1);
		stateReader.ExpectingSeparator								= false;
		break;
	case ':'	:
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator && ::gpk::JSON_TYPE_KEY != stateReader.CurrentElement->Type, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar); // Test if we should be expecting this separator or not.
		stateReader.ExpectingSeparator								= false;
		seterr_break_if(errored(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_KEY) ), "Failed to close key. %s", "Unknown error.");	// close the key before the : character
		seterr_break_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1)), "Failed to open element at index %i.", tokens.size()); // open the value after the : character
		break;
	case ']'	: test_first_position(); errVal = ::jsonCloseContainer(stateReader, tokens, ::gpk::JSON_TYPE_ARRAY); break;
	case '}'	: test_first_position(); seterr_break_if(::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type && tokens.size() - 1 != (uint32_t)stateReader.IndexCurrentElement, "Invalid format: %s", "Keys cannot be left without a value."); errVal = ::jsonCloseContainer(stateReader, tokens, ::gpk::JSON_TYPE_OBJECT); break;
	case '{'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_OBJECT	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_KEY	, stateReader.IndexCurrentChar + 1); } break;
	case '['	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_ARRAY	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE	, stateReader.IndexCurrentChar + 1); } break;
	case '"'	: GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_STRING	, stateReader.IndexCurrentChar + 1);	// skip the " character in order to set the begin the string
		stateReader.InsideString									= true;
		break;
	}
	stateReader.Escaping										= false;
	return errVal;
}

			::gpk::error_t									gpk::jsonParseStep									(::gpk::SJSONReader& reader, const ::gpk::view_const_char& jsonAsString)	{
	reader.StateRead.CharCurrent								= jsonAsString[reader.StateRead.IndexCurrentChar];
	::gpk::error_t													errVal												= (reader.StateRead.InsideString)
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Token, jsonAsString)
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Token, jsonAsString)
		;
	if errored(errVal) {
		const bool														validElement										= (uint32_t)reader.StateRead.IndexCurrentElement < reader.Token.size();
		const ::gpk::SJSONToken											* currentElement									= validElement ? &reader.Token[reader.StateRead.IndexCurrentElement] : 0;
		json_error_printf("Error during read step. Malformed JSON?"
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

			::gpk::error_t									gpk::jsonParse										(::gpk::SJSONReader& reader, const ::gpk::view_const_char& jsonAsString)	{
	SJSONReaderState												& stateReader										= reader.StateRead;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < jsonAsString.size(); ++stateReader.IndexCurrentChar) {
		gpk_necall(::gpk::jsonParseStep(reader, jsonAsString), "%s", "Parse step failed.");
		bi_if(reader.StateRead.DoneReading, "%i json characters read.", stateReader.IndexCurrentChar + 1);
	}
	ree_if(stateReader.NestLevel, "Nest level: %i (Needs to be zero).", stateReader.NestLevel);
	reader.View.resize(reader.Token.size());
	for(uint32_t iView = 0; iView < reader.Token.size(); ++iView) {
		const ::gpk::SJSONToken											& currentElement									= reader.Token[iView];
		gpk_necall(jsonAsString.slice(reader.View[iView], currentElement.Span.Begin, currentElement.Span.End - currentElement.Span.Begin), "%s", "");
	}
	return ::gpk::jsonTreeRebuild(reader.Token, reader.Tree);
}

			::gpk::error_t									gpk::jsonObjectKeyList								(const ::gpk::SJSONNode& node_object, const ::gpk::view_array<::gpk::view_const_char>& views, ::gpk::array_pod<int32_t> & indices, ::gpk::array_obj<::gpk::view_const_char> & keys)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::view_const_char									& view												= views[node->ObjectIndex];
		gpk_necall(indices	.push_back(node->ObjectIndex)	, "Failed! %s", "Too many nodes?");
		gpk_necall(keys		.push_back(view)				, "Failed! %s", "Too many nodes?");
	}
	return 0;
}

			::gpk::error_t									gpk::jsonObjectKeyList								(const ::gpk::SJSONNode& node_object, ::gpk::array_pod<int32_t> & indices)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		gpk_necall(indices.push_back(node->ObjectIndex)	, "Failed! %s", "Too many nodes?");
	}
	return 0;
}

			::gpk::error_t									gpk::jsonObjectKeyList								(const ::gpk::SJSONNode& node_object, const ::gpk::view_array<::gpk::view_const_char>& views, ::gpk::array_obj<::gpk::view_const_char> & keys)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::view_const_char									& view												= views[node->ObjectIndex];
		gpk_necall(keys.push_back(view)				, "Failed! %s", "Too many nodes?");
	}
	return 0;
}

			::gpk::error_t									gpk::jsonObjectValueGet								(const ::gpk::SJSONNode& node_object, const ::gpk::view_array<::gpk::view_const_char>& views, const ::gpk::view_const_string& key)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode											* node												= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::view_const_char									& view												= views[node->ObjectIndex];
		if(key == view)
			return (::gpk::error_t)node->ObjectIndex + 2; // one for value and other for the actual element
	}
	return -1;
}

			::gpk::error_t									gpk::jsonArrayValueGet								(const ::gpk::SJSONNode& tree, uint32_t index)				{
	ree_if(::gpk::JSON_TYPE_ARRAY != tree.Token->Type, "Invalid node type: %i (%s). Only arrays are allowed to be accessed by index.", tree.Token->Type, ::gpk::get_value_label(tree.Token->Type).begin());
	ree_if(index >= tree.Children.size(), "Index out of range: %i. Max index: %i.", index, tree.Children.size() - 1);
	const ::gpk::SJSONNode											* node												= tree.Children[index];	// Get the
	ree_if(0 == node, "Invalid or corrupt tree. %s", "Nodes cannot be null.");
	return node->ObjectIndex;
}

			::gpk::error_t									gpk::jsonCompareArray		(const ::gpk::SJSONNode& node, const ::gpk::view_array<::gpk::view_const_char>& views, const ::gpk::SJSONNode& other, const ::gpk::view_array<::gpk::view_const_char>& otherViews) {
	if(node.Children.size() != other.Children.size())
		return 0;
	if(node.Children.size() == 0)
		return 1;
	for(uint32_t iChild = 0; iChild < node.Children.size(); ++iChild) {
		const ::gpk::SJSONNode											& childNode					= *node	.Children[iChild];
		const ::gpk::SJSONNode											& childOther				= *other.Children[iChild];
		const ::gpk::JSON_TYPE											typeNode					= childNode	.Token->Type;
		const ::gpk::JSON_TYPE											typeOther					= childOther.Token->Type;
		if(typeNode != typeOther)
			return 0;
		::gpk::error_t													result						= -1;
		switch(typeNode) {
		case ::gpk::JSON_TYPE_ARRAY		: result = ::gpk::jsonCompareArray	(childNode, views, childOther, otherViews); break;
		case ::gpk::JSON_TYPE_OBJECT	: result = ::gpk::jsonCompareObject	(childNode, views, childOther, otherViews); break;
		case ::gpk::JSON_TYPE_DOUBLE	:
		case ::gpk::JSON_TYPE_INTEGER	: result = ::gpk::jsonCompareNumber	(childNode, views, childOther, otherViews); break;
		default:
			result														= views[node.ObjectIndex] == otherViews[other.ObjectIndex];
		}
		if(0 == result)
			return 0;
	}
	if(views[node.ObjectIndex] == otherViews[other.ObjectIndex])
		return 1;
	return 1;
}

			::gpk::error_t									gpk::jsonCompareObject		(const ::gpk::SJSONNode& node, const ::gpk::view_array<::gpk::view_const_char>& views, const ::gpk::SJSONNode& other, const ::gpk::view_array<::gpk::view_const_char>& otherViews) { if(node.Children.size() != other.Children.size()) return 0; if(node.Children.size() == 0) return 1; for(uint32_t iChild = 0; iChild < node.Children.size(); iChild += 2)	if(node.Token->Type != other.Token->Type) return 0; if(views[node.ObjectIndex] == otherViews[other.ObjectIndex]) return 1; return 1; }
			::gpk::error_t									gpk::jsonCompareNumber		(const ::gpk::SJSONNode& node, const ::gpk::view_array<::gpk::view_const_char>& views, const ::gpk::SJSONNode& other, const ::gpk::view_array<::gpk::view_const_char>& otherViews) { if(node.Children.size() != other.Children.size()) return 0; if(node.Children.size() == 0) return 1; if(node.Token->Type != other.Token->Type) return 0; if(views[node.ObjectIndex] == otherViews[other.ObjectIndex]) return 1; return 1; }


// Get the codepoint from characters of an unicode escape sequence
//	For input "\u c1 c2 c3 c4" the codepoint is:
//		(c1 * 0x1000)	+ (c2 * 0x0100)	+ (c3 * 0x0010) + c4
// aka:	(c1 << 12)		+ (c2 << 8)		+ (c3 << 4)		+ (c4 << 0)
//
//	Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f' must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp.
//	The conversion is done by subtracting the offset (0x30, 0x37, and 0x57) between the ASCII value of the character and the desired integer value.
//	Returns codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or non-hex character)
static		::gpk::error_t									decodeUnicodeEscapeSequence							(::gpk::view_const_char input, uint32_t& ret_unicode)		{
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
::gpk::error_t												jsonToCodePoint										(::gpk::view_const_char input, uint32_t& unicode)			{
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
