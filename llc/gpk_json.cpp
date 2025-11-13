#define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG

#include "gpk_json.h"
#include "gpk_file.h"
#include "gpk_parse.h"
#include "gpk_math.h"

GPK_USING_TYPEINT();

#define json_info_printf(...) // info_printf
#define json_error_printf error_printf

gpk::err_t			gpk::jsonCompareObject		(cnst gpk::SJSONNode & node, cnst gpk::view<vcsc_t> & views, cnst gpk::SJSONNode & other, cnst gpk::view<vcsc_t> & otherViews) { if(node.Children.size() != other.Children.size()) rtrn 0; if(node.Children.size() == 0) rtrn 1; for(u2_t iChild = 0; iChild < node.Children.size(); iChild += 2)	if(node.Token->Type != other.Token->Type) rtrn 0; rtrn (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }
gpk::err_t			gpk::jsonCompareNumber		(cnst gpk::SJSONNode & node, cnst gpk::view<vcsc_t> & views, cnst gpk::SJSONNode & other, cnst gpk::view<vcsc_t> & otherViews) { if(node.Children.size() != other.Children.size()) rtrn 0; if(node.Children.size() == 0) rtrn 1; if(node.Token->Type != other.Token->Type) rtrn 0; rtrn (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }

gpk::err_t			gpk::jsonMapToFields
(	gpk::as2_t											& indicesOfFields
,	cnst gpk::view<cnst gpk::SJSONFieldBinding>	fields
,	cnst gpk::view<cnst gpk::TKeyValConstChar>	fieldMaps
) {
	for(u2_t iMap = 0; iMap < fieldMaps.size(); ++iMap)
	for(u2_t iField = 0; iField < fields.size(); ++iField) {
		cnst gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
		if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
			gpk_necs(indicesOfFields.push_back(iField));
			break;
		}
	}
	rtrn indicesOfFields.size();
}

gpk::err_t			gpk::jsonFieldsToMap
(	gpk::as2_t											& indicesOfMaps
,	cnst gpk::view<cnst gpk::SJSONFieldBinding>	fields
,	cnst gpk::view<cnst gpk::TKeyValConstChar>	fieldMaps
) {
	u2_t					countFields					= fields.size();
	for(u2_t iField = 0; iField < countFields; ++iField) {
		b8_t						bNotAdded					= true;
		for(u2_t iMap = 0; iMap < fieldMaps.size(); ++iMap) {
			cnst gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
			if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
				gpk_necs(indicesOfMaps.push_back(iMap));
				bNotAdded				= false;
				break;
			}
		}
		if(bNotAdded)
			gpk_necs(indicesOfMaps.push_back(-1));
	}
	rtrn indicesOfMaps.size();
}

gpk::err_t			gpk::jsonFileRead			(gpk::SJSONFile & file, gpk::vcsc_c & filename) {
	json_info_printf("Loading json file: %s.", filename.begin());
	gpk_necall(gpk::fileToMemory(filename, file.Bytes), "Failed to load file: '%s'", filename.begin());
	rtrn gpk::jsonParse(file.Reader, file.Bytes);
}

gpk::err_t			gpk::jsonArraySplit			(cnst gpk::SJSONNode & jsonArrayToSplit, cnst gpk::view<vcsc_t> & jsonViews, u2_c blockSize, gpk::aobj<gpk::apod<sc_t>> & outputJsons)		{
	u2_c				remainder					= jsonArrayToSplit.Children.size() % blockSize;
	u2_c				countParts					= jsonArrayToSplit.Children.size() / blockSize + one_if(remainder);
	gpk_necs(outputJsons.resize(countParts));
	u2_t					iSourceRecord						= 0;
	for(u2_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		gpk::apod<sc_t>			& outputJson				= outputJsons[iPart];
		gpk_necs(outputJson.push_back('['));
		for(u2_t iPartRecord = 0, countPartRecords = (remainder && iPart == countParts - 1) ? remainder : blockSize
			; iPartRecord < countPartRecords
			; ++iPartRecord) {
			gpk_necs(gpk::jsonWrite(jsonArrayToSplit.Children[iSourceRecord++], jsonViews, outputJson));;
			if(iPartRecord < countPartRecords - 1)
				gpk_necs(outputJson.push_back(','));
		}
		gpk_necs(outputJson.push_back(']'));
	}
	rtrn 0;
}

gpk::err_t			gpk::jsonWrite				(cnst gpk::SJSONNode* node, cnst gpk::view<vcsc_t> & jsonViews, gpk::apod<sc_t> & output)			{
	if(node->Token->Type == gpk::JSON_TYPE_VALUE && node->Children.size())
		node				= node->Children[0];
	switch(node->Token->Type) {
	case gpk::JSON_TYPE_INTEGER		: {
		sc_t					temp[64]					= {};
		snprintf(temp, gpk::size(temp) - 2, "%" GPK_FMT_S3, (u3_t)node->Token->Value);
		gpk_necs(output.append_string(temp));
	}
		break;
	case gpk::JSON_TYPE_DECIMAL		: {
		f3_t					f							= 0;
		memcpy(&f, &node->Token->Value, szof(f3_t));
		sc_t					temp[64]					= {};
		u2_t					lenNum						= snprintf(temp, gpk::size(temp) - 2, "%.16f", f);
		while(lenNum > 0 && (temp[lenNum] == 0 || temp[lenNum] == '0')) {
			temp[lenNum] = 0;
			--lenNum;
		}
		gpk_necs(output.append_string(temp));
	}
		break;
	case gpk::JSON_TYPE_BOOLEAN			:
		gpk_necs(output.append(bool2vcc(node->Token->Value)));
		break;
	case gpk::JSON_TYPE_NULL			:
		gpk_necs(output.append(jsonViews[node->ObjectIndex]));
		break;
	case gpk::JSON_TYPE_STRING		:
		gpk_necs(output.push_back('"'));
		gpk_necs(output.append(jsonViews[node->ObjectIndex]));
		gpk_necs(output.push_back('"'));
		break;
	case gpk::JSON_TYPE_OBJECT		:
		gpk_necs(output.push_back('{'));
		for(u2_t iChildren = 0; iChildren < node->Children.size(); iChildren += 2) {
			gpk_necs(gpk::jsonWrite(node->Children[iChildren + 0], jsonViews, output));;
			gpk_necs(output.push_back(':'));
			gpk_necs(gpk::jsonWrite(node->Children[iChildren + 1], jsonViews, output));;
			if(iChildren < node->Children.size() - 2)
				gpk_necs(output.push_back(','));
		}
		gpk_necs(output.push_back('}'));
		break;
	case gpk::JSON_TYPE_ARRAY			:
		gpk_necs(output.push_back('['));
		for(u2_t iChildren = 0; iChildren < node->Children.size(); ++iChildren) {
			gpk_necs(gpk::jsonWrite(node->Children[iChildren], jsonViews, output));;
			if(iChildren < node->Children.size() - 1)
				gpk_necs(output.push_back(','));
		}
		gpk_necs(output.push_back(']'));
	}
	rtrn 0;
}

sttc	gpk::err_t	jsonCloseElement			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, u2_t indexChar) {
	ree_if(tokens.size() <= (u2_t)stateReader.IndexCurrentElement, "Invalid parser state. Cannot close element: %" GPK_FMT_S2 ".", stateReader.IndexCurrentElement);
	gpk::SJSONToken			* closing					= 0;
	closing					= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	closing->Span.End		= (u2_t)indexChar + 1;
	gpk::vcsc_c			labelType				= gpk::get_value_label(closing->Type);
	const sc_t					* labelText				= labelType.begin();
	(void)labelText;
	json_info_printf("%s closed. Index %.2i. Level: %" GPK_FMT_S2 ". Parent index: %" GPK_FMT_S2 ". Node type: %" GPK_FMT_S2 ". Begin: %" GPK_FMT_S2 ". End: %" GPK_FMT_S2 ".", labelText, stateReader.IndexCurrentElement, stateReader.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End);
	stateReader.IndexCurrentElement					= closing->ParentIndex;
	--stateReader.NestLevel;
	stateReader.CurrentElement						= ((u2_t)stateReader.IndexCurrentElement < tokens.size()) ? &tokens[stateReader.IndexCurrentElement] : 0;
	if(0 == stateReader.NestLevel)
		stateReader.DoneReading							= true;
	rtrn 0;	// Need to report that a list has been exited
}

sttc	gpk::err_t	jsonCloseElement			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & object, u2_t indexChar, gpk::JSON_TYPE jsonType) {
	cnst gpk::SJSONToken		* open						= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	ree_if(jsonType != open->Type, "Invalid object type: open: %" GPK_FMT_U2 " (%s). closing: %" GPK_FMT_U2 " (%s).", open->Type, gpk::get_value_label(open->Type).begin(), jsonType, gpk::get_value_label(jsonType).begin());
	rtrn ::jsonCloseElement(stateReader, object, indexChar);
}

sttc	gpk::err_t	jsonTestAndCloseValue		(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens) {
	if_null_fe(stateReader.CurrentElement);
	if_true_vv(1, gpk::JSON_TYPE_VALUE != stateReader.CurrentElement->Type);
	gpk::err_c			result			= ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar);
	stateReader.ExpectingSeparator	= true;
	if(stateReader.CurrentElement) {
		gpk::SJSONToken		& element		= *stateReader.CurrentElement;
		if(gpk::JSON_TYPE_ARRAY  == element.Type
		|| gpk::JSON_TYPE_OBJECT == element.Type
		) ++element.Value;
	}
	rtrn result;
}

sttc	gpk::err_t	jsonTestAndCloseKey			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens) {
	if_null_fe(stateReader.CurrentElement);
	if_true_vv(1, gpk::JSON_TYPE_KEY != stateReader.CurrentElement->Type);
	stateReader.ExpectingSeparator	= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
	rtrn ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar);
}

sttc	gpk::err_t	jsonCloseOrDiscardEmptyKOrV	(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::JSON_TYPE containerType) {
	gpk::err_t				errVal						= 0;
	if(tokens[tokens.size() - 1].Type == containerType) {
		json_info_printf("Discarding empty container element at index %" GPK_FMT_S2 " (%s). Level: %" GPK_FMT_S2 "", tokens.size() - 1, gpk::get_value_label(containerType).begin(), stateReader.NestLevel);
		stateReader.IndexCurrentElement	= tokens[tokens.size() - 1].ParentIndex;
		gpk_necs(tokens.pop_back());
		--stateReader.NestLevel;
		if((u2_t)stateReader.IndexCurrentElement < tokens.size())
			stateReader.CurrentElement	= &tokens[stateReader.IndexCurrentElement];
	}
	else {
		json_info_printf("Closing container at index %" GPK_FMT_S2 " (%s).", tokens.size() - 1, gpk::get_value_label(containerType).begin());
			 if(gpk::JSON_TYPE_VALUE	== containerType) errVal = ::jsonTestAndCloseValue	(stateReader, tokens);
		else if(gpk::JSON_TYPE_KEY		== containerType) errVal = ::jsonTestAndCloseKey	(stateReader, tokens);
	}
	rtrn errVal;
}

#define seterr_break_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		json_error_printf(format, __VA_ARGS__);	\
		break;									\
	}

