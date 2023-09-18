#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG

#include "gpk_label.h"
#include "gpk_json.h"
#include "gpk_file.h"
#include "gpk_parse.h"
#include "gpk_math.h"

#define json_info_printf(...) // info_printf
#define json_error_printf error_printf

::gpk::error_t			gpk::jsonCompareObject		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & otherViews) { if(node.Children.size() != other.Children.size()) return 0; if(node.Children.size() == 0) return 1; for(uint32_t iChild = 0; iChild < node.Children.size(); iChild += 2)	if(node.Token->Type != other.Token->Type) return 0; return (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }
::gpk::error_t			gpk::jsonCompareNumber		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc> & views, const ::gpk::SJSONNode & other, const ::gpk::view<::gpk::vcc> & otherViews) { if(node.Children.size() != other.Children.size()) return 0; if(node.Children.size() == 0) return 1; if(node.Token->Type != other.Token->Type) return 0; return (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }

::gpk::error_t			gpk::jsonMapToFields
(	::gpk::ai32											& indicesOfFields
,	const ::gpk::view<const ::gpk::SJSONFieldBinding>	fields
,	const ::gpk::view<const ::gpk::TKeyValConstChar>	fieldMaps
) {
	for(uint32_t iMap = 0; iMap < fieldMaps.size(); ++iMap)
	for(uint32_t iField = 0; iField < fields.size(); ++iField) {
		const ::gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
		if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
			indicesOfFields.push_back(iField);
			break;
		}
	}
	return indicesOfFields.size();
}

::gpk::error_t			gpk::jsonFieldsToMap
(	::gpk::ai32											& indicesOfMaps
,	const ::gpk::view<const ::gpk::SJSONFieldBinding>	fields
,	const ::gpk::view<const ::gpk::TKeyValConstChar>	fieldMaps
) {
	uint32_t					countFields					= fields.size();
	for(uint32_t iField = 0; iField < countFields; ++iField) {
		bool						bNotAdded					= true;
		for(uint32_t iMap = 0; iMap < fieldMaps.size(); ++iMap) {
			const ::gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
			if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
				indicesOfMaps.push_back(iMap);
				bNotAdded				= false;
				break;
			}
		}
		if(bNotAdded)
			indicesOfMaps.push_back(-1);
	}
	return indicesOfMaps.size();
}

::gpk::error_t			gpk::jsonFileRead			(::gpk::SJSONFile & file, const ::gpk::vcc & filename) {
	json_info_printf("Loading json file: %s.", filename.begin());
	gpk_necall(gpk::fileToMemory({filename.begin(), filename.size()}, file.Bytes), "Failed to load file: '%s'", filename.begin());
	return ::gpk::jsonParse(file.Reader, file.Bytes);
}

::gpk::error_t			gpk::jsonArraySplit			(const ::gpk::SJSONNode & jsonArrayToSplit, const ::gpk::view<::gpk::vcc> & jsonViews, const uint32_t blockSize, ::gpk::aobj<::gpk::apod<char>> & outputJsons)		{
	const uint32_t				remainder					= jsonArrayToSplit.Children.size() % blockSize;
	const uint32_t				countParts					= jsonArrayToSplit.Children.size() / blockSize + one_if(remainder);
	gpk_necs(outputJsons.resize(countParts));
	uint32_t					iSourceRecord						= 0;
	for(uint32_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		::gpk::apod<char>			& outputJson				= outputJsons[iPart];
		gpk_necs(outputJson.push_back('['));
		for(uint32_t iPartRecord = 0, countPartRecords = (remainder && iPart == countParts - 1) ? remainder : blockSize
			; iPartRecord < countPartRecords
			; ++iPartRecord) {
			gpk_necs(gpk::jsonWrite(jsonArrayToSplit.Children[iSourceRecord++], jsonViews, outputJson));;
			if(iPartRecord < countPartRecords - 1)
				gpk_necs(outputJson.push_back(','));
		}
		gpk_necs(outputJson.push_back(']'));
	}
	return 0;
}

