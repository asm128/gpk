#include "gpk_enum.h"
#include "gpk_coord.h"

#ifndef GPK_PARSE_H_298374298347
#define GPK_PARSE_H_298374298347

namespace gpk 
{
	::gpk::error_t					parseArbitraryBaseInteger	(uint32_t base, const ::gpk::view_const_string& symbolList, const ::gpk::view_const_string& sourceChars, uint64_t* number_);

	GDEFINE_ENUM_TYPE(STRIP_LITERAL_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, LITERAL	, 0);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, TOKEN	, 1);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, COUNT	, 2);
	GDEFINE_ENUM_VALUE(STRIP_LITERAL_TYPE, UNKNOWN	, -1);
#pragma pack(push, 1)
	struct SStripLiteralType {
		int32_t							ParentIndex;
		STRIP_LITERAL_TYPE				Type;
		::gpk::SSlice<uint32_t>			Span;
	};

	struct SStripLiteralState {
		uint32_t						IndexCurrentChar				= 0;
		int32_t							IndexCurrentElement				= -1;
		::gpk::SStripLiteralType		* CurrentElement				= 0;
		int32_t							NestLevel						= 0;
		char							CharCurrent						= 0;
		bool							Escaping						= false;
		bool							InsideToken						= false;
	};
#pragma pack(pop)
	::gpk::error_t					stripLiteralParse			(::gpk::SStripLiteralState& stateReading, ::gpk::array_pod<::gpk::SStripLiteralType> & out_types, const ::gpk::view_const_string& in_format);
	::gpk::error_t					stripLiteralParseStep		(::gpk::SStripLiteralState & work_state, ::gpk::array_pod<::gpk::SStripLiteralType> & out_types, const ::gpk::view_const_string& in_format);
	::gpk::error_t					stripLiteralGetViews		(::gpk::array_obj<::gpk::view_const_string>	& out_views, const ::gpk::view_array<const ::gpk::SStripLiteralType> & in_resultOfParser, const ::gpk::view_const_string & in_format);
} // namespace

#endif // GPK_PARSE_H_298374298347