#ifndef GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#	define GPK_DISABLE_DEBUG_BREAK_ON_ERROR_LOG
#endif

#include "gpk_json.h"

#include "gpk_file.h"
#include "gpk_parse.h"
#include "gpk_math.h"
#include "gpk_cstring.h"

GPK_USING_TYPEINT();

#define json_info_printf(...) // info_printf
#define json_error_printf error_printf

gpk::err_t			gpk::jsonCompareObject		(cnst gpk::SJSONNode & node, gpk::view<cnst vcst_t> views, cnst gpk::SJSONNode & other, gpk::view<cnst vcst_t> otherViews) { if(node.Children.size() != other.Children.size()) rtrn 0; if(node.Children.size() == 0) rtrn 1; for(u2_t iChild = 0; iChild < node.Children.size(); iChild += 2)	if(node.Token->Type != other.Token->Type) rtrn 0; rtrn (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }
gpk::err_t			gpk::jsonCompareNumber		(cnst gpk::SJSONNode & node, gpk::view<cnst vcst_t> views, cnst gpk::SJSONNode & other, gpk::view<cnst vcst_t> otherViews) { if(node.Children.size() != other.Children.size()) rtrn 0; if(node.Children.size() == 0) rtrn 1; if(node.Token->Type != other.Token->Type) rtrn 0; rtrn (views[node.ObjectIndex] == otherViews[other.ObjectIndex]) ? 1 : 0; }

gpk::err_t			gpk::jsonMapToFields
(	gpk::as2_t											& indicesOfFields
,	gpk::view<cnst gpk::SJSONFieldBinding>	fields
,	gpk::view<cnst gpk::keyval<vcst_t>>	fieldMaps
) {
	for(u2_t iMap = 0; iMap < fieldMaps.size(); ++iMap)
	for(u2_t iField = 0; iField < fields.size(); ++iField) {
		cnst gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
		if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
			if_fail_fe(indicesOfFields.push_back(iField));
			break;
		}
	}
	rtrn indicesOfFields.size();
}

gpk::err_t			gpk::jsonFieldsToMap
(	gpk::as2_t											& indicesOfMaps
,	gpk::view<cnst gpk::SJSONFieldBinding>	fields
,	gpk::view<cnst gpk::keyval<vcst_t>>	fieldMaps
) {
	u2_t					countFields					= fields.size();
	for(u2_t iField = 0; iField < countFields; ++iField) {
		b8_t						bNotAdded					= true;
		for(u2_t iMap = 0; iMap < fieldMaps.size(); ++iMap) {
			cnst gpk::SJSONFieldBinding	& fieldToAdd			= fields[iField];
			if(fieldToAdd.Field.size() && fieldToAdd.Field == fieldMaps[iMap].Key) {
				if_fail_fe(indicesOfMaps.push_back(iMap));
				bNotAdded				= false;
				break;
			}
		}
		if(bNotAdded)
			if_fail_fe(indicesOfMaps.push_back(-1));
	}
	rtrn indicesOfMaps.size();
}

gpk::err_t			gpk::jsonFileRead			(gpk::SJSONFile & file, gpk::vcst_t filename) {
	json_info_printf("Loading json file: %.*s.", (int)filename.size(), filename.begin());
	if_fail_fef(gpk::fileToMemory(filename, file.Bytes), "Failed to load file: '%.*s'", (int)filename.size(), filename.begin());
	rtrn gpk::jsonParse(file.Reader, file.Bytes);
}

gpk::err_t			gpk::jsonArraySplit			(cnst gpk::SJSONNode & jsonArrayToSplit, gpk::view<cnst vcst_t> jsonViews, u2_c blockSize, gpk::aobj<gpk::string> & outputJsons)		{
	u2_c				remainder					= jsonArrayToSplit.Children.size() % blockSize;
	u2_c				countParts					= jsonArrayToSplit.Children.size() / blockSize + one_if(remainder);
	if_fail_fe(outputJsons.resize(countParts));
	u2_t					iSourceRecord						= 0;
	for(u2_t iPart = 0; iPart < outputJsons.size(); ++iPart) {
		gpk::string			& outputJson				= outputJsons[iPart];
		if_fail_fe(outputJson.push_back('['));
		for(u2_t iPartRecord = 0, countPartRecords = (remainder && iPart == countParts - 1) ? remainder : blockSize
			; iPartRecord < countPartRecords
			; ++iPartRecord) {
			if_fail_fe(gpk::jsonWrite(jsonArrayToSplit.Children[iSourceRecord++], jsonViews, outputJson));;
			if(iPartRecord < countPartRecords - 1)
				if_fail_fe(outputJson.push_back(','));
		}
		if_fail_fe(outputJson.push_back(']'));
	}
	rtrn 0;
}

