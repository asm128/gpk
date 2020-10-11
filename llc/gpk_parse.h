#include "gpk_enum.h"
#include "gpk_coord.h"

#ifndef GPK_PARSE_H_298374298347
#define GPK_PARSE_H_298374298347

namespace gpk
{
										::gpk::error_t			parseArbitraryBaseInteger		(uint32_t base, const ::gpk::view_const_char& symbolList, const ::gpk::view_const_char& sourceChars, uint64_t* number_);
	template<typename _tInt>	inline	::gpk::error_t			parseIntegerDecimal				(const ::gpk::view_const_char& sourceChars, _tInt* number_)	{ uint64_t number = 0; const ::gpk::error_t countDigits = ::gpk::parseArbitraryBaseInteger(10, ::gpk::view_const_string{"0123456789"		}, sourceChars, &number); gpk_necall(countDigits, "%s", "Unknown error."); gpk_safe_assign(number_, (_tInt)number); return countDigits; }
	template<typename _tInt>	inline	::gpk::error_t			parseIntegerHexadecimal			(const ::gpk::view_const_char& sourceChars, _tInt* number_)	{ uint64_t number = 0; const ::gpk::error_t countDigits = ::gpk::parseArbitraryBaseInteger(16, ::gpk::view_const_string{"0123456789abcdef"	}, sourceChars, &number); gpk_necall(countDigits, "%s", "Unknown error."); gpk_safe_assign(number_, (_tInt)number); return countDigits; }

	GDEFINE_ENUM_TYPE(STRIP_LITERAL_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, UNKNOWN	, -1);
#pragma pack(push, 1)
	struct SStripLiteralType {
		int32_t														ParentIndex;
		STRIP_LITERAL_TYPE											Type;
		::gpk::SSlice<uint32_t>										Span;
	};

	struct SStripLiteralState {
		uint32_t													IndexCurrentChar				= 0;
		int32_t														IndexCurrentElement				= -1;
		::gpk::SStripLiteralType									* CurrentElement				= 0;
		int32_t														NestLevel						= 0;
		char														CharCurrent						= 0;
		bool														Escaping						= false;
		bool														InsideToken						= false;
		int32_t														BracketsToSkip					= 0;
	};
#pragma pack(pop)
	::gpk::error_t												stripLiteralParse				(::gpk::SStripLiteralState	& stateReading	, ::gpk::array_pod<::gpk::SStripLiteralType> & out_types, const ::gpk::view_const_char& in_format);
	::gpk::error_t												stripLiteralParseStep			(::gpk::SStripLiteralState	& work_state	, ::gpk::array_pod<::gpk::SStripLiteralType> & out_types, const ::gpk::view_const_char& in_format);
	::gpk::error_t												stripLiteralGetViews			(::gpk::array_obj<::gpk::view_const_char>	& out_views, const ::gpk::view_array<const ::gpk::SStripLiteralType> & in_resultOfParser, const ::gpk::view_const_char & in_format);
} // namespace

#endif // GPK_PARSE_H_298374298347