#define seterr_if(condition, format, ...)	\
	if(condition) {								\
		errVal					= -1;			\
		json_error_printf(format, __VA_ARGS__);	\
	}
#define seterr_break_if_failed(condition, format, ...)	seterr_break_if	(gpk::failed(condition), format, __VA_ARGS__)
#define seterr_if_failed(condition, format, ...)		seterr_if		(gpk::failed(condition), format, __VA_ARGS__)

sttc	gpk::err_t	jsonParseStringCharacter	(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcsc_c & jsonAsString)	{
	gpk::SJSONToken			currentElement				= {};
	gpk::err_t				errVal						= 0;
	switch(stateReader.CharCurrent) {
	default:
		seterr_break_if(((gpk::uc_t)stateReader.CharCurrent) < 0x20 || ((gpk::uc_t)stateReader.CharCurrent) > 0xFE, "Invalid character: %" GPK_FMT_S2 " (%" GPK_FMT_U2 ") '%c'.", stateReader.CharCurrent, (gpk::uc_t)stateReader.CharCurrent, stateReader.CharCurrent);
		seterr_break_if(stateReader.Escaping, "Cannot escape character: %" GPK_FMT_S2 " (%" GPK_FMT_U2 ") '%c'.", stateReader.CharCurrent, (gpk::uc_t)stateReader.CharCurrent, stateReader.CharCurrent);
		break;
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		gpk::skipToNextCharacter(stateReader.IndexCurrentChar, jsonAsString);
		--stateReader.IndexCurrentChar;
		break;
	case 'b': case 'f': case 'n': case 'r': case 't':
		break;	// these characters are both valid as part of the string and as escapable characters.
	case 'u':
		if(false == stateReader.Escaping)
			break;
		stateReader.IndexCurrentChar	+= 1;	// skip the u to get the next 4 digits.
		seterr_break_if(jsonAsString.size() - stateReader.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %s. Current index: %" GPK_FMT_U2 ".", jsonAsString.size(), stateReader.IndexCurrentChar);
		json_info_printf("Unicode code point found: %4.4s", &jsonAsString[stateReader.IndexCurrentChar]);
		currentElement		= {stateReader.IndexCurrentElement, gpk::JSON_TYPE_CODEPOINT, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + 4}, {}};
		seterr_if_failed(tokens.push_back(currentElement), "token count: %" GPK_FMT_S2 "", tokens.size());
		stateReader.CurrentElement		= &tokens[stateReader.IndexCurrentElement];
		stateReader.IndexCurrentChar	+= 3;	// Parse unicode code point
		break;
	case '\\'	:
		if(false == stateReader.Escaping) {
			stateReader.Escaping			= true;
			rtrn 0;
		}
		break;
	case '"'	:
		if(false == stateReader.Escaping) {
			seterr_if_failed(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, gpk::JSON_TYPE_STRING), "Failed to close string elment at character %" GPK_FMT_S2 "", stateReader.IndexCurrentChar - 1);//gpk::get_value_namep(tokens));
			stateReader.InsideString		= false;
			::jsonTestAndCloseValue(stateReader, tokens);
			stateReader.ExpectingSeparator	= true;	// actually we expect the separator AFTER calling jsonCloseElement(). However, such function doesn't care about this value, so we can simplify the code by reversing the operations.
		}
	}
	stateReader.Escaping	= false;
	rtrn errVal;
}

