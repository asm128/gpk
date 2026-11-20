#include "gpk_parse_core.h"
#include "gpk_enum.h"
#include "gpk_slice.h"

#ifndef GPK_PARSE_H_23627
#define GPK_PARSE_H_23627

nmsp gpk
{
	GDEFINE_ENUM_TYPE(STRIP_LITERAL_TYPE, s0_t);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, UNKNOWN	, -1);
#pragma pack(push, 1)
	stct SStripLiteralToken {
		s2_t				ParentIndex;
		STRIP_LITERAL_TYPE	Type;
		sliceu2_t			Span;
	};

	stct SStripLiteralState {
		u2_t				IndexCurrentChar				= 0;
		s2_t				IndexCurrentElement				= -1;
		SStripLiteralToken	* CurrentElement				= 0;
		s2_t				NestLevel						= 0;
		sc_t				CharCurrent						= 0;
		b8_t				Escaping						= false;
		b8_t				InsideToken						= false;
		s2_t				BracketsToSkip					= 0;
	};
#pragma pack(pop)
	err_t				stripLiteralParse				(SStripLiteralState	& stateReading	, apod<SStripLiteralToken> & out_types, vcst_t in_format);
	err_t				stripLiteralParseStep			(SStripLiteralState	& work_state	, apod<SStripLiteralToken> & out_types, vcst_t in_format);
	err_t				stripLiteralGetViews			(aobj<vcst_t>	& out_views, cnst view<cnst SStripLiteralToken> & in_resultOfParser, vcst_t in_format);

	err_t				skipToNextCharacter				(u2_t & indexCurrentChar, vcst_t expression);

} // nmsp

#endif // GPK_PARSE_H_23627