gpk::err_t			gpk::jsonWrite				(cnst gpk::SJSONNode* node, gpk::view<cnst vcst_t> jsonViews, gpk::string & output)			{
	if(node->Token->Type == gpk::JSON_TYPE_VALUE && node->Children.size())
		node				= node->Children[0];
	switch(node->Token->Type) {
	case gpk::JSON_TYPE_INTEGER		: {
		sc_t					temp[64]					= {};
		if_fail_fe(::gpk::sprintf_s(temp, "%" GPK_FMT_S3, (s3_t)node->Token->Value));
		if_fail_fe(output.append_string(temp));
	}
		break;
	case gpk::JSON_TYPE_DECIMAL		: {
		f3_t					f							= 0;
		memcpy(&f, &node->Token->Value, szof(f3_t));
		sc_t					temp[64]					= {};
		err_t					lenNum						= ::gpk::sprintf_s(temp, "%.16f", f);
		if_fail_fe(lenNum);
		if_true_fef(lenNum >= (err_t)::gpk::size(temp), "JSON decimal exceeds the formatting buffer: %.16f.", f);
		while(lenNum > 0 && (0 == temp[lenNum] || '0' == temp[lenNum]))
			temp[lenNum--]					= 0;
		if_fail_fe(output.append_string(temp));
	}
		break;
	case gpk::JSON_TYPE_BOOLEAN			:
		if_fail_fe(output.append(b82st(node->Token->Value)));
		break;
	case gpk::JSON_TYPE_NULL			:
		if_fail_fe(output.append(jsonViews[node->ObjectIndex]));
		break;
	case gpk::JSON_TYPE_STRING		:
		if_fail_fe(output.push_back('"'));
		if_fail_fe(output.append(jsonViews[node->ObjectIndex]));
		if_fail_fe(output.push_back('"'));
		break;
	case gpk::JSON_TYPE_OBJECT		:
		if_fail_fe(output.push_back('{'));
		for(u2_t iChildren = 0; iChildren < node->Children.size(); iChildren += 2) {
			if_fail_fe(gpk::jsonWrite(node->Children[iChildren + 0], jsonViews, output));;
			if_fail_fe(output.push_back(':'));
			if_fail_fe(gpk::jsonWrite(node->Children[iChildren + 1], jsonViews, output));;
			if(iChildren < node->Children.size() - 2)
				if_fail_fe(output.push_back(','));
		}
		if_fail_fe(output.push_back('}'));
		break;
	case gpk::JSON_TYPE_ARRAY			:
		if_fail_fe(output.push_back('['));
		for(u2_t iChildren = 0; iChildren < node->Children.size(); ++iChildren) {
			if_fail_fe(gpk::jsonWrite(node->Children[iChildren], jsonViews, output));;
			if(iChildren < node->Children.size() - 1)
				if_fail_fe(output.push_back(','));
		}
		if_fail_fe(output.push_back(']'));
	}
	rtrn 0;
}

sttc	gpk::err_t	jsonCloseElement			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, u2_t indexChar) {
	if_true_fef(tokens.size() <= (u2_t)stateReader.IndexCurrentElement, "Invalid parser state. Cannot close element: %" GPK_FMT_S2 ".", stateReader.IndexCurrentElement);
	gpk::SJSONToken			* closing					= 0;
	closing					= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	closing->Span.End		= (u2_t)indexChar + 1;
	gpk::vcst_t			labelType				= gpk::get_value_label(closing->Type);
	json_info_printf("%.*s closed. Index %.2i. Level: %" GPK_FMT_S2 ". Parent index: %" GPK_FMT_S2 ". Node type: %" GPK_FMT_S2 ". Begin: %" GPK_FMT_S2 ". End: %" GPK_FMT_S2 ".", (int)labelType.size(), labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, closing->ParentIndex, closing->Type, closing->Span.Begin, closing->Span.End);
	stateReader.IndexCurrentElement					= closing->ParentIndex;
	--stateReader.NestLevel;
	stateReader.CurrentElement						= ((u2_t)stateReader.IndexCurrentElement < tokens.size()) ? &tokens[stateReader.IndexCurrentElement] : 0;
	if(0 == stateReader.NestLevel)
		stateReader.DoneReading							= true;
	rtrn 0;	// Need to report that a list has been exited
}