sttc	gpk::err_t	jsonParseKeyword			(gpk::vcsc_c & token, gpk::JSON_TYPE jsonType, gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & object, gpk::vcsc_c & jsonAsString)	{
	ree_if(token.size() > jsonAsString.size() - stateReader.IndexCurrentChar, "End of stream while parsing token: %s.", token.begin());
	ree_if(0 != strncmp(token.begin(), &jsonAsString[stateReader.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%s'.", token.begin());
	json_info_printf("JSON token found: %s.", token.begin());

	gpk::SJSONToken			boolElement					= {stateReader.IndexCurrentElement, jsonType, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + token.size()}, gpk::vcc2bool(token)};
	gpk_necall(object.push_back(boolElement), "Failed to push! Out of memory? object count: %" GPK_FMT_U2 ".", object.size());
	stateReader.IndexCurrentChar	+= token.size() - 1;
	stateReader.CurrentElement		= &object[stateReader.IndexCurrentElement];
	rtrn 0;
}

sttc	gpk::err_t	lengthJsonNumber			(u2_t indexCurrentChar, gpk::vcsc_c & jsonAsString)	{
	u2_c				offset						= indexCurrentChar;
	sc_t						charCurrent					= jsonAsString[indexCurrentChar];
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
	rtrn indexCurrentChar - offset;
}

//sttc	gpk::err_t	parseJsonAbsolutePart		(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, vcsc_t absolutePart, gpk::SJSONToken & output, u2_c sizeNum, const b8_t isFloat, u2_c index, const s2_t indexCurrentElement, const s2_t indexCurrentChar)	{
//	output					= {indexCurrentElement, isFloat ? gpk::JSON_TYPE_DECIMAL : gpk::JSON_TYPE_INTEGER, {(u2_t)indexCurrentChar, indexCurrentChar + sizeNum + (index - indexCurrentChar)}};
//	gpk::err_t				intCount					= gpk::parseIntegerDecimal(absolutePart, output.Value);
//	if(intCount < (s2_t)sizeNum) {
//		output.Type		= gpk::JSON_TYPE_DECIMAL;
//		f3_t						finalValue					= (f3_t)output.Value;
//		json_info_printf("Integer part: %f.", finalValue);
//		++intCount;	// Skip dot.
//		u2_c				offsetStart					= output.Span.Begin + sizeNum - (sizeNum - intCount);
//		u2_c				lenDec						= sizeNum - intCount;
//		if(lenDec) {
//			f3_t						decValue					= 0;
//			if(absolutePart[0] == '.')
//				gpk_necs(absolutePart.slice(absolutePart, offsetStart + 1, lenDec - 1));
//			else
//				gpk_necs(absolutePart.slice(absolutePart, offsetStart, lenDec));
//			cnst gpk::err_t		decCount					= gpk::parseIntegerDecimal(absolutePart, decValue);
//			if_failed_ve(decCount);
//			decValue				/= gpk::powui(10, decCount);
//			json_info_printf("Decimal part: %f.", decValue);
//			finalValue				+= decValue;
//		}
//		memcpy((f3_t*)&output.Value, &finalValue, szof(uint64_t));
//	}
//	else {
//		json_info_printf("Integer part: %" GPK_FMT_S2 ".", output.Value);
//	}
//	rtrn 0;
//}

sttc	gpk::err_t	parseSign					(gpk::vcsc_c & strNumber, b8_t & isNegative, b8_t & isFloat)	{
	u2_t					index						= 0;
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
	rtrn (gpk::err_t)index;
}

sttc	gpk::err_t	parseJsonNumber				(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcsc_c & jsonAsString)	{
	u2_c					offset						= stateReader.IndexCurrentChar;
	sc_t					charCurrent					= jsonAsString[offset];
	b8_t					isNegative					= false;
	b8_t					isFloat						= false;
	s2_t					signLength					= parseSign({&jsonAsString[offset], jsonAsString.size() - offset}, isNegative, isFloat);
	u2_t					index						= offset + signLength;
	charCurrent			= jsonAsString[index];
	ree_if(index < jsonAsString.size() && (charCurrent != '0' && (charCurrent < '1' || charCurrent > '9')), "Character '%c' at index %" GPK_FMT_S2 " is not a number.", charCurrent, index);

	u2_c					sizeNum						= lengthJsonNumber(index, jsonAsString);
	gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, isFloat ? gpk::JSON_TYPE_DECIMAL : gpk::JSON_TYPE_INTEGER, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + signLength + sizeNum}, {}};
	gpk::vcsc_t				numString					= {};
	gpk_necs(jsonAsString.slice(numString, index, sizeNum));

	gpk::err_t				intCount					= gpk::parseIntegerDecimal(numString, currentElement.Value = 0);
	if(isFloat) { // The number begins with a dot and we already skipped it so calculate decimal places for the fractional part.
		f3_t						valuef64					= currentElement.Value / f3_t(gpk::powui(10, numString.size()));
		if(isNegative) {
			valuef64 	*= -1;
		}
		memcpy(&(currentElement.Value = 0), &valuef64, szof(uint64_t));
		json_info_printf("Decimal read: %f.", valuef64);
	}
	else { // there is an integer part so we need to check for decimals
		if(intCount >= (s2_t)sizeNum) // No more digits after the integer part. Leave it this way.
			json_info_printf("Integer read: %lli.", currentElement.Value);
		else { 
			json_info_printf("Integer part: %lli.", currentElement.Value);
			currentElement.Type		= gpk::JSON_TYPE_DECIMAL;
			f3_t						finalValue					= (f3_t)currentElement.Value;
			++intCount;	// Skip dot.
			s2_t						lenDec						= sizeNum - intCount;
			if(lenDec > 0) { // there are leftover digits after the integer part
				u2_t					offsetStart					= currentElement.Span.Begin + sizeNum - (sizeNum - intCount);
				f3_t						decValue					= 0;
				gpk_necs(jsonAsString.slice(numString, offsetStart, lenDec));
				cnst gpk::err_t		decCount					= gpk::parseIntegerDecimal(numString, decValue);
				rees_if_failed(decCount);
				decValue				/= gpk::powui(10, decCount);
				json_info_printf("Decimal part: %f.", decValue);
				finalValue				+= decValue;
			}
			if(isNegative)
				finalValue *= -1;
			memcpy(&currentElement.Value, &finalValue, szof(uint64_t));
		}
	}
	gpk_necs(tokens.push_back(currentElement));
	stateReader.CurrentElement						= &tokens[stateReader.IndexCurrentElement];
	json_info_printf("Number found: %s. Length: %" GPK_FMT_U2 ". Negative: %s. Float: %s."
		, gpk::toString({&jsonAsString[index], sizeNum}).begin()
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
	rtrn 0;
}

sttc	gpk::err_t	jsonCloseContainer			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::JSON_TYPE containerType) {
	gpk::err_t				errVal						= 0;
	gpk_necall(::jsonCloseOrDiscardEmptyKOrV(stateReader, tokens, (gpk::JSON_TYPE_ARRAY == containerType) ? gpk::JSON_TYPE_VALUE : gpk::JSON_TYPE_KEY), "Failed to close container at index %" GPK_FMT_S2 " (%s).", stateReader.IndexCurrentElement, gpk::get_value_label(containerType).begin());
	errVal					= ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar, containerType);
	(void)errVal;
	stateReader.ExpectingSeparator	= false;
	if(stateReader.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		rtrn ::jsonTestAndCloseValue(stateReader, tokens);
	rtrn 0;
}

sttc	gpk::err_t	jsonOpenElement				(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::JSON_TYPE jsonType, u2_t indexChar) {
	gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, jsonType, {indexChar, indexChar}, {}};
	gpk_necs(stateReader.IndexCurrentElement = tokens.push_back(currentElement));
	stateReader.CurrentElement	= &tokens[stateReader.IndexCurrentElement];
	gpk::vcsc_c			labelType					= gpk::get_value_label(currentElement.Type);
	(void)labelType;
	++stateReader.NestLevel;
	json_info_printf("%s open. Index %.2i. Level: %" GPK_FMT_S2 ". Parent index: %" GPK_FMT_S2 ". Node type: %" GPK_FMT_S2 ". Begin: %" GPK_FMT_S2 ".", labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin);
	rtrn 0;
}