::gpk::error_t			gpk::jsonWrite				(const ::gpk::SJSONNode* node, const ::gpk::view<::gpk::vcc> & jsonViews, ::gpk::apod<char> & output)			{
	if(node->Token->Type == ::gpk::JSON_TYPE_VALUE && node->Children.size())
		node					= node->Children[0];
	switch(node->Token->Type) {
	case ::gpk::JSON_TYPE_INTEGER		: {
		char						temp[64];
#if defined(GPK_WINDOWS)
		snprintf(temp, ::gpk::size(temp) - 2, "%lli", (uint64_t)node->Token->Value);
#else
		snprintf(temp, ::gpk::size(temp) - 2, "%lli", (long long)node->Token->Value);
#endif
		output.append_string(temp);
	}
		break;
	case ::gpk::JSON_TYPE_DECIMAL		: {
		double						f							= 0;
		memcpy(&f, &node->Token->Value, sizeof(double));
		char						temp[64]					= {};
		uint32_t					lenNum						= snprintf(temp, ::gpk::size(temp) - 2, "%.16f", f);
		while(lenNum > 0 && (temp[lenNum] == 0 || temp[lenNum] == '0')) {
			temp[lenNum] = 0;
			--lenNum;
		}
		gpk_necs(output.append_string(temp));
	}
		break;
	case ::gpk::JSON_TYPE_BOOLEAN			:
		gpk_necs(output.append(bool2vcc(node->Token->Value)));
		break;
	case ::gpk::JSON_TYPE_NULL			:
		gpk_necs(output.append(jsonViews[node->ObjectIndex]));
		break;
	case ::gpk::JSON_TYPE_STRING		:
		gpk_necs(output.push_back('"'));
		gpk_necs(output.append(jsonViews[node->ObjectIndex]));
		gpk_necs(output.push_back('"'));
		break;
	case ::gpk::JSON_TYPE_OBJECT		:
		gpk_necs(output.push_back('{'));
		for(uint32_t iChildren = 0; iChildren < node->Children.size(); iChildren += 2) {
			gpk_necs(gpk::jsonWrite(node->Children[iChildren + 0], jsonViews, output));;
			gpk_necs(output.push_back(':'));
			gpk_necs(gpk::jsonWrite(node->Children[iChildren + 1], jsonViews, output));;
			if(iChildren < node->Children.size() - 2)
				gpk_necs(output.push_back(','));
		}
		gpk_necs(output.push_back('}'));
		break;
	case ::gpk::JSON_TYPE_ARRAY			:
		gpk_necs(output.push_back('['));
		for(uint32_t iChildren = 0; iChildren < node->Children.size(); ++iChildren) {
			gpk_necs(gpk::jsonWrite(node->Children[iChildren], jsonViews, output));;
			if(iChildren < node->Children.size() - 1)
				gpk_necs(output.push_back(','));
		}
		gpk_necs(output.push_back(']'));
	}
	return 0;
}

static	::gpk::error_t	jsonCloseElement			(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, uint32_t indexChar) {
	ree_if(tokens.size() <= (uint32_t)stateReader.IndexCurrentElement, "Invalid parser state. Cannot close element: %i.", stateReader.IndexCurrentElement);
	::gpk::SJSONToken			* closing					= 0;
	closing					= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	closing->Span.End		= (uint32_t)indexChar + 1;
	const ::gpk::vcc			labelType				= ::gpk::get_value_label(closing->Type);
	const char					* labelText				= labelType.begin();
	(void)labelText;
	json_info_printf("%s closed. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i. End: %i.", labelText, stateReader.IndexCurrentElement, stateReader.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End);
	stateReader.IndexCurrentElement					= closing->ParentIndex;
	--stateReader.NestLevel;
	stateReader.CurrentElement						= ((uint32_t)stateReader.IndexCurrentElement < tokens.size()) ? &tokens[stateReader.IndexCurrentElement] : 0;
	if(0 == stateReader.NestLevel)
		stateReader.DoneReading							= true;
	return 0;	// Need to report that a list has been exited
}

static	::gpk::error_t	jsonCloseElement			(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & object, uint32_t indexChar, ::gpk::JSON_TYPE jsonType) {
	const ::gpk::SJSONToken		* open						= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	ree_if(jsonType != open->Type, "Invalid object type: open: %u (%s). closing: %u (%s).", open->Type, ::gpk::get_value_label(open->Type).begin(), jsonType, ::gpk::get_value_label(jsonType).begin());
	return ::jsonCloseElement(stateReader, object, indexChar);
}

static	::gpk::error_t	jsonTestAndCloseValue		(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & object) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_VALUE == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator	= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
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

