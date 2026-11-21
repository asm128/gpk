#include "gpk_enum.h"
#include "gpk_slice.h"

#ifndef GPK_PARSE_H_23627
#define GPK_PARSE_H_23627

namespace gpk
{
	stxp	vcst_c	DIGITS_HEX					= "0123456789abcdef";
	stxp	vcst_c	DIGITS_DECIMAL				= "0123456789";
	err_t			parseArbitraryBaseInteger	(u2_t base, vcst_c & symbolList, vcst_c & sourceChars, u3_t & number_);
	tplt<tpnm _tInt>	
	inline	err_t	parseIntegerDecimal			(vcst_c & sourceChars, _tInt & number_)	{ 
		u3_t				number						= 0; 
		err_t				countDigits; 
		if_fail_fe(countDigits = parseArbitraryBaseInteger(10, DIGITS_DECIMAL, sourceChars, &number)); 
		number_ = (_tInt)number; 
		return countDigits; 
	}

	tplt<tpnm _tInt>	
	inline	err_t	parseIntegerHexadecimal		(vcst_c & sourceChars, _tInt & number_)	{ 
		u3_t				number						= 0; 
		err_t				countDigits;
		if_fail_fe(countDigits = parseArbitraryBaseInteger(16, DIGITS_HEX, sourceChars, &number)); 
		number_ = (_tInt)number; 
		return countDigits; 
	}

	GDEFINE_ENUM_TYPE(STRIP_LITERAL_TYPE, ::gpk::s0_t);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, UNKNOWN	, -1);
#pragma pack(push, 1)
	stct SStripLiteralToken {
		s2_t						ParentIndex;
		STRIP_LITERAL_TYPE			Type;
		sliceu2_t			Span;
	};

	stct SStripLiteralState {
		u2_t						IndexCurrentChar				= 0;
		s2_t						IndexCurrentElement				= -1;
		SStripLiteralToken	* CurrentElement				= 0;
		s2_t						NestLevel						= 0;
		char						CharCurrent						= 0;
		bool						Escaping						= false;
		bool						InsideToken						= false;
		s2_t						BracketsToSkip					= 0;
	};
#pragma pack(pop)
	err_t				stripLiteralParse				(SStripLiteralState	& stateReading	, apod<SStripLiteralToken> & out_types, vcsc_c & in_format);
	err_t				stripLiteralParseStep			(SStripLiteralState	& work_state	, apod<SStripLiteralToken> & out_types, vcsc_c & in_format);
	err_t				stripLiteralGetViews			(aobj<vcsc_t>	& out_views, const view<const SStripLiteralToken> & in_resultOfParser, vcsc_c & in_format);

	bool						isSpaceCharacter				(const char characterToTest);
	err_t				skipToNextCharacter				(u2_t & indexCurrentChar, vcsc_c & expression);

} // namespace

#endif // GPK_PARSE_H_23627