sttc	gpk::err_t	jsonParseDocumentCharacter	(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcsc_c & jsonAsString)	{
	gpk::err_t				errVal						= 0;
	gpk::err_t				nextMatch					= -1;
#define GPK_JSON_EXPECTS_SEPARATOR()		\
	if (stateReader.ExpectingSeparator) {	\
		errVal					= -1; 		\
		json_error_printf("Expected separator, found '%c'.", stateReader.CharCurrent); 	\
		break; 								\
	}

#define test_first_position()	ree_if(0 == stateReader.CharCurrent, "Character found at invalid position. Index: %" GPK_FMT_U2 ". Character: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);

	switch(stateReader.CharCurrent) {
	case ' '	: case '\t'	: case '\r'	: case '\n'	: case '\f'	: case '\b'	: // Skip these characters without error.
		break;
	default: // Fallback error for every character that is not recognized by the parser.
		GPK_JSON_EXPECTS_SEPARATOR();
		errVal		= -1;
		json_error_printf("Invalid character at index %" GPK_FMT_S2 ". '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		break;
	case '/':
		seterr_break_if(stateReader.IndexCurrentChar >= jsonAsString.size() || '/' != jsonAsString[stateReader.IndexCurrentChar + 1], "Invalid character at index %" GPK_FMT_S2 ". '%c'", stateReader.IndexCurrentChar, stateReader.CharCurrent);
		if(-1 != (nextMatch = gpk::find('\n', jsonAsString, stateReader.IndexCurrentChar)))
			stateReader.IndexCurrentChar	= nextMatch - 1;
		else
			stateReader.IndexCurrentChar	= jsonAsString.size() - 1;
	break;
	case 'f': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) if_fail_vef(-1, ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar), "Failed to open element at index %" GPK_FMT_S2 ".", tokens.size()); errVal = ::jsonParseKeyword(gpk::VCC_FALSE	, gpk::JSON_TYPE_BOOLEAN, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 't': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) if_fail_vef(-1, ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar), "Failed to open element at index %" GPK_FMT_S2 ".", tokens.size()); errVal = ::jsonParseKeyword(gpk::VCC_TRUE	, gpk::JSON_TYPE_BOOLEAN, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case 'n': GPK_JSON_EXPECTS_SEPARATOR(); if(-1 == stateReader.IndexCurrentElement) if_fail_vef(-1, ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar), "Failed to open element at index %" GPK_FMT_S2 ".", tokens.size()); errVal = ::jsonParseKeyword(gpk::VCC_NULL	, gpk::JSON_TYPE_NULL, stateReader, tokens, jsonAsString); ::jsonTestAndCloseValue(stateReader, tokens); break;
	case '0': case '1'	: case '2'	: case '3'	: case '4'	: case '5'	: case '6'	: case '7'	: case '8'	: case '9'	:
	case '.': case '-'	: case '+'	: //case 'x'	: // parse int or f2_t accordingly
		GPK_JSON_EXPECTS_SEPARATOR();
		if(-1 == stateReader.IndexCurrentElement)
			if_fail_vef(-1, ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar), "Failed to open element at index %" GPK_FMT_S2 ".", tokens.size());
		seterr_break_if(stateReader.Escaping, "Invalid character found at index %" GPK_FMT_U2 ": %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);	// Set an error or something and skip this character.
		seterr_break_if(gpk::JSON_TYPE_VALUE != stateReader.CurrentElement->Type, "%s", "Number found outside a value.");
		seterr_break_if_failed(::parseJsonNumber(stateReader, tokens, jsonAsString), "%s", "Error parsing JSON number.");
		::jsonTestAndCloseValue(stateReader, tokens);
		break;
	case ',':
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator, "Separator found when not expected at index %" GPK_FMT_S2 ".", stateReader.IndexCurrentChar);
		if(gpk::JSON_TYPE_OBJECT == stateReader.CurrentElement->Type)
			errVal	= ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_KEY, stateReader.IndexCurrentChar + 1);
		else if(gpk::JSON_TYPE_ARRAY == stateReader.CurrentElement->Type) // Test
			errVal	= ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1);
		stateReader.ExpectingSeparator	= false;
		break;
	case ':':
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator && gpk::JSON_TYPE_KEY != stateReader.CurrentElement->Type, "Separator found when not expected at index %" GPK_FMT_S2 ".", stateReader.IndexCurrentChar); // Test if we should be expecting this separator or not.
		stateReader.ExpectingSeparator	= false;
		seterr_break_if_failed(::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar - 1, gpk::JSON_TYPE_KEY) , "Failed to close key. %s", "Unknown error.");	// close the key before the : character
		seterr_break_if_failed(::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1), "Failed to open element at index %" GPK_FMT_S2 ".", tokens.size()); // open the value after the : character
		break;
	case ']': test_first_position(); errVal = ::jsonCloseContainer(stateReader, tokens, gpk::JSON_TYPE_ARRAY); break;
	case '}': test_first_position(); seterr_break_if(gpk::JSON_TYPE_KEY == stateReader.CurrentElement->Type && tokens.size() - 1 != (u2_t)stateReader.IndexCurrentElement, "Invalid format: %s", "Keys cannot be left without a value."); errVal = ::jsonCloseContainer(stateReader, tokens, gpk::JSON_TYPE_OBJECT); break;
	case '{': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_OBJECT	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_KEY	, stateReader.IndexCurrentChar + 1); } break;
	case '[': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_ARRAY	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE	, stateReader.IndexCurrentChar + 1); } break;
	case '"': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_STRING	, stateReader.IndexCurrentChar + 1);	// skip the " character in order to set the begin the string
		stateReader.InsideString	= true;
		break;
	}
	stateReader.Escaping	= false;
	rtrn errVal;
}