sttc	gpk::err_t	jsonCloseElement			(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & object, u2_t indexChar, gpk::JSON_TYPE jsonType) {
	cnst gpk::SJSONToken		* open						= stateReader.CurrentElement; //&object[stateReader.IndexCurrentElement];
	cnst gpk::vcst_t			labelOpen					= gpk::get_value_label(open->Type);
	cnst gpk::vcst_t			labelClose				= gpk::get_value_label(jsonType);
	if_true_fef(jsonType != open->Type, "Invalid object type: open: %" GPK_FMT_U2 " (%.*s). closing: %" GPK_FMT_U2 " (%.*s).", open->Type, (int)labelOpen.size(), labelOpen.begin(), jsonType, (int)labelClose.size(), labelClose.begin());
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
	cnst gpk::vcst_t			labelType				= gpk::get_value_label(containerType);
	if(tokens[tokens.size() - 1].Type == containerType) {
		json_info_printf("Discarding empty container element at index %" GPK_FMT_S2 " (%.*s). Level: %" GPK_FMT_S2 "", tokens.size() - 1, (int)labelType.size(), labelType.begin(), stateReader.NestLevel);
		stateReader.IndexCurrentElement	= tokens[tokens.size() - 1].ParentIndex;
		if_fail_fe(tokens.pop_back());
		--stateReader.NestLevel;
		if((u2_t)stateReader.IndexCurrentElement < tokens.size())
			stateReader.CurrentElement	= &tokens[stateReader.IndexCurrentElement];
	}
	else {
		json_info_printf("Closing container at index %" GPK_FMT_S2 " (%.*s).", tokens.size() - 1, (int)labelType.size(), labelType.begin());
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

sttc	gpk::err_t	jsonParseStringCharacter	(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcst_t jsonAsString)	{
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
		seterr_break_if(jsonAsString.size() - stateReader.IndexCurrentChar < 4, "End of stream during unicode code point parsing. JSON length: %.*s. Current index: %" GPK_FMT_U2 ".", (int)(jsonAsString.size() - stateReader.IndexCurrentChar), &jsonAsString[stateReader.IndexCurrentChar], stateReader.IndexCurrentChar);
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

sttc	gpk::err_t	jsonParseKeyword			(gpk::vcst_t token, gpk::JSON_TYPE jsonType, gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & object, gpk::vcst_t jsonAsString)	{
	if_true_fef(token.size() > jsonAsString.size() - stateReader.IndexCurrentChar, "End of stream while parsing token: %.*s.", int(token.size()), token.begin());
	if_true_fef(0 != strncmp(token.begin(), &jsonAsString[stateReader.IndexCurrentChar], token.size()), "Unrecognized token found while looking for '%.*s'.", int(token.size()),	 token.begin());
	json_info_printf("JSON token found: %.*s.", int(token.size()), token.begin());

	gpk::SJSONToken			boolElement					= {stateReader.IndexCurrentElement, jsonType, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + token.size()}, gpk::cst2b8(token)};
	if_fail_fef(object.push_back(boolElement), "Failed to push! Out of memory? object count: %" GPK_FMT_U2 ".", object.size());
	stateReader.IndexCurrentChar	+= token.size() - 1;
	stateReader.CurrentElement		= &object[stateReader.IndexCurrentElement];
	rtrn 0;
}

sttc	gpk::err_t	lengthJsonNumber			(u2_t indexCurrentChar, gpk::vcst_t jsonAsString)	{
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

//sttc	gpk::err_t	parseJsonAbsolutePart		(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, vcst_t  absolutePart, gpk::SJSONToken & output, u2_c sizeNum, const b8_t isFloat, u2_c index, const s2_t indexCurrentElement, const s2_t indexCurrentChar)	{
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
//				if_fail_fe(absolutePart.slice(absolutePart, offsetStart + 1, lenDec - 1));
//			else
//				if_fail_fe(absolutePart.slice(absolutePart, offsetStart, lenDec));
//			cnst gpk::err_t		decCount					= gpk::parseIntegerDecimal(absolutePart, decValue);
//			if_failed_ve(decCount);
//			decValue				/= gpk::powui(10, decCount);
//			json_info_printf("Decimal part: %f.", decValue);
//			finalValue				+= decValue;
//		}
//		memcpy((f3_t*)&output.Value, &finalValue, szof(u3_t));
//	}
//	else {
//		json_info_printf("Integer part: %" GPK_FMT_S2 ".", output.Value);
//	}
//	rtrn 0;
//}

sttc	gpk::err_t	parseSign					(gpk::vcst_t strNumber, b8_t & isNegative, b8_t & isFloat)	{
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

sttc	gpk::err_t	parseJsonNumber				(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcst_t jsonAsString)	{
	u2_c					offset						= stateReader.IndexCurrentChar;
	sc_t					charCurrent					= jsonAsString[offset];
	b8_t					isNegative					= false;
	b8_t					isFloat						= false;
	s2_t					signLength					= parseSign({&jsonAsString[offset], jsonAsString.size() - offset}, isNegative, isFloat);
	u2_t					index						= offset + signLength;
	charCurrent			= jsonAsString[index];
	if_true_fef(index < jsonAsString.size() && (charCurrent != '0' && (charCurrent < '1' || charCurrent > '9')), "Character '%c' at index %" GPK_FMT_S2 " is not a number.", charCurrent, index);

	u2_c					sizeNum						= lengthJsonNumber(index, jsonAsString);
	gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, isFloat ? gpk::JSON_TYPE_DECIMAL : gpk::JSON_TYPE_INTEGER, {stateReader.IndexCurrentChar, stateReader.IndexCurrentChar + signLength + sizeNum}, {}};
	gpk::vcst_t 				numString					= {};
	if_fail_fe(jsonAsString.slice(numString, index, sizeNum));

	gpk::err_t				intCount					= gpk::parseIntegerDecimal(numString, currentElement.Value = 0);
	if(isFloat) { // The number begins with a dot and we already skipped it so calculate decimal places for the fractional part.
		f3_t						valuef64					= currentElement.Value / f3_t(gpk::powui(10, numString.size()));
		if(isNegative) {
			valuef64 	*= -1;
		}
		memcpy(&(currentElement.Value = 0), &valuef64, szof(u3_t));
		json_info_printf("Decimal read: %f.", valuef64);
	}
	else { // there is an integer part so we need to check for decimals
		if(intCount >= (s2_t)sizeNum) { // No more digits after the integer part. Leave it this way.
			if(isNegative)
				currentElement.Value	= 0ULL - currentElement.Value;
			json_info_printf("Integer read: %lli.", currentElement.Value);
		}
		else { 
			json_info_printf("Integer part: %lli.", currentElement.Value);
			currentElement.Type		= gpk::JSON_TYPE_DECIMAL;
			f3_t						finalValue					= (f3_t)currentElement.Value;
			++intCount;	// Skip dot.
			s2_t						lenDec						= sizeNum - intCount;
			if(lenDec > 0) { // there are leftover digits after the integer part
				u2_t					offsetStart					= currentElement.Span.Begin + sizeNum - (sizeNum - intCount);
				f3_t						decValue					= 0;
				if_fail_fe(jsonAsString.slice(numString, offsetStart, lenDec));
				gpk::err_t		decCount;
				if_fail_fe(decCount = gpk::parseIntegerDecimal(numString, decValue));
				decValue				/= gpk::powui(10, decCount);
				json_info_printf("Decimal part: %f.", decValue);
				finalValue				+= decValue;
			}
			if(isNegative)
				finalValue *= -1;
			memcpy(&currentElement.Value, &finalValue, szof(u3_t));
		}
	}
	if_fail_fe(tokens.push_back(currentElement));
	stateReader.CurrentElement						= &tokens[stateReader.IndexCurrentElement];
	json_info_printf("Number found: %.*s. Length: %" GPK_FMT_U2 ". Negative: %s. Float: %s."
		, sizeNum, &jsonAsString[index]
		, sizeNum
		, isNegative	? "true" : "false"
		, isFloat		? "true" : "false"
	);
	stateReader.IndexCurrentChar					+= sizeNum + (index - offset) - 1;
	if(stateReader.IndexCurrentChar + 1 < jsonAsString.size() - 1) {
		charCurrent										= jsonAsString[stateReader.IndexCurrentChar+1];
		if_true_fef(charCurrent != ' '
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
	cnst gpk::vcst_t			labelType				= gpk::get_value_label(containerType);
	if_fail_fef(::jsonCloseOrDiscardEmptyKOrV(stateReader, tokens, (gpk::JSON_TYPE_ARRAY == containerType) ? gpk::JSON_TYPE_VALUE : gpk::JSON_TYPE_KEY), "Failed to close container at index %" GPK_FMT_S2 " (%.*s).", stateReader.IndexCurrentElement, (int)labelType.size(), labelType.begin());
	errVal					= ::jsonCloseElement(stateReader, tokens, stateReader.IndexCurrentChar, containerType);
	(void)errVal;
	stateReader.ExpectingSeparator	= false;
	if(stateReader.NestLevel > 0) // Root container isn't inside a value. Every other container is.
		rtrn ::jsonTestAndCloseValue(stateReader, tokens);
	rtrn 0;
}

sttc	gpk::err_t	jsonOpenElement				(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::JSON_TYPE jsonType, u2_t indexChar) {
	gpk::SJSONToken			currentElement				= {stateReader.IndexCurrentElement, jsonType, {indexChar, indexChar}, {}};
	if_fail_fe(stateReader.IndexCurrentElement = tokens.push_back(currentElement));
	stateReader.CurrentElement	= &tokens[stateReader.IndexCurrentElement];
	gpk::vcst_t				labelType					= gpk::get_value_label(currentElement.Type);
	++stateReader.NestLevel;
	json_info_printf("%.*s open. Index %.2i. Level: %" GPK_FMT_S2 ". Parent index: %" GPK_FMT_S2 ". Node type: %" GPK_FMT_S2 ". Begin: %" GPK_FMT_S2 ".", (int)labelType.size(), labelType.begin(), stateReader.IndexCurrentElement, stateReader.NestLevel, currentElement.ParentIndex, currentElement.Type, currentElement.Span.Begin);
	rtrn 0;
}

sttc	gpk::err_t	jsonParseDocumentCharacter	(gpk::SJSONReaderState & stateReader, gpk::apod<gpk::SJSONToken> & tokens, gpk::vcst_t jsonAsString)	{
	gpk::err_t				errVal						= 0;
	gpk::err_t				nextMatch					= -1;
#define GPK_JSON_EXPECTS_SEPARATOR()		\
	if (stateReader.ExpectingSeparator) {	\
		errVal					= -1; 		\
		json_error_printf("Expected separator, found '%c'.", stateReader.CharCurrent); 	\
		break; 								\
	}

#define test_first_position()	if_true_fef(0 == stateReader.CharCurrent, "Character found at invalid position. Index: %" GPK_FMT_U2 ". Character: %c.", stateReader.IndexCurrentChar, stateReader.CharCurrent);

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
		if_fail_w(::jsonTestAndCloseValue(stateReader, tokens));
		break;
	case ',':
		test_first_position();
		seterr_break_if(false == stateReader.ExpectingSeparator, "Separator found when not expected at index %" GPK_FMT_S2 ".", stateReader.IndexCurrentChar);
		if(gpk::JSON_TYPE_OBJECT == stateReader.CurrentElement->Type) {
			if_fail_w(errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_KEY, stateReader.IndexCurrentChar + 1));
		}
		else if(gpk::JSON_TYPE_ARRAY == stateReader.CurrentElement->Type) {
			if_fail_w(errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE, stateReader.IndexCurrentChar + 1));
		}
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
	case '{': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_OBJECT	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_KEY		, stateReader.IndexCurrentChar + 1); } break;
	case '[': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_ARRAY	, stateReader.IndexCurrentChar); if(0 <= errVal) { errVal = jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_VALUE	, stateReader.IndexCurrentChar + 1); } break;
	case '"': GPK_JSON_EXPECTS_SEPARATOR(); errVal = ::jsonOpenElement(stateReader, tokens, gpk::JSON_TYPE_STRING	, stateReader.IndexCurrentChar + 1);	// skip the " character in order to set the begin the string
		stateReader.InsideString	= true;
		break;
	}
	stateReader.Escaping	= false;
	rtrn errVal;
}

