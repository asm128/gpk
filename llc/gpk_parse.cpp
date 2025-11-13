#include "gpk_parse.h"
#include "gpk_array_pod.h"
#include "gpk_safe.h"
#include "gpk_math.h"
#include <ctype.h>

bool					gpk::isSpaceCharacter						(const char characterToTest)		{
	switch(characterToTest) {
	case ' ': case '\t': case '\r': case '\n'	: case '\f'	: case '\b'	:
		return true;
	default:
		return false;
	}
}

::gpk::error_t			gpk::skipToNextCharacter					(u2_t & indexCurrentChar, ::gpk::vcsc_c & expression)		{
	while(indexCurrentChar < expression.size()) {
		if(::gpk::isSpaceCharacter(expression[indexCurrentChar]))
			++indexCurrentChar;
		else
			break;
	}
	return 0;
}

::gpk::error_t			gpk::parseArbitraryBaseInteger				(u2_t base, ::gpk::vcsc_c & symbolList, ::gpk::vcsc_c & sourceChars, u3_t * number_)	{
	u2_t										totalCharsProcessed							= 0;
	::gpk::apod<char>								stringToParse								= {};
	for(u2_t iChar = 0; iChar < sourceChars.size() && 0 != sourceChars[iChar];) {
		const char										sourceChar									= (char)tolower(sourceChars[iChar]);
		bool											bSymbolProcessed							= false;
		for(u2_t iSymbol = 0; iSymbol < base; ++iSymbol) {
			if(symbolList[iSymbol] == sourceChar) {
				bSymbolProcessed							= true;
				gpk_necs(stringToParse.push_back(sourceChar));
				++iChar;
				++totalCharsProcessed;
				break;
			}
		}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	gpk_necs(gpk::reverse(stringToParse));		// we assigned the digits backwards so we need to reverse the string.
	u3_t										number										= 0;
	totalCharsProcessed							= 0;
	for(u2_t iChar = 0; iChar < stringToParse.size() && 0 != stringToParse[iChar];) {
		bool											bSymbolProcessed							= false;
		for( u2_t iSymbol = 0; iSymbol < base; ++iSymbol )
			if(symbolList[iSymbol] == stringToParse[iChar]) {
				number										+= iSymbol * ::gpk::powui(base, totalCharsProcessed);
				bSymbolProcessed							= true;
				++iChar;
				++totalCharsProcessed;
				break;
			}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	gpk_safe_assign(number_, number);
	return totalCharsProcessed;
}

::gpk::error_t			stripLiteralsParseToken		(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, ::gpk::vcsc_c & in_format)		{
	(void)in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '{'	:
		++work_state.BracketsToSkip;
		break;
	case '}'	:
		if(work_state.Escaping)
			break;	// do nothing if this bracket is escaped
		if(0 < work_state.BracketsToSkip) {
			--work_state.BracketsToSkip;
			break;
		}
		out_types[work_state.IndexCurrentElement].Span.End	= work_state.IndexCurrentChar + 1;
		work_state.InsideToken						= false;
		gpk_necs(work_state.IndexCurrentElement = out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar + 1, work_state.IndexCurrentChar + 1}}));
		work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
	case '\\'	:
		if(false == work_state.Escaping) {
			work_state.Escaping							= true;
			return 0;
		}
		break;
	}
	work_state.Escaping							= false;
	if(work_state.IndexCurrentChar == in_format.size() - 1) { // if this is the last character, make sure to close open key and root expression
		++work_state.IndexCurrentChar;
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
		work_state.InsideToken						= false;
	}
	return 0;
}

::gpk::error_t			stripLiteralsParseLiteral	(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, ::gpk::vcsc_c & in_format)		{
	(void)in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '{'	:
		if(work_state.Escaping)
			break;	// do nothing if this bracket is escaped
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
		work_state.InsideToken						= true;
		gpk_necs(work_state.IndexCurrentElement = out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_TOKEN, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}}));
		work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
		// do work
		break;
	case '\\'	:
		if(false == work_state.Escaping) {
			work_state.Escaping							= true;
			return 0;
		}
		break;
	}
	work_state.Escaping							= false;
	if(work_state.IndexCurrentChar == in_format.size() - 1) { // if this is the last character, make sure to close open key and root expression
		++work_state.IndexCurrentChar;
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
	}
	return 0;
}

::gpk::error_t			gpk::stripLiteralParseStep		(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, ::gpk::vcsc_c & in_format)		{
	if(work_state.InsideToken)
		return ::stripLiteralsParseToken(work_state, out_types, in_format);

	if(0 == out_types.size()) {	// The format string always start from a literal.
		work_state.IndexCurrentElement				= out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});
		work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
	}
	return ::stripLiteralsParseLiteral(work_state, out_types, in_format);
}

::gpk::error_t			gpk::stripLiteralParse			(::gpk::SStripLiteralState & stateReading, ::gpk::apod<::gpk::SStripLiteralType> & out_types, ::gpk::vcsc_c & in_format)		{
	for(stateReading.IndexCurrentChar = 0; stateReading.IndexCurrentChar < in_format.size(); ++stateReading.IndexCurrentChar) {
		stateReading.CharCurrent					= in_format[stateReading.IndexCurrentChar];
		gpk_necs(gpk::stripLiteralParseStep(stateReading, out_types, in_format));
	}
	return 0;
}

::gpk::error_t			gpk::stripLiteralGetViews		(::gpk::aobj<::gpk::vcsc_t> & out_views, const ::gpk::view<const ::gpk::SStripLiteralType> & in_resultOfParser, ::gpk::vcsc_c & in_format)		{
	for(u2_t iType = 0; iType < in_resultOfParser.size(); ++iType) {
		const ::gpk::SStripLiteralType					& type						= in_resultOfParser[iType];
		::gpk::vcst_t						view						= {};
		u2_t										offsetView					= 0;
		u2_t										lenView						= 0;
		if(type.Type == ::gpk::STRIP_LITERAL_TYPE_TOKEN) {
			offsetView									= ::gpk::min(in_format.size() - 1, type.Span.Begin + 1);
			lenView										= type.Span.End - type.Span.Begin - 2;
		}
		else if(type.Type == ::gpk::STRIP_LITERAL_TYPE_LITERAL) {
			offsetView									= ::gpk::min(in_format.size() - 1, type.Span.Begin);
			lenView										= ::gpk::min(in_format.size() - type.Span.Begin, type.Span.End - type.Span.Begin);
		}
		if(0 >= lenView)
			continue;
		view										= {&in_format[offsetView], lenView};
		gpk_necs(out_views.push_back(view));
	}
	return 0;
}