gpk::err_t			gpk::jsonParseStep			(gpk::SJSONReader & reader, gpk::vcsc_c & jsonAsString)	{
	reader.StateRead.CharCurrent	= jsonAsString[reader.StateRead.IndexCurrentChar];
	gpk::err_t				errVal						= (reader.StateRead.InsideString)
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Token, jsonAsString)
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Token, jsonAsString)
		;
	if_failed(errVal) {
		const b8_t					validElement				= (u2_t)reader.StateRead.IndexCurrentElement < reader.Token.size();
		cnst gpk::SJSONToken		* currentElement			= validElement ? &reader.Token[reader.StateRead.IndexCurrentElement] : 0;
		json_error_printf("Error during read step. Malformed JSON?"
			"\nPosition  : %" GPK_FMT_S2 "."
			"\nCharacter : '%c' (0x%x)."
			"\nElement   : %" GPK_FMT_S2 "."
			"\nString    : %s."
			"\nEscaping  : %s."
			"\nNestLevel : %" GPK_FMT_S2 "."
			"\nParent    : %" GPK_FMT_S2 "."
			"\nType      : %" GPK_FMT_S2 " (%s)."
			"\nOffset    : %" GPK_FMT_S2 "."
			, reader.StateRead.IndexCurrentChar
			, reader.StateRead.CharCurrent, reader.StateRead.CharCurrent
			, reader.StateRead.IndexCurrentElement
			, reader.StateRead.InsideString ? "true" : "false"
			, reader.StateRead.Escaping		? "true" : "false"
			, reader.StateRead.NestLevel
			, validElement ? currentElement->ParentIndex					: -1
			, validElement ? currentElement->Type							: -1
			, validElement ? gpk::get_value_namep(currentElement->Type)	: "N/A"
			, validElement ? currentElement->Span.Begin						: -1
		);
	}
	rtrn errVal;
}

gpk::err_t			gpk::jsonTreeRebuild		(gpk::view<gpk::SJSONToken>& in_object, gpk::apobj<gpk::SJSONNode> & out_nodes)								{
	gpk::apobj<gpk::SJSONNode>	& tree					= out_nodes;
	gpk_necs(tree.resize(in_object.size()));

	// -- Build all nodes linearly, without assigning the children
	for(u2_t iObject = 0; iObject < tree.size(); ++iObject) {
		gpk::pobj<gpk::SJSONNode>	& nodeCurrent		= tree[iObject];
		if(!nodeCurrent) {
			rees_if(0 == nodeCurrent.create());
		}
		nodeCurrent->Token			= &in_object[iObject];
		nodeCurrent->Parent			= ((u2_t)nodeCurrent->Token->ParentIndex < tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Token->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex	= iObject;
		nodeCurrent->Children.clear();
		if(nodeCurrent->Parent)
			gpk_necs(nodeCurrent->Parent->Children.push_back(nodeCurrent));
	}

	// -- Remove the key/value wrappers from objects.
	for(u2_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		gpk::pobj<gpk::SJSONNode>				& nodeCurrent										= tree[iObject];
		rees_if(!nodeCurrent);
		if( gpk::JSON_TYPE_ARRAY	!= nodeCurrent->Token->Type
		 && gpk::JSON_TYPE_OBJECT	!= nodeCurrent->Token->Type
		)
			continue;
		gpk::apobj<gpk::SJSONNode>	newChildren;
		gpk_necs(newChildren.resize(nodeCurrent->Children.size()));
		for(u2_t iChild = 0, countChild = newChildren.size(); iChild < countChild; ++iChild)
			newChildren[iChild]						= nodeCurrent->Children[iChild]->Children[0];
		nodeCurrent->Children					= newChildren;
	}
	rtrn 0;
}

#define json_bi_if(condition, format, ...) if(condition) break; //

gpk::err_t			gpk::jsonParse				(gpk::SJSONReader & reader, gpk::vcsc_c & jsonAsString, bool buildTree, bool buildViews)	{
	gpk::SJSONReaderState		& stateReader			= reader.StateRead;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < jsonAsString.size(); ++stateReader.IndexCurrentChar) {
		gpk_necs(gpk::jsonParseStep(reader, jsonAsString));
		json_bi_if(reader.StateRead.DoneReading, "%" GPK_FMT_S2 " json characters read.", stateReader.IndexCurrentChar + 1);
	}
	ree_if(stateReader.NestLevel, "Nest level: %" GPK_FMT_S2 " (Needs to be zero).", stateReader.NestLevel);
	if(false == buildViews) {
		gpk_necs(reader.View.resize(1));
		reader.View[0] = jsonAsString;
	}
	else {
		gpk_necs(reader.View.resize(reader.Token.size()));
		for(u2_t iView = 0; iView < reader.View.size(); ++iView) {
			cnst gpk::SJSONToken		& currentElement									= reader.Token[iView];
			gpk_necs(jsonAsString.slice(reader.View[iView], currentElement.Span.Begin, currentElement.Span.End - currentElement.Span.Begin));
		}
	}	
	rtrn buildTree ? gpk::jsonTreeRebuild(reader.Token, reader.Tree) : 0;
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, cnst gpk::view<vcsc_t> & views, gpk::as2_t & indices, gpk::avcsc_t & keys)	{
	ree_if(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, gpk::get_value_label(node_object.Token->Type).begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node					= node_object.Children[iNode];
		ree_if(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, gpk::get_value_label(node->Token->Type).begin(), gpk::JSON_TYPE_STRING);
		gpk::vcsc_c			& view					= views[node->ObjectIndex];
		gpk_necs(indices.push_back(node->ObjectIndex));
		gpk_necs(keys	.push_back(view));
	}
	rtrn indices.size();
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, gpk::as2_t & indices)	{
	ree_if(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, gpk::get_value_label(node_object.Token->Type).begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, gpk::get_value_label(node->Token->Type).begin(), gpk::JSON_TYPE_STRING);
		gpk_necs(indices.push_back(node->ObjectIndex));
	}
	rtrn indices.size();
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, cnst gpk::view<vcsc_t> & views, gpk::avcsc_t & keys)	{
	ree_if(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%s). Only objects are allowed to be accessed by key.", node_object.Token->Type, gpk::get_value_label(node_object.Token->Type).begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, gpk::get_value_label(node->Token->Type).begin(), gpk::JSON_TYPE_STRING);
		gpk::vcsc_c			& view						= views[node->ObjectIndex];
		gpk_necs(keys.push_back(view));
	}
	rtrn keys.size();
}