gpk::err_t			gpk::jsonParseStep			(gpk::SJSONReader & reader, gpk::vcst_t jsonAsString, gpk::SJSONParseOptions options)	{
	(void)options;
	reader.StateRead.CharCurrent	= jsonAsString[reader.StateRead.IndexCurrentChar];
	if_true_block_logf(error_printf, gpk::failed(reader.StateRead.InsideString
		? ::jsonParseStringCharacter	(reader.StateRead, reader.Token, jsonAsString)
		: ::jsonParseDocumentCharacter	(reader.StateRead, reader.Token, jsonAsString)
		), {
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
			}, "InsideString:%s", reader.StateRead.InsideString ? "true" : "false"
		);
	rtrn 0;
}

gpk::err_t			gpk::jsonTreeRebuild		(gpk::view<cnst gpk::SJSONToken> in_object, gpk::apobj<gpk::SJSONNode> & out_nodes)								{
	gpk::apobj<gpk::SJSONNode>	& tree					= out_nodes;
	if_fail_fe(tree.resize(in_object.size()));

	// -- Build all nodes linearly, without assigning the children
	for(u2_t iObject = 0; iObject < tree.size(); ++iObject) {
		gpk::pobj<gpk::SJSONNode>	& nodeCurrent		= tree[iObject];
		if(!nodeCurrent) {
			if_null_fe(nodeCurrent.create());
		}
		nodeCurrent->Token			= &in_object[iObject];
		nodeCurrent->Parent			= ((u2_t)nodeCurrent->Token->ParentIndex < tree.size()) ? (gpk::SJSONNode*)tree[nodeCurrent->Token->ParentIndex] : nullptr;
		nodeCurrent->ObjectIndex	= iObject;
		nodeCurrent->Children.clear();
		if(nodeCurrent->Parent)
			if_fail_fe(nodeCurrent->Parent->Children.push_back(nodeCurrent));
	}

	// -- Remove the key/value wrappers from objects.
	for(u2_t iObject = 0, countNodes = tree.size(); iObject < countNodes; ++iObject) {
		gpk::pobj<gpk::SJSONNode>				& nodeCurrent										= tree[iObject];
		if_true_fe(!nodeCurrent);
		if( gpk::JSON_TYPE_ARRAY	!= nodeCurrent->Token->Type
		 && gpk::JSON_TYPE_OBJECT	!= nodeCurrent->Token->Type
		)
			continue;
		gpk::apobj<gpk::SJSONNode>	newChildren;
		if_fail_fe(newChildren.resize(nodeCurrent->Children.size()));
		for(u2_t iChild = 0, countChild = newChildren.size(); iChild < countChild; ++iChild)
			newChildren[iChild]						= nodeCurrent->Children[iChild]->Children[0];
		nodeCurrent->Children					= newChildren;
	}
	rtrn 0;
}

