#include "gpk_parse.h"
#include "gpk_array_pod.h"

gpk::err_t			gpk::skipToNextCharacter		(u2_t & indexCurrentChar, vcst_t expression)		{
	while(indexCurrentChar < expression.size()) {
		if(is_space_character(expression[indexCurrentChar]))
			++indexCurrentChar;
		else
			break;
	}
	return 0;
}
gpk::err_t			stripLiteralsParseToken			(gpk::SStripLiteralState & work_state, gpk::apod<gpk::SStripLiteralToken> & out_types, gpk::vcst_t in_format)		{
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
		if_fail_fe(work_state.IndexCurrentElement = out_types.push_back({-1, gpk::STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar + 1, work_state.IndexCurrentChar + 1}}));
		work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
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
		work_state.InsideToken						= false;
	}
	return 0;
}
gpk::err_t			stripLiteralsParseLiteral		(gpk::SStripLiteralState & work_state, gpk::apod<gpk::SStripLiteralToken> & out_types, gpk::vcst_t in_format)		{
	(void)in_format;
	switch(work_state.CharCurrent) {
	default		: break;
	case '{'	:
		if(work_state.Escaping)
			break;	// do nothing if this bracket is escaped
		out_types[out_types.size() - 1].Span.End	= work_state.IndexCurrentChar;
		work_state.InsideToken						= true;
		if_fail_fe(work_state.IndexCurrentElement = out_types.push_back({-1, gpk::STRIP_LITERAL_TYPE_TOKEN, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}}));
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
gpk::err_t			gpk::stripLiteralParseStep		(SStripLiteralState & work_state, apod<SStripLiteralToken> & out_types, vcst_t in_format)		{
	if(work_state.InsideToken)
		return ::stripLiteralsParseToken(work_state, out_types, in_format);

	if(0 == out_types.size()) {	// The format string always start from a literal.
		work_state.IndexCurrentElement				= out_types.push_back({-1, STRIP_LITERAL_TYPE_LITERAL, {work_state.IndexCurrentChar, work_state.IndexCurrentChar}});
		work_state.CurrentElement					= &out_types[work_state.IndexCurrentElement];
	}
	return ::stripLiteralsParseLiteral(work_state, out_types, in_format);
}
gpk::err_t			gpk::stripLiteralParse			(SStripLiteralState & stateReading, apod<SStripLiteralToken> & out_types, vcst_t in_format)		{
	for(stateReading.IndexCurrentChar = 0; stateReading.IndexCurrentChar < in_format.size(); ++stateReading.IndexCurrentChar) {
		stateReading.CharCurrent					= in_format[stateReading.IndexCurrentChar];
		if_fail_fe(gpk::stripLiteralParseStep(stateReading, out_types, in_format));
	}
	return 0;
}
gpk::err_t			gpk::stripLiteralGetViews		(aobj<vcst_t> & out_views, cnst view<cnst SStripLiteralToken> & in_resultOfParser, vcst_t in_format)		{
	for(u2_t iType = 0; iType < in_resultOfParser.size(); ++iType) {
		cnst SStripLiteralToken					& type						= in_resultOfParser[iType];
		vcst_t						view						= {};
		u2_t										offsetView					= 0;
		u2_t										lenView						= 0;
		if(type.Type == STRIP_LITERAL_TYPE_TOKEN) {
			offsetView									= min(in_format.size() - 1, type.Span.Begin + 1);
			lenView										= type.Span.End - type.Span.Begin - 2;
		}
		else if(type.Type == STRIP_LITERAL_TYPE_LITERAL) {
			offsetView									= min(in_format.size() - 1, type.Span.Begin);
			lenView										= min(in_format.size() - type.Span.Begin, type.Span.End - type.Span.Begin);
		}
		if(0 >= lenView)
			continue;
		view										= {&in_format[offsetView], lenView};
		if_fail_fe(out_views.push_back(view));
	}
	return 0;
}