static	::gpk::error_t	jsonParseStringCharacter	(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, const ::gpk::vcc & jsonAsString)	{
	::gpk::SJSONToken			currentElement				= {};
	::gpk::error_t				errVal						= 0;
	switch(stateReader.CharCurrent) {
	default:
		seterr_break_if(((::gpk::uchar_t)stateReader.CharCurrent) < 0x20 || ((::gpk::uchar_t)stateReader.CharCurrent) > 0xFE, "Invalid character: %i (%u) '%c'.", stateReader.CharCurrent, (::gpk::uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		seterr_break_if(stateReader.Escaping, "Cannot escape character: %i (%u) '%c'.", stateReader.CharCurrent, (::gpk::uchar_t)stateReader.CharCurrent, stateReader.CharCurrent);
		break;
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		::gpk::skipToNextCharacter(stateReader.IndexCurrentChar, jsonAsString);
		--stateReader.IndexCurrentChar;
		break;
	case 'b': case 'f': case 'n': case 'r': case 't':
		break;	// these characters are both valid as part of the string and as escapable characters.
	case 'u':
		if(false == stateReader.Escaping)
			break;
		stateReader.IndexCurrentChar	+= 1;	// skip the u to get the next 4 digits.
		seterr_break_if(jsonAsString.size() - stateReader.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %s. Current index: %u.", jsonAsString.size(), stateReader.IndexCurrentChar);
		json_info_printf("Unicode code point found: %4.4s", &jsonAsString[stateReader.IndexCurrentChar]);
		currentElement		= {stateReader.IndexCurrentElement, ::gpk::JSON_TYPE_CODEPOINT, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + 4}};
		seterr_if(errored(tokens.push_back(currentElement)), "token count: %i", tokens.size());
		stateReader.CurrentElement		= &tokens[stateReader.IndexCurrentElement];
		stateReader.IndexCurrentChar	+= 3;	// Parse unicode code point
		break;
	case '\\'	:
		if(false == stateReader.Escaping) {
			stateReader.Escaping			= true;
			return 0;
		}
		break;
	case '"'	:
		if(false == stateReader.Escaping) {
			seterr_if(errored(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_STRING)), "Failed to close string elment at character %i", stateReader.IndexCurrentChar - 1);//::gpk::get_value_namep(tokens));
			stateReader.InsideString		= false;
			::jsonTestAndCloseValue(stateReader, tokens);
			stateReader.ExpectingSeparator	= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		}
	}
	stateReader.Escaping	= false;
	return errVal;
}

static	::gpk::error_t	jsonParseKeyword			(const ::gpk::vcc & token, ::gpk::JSON_TYPE jsonType, ::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & object, const ::gpk::vcc & jsonAsString)	{
	ree_if(token.size() > jsonAsString.size() - stateReader.IndexCurrentChar, "End of stream while parsing token: %s.", token.begin());
	ree_if(0 != strncmp(token.begin(), &jsonAsString[stateReader.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%s'.", token.begin());
	json_info_printf("JSON token found: %s.", token.begin());

	::gpk::SJSONToken			boolElement					= {stateReader.IndexCurrentElement, jsonType, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + token.size()}, ::gpk::vcc2bool(token)};
	gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %u.", object.size());
	stateReader.IndexCurrentChar	+= token.size() - 1;
	stateReader.CurrentElement		= &object[stateReader.IndexCurrentElement];
	return 0;
}

static	::gpk::error_t	lengthJsonNumber			(uint32_t indexCurrentChar, const ::gpk::vcc & jsonAsString)	{
	const uint32_t				offset						= indexCurrentChar;
	char						charCurrent					= jsonAsString[indexCurrentChar];
	while(indexCurrentChar < jsonAsString.size() &&
		( ( charCurrent >= '1' && charCurrent <= '9')
		 || charCurrent == '0'
		 || charCurrent == '.'
		 )
		) {
		++indexCurrentChar;
		if(indexCurrentChar >= jsonAsString.size())
			break;
		charCurrent				= jsonAsString[indexCurrentChar];
	}
	return indexCurrentChar - offset;
}

//static	::gpk::error_t	parseJsonAbsolutePart		(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, ::gpk::vcc absolutePart, ::gpk::SJSONToken & output, const uint32_t sizeNum, const bool isFloat, const uint32_t index, const int32_t indexCurrentElement, const int32_t indexCurrentChar)	{
//	output					= {indexCurrentElement, isFloat ? ::gpk::JSON_TYPE_DECIMAL : ::gpk::JSON_TYPE_INTEGER, {(uint32_t)indexCurrentChar, indexCurrentChar + sizeNum + (index - indexCurrentChar)}};
//	::gpk::error_t				intCount					= ::gpk::parseIntegerDecimal(absolutePart, output.Value);
//	if(intCount < (int32_t)sizeNum) {
//		output.Type		= ::gpk::JSON_TYPE_DECIMAL;
//		double						finalValue					= (double)output.Value;
//		json_info_printf("Integer part: %f.", finalValue);
//		++intCount;	// Skip dot.
//		const uint32_t				offsetStart					= output.Span.Begin + sizeNum - (sizeNum - intCount);
//		const uint32_t				lenDec						= sizeNum - intCount;
//		if(lenDec) {
//			double						decValue					= 0;
//			if(absolutePart[0] == '.')
//				gpk_necs(absolutePart.slice(absolutePart, offsetStart + 1, lenDec - 1));
//			else
//				gpk_necs(absolutePart.slice(absolutePart, offsetStart, lenDec));
//			const ::gpk::error_t		decCount					= ::gpk::parseIntegerDecimal(absolutePart, decValue);
//			rees_if(errored(decCount));
//			decValue				/= ::gpk::powui(10, decCount);
//			json_info_printf("Decimal part: %f.", decValue);
//			finalValue				+= decValue;
//		}
//		memcpy((double*)&output.Value, &finalValue, sizeof(uint64_t));
//	}
//	else {
//		json_info_printf("Integer part: %i.", output.Value);
//	}
//	return 0;
//}

static	::gpk::error_t	parseSign					(const ::gpk::vcc & strNumber, bool & isNegative, bool & isFloat)	{
	uint32_t					index						= 0;
	if(index < strNumber.size() && strNumber[index] == '+')
		++index;
	if((isNegative = index < strNumber.size() && strNumber[index] == '-')) {
		++index;
	}
	if(index < strNumber.size() && strNumber[index] == '+')
		++index;
	if((isFloat = index < strNumber.size() && strNumber[index] == '.')) {
		++index;
	}
	return (::gpk::error_t)index;
}

static	::gpk::error_t	parseJsonNumber				(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, const ::gpk::vcc & jsonAsString)	{
	const uint32_t				offset						= stateReader.IndexCurrentChar;
	char						charCurrent					= jsonAsString[offset];

	bool						isNegative					= false;
	bool						isFloat						= false;
	int							signLength					= parseSign({&jsonAsString[offset], jsonAsString.size() - offset}, isNegative, isFloat);
	uint32_t					index						= offset + signLength;
	charCurrent				= jsonAsString[index];
	ree_if(index < jsonAsString.size() && (charCurrent != '0' && (charCurrent < '1' || charCurrent > '9')), "Character '%c' at index %i is not a number.", charCurrent, index);

	const uint32_t				sizeNum						= lengthJsonNumber(index, jsonAsString);
	::gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, isFloat ? ::gpk::JSON_TYPE_DECIMAL : ::gpk::JSON_TYPE_INTEGER, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + signLength + sizeNum}};
	::gpk::vcc					numString					= {};
	gpk_necs(jsonAsString.slice(numString, index, sizeNum));

	::gpk::error_t				intCount					= ::gpk::parseIntegerDecimal(numString, currentElement.Value = 0);
	if(isFloat) { // The number begins with a dot and we already skipped it so calculate decimal places for the fractional part.
		double						valuef64					= currentElement.Value / double(::gpk::powui(10, numString.size()));
		if(isNegative) {
			valuef64 	*= -1;
		}
		memcpy(&(currentElement.Value = 0), &valuef64, sizeof(uint64_t));
		json_info_printf("Decimal read: %f.", valuef64);
	}
	else { // there is an integer part so we need to check for decimals
		if(intCount >= (int32_t)sizeNum) // No more digits after the integer part. Leave it this way.
			json_info_printf("Integer read: %lli.", currentElement.Value);
		else { 
			json_info_printf("Integer part: %lli.", currentElement.Value);
			currentElement.Type		= ::gpk::JSON_TYPE_DECIMAL;
			double						finalValue					= (double)currentElement.Value;
			++intCount;	// Skip dot.
			int32_t						lenDec						= sizeNum - intCount;
			if(lenDec > 0) { // there are leftover digits after the integer part
				uint32_t					offsetStart					= currentElement.Span.Begin + sizeNum - (sizeNum - intCount);
				double						decValue					= 0;
				gpk_necs(jsonAsString.slice(numString, offsetStart, lenDec));
				const ::gpk::error_t		decCount					= ::gpk::parseIntegerDecimal(numString, decValue);
				rees_if_failed(decCount);
				decValue				/= ::gpk::powui(10, decCount);
				json_info_printf("Decimal part: %f.", decValue);
				finalValue				+= decValue;
			}
			if(isNegative)
				finalValue *= -1;
			memcpy(&currentElement.Value, &finalValue, sizeof(uint64_t));
		}
	}
	gpk_necs(tokens.push_back(currentElement));
	stateReader.CurrentElement						= &tokens[stateReader.IndexCurrentElement];
	json_info_printf("Number found: %s. Length: %u. Negative: %s. Float: %s."
		, ::gpk::toString({&jsonAsString[index], sizeNum}).begin()
		, sizeNum
		, isNegative	? "true" : "false"
		, isFloat		? "true" : "false"
	);
	stateReader.IndexCurrentChar					+= sizeNum + (index - offset) - 1;
	if(stateReader.IndexCurrentChar + 1 < jsonAsString.size() - 1) {
		charCurrent										= jsonAsString[stateReader.IndexCurrentChar+1];
		ree_if(charCurrent != ' '
			&& charCurrent != '\t'
			&& charCurrent != '\n'
			&& charCurrent != '\r'
			&& charCurrent != '\n'
			&& charCurrent != ']'
			&& charCurrent != '}'
			&& charCurrent != ','
			, "Invalid number termination: '%c'.", charCurrent);
	}
	return 0;
}

static	::gpk::error_t	jsonTestAndCloseKey			(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens) {
	if(stateReader.CurrentElement && ::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type) {
		stateReader.ExpectingSeparator	= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		return ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar);
	}
	return 1;
}