gpk::err_t			gpk::jsonTreeInvalidate	(gpk::SJSONReader & reader) {
	if_zero_fe(reader.Tree.size());
	if_null_fe(reader.Tree[0]);
	rtrn truncate<err_t>(reader.Tree[0]->Token = {});
}

gpk::err_t			gpk::jsonTreeEnsure		(gpk::SJSONReader & reader) {
	if_zero_fe(reader.Token.size());
	if_zero_fe(reader.Tree.size());
	if_null_fe(reader.Tree[0]);
	if(reader.Tree[0]->Token)
		rtrn 0;
	gpk::err_t result;
	if_fail_fe(result = gpk::jsonTreeRebuild(reader.Token, reader.Tree));
	rtrn result;
}

#define json_bi_if(condition, format, ...) if(condition) break; //

gpk::err_t			gpk::jsonParse				(gpk::SJSONReader & reader, gpk::vcst_t jsonAsString, gpk::SJSONParseOptions options)	{
	gpk::SJSONReaderState		& stateReader			= reader.StateRead;
	for(stateReader.IndexCurrentChar = 0; stateReader.IndexCurrentChar < jsonAsString.size(); ++stateReader.IndexCurrentChar) {
		if_fail_fe(gpk::jsonParseStep(reader, jsonAsString, options));
		json_bi_if(reader.StateRead.DoneReading, "%" GPK_FMT_S2 " json characters read.", stateReader.IndexCurrentChar + 1);
	}
	if_true_fef(stateReader.NestLevel, "Nest level: %" GPK_FMT_S2 " (Needs to be zero).", stateReader.NestLevel);
	if(false == options.All(gpk::JSON_PARSE_OPTION_BUILD_VIEWS)) {
		if_fail_fe(reader.View.resize(1));
		reader.View[0] = jsonAsString;
	}
	else {
		if_fail_fe(reader.View.resize(reader.Token.size()));
		for(u2_t iView = 0; iView < reader.View.size(); ++iView) {
			cnst gpk::SJSONToken		& currentElement									= reader.Token[iView];
			if_fail_fe(jsonAsString.slice(reader.View[iView], currentElement.Span.Begin, currentElement.Span.End - currentElement.Span.Begin));
		}
	}	
	rtrn options.All(gpk::JSON_PARSE_OPTION_BUILD_TREE) ? gpk::jsonTreeRebuild(reader.Token, reader.Tree) : 0;
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, gpk::view<cnst vcst_t> views, gpk::as2_t & indices, gpk::aobj<vcst_t> & keys)	{
	cnst gpk::vcst_t		objectTypeName			= gpk::get_value_label(node_object.Token->Type);
	if_true_fef(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%.*s). Only objects are allowed to be accessed by key.", node_object.Token->Type, (int)objectTypeName.size(), objectTypeName.begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node					= node_object.Children[iNode];
		cnst gpk::vcst_t		nodeTypeName				= gpk::get_value_label(node->Token->Type);
		if_true_fef(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%.*s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, (int)nodeTypeName.size(), nodeTypeName.begin(), gpk::JSON_TYPE_STRING);
		gpk::vcst_c			& view					= views[node->ObjectIndex];
		if_fail_fe(indices.push_back(node->ObjectIndex));
		if_fail_fe(keys	.push_back(view));
	}
	rtrn indices.size();
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, gpk::as2_t & indices)	{
	cnst gpk::vcst_t		objectTypeName			= gpk::get_value_label(node_object.Token->Type);
	if_true_fef(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%.*s). Only objects are allowed to be accessed by key.", node_object.Token->Type, (int)objectTypeName.size(), objectTypeName.begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		cnst gpk::vcst_t		nodeTypeName				= gpk::get_value_label(node->Token->Type);
		if_true_fef(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%.*s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, (int)nodeTypeName.size(), nodeTypeName.begin(), gpk::JSON_TYPE_STRING);
		if_fail_fe(indices.push_back(node->ObjectIndex));
	}
	rtrn indices.size();
}