gpk::err_t			gpk::jsonObjectValueGet		(cnst gpk::SJSONNode & node_object, cnst gpk::view<vcsc_t> & views, cnst gpk::vcst_t & key)	{
	ree_if(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%s). Only objects are allowed to be accessed by key (\"%s\").", node_object.Token->Type, gpk::get_value_label(node_object.Token->Type).begin(), key.begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		ree_if(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, gpk::get_value_label(node->Token->Type).begin(), gpk::JSON_TYPE_STRING);

		gpk::vcsc_c			& view						= views[node->ObjectIndex];
		if(key == view)
			rtrn (gpk::err_t)node->ObjectIndex + 2; // one for value and other for the actual element
	}
	rtrn -1;
}

gpk::err_t			gpk::jsonArrayValueGet		(cnst gpk::SJSONNode & tree, u2_t index)				{
	ree_if(gpk::JSON_TYPE_ARRAY != tree.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%s). Only arrays are allowed to be accessed by index.", tree.Token->Type, gpk::get_value_label(tree.Token->Type).begin());
	ree_if(index >= tree.Children.size(), "Index out of range: %" GPK_FMT_S2 ". Max index: %" GPK_FMT_S2 ".", index, tree.Children.size() - 1);
	cnst gpk::SJSONNode		* node						= tree.Children[index];	// Get the
	ree_if(0 == node, "Invalid or corrupt tree. %s", "Nodes cannot be null.");
	rtrn node->ObjectIndex;
}

gpk::err_t			gpk::jsonCompareArray		(cnst gpk::SJSONNode & node, cnst gpk::view<vcsc_t> & views, cnst gpk::SJSONNode & other, cnst gpk::view<vcsc_t> & otherViews) {
	if_true_vif(0, node.Children.size() != other.Children.size(), GPK_FMT_NE_U2, node.Children.size(), other.Children.size())
	if_zero_vi(1, node.Children.size());
	for(u2_t iChild = 0; iChild < node.Children.size(); ++iChild) {
		cnst gpk::SJSONNode		& childNode					= *node	.Children[iChild];
		cnst gpk::SJSONNode		& childOther				= *other.Children[iChild];
		cnst gpk::JSON_TYPE		typeNode					= childNode	.Token->Type;
		cnst gpk::JSON_TYPE		typeOther					= childOther.Token->Type;
		if_true_vif(0, typeNode != typeOther, "%s != %s", get_value_namep(typeNode), get_value_namep(typeOther));
		gpk::err_t				result						= -1;
		switch(typeNode) {
		case gpk::JSON_TYPE_ARRAY	: result = gpk::jsonCompareArray	(childNode, views, childOther, otherViews); break;
		case gpk::JSON_TYPE_OBJECT	: result = gpk::jsonCompareObject	(childNode, views, childOther, otherViews); break;
		case gpk::JSON_TYPE_DECIMAL	:
		case gpk::JSON_TYPE_INTEGER	: result = gpk::jsonCompareNumber	(childNode, views, childOther, otherViews); break;
		default:
			result					= views[node.ObjectIndex] == otherViews[other.ObjectIndex];
		}
		if_zero_vi(0, result);
	}
	if_true_vi(1, views[node.ObjectIndex] == otherViews[other.ObjectIndex]);
	rtrn 1;
}

// Get the codepoint from characters of an unicode escape sequence
//	For input "\u c1 c2 c3 c4" the codepoint is:
//		(c1 * 0x1000)	+ (c2 * 0x0100)	+ (c3 * 0x0010) + c4
// aka:	(c1 << 12)		+ (c2 << 8)		+ (c3 << 4)		+ (c4 << 0)
//
//	Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f' must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp.
//	The conversion is done by subtracting the offset (0x30, 0x37, and 0x57) between the ASCII value of the character and the desired integer value.
//	Returns codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or non-hex character)
sttc	gpk::err_t	decodeUnicodeEscapeSequence	(gpk::vcsc_t input, u2_t & ret_unicode)		{
	ree_if(input.size() < 4, "Invalid escape sequence: %s.", input.begin());
	rtrn gpk::parseIntegerHexadecimal(input, ret_unicode);
}

