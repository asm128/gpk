#include "gpk_enum.h"

#ifndef GPK_PARSE_H_23627
#define GPK_PARSE_H_23627

namespace gpk
{
	stacxpr	::gpk::vcc		DIGITS_HEX						= ::gpk::vcc{16, "0123456789abcdef"};
	stacxpr	::gpk::vcc		DIGITS_DECIMAL					= ::gpk::vcc{10, "0123456789"};
	::gpk::error_t			parseArbitraryBaseInteger		(uint32_t base, const ::gpk::vcc & symbolList, const ::gpk::vcc & sourceChars, uint64_t * number_);
	template<typename _tInt>	
	inline	::gpk::error_t	parseIntegerDecimal				(const ::gpk::vcc & sourceChars, _tInt & number_)	{ 
		uint64_t					number							= 0; 
		::gpk::error_t				countDigits; 
		gpk_necs(countDigits = ::gpk::parseArbitraryBaseInteger(10, ::gpk::vcs{"0123456789"}, sourceChars, &number)); 
		number_ = (_tInt)number; 
		return countDigits; 
	}

	template<typename _tInt>	
	inline	::gpk::error_t	parseIntegerHexadecimal			(const ::gpk::vcc & sourceChars, _tInt & number_)	{ 
		uint64_t					number							= 0; 
		::gpk::error_t countDigits;
		gpk_necs(countDigits = ::gpk::parseArbitraryBaseInteger(16, ::gpk::vcs{"0123456789abcdef"}, sourceChars, &number)); 
		number_ = (_tInt)number; 
		return countDigits; 
	}

	GDEFINE_ENUM_TYPE(STRIP_LITERAL_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, UNKNOWN	, -1);
#pragma pack(push, 1)
	struct SStripLiteralType {
		int32_t						ParentIndex;
		STRIP_LITERAL_TYPE			Type;
		::gpk::sliceu32				Span;
	};

	struct SStripLiteralState {
		uint32_t					IndexCurrentChar				= 0;
		int32_t						IndexCurrentElement				= -1;
		::gpk::SStripLiteralType	* CurrentElement				= 0;
		int32_t						NestLevel						= 0;
		char						CharCurrent						= 0;
		bool						Escaping						= false;
		bool						InsideToken						= false;
		int32_t						BracketsToSkip					= 0;
	};
#pragma pack(pop)
	::gpk::error_t				stripLiteralParse				(::gpk::SStripLiteralState	& stateReading	, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format);
	::gpk::error_t				stripLiteralParseStep			(::gpk::SStripLiteralState	& work_state	, ::gpk::apod<::gpk::SStripLiteralType> & out_types, const ::gpk::vcc & in_format);
	::gpk::error_t				stripLiteralGetViews			(::gpk::aobj<::gpk::vcc>	& out_views, const ::gpk::view<const ::gpk::SStripLiteralType> & in_resultOfParser, const ::gpk::vcc & in_format);

	bool						isSpaceCharacter				(const char characterToTest);
	::gpk::error_t				skipToNextCharacter				(uint32_t & indexCurrentChar, const ::gpk::vcc & expression);

} // namespace

#endif // GPK_PARSE_H_23627