gpk::err_t			gpk::jsonObjectKeyList		(cnst gpk::SJSONNode & node_object, gpk::view<cnst vcst_t> views, gpk::aobj<vcst_t> & keys)	{
	cnst gpk::vcst_t		objectTypeName			= gpk::get_value_label(node_object.Token->Type);
	if_true_fef(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%.*s). Only objects are allowed to be accessed by key.", node_object.Token->Type, (int)objectTypeName.size(), objectTypeName.begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		cnst gpk::vcst_t		nodeTypeName				= gpk::get_value_label(node->Token->Type);
		if_true_fef(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%.*s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, (int)nodeTypeName.size(), nodeTypeName.begin(), gpk::JSON_TYPE_STRING);
		gpk::vcst_c			& view						= views[node->ObjectIndex];
		if_fail_fe(keys.push_back(view));
	}
	rtrn keys.size();
}

gpk::err_t			gpk::jsonObjectValueGet		(cnst gpk::SJSONNode & node_object, gpk::view<cnst vcst_t> views, gpk::vcst_t key)	{
	cnst gpk::vcst_t		objectTypeName			= gpk::get_value_label(node_object.Token->Type);
	if_true_fef(gpk::JSON_TYPE_OBJECT != node_object.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%.*s). Only objects are allowed to be accessed by key (\"%.*s\").", node_object.Token->Type, (int)objectTypeName.size(), objectTypeName.begin(), (int)key.size(), key.begin());
	for(u2_t iNode = 0, countNodes = node_object.Children.size(); iNode < countNodes; iNode += 2) {
		cnst gpk::SJSONNode		* node						= node_object.Children[iNode];
		cnst gpk::vcst_t		nodeTypeName				= gpk::get_value_label(node->Token->Type);
		if_true_fef(gpk::JSON_TYPE_STRING != node->Token->Type, "Invalid node type: %" GPK_FMT_U2 " (%.*s). Only string types (%" GPK_FMT_U2 ") can be keys of JSON objects.", node->Token->Type, (int)nodeTypeName.size(), nodeTypeName.begin(), gpk::JSON_TYPE_STRING);

		gpk::vcst_c			& view						= views[node->ObjectIndex];
		if(key == view)
			rtrn (gpk::err_t)node->ObjectIndex + 2; // one for value and other for the actual element
	}
	rtrn -1;
}

