#include "gpk_enum.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"

#ifndef GPK_EXPRESSION_H_92837498237
#define GPK_EXPRESSION_H_92837498237

namespace gpk
{
	GDEFINE_ENUM_TYPE (EXPRESSION_READER_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_ANY			, 0);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_KEY			, 1);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_INDEX		, 2);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, KEY				, 3);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, LITERAL			, 4);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, EVALUATION		, 5);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, CODEPOINT		, 6);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, UNKNOWN			, -1);

#pragma pack(push, 1)
	struct SExpressionReaderType {
		int32_t														ParentIndex;
		EXPRESSION_READER_TYPE										Type;
		::gpk::SSlice<uint32_t>										Span;
		bool														ClosingEvaluation				= false;
	};

	struct SExpressionReaderState {
		uint32_t													IndexCurrentChar				= 0;
		int32_t														IndexCurrentElement				= -1;
		::gpk::SExpressionReaderType								* CurrentElement				= 0;
		int32_t														NestLevel						= 0;
		char														CharCurrent						= 0;
		bool														Escaping						= false;
		bool														ExpectsSeparator				= false;
		bool														InsideString					= false;
		char														ClosingQuotes					= '0';
	};

	struct SExpressionNode {
		SExpressionReaderType										* Object						= 0;
		SExpressionNode												* Parent						= 0;
		::gpk::array_obj<::gpk::ptr_obj<SExpressionNode>>			Children						= {};
		int32_t														ObjectIndex						= -1;
	};


	struct SExpressionReader {
		::gpk::array_pod<::gpk::SExpressionReaderType>				Object;
		::gpk::array_obj<::gpk::view_const_string>					View;
		::gpk::array_obj<::gpk::ptr_obj	<::gpk::SExpressionNode>>	Tree;
		::gpk::SExpressionReaderState								StateRead;
	};

	::gpk::error_t												expressionReaderParse			(::gpk::SExpressionReader& reader, const ::gpk::view_const_string& expression);
	::gpk::error_t												expressionReaderParseStep		(::gpk::SExpressionReader& reader, const ::gpk::view_const_string& expression);
#pragma pack(pop)
} // namespace

#endif // GPK_EXPRESSION_H_92837498237