static	::gpk::error_t	jsonCloseOrDiscardEmptyKOrV	(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t				errVal						= 0;
	if(tokens[tokens.size() - 1].Type == containerType) {
		json_info_printf("Discarding empty container element at index %i (%s). Level: %i", tokens.size() - 1, ::gpk::get_value_label(containerType).begin(), stateReader.NestLevel);
		stateReader.IndexCurrentElement	= tokens[tokens.size() - 1].ParentIndex;
		gpk_necs(tokens.pop_back());
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
static	::gpk::error_t	jsonCloseContainer			(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, ::gpk::JSON_TYPE containerType) {
	::gpk::error_t				errVal						= 0;
	gpk_necall(::jsonCloseOrDiscardEmptyKOrV(stateReader, tokens, (::gpk::JSON_TYPE_ARRAY == containerType) ? ::gpk::JSON_TYPE_VALUE : ::gpk::JSON_TYPE_KEY), "Failed to close container at index %i (%s).", stateReader.IndexCurrentElement, ::gpk::get_value_label(containerType).begin());
	errVal					= ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar, containerType);
	(void)errVal;
	stateReader.ExpectingSeparator	= false;
	if(stateReader.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		::jsonTestAndCloseValue(stateReader, tokens);
	return 0;
}

static	::gpk::error_t	jsonOpenElement				(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, ::gpk::JSON_TYPE jsonType, uint32_t indexChar) {
	::gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, jsonType, {indexChar, indexChar}};
	gpk_necs(stateReader.IndexCurrentElement = tokens.push_back(currentElement));
	stateReader.CurrentElement	= &tokens[stateReader.IndexCurrentElement];
	const ::gpk::vcc			labelType					= ::gpk::get_value_label(currentElement.Type);
	(void)labelType;
	++stateReader.NestLevel;
	json_info_printf("%s open. Index %.2i. Level: %i. Parent index: %i. Node type: %i. Begin: %i.", labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin);
	return 0;
}

static	::gpk::error_t	jsonParseDocumentCharacter	(::gpk::SJSONReaderState & stateReader, ::gpk::apod<::gpk::SJSONToken> & tokens, const ::gpk::vcc & jsonAsString)	{
	::gpk::error_t				errVal						= 0;
	::gpk::error_t				nextMatch					= -1;
#define GPK_JSON_EXPECTS_SEPARATOR()		\
	if (stateReader.ExpectingSeparator) {	\
		errVal					= -1; 		\
		json_error_printf("Expected separator, found '%c'.", stateReader.CharCurrent); 	\
		break; 								\
	}

#define test_first_position()	ree_if(0 == stateReader.CharCurrent, "Character found at invalid position. Index: %u. Character: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);

	switch(stateReader.CharCurrent) {
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		break;
	default: // Fallback error for every character that is not recognized by the parser.
		GPK_JSON_EXPECTS_SEPARATOR();
		errVal		= -1;
		json_error_printf("Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		break;
	case '/':
		seterr_break_if(stateReader.IndexCurrentChar >= jsonAsString.size() || '/' != jsonAsString[stateReader.IndexCurrentChar + 1], "Invalid character at index %i. '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		if(-1 != (nextMatch = ::gpk::find('\n', jsonAsString, stateReader.IndexCurrentChar)))
			stateReader.IndexCurrentChar	= nextMatch - 1;
		else
			stateReader.IndexCurrentChar	= jsonAsString.size() - 1;
	break;
	case 'f': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(::gpk::VCC_FALSE	, ::gpk::JSON_TYPE_BOOLEAN, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 't': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(::gpk::VCC_TRUE	, ::gpk::JSON_TYPE_BOOLEAN, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 'n': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size()); errVal = ::jsonParseKeyword(::gpk::VCC_NULL	, ::gpk::JSON_TYPE_NULL, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case '0': case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.': case '-'	: case '+'	: //case 'x'	: // parse int or float accordingly
		GPK_JSON_EXPECTS_SEPARATOR();
		if(-1 == stateReader.IndexCurrentElement)
			ree_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar)), "Failed to open element at index %i.", tokens.size());
		seterr_break_if(stateReader.Escaping, "Invalid character found at index %u: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);	// Set an error or something and skip this character.
		seterr_break_if(::gpk::JSON_TYPE_VALUE != stateReader.CurrentElement->Type, "%s", "Number found outside a value.");
		seterr_break_if(errored(::parseJsonNumber(stateReader, tokens, jsonAsString)), "%s", "Error parsing JSON number.");
		::jsonTestAndCloseValue(stateReader, tokens);
		break;
	case ',':
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar);
		if(::gpk::JSON_TYPE_OBJECT == stateReader.CurrentElement->Type)
			errVal	= ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_KEY, stateReader.IndexCurrentChar + 1);
		else if(::gpk::JSON_TYPE_ARRAY == stateReader.CurrentElement->Type) // Test
			errVal	= ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1);
		stateReader.ExpectingSeparator	= false;
		break;
	case ':':
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator && ::gpk::JSON_TYPE_KEY != stateReader.CurrentElement->Type, "Separator found when not expected at index %i.", stateReader.IndexCurrentChar); // Test if we should be expecting this separator or not.
		stateReader.ExpectingSeparator	= false;
		seterr_break_if(errored(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, ::gpk::JSON_TYPE_KEY) ), "Failed to close key. %s", "Unknown error.");	// close the key before the : character
		seterr_break_if(errored(::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1)), "Failed to open element at index %i.", tokens.size()); // open the value after the : character
		break;
	case ']': test_first_position(); errVal = ::jsonCloseContainer(stateReader, tokens, ::gpk::JSON_TYPE_ARRAY); break;
	case '}': test_first_position(); seterr_break_if(::gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type && tokens.size() - 1 != (uint32_t)stateReader.IndexCurrentElement, "Invalid format: %s", "Keys cannot be left without a value."); errVal = ::jsonCloseContainer(stateReader, tokens, ::gpk::JSON_TYPE_OBJECT); break;
	case '{': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_OBJECT	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_KEY	, stateReader.IndexCurrentChar + 1); } break;
	case '[': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_ARRAY	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_VALUE	, stateReader.IndexCurrentChar + 1); } break;
	case '"': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, ::gpk::JSON_TYPE_STRING	, stateReader.IndexCurrentChar + 1);	// skip the " character in order to set the begin the string
		stateReader.InsideString	= true;
		break;
	}
	stateReader.Escaping	= false;
	return errVal;
}