gpk::err_t			gpk::jsonArrayValueGet		(cnst gpk::SJSONNode & tree, u2_t index)				{
	cnst gpk::vcst_t		treeTypeName				= gpk::get_value_label(tree.Token->Type);
	if_true_fef(gpk::JSON_TYPE_ARRAY != tree.Token->Type, "Invalid node type: %" GPK_FMT_S2 " (%.*s). Only arrays are allowed to be accessed by index.", tree.Token->Type, (int)treeTypeName.size(), treeTypeName.begin());
	if_true_fef(index >= tree.Children.size(), "Index out of range: %" GPK_FMT_S2 ". Max index: %" GPK_FMT_S2 ".", index, tree.Children.size() - 1);
	cnst gpk::SJSONNode		* node;;	// Get the
	if_null_fe(node = tree.Children[index]); // Nodes cannot be null, so check for it and log the error and propagate it.
	rtrn node->ObjectIndex;
}

gpk::err_t			gpk::jsonCompareArray		(cnst gpk::SJSONNode & node, gpk::view<cnst vcst_t> views, cnst gpk::SJSONNode & other, gpk::view<cnst vcst_t> otherViews) {
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
//	Decodes exactly four hexadecimal digits into ret_unicode. Returns 0 on success or -1 on error.
sttc	gpk::err_t	decodeUnicodeEscapeSequence	(gpk::vcst_t  input, u2_t & ret_unicode)		{
	if_true_fef(input.size() < 4, "Invalid escape sequence: %.*s.", int(input.size()), input.begin());
	cnst gpk::err_t			digitsDecoded				= gpk::parseIntegerHexadecimal({input.begin(), 4U}, ret_unicode);
	if_true_fef(4 != digitsDecoded, "Invalid escape sequence: %.*s.", 4, input.begin());
	rtrn 0;
}

// this function only makes sense after reading the first `\u`
gpk::err_t			gpk::jsonToCodePoint		(gpk::vcst_t  input, u2_t & unicode)			{
	if_fail_fef(::decodeUnicodeEscapeSequence(input, unicode), "Invalid escape sequence: %.*s.", int(input.size()), input.begin());
	if(unicode < 0xD800 || unicode > 0xDFFF)
		rtrn 0;
	if_true_fef(unicode > 0xDBFF, "Unexpected low surrogate: 0x%04X.", unicode);
	// surrogate pairs
	if_true_fef(input.size() < 10, "expecting a \\u token for the second half of the surrogate pair. input.size(): %u", input.size());
	if_true_fef(input[4] != '\\' || input[5] != 'u', "expecting a \\u token for the second half of the surrogate pair. input[4]: %c, input[5]: %c", input[4], input[5]);
	u2_t						surrogatePair;
	if_fail_fef(::decodeUnicodeEscapeSequence({&input[6], 4U}, surrogatePair), "Invalid escape sequence: %.*s.", int(input.size()), input.begin());
	if_true_fef(surrogatePair < 0xDC00 || surrogatePair > 0xDFFF, "Invalid low surrogate: 0x%04X.", surrogatePair);
	unicode														= 0x10000 + ((unicode & 0x3FF) << 10) + (surrogatePair & 0x3FF);
	rtrn 0;
}
tplTstin	gpk::err_t	jsonObjectGetInteger	(cnst gpk::SJSONReader & reader, u2_t iNode, T & value)	{ value = (T)reader.Token[iNode].Value; rtrn iNode; }
tplTstin	gpk::err_t	jsonObjectGetDecimal	(cnst gpk::SJSONReader & reader, u2_t iNode, T & value)	{ f3_t dealiased; memcpy(&dealiased, &reader[iNode]->Token->Value, szof(f3_t)); value = (T)dealiased; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetCString	(cnst SJSONReader & reader, u2_t iNode, vcst_t 	& value)	{ value = reader.View[iNode]; rtrn iNode; }
gpk::err_t	gpk::jsonObjectGetString	(cnst SJSONReader & reader, u2_t iNode, vcst_t 	& value)	{ rtrn jsonObjectGetCString(reader, iNode, value); }
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
//gpk::err_t	gpk::jsonObjectGetIntegerAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t 	& value)	{ s3_t	integer; jsonObjectGetInteger(reader, iNode, integer); sc_t tmp[64]; sprintf_s(tmp, "%lli", integer); value = gpk::label(tmp); rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetDecimalAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t 	& value)	{ f3_t	decimal; jsonObjectGetDecimal(reader, iNode, decimal); value = decimal; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetBooleanAsString (cnst SJSONReader & reader, u2_t iNode, vcst_t 	& value)	{ b8_t		boolean; jsonObjectGetBoolean(reader, iNode, boolean); value = boolean; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsInteger (cnst SJSONReader & reader, u2_t iNode, i3_t		& value)	{ vcst_t 	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(string, value); rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsBoolean (cnst SJSONReader & reader, u2_t iNode, b8_t		& value)	{ vcst_t 	string ; jsonObjectGetString (reader, iNode, string ); value = string.size() ? st2b8(string) : false; rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetStringAsDecimal (cnst SJSONReader & reader, u2_t iNode, f3_t		& value)	{ vcst_t 	string ; jsonObjectGetString (reader, iNode, string ); ::parseJsonNumber(value = string ; rtrn iNode; }
// 
//gpk::err_t	gpk::jsonObjectGetAsString	(cnst gpk::SJSONReader & reader, u2_t iNode, vcst_t 		& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_STRING ) rtrn jsonObjectGetString (reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_BOOLEAN) { b8_t boolean = false; if_fail_fe(jsonObjectGetBoolean(reader, iNode, boolean)); gpk::b82sc(boolean, value); } rtrn iNode; }
//gpk::err_t	gpk::jsonObjectGetAsBoolean	(cnst gpk::SJSONReader & reader, u2_t iNode, b8_t	& value)	{ if(reader.Token[iNode].Type == gpk::JSON_TYPE_BOOLEAN) rtrn jsonObjectGetBoolean(reader, iNode, value); else if(reader.Token[iNode].Type == gpk::JSON_TYPE_STRING ) { vcst_t  boolean = {}; if_fail_fe(jsonObjectGetString(reader, iNode, boolean)); value = gpk::st2b8(boolean); }; rtrn iNode; }
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

#define json_if_fail_fwf if_fail_fwf // if_fail_ve(-1, condition) /// rew_if_failed

gpk::err_t	gpk::jsonObjectGetAsString	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, vcst_t 	& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetString (reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsBoolean	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, b8_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetBoolean(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u0_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u1_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u2_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, u3_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s0_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s1_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s2_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsInteger	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, s3_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetInteger(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, f2_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetDecimal(reader, index, value); } 
gpk::err_t	gpk::jsonObjectGetAsDecimal	(cnst SJSONReader & reader, u2_t iNode, vcst_t key, f3_t		& value) { s2_t index; json_if_fail_fwf(index = jsonObjectValueGet(reader, iNode, key), "iNode: %" GPK_FMT_S2 ", key: '%.*s', node: '%.*s'.", iNode, (int)key.size(), key.begin(), (int)reader.View[iNode].size(), reader.View[iNode].begin()); rtrn jsonObjectGetDecimal(reader, index, value); } 
