#include "gpk_parse.h"
#include "gpk_array.h"
#include "gpk_safe.h"
#include <ctype.h>

bool										gpk::isSpaceCharacter						(const char characterToTest)		{
	switch(characterToTest) {
	case ' ': case '\t': case '\r': case '\n'	: case '\f'	: case '\b'	:
		return true;
	default:
		return false;
	}
}

::gpk::error_t								gpk::skipToNextCharacter					(uint32_t & indexCurrentChar, const ::gpk::vcc & expression)		{
	while(indexCurrentChar < expression.size()) {
		if(::gpk::isSpaceCharacter(expression[indexCurrentChar]))
			++indexCurrentChar;
		else
			break;
	}
	return 0;
}

::gpk::error_t								gpk::parseArbitraryBaseInteger				(uint32_t base, const ::gpk::vcc & symbolList, const ::gpk::vcc & sourceChars, uint64_t* number_)	{
	uint32_t										totalCharsProcessed							= 0;
	::gpk::apod<char_t>								stringToParse								= {};
	for(uint32_t iChar = 0; iChar < sourceChars.size() && 0 != sourceChars[iChar];) {
		const char										sourceChar									= (char)tolower(sourceChars[iChar]);
		bool											bSymbolProcessed							= false;
		for(uint32_t iSymbol = 0; iSymbol < base; ++iSymbol) {
			if(symbolList[iSymbol] == sourceChar) {
				bSymbolProcessed							= true;
				gpk_necall(stringToParse.push_back(sourceChar), "%s", "Failed to append character! Out of memory?");
				++iChar;
				++totalCharsProcessed;
				break;
			}
		}

		if(!bSymbolProcessed)
			break;	// number ends with any character that is not a symbol
	}
	gpk_necall(::gpk::reverse(stringToParse), "%s", "Cannot reverse string for parsing! How could this ever happen?");		// we assigned the digits backwards so we need to reverse the string.
	uint64_t										number										= 0;
	totalCharsProcessed							= 0;
	for(uint32_t iChar = 0; iChar < stringToParse.size() && 0 != stringToParse[iChar];) {
		bool											bSymbolProcessed							= false;
		for( uint32_t iSymbol = 0; iSymbol < base; ++iSymbol )
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

::gpk::error_t								stripLiteralsParseToken		(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format)		{
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
		gpk_necall(work_state.IndexCurrentElement = out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar + 1, work_state.IndexCurrentChar + 1}}), "Failed to push_back(). %s", "Out of memory?");
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

::gpk::error_t								stripLiteralsParseLiteral	(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format)		{
	(void)in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '{'	:
		if(work_state.Escaping)
			break;	// do nothing if this bracket is escaped
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
		work_state.InsideToken						= true;
		gpk_necall(work_state.IndexCurrentElement = out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_TOKEN, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}}), "Failed to push_back(). %s", "Out of memory?");
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

::gpk::error_t								gpk::stripLiteralParseStep		(::gpk::SStripLiteralState & work_state, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format)		{
	if(work_state.InsideToken)
		::stripLiteralsParseToken(work_state, out_types, in_format);
	else {
		if(0 == out_types.size()) {	// The format string always start from a literal.
			work_state.IndexCurrentElement				= out_types.push_back({-1, ::gpk::STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});
			work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
		}

		::stripLiteralsParseLiteral(work_state, out_types, in_format);
	}
	return 0;
}

::gpk::error_t								gpk::stripLiteralParse			(::gpk::SStripLiteralState & stateReading, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format)		{
	for(stateReading.IndexCurrentChar = 0; stateReading.IndexCurrentChar < in_format.size(); ++stateReading.IndexCurrentChar) {
		stateReading.CharCurrent					= in_format[stateReading.IndexCurrentChar];
		gpk_necall(::gpk::stripLiteralParseStep(stateReading, out_types, in_format), "%s", "Parse step failed.");
	}
	return 0;
}

::gpk::error_t								gpk::stripLiteralGetViews		(::gpk::aobj<::gpk::vcc> & out_views, const ::gpk::view<const ::gpk::SStripLiteralType> & in_resultOfParser, const ::gpk::vcc & in_format)		{
	for(uint32_t iType = 0; iType < in_resultOfParser.size(); ++iType) {
		const ::gpk::SStripLiteralType					& type						= in_resultOfParser[iType];
		::gpk::vcs						view						= {};
		uint32_t										offsetView					= 0;
		uint32_t										lenView						= 0;
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
		gpk_necall(out_views.push_back(view), "%s", "Failed to push back. Out of memory?");
	}
	return 0;
}