::gpk::error_t			gpk::jsonParseStep			(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString)	{
	reader.StateRead.CharCurrent	= jsonAsString[reader.StateRead.IndexCurrentChar];
	::gpk::error_t				errVal						= (reader.StateRead.InsideString)
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Token, jsonAsString)
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Token, jsonAsString)
		;
	if (errored(errVal)) {
		const bool					validElement				= (uint32_t)reader.StateRead.IndexCurrentElement < reader.Token.size();
		const ::gpk::SJSONToken		* currentElement			= validElement ? &reader.Token[reader.StateRead.IndexCurrentElement] : 0;
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
			, validElement ? currentElement->ParentIndex					: -1
			, validElement ? currentElement->Type							: -1
			, validElement ? ::gpk::get_value_namep(currentElement->Type)	: "N/A"
			, validElement ? currentElement->Span.Begin						: -1
		);
	}
	return errVal;
}

::gpk::error_t			gpk::jsonTreeRebuild		(::gpk::view<::gpk::SJSONToken>& in_object, ::gpk::apobj<::gpk::SJSONNode> & out_nodes)								{
	::gpk::apobj<::gpk::SJSONNode>	& tree					= out_nodes;
	gpk_necall(tree.resize(in_object.size()), "Out of memory? Object count: %u.", in_object.size());

	// -- Build all nodes linearly, without assigning the children
	for(uint32_t iObject = 0; iObject < tree.size(); ++iObject) {
		::gpk::pobj<::gpk::SJSONNode>	& nodeCurrent		= tree[iObject];
		nodeCurrent->Token			= &in_object[iObject];
		nodeCurrent->Parent			= ((uint32_t)nodeCurrent->Token->ParentIndex < tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Token->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex	= iObject;
		nodeCurrent->Children.clear();
		if(nodeCurrent->Parent)
			nodeCurrent->Parent->Children.push_back(nodeCurrent);
	}

	// -- Remove the key/value wrappers from objects.
	for(uint32_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		::gpk::pobj<::gpk::SJSONNode>				& nodeCurrent										= tree[iObject];
		if( ::gpk::JSON_TYPE_ARRAY	!= nodeCurrent->Token->Type
		 && ::gpk::JSON_TYPE_OBJECT	!= nodeCurrent->Token->Type
		)
			continue;
		::gpk::apobj<::gpk::SJSONNode>	newChildren;
		gpk_necs(newChildren.resize(nodeCurrent->Children.size()));
		for(uint32_t iChild = 0, countChild = newChildren.size(); iChild < countChild; ++iChild)
			newChildren[iChild]						= nodeCurrent->Children[iChild]->Children[0];
		nodeCurrent->Children					= newChildren;
	}
	return 0;
}

#define json_bi_if(condition, format, ...) if(condition) break; //

::gpk::error_t			gpk::jsonParse				(::gpk::SJSONReader & reader, const ::gpk::vcc & jsonAsString)	{
	::gpk::SJSONReaderState		& stateReader				= reader.StateRead;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < jsonAsString.size(); ++stateReader.IndexCurrentChar) {
		gpk_necs(gpk::jsonParseStep(reader, jsonAsString));
		json_bi_if(reader.StateRead.DoneReading, "%i json characters read.", stateReader.IndexCurrentChar + 1);
	}
	ree_if(stateReader.NestLevel, "Nest level: %i (Needs to be zero).", stateReader.NestLevel);
	reader.View.resize(reader.Token.size());
	for(uint32_t iView = 0; iView < reader.Token.size(); ++iView) {
		const ::gpk::SJSONToken		& currentElement									= reader.Token[iView];
		gpk_necs(jsonAsString.slice(reader.View[iView], currentElement.Span.Begin, currentElement.Span.End - currentElement.Span.Begin));
	}
	return ::gpk::jsonTreeRebuild(reader.Token, reader.Tree);
}