// this function only makes sense after reading the first `\u`
gpk::err_t			jsonToCodePoint				(gpk::vcsc_t input, u2_t & unicode)			{
	gpk_necall(::decodeUnicodeEscapeSequence(input, unicode), "Invalid escape sequence: %s.", gpk::toString(input).begin());
	if (unicode < 0xD800 || unicode > 0xDBFF)
		rtrn 0;
	// surrogate pairs
	ree_if(input.size() < 7, "%s", "expecting a \\u token for the second half of the surrogate pair");
	ree_if(input[4] != '\\' || input[5] == 'u', "%s", "expecting a \\u token for the second half of the surrogate pair");
	u2_t						surrogatePair;
	gpk_necall(::decodeUnicodeEscapeSequence({&input[6], 4U}, surrogatePair), "Invalid escape sequence: %s.", gpk::toString(input).begin());
	unicode														= 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
	rtrn 0;
}
tplTstin	gpk::err_t	jsonObjectGetInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, T & value)	{ value = (T)reader.Token[iNode].Value; rtrn iNode; }
tplTstin	gpk::err_t	jsonObjectGetDecimal	(cnst gpk::SJSONReader & reader, u2_t iNode, T & value)	{ f3_t dealiased; memcpy(&dealiased, &reader[iNode]->Token->Value, szof(f3_t)); value = (T)dealiased; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetCString	(cnst SJSONReader & reader, u2_t iNode, vcsc_t	& value)	{ value = reader.View[iNode]; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetString	(cnst SJSONReader & reader, u2_t iNode, vcsc_t	& value)	{ rtrn jsonObjectGetCString(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, u0_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, u1_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, u2_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, u3_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, s0_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, s1_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, s2_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetInteger	(cnst SJSONReader & reader, u2_t iNode, s3_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetBoolean	(cnst SJSONReader & reader, u2_t iNode, b8_t	& value)	{ rtrn ::jsonObjectGetInteger(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetDecimal	(cnst SJSONReader & reader, u2_t iNode, f3_t	& value)	{ rtrn ::jsonObjectGetDecimal(reader, iNode, value); }
gpk::err_t	gpk::jsonObjectGetDecimal	(cnst SJSONReader & reader, u2_t iNode, f2_t	& value)	{ rtrn ::jsonObjectGetDecimal(reader, iNode, value); }

// TODO: Finish
gpk::err_t	gpk::jsonObjectGetIntegerAsBoolean(cnst SJSONReader & reader, u2_t iNode, b8_t & value)	{ s3_t	integer; jsonObjectGetInteger(reader, iNode, integer); value = 0 != integer; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetIntegerAsDecimal(cnst SJSONReader & reader, u2_t iNode, f3_t & value)	{ s3_t	integer; jsonObjectGetInteger(reader, iNode, integer); value = f3_t(integer); rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetDecimalAsInteger(cnst SJSONReader & reader, u2_t iNode, s3_t & value)	{ f3_t	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = s3_t (decimal); rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetDecimalAsBoolean(cnst SJSONReader & reader, u2_t iNode, b8_t & value)	{ f3_t	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = 0 != decimal; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetBooleanAsInteger(cnst SJSONReader & reader, u2_t iNode, s3_t & value)	{ b8_t	boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = one_if(boolean); rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetBooleanAsDecimal(cnst SJSONReader & reader, u2_t iNode, f3_t & value)	{ b8_t	boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = one_if(boolean); rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetIntegerAsString (cnst SJSONReader & reader, u2_t iNode, vcsc_t	& value)	{ s3_t	integer; jsonObjectGetInteger(reader, iNode, integer); sc_t tmp[64]; sprintf_s(tmp, "%lli", integer); value = gpk::label(tmp); rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetDecimalAsString (cnst SJSONReader & reader, u2_t iNode, vcsc_t	& value)	{ f3_t	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = decimal; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetBooleanAsString (cnst SJSONReader & reader, u2_t iNode, vcsc_t	& value)	{ b8_t		boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = boolean; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsInteger (cnst SJSONReader & reader, u2_t iNode, i3_t		& value)	{ vcsc_t	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(string, value); rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsBoolean (cnst SJSONReader & reader, u2_t iNode, b8_t		& value)	{ vcsc_t	string ; jsonObjectGetString (reader, iNode, string ); value = string.size() ? vcc2bool(string) : false; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsDecimal (cnst SJSONReader & reader, u2_t iNode, f3_t		& value)	{ vcsc_t	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(value = string ; rtrn iNode; }
// 
//gpk::err_t	gpk::jsonObjectGetAsString	(cnst gpk::SJSONReader & reader, u2_t iNode, vcsc_t		& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_STRING ) rtrn jsonObjectGetString (reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_BOOLEAN) { b8_t boolean = false; gpk_necs(jsonObjectGetBoolean(reader, iNode, boolean)); gpk::bool2char(boolean, value); } rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsBoolean	(cnst gpk::SJSONReader & reader, u2_t iNode, b8_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_BOOLEAN) rtrn jsonObjectGetBoolean(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_STRING ) { vcsc_t boolean = {}; gpk_necs(jsonObjectGetString(reader, iNode, boolean)); value = gpk::vcc2bool(boolean); }; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, s3_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, i32_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, i16_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, i8_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, u64_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, u32_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, u16_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, u8_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) rtrn jsonObjectGetInteger(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst gpk::SJSONReader & reader, u2_t iNode, f3_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) rtrn jsonObjectGetDecimal(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) {}; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst gpk::SJSONReader & reader, u2_t iNode, f2_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_DECIMAL) rtrn jsonObjectGetDecimal(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_INTEGER) {}; rtrn iNode; }

#define json_rew_if_failed(condition, format, ...) rews_if_failed(condition) /// rew_if_failed

gpk::err_t	gpk::jsonObjectGetAsString	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, vcsc_t	& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetString (reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsBoolean	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, b8_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetBoolean(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, u0_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, u1_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, u2_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, u3_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, s0_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, s1_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, s2_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, s3_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, f2_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetDecimal(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_c & key, f3_t		& value)	{ s2_t index; json_rew_if_failed(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%s', node: '%s'.", iNode, toString(key).begin(), toString(reader.View[iNode]).begin()); rtrn jsonObjectGetDecimal(reader, index, value); } 