::gpk::error_t			gpk::jsonObjectKeyList		(const ::gpk::SJSONNode & node_object, const ::gpk::vvcc & views, ::gpk::ai32 & indices, ::gpk::avcc & keys)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode		* node					= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::vcc			& view					= views[node->ObjectIndex];
		gpk_necs(indices.push_back(node->ObjectIndex));
		gpk_necs(keys	.push_back(view));
	}
	return indices.size();
}

::gpk::error_t			gpk::jsonObjectKeyList		(const ::gpk::SJSONNode & node_object, ::gpk::ai32 & indices)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		gpk_necs(indices.push_back(node->ObjectIndex));
	}
	return indices.size();
}

::gpk::error_t			gpk::jsonObjectKeyList		(const ::gpk::SJSONNode & node_object, const ::gpk::vvcc & views, ::gpk::avcc & keys)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::vcc			& view						= views[node->ObjectIndex];
		gpk_necs(keys.push_back(view));
	}
	return keys.size();
}

::gpk::error_t			gpk::jsonObjectValueGet		(const ::gpk::SJSONNode & node_object, const ::gpk::vvcc & views, const ::gpk::vcs & key)	{
	ree_if(::gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %i (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, ::gpk::get_value_label(node_object.Token->Type).begin());
	for(uint32_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		const ::gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(::gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %u (%s). Only string types (%u) can be keys of JSON objects.", node->Token->Type, ::gpk::get_value_label(node->Token->Type).begin(), ::gpk::JSON_TYPE_STRING);
		const ::gpk::vcc			& view						= views[node->ObjectIndex];
		if(key == view)
			return (::gpk::error_t)node->ObjectIndex + 2; // one for value and other for the actual element
	}
	return -1;
}

::gpk::error_t			gpk::jsonArrayValueGet		(const ::gpk::SJSONNode& tree, uint32_t index)				{
	ree_if(::gpk::JSON_TYPE_ARRAY != tree.Token->Type, "Invalid node type: %i (%s). Only arrays are allowed to be accessed by index.", tree.Token->Type, ::gpk::get_value_label(tree.Token->Type).begin());
	ree_if(index >= tree.Children.size(), "Index out of range: %i. Max index: %i.", index, tree.Children.size() - 1);
	const ::gpk::SJSONNode		* node						= tree.Children[index];	// Get the
	ree_if(0 == node, "Invalid or corrupt tree. %s", "Nodes cannot be null.");
	return node->ObjectIndex;
}

::gpk::error_t			gpk::jsonCompareArray		(const ::gpk::SJSONNode & node, const ::gpk::view<::gpk::vcc>& views, const ::gpk::SJSONNode& other, const ::gpk::view<::gpk::vcc>& otherViews) {
	if(node.Children.size() != other.Children.size())
		return 0;
	if(node.Children.size() == 0)
		return 1;
	for(uint32_t iChild = 0; iChild < node.Children.size(); ++iChild) {
		const ::gpk::SJSONNode		& childNode					= *node	.Children[iChild];
		const ::gpk::SJSONNode		& childOther				= *other.Children[iChild];
		const ::gpk::JSON_TYPE		typeNode					= childNode	.Token->Type;
		const ::gpk::JSON_TYPE		typeOther					= childOther.Token->Type;
		if(typeNode != typeOther)
			return 0;
		::gpk::error_t				result						= -1;
		switch(typeNode) {
		case ::gpk::JSON_TYPE_ARRAY		: result = ::gpk::jsonCompareArray	(childNode, views, childOther, otherViews); break;
		case ::gpk::JSON_TYPE_OBJECT	: result = ::gpk::jsonCompareObject	(childNode, views, childOther, otherViews); break;
		case ::gpk::JSON_TYPE_DECIMAL	:
		case ::gpk::JSON_TYPE_INTEGER	: result = ::gpk::jsonCompareNumber	(childNode, views, childOther, otherViews); break;
		default:
			result					= views[node.ObjectIndex] == otherViews[other.ObjectIndex];
		}
		if(0 == result)
			return 0;
	}
	if(views[node.ObjectIndex] == otherViews[other.ObjectIndex])
		return 1;
	return 1;
}

// Get the codepoint from characters of an unicode escape sequence
//	For input "\u c1 c2 c3 c4" the codepoint is:
//		(c1 * 0x1000)	+ (c2 * 0x0100)	+ (c3 * 0x0010) + c4
// aka:	(c1 << 12)		+ (c2 << 8)		+ (c3 << 4)		+ (c4 << 0)
//
//	Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f' must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp.
//	The conversion is done by subtracting the offset (0x30, 0x37, and 0x57) between the ASCII value of the character and the desired integer value.
//	Returns codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or non-hex character)
static	::gpk::error_t	decodeUnicodeEscapeSequence	(::gpk::vcc input, uint32_t& ret_unicode)		{
	ree_if(input.size() < 4, "Invalid escape sequence: %s.", input.begin());
	return ::gpk::parseIntegerHexadecimal(input, ret_unicode);
}

// this function only makes sense after reading the first `\u`
::gpk::error_t			jsonToCodePoint				(::gpk::vcc input, uint32_t& unicode)			{
	gpk_necall(::decodeUnicodeEscapeSequence(input, unicode), "Invalid escape sequence: %s.", ::gpk::toString(input).begin());
	if (unicode < 0xD800 || unicode > 0xDBFF)
		return 0;
	// surrogate pairs
	ree_if(input.size() < 7, "%s", "expecting a \\u token for the second half of the surrogate pair");
	ree_if(input[4] != '\\' || input[5] == 'u', "%s", "expecting a \\u token for the second half of the surrogate pair");
	uint32_t						surrogatePair;
	gpk_necall(::decodeUnicodeEscapeSequence({&input[6], 4U}, surrogatePair), "Invalid escape sequence: %s.", ::gpk::toString(input).begin());
	unicode														= 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
	return 0;
}


::gpk::error_t	gpk::jsonObjectGetString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	& value)	{ value = reader.View[iNode]; return iNode; }
::gpk::error_t	gpk::jsonObjectGetBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value)	{ value = (bool )reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i64_t	& value)	{ value = (i64_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i32_t	& value)	{ value = (i32_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i16_t	& value)	{ value = (i16_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i8_t	& value)	{ value = (i8_t )reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u64_t	& value)	{ value = (u64_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u32_t	& value)	{ value = (u32_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u16_t	& value)	{ value = (u16_t)reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, u8_t	& value)	{ value = (u8_t )reader.Token[iNode].Value; return iNode; }
::gpk::error_t	gpk::jsonObjectGetDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value)	{ double dealiased; memcpy(&dealiased, &reader[iNode]->Token->Value, sizeof(double)); value = dealiased; return iNode; }
::gpk::error_t	gpk::jsonObjectGetDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, float	& value)	{ double dealiased; memcpy(&dealiased, &reader[iNode]->Token->Value, sizeof(double)); value = (float)dealiased; return iNode; }

// TODO: Finish
//::gpk::error_t	gpk::jsonObjectGetIntegerAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	 & value)	{ int64_t	integer; jsonObjectGetInteger(reader, iNode, integer); char tmp[64]; sprintf_s(tmp, "%lli", integer); value = ::gpk::label(tmp); return iNode; }
::gpk::error_t	gpk::jsonObjectGetIntegerAsBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	 & value)	{ int64_t	integer; jsonObjectGetInteger(reader, iNode, integer); value = 0 != integer; return iNode; }
::gpk::error_t	gpk::jsonObjectGetIntegerAsDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double & value)	{ int64_t	integer; jsonObjectGetInteger(reader, iNode, integer); value = double(integer); return iNode; }
::gpk::error_t	gpk::jsonObjectGetDecimalAsInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i64_t	 & value)	{ double	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = i64_t (decimal); return iNode; }
//::gpk::error_t	gpk::jsonObjectGetDecimalAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	 & value)	{ double	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = decimal; return iNode; }
::gpk::error_t	gpk::jsonObjectGetDecimalAsBoolean(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	 & value)	{ double	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = 0 != decimal; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetStringAsInteger (const ::gpk::SJSONReader & reader, uint32_t iNode, i64_t	 & value)	{ gpk::vcc	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(string, value); return iNode; }
//::gpk::error_t	gpk::jsonObjectGetStringAsBoolean (const ::gpk::SJSONReader & reader, uint32_t iNode, bool	 & value)	{ gpk::vcc	string ; jsonObjectGetString (reader, iNode, string ); value = string.size() ? vcc2bool(string) : false; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetStringAsDecimal (const ::gpk::SJSONReader & reader, uint32_t iNode, double & value)	{ gpk::vcc	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(value = string ; return iNode; }
::gpk::error_t	gpk::jsonObjectGetBooleanAsInteger(const ::gpk::SJSONReader & reader, uint32_t iNode, i64_t	 & value)	{ bool		boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = one_if(boolean); return iNode; }
//::gpk::error_t	gpk::jsonObjectGetBooleanAsString (const ::gpk::SJSONReader & reader, uint32_t iNode, vcc	 & value)	{ bool		boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = boolean; return iNode; }
::gpk::error_t	gpk::jsonObjectGetBooleanAsDecimal(const ::gpk::SJSONReader & reader, uint32_t iNode, double & value)	{ bool		boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = one_if(boolean); return iNode; }
// 
//::gpk::error_t	gpk::jsonObjectGetAsString	(const ::gpk::SJSONReader & reader, uint32_t iNode, vcc		& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_STRING ) return jsonObjectGetString (reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_BOOLEAN) { bool boolean = false; gpk_necs(jsonObjectGetBoolean(reader, iNode, boolean)); ::gpk::bool2char(boolean, value); } return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsBoolean	(const ::gpk::SJSONReader & reader, uint32_t iNode, bool	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_BOOLEAN) return jsonObjectGetBoolean(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_STRING ) { ::gpk::vcc boolean = {}; gpk_necs(jsonObjectGetString(reader, iNode, boolean)); value = ::gpk::vcc2bool(boolean); }; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, i64_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, i32_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, i16_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, i8_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u64_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u32_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u16_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, u8_t	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) return jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, double	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) return jsonObjectGetDecimal(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) {}; return iNode; }
//::gpk::error_t	gpk::jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, float	& value)	{ if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_DECIMAL) return jsonObjectGetDecimal(reader, iNode, value); else if(reader.Token[iNode].Type == ::gpk::JSON_TYPE_INTEGER) {}; return iNode; }

#define json_rew_if_failed(condition, format, ...) rews_if_failed(condition) /// rew_if_failed

::gpk::error_t	gpk::jsonObjectGetAsString	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, vcc		& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetString (reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsBoolean	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, bool	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetBoolean(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, i64_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, i32_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, i16_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, i8_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u64_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u32_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u16_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsInteger	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, u8_t	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetInteger(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, double	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetDecimal(reader, index, value); } 
::gpk::error_t	gpk::jsonObjectGetAsDecimal	(const ::gpk::SJSONReader & reader, uint32_t iNode, const ::gpk::vcs & key, float	& value)	{ int32_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %i, key: '%s', node: '%s'.", iNode, ::gpk::toString(key).begin(), ::gpk::toString(reader.View[iNode]).begin()); return jsonObjectGetDecimal(reader, index, value); } 
