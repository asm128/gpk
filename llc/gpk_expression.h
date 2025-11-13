#include "gpk_enum.h"
#include "gpk_array_ptr.h"
#include "gpk_slice.h"

#ifndef GPK_EXPRESSION_H_23627
#define GPK_EXPRESSION_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE (EXPRESSION_READER_TYPE, int8_t);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_ANY		,  0);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_KEY		,  1);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_INDEX	,  2);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_BOOL	,  3);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_AND		,  4);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_OR		,  5);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, TERM_EQUALS	,  6);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, KEY			,  7);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, LITERAL		,  8);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, UNARY_NOT	,  9);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, CODEPOINT	, 10);
	GDEFINE_ENUM_VALUE(EXPRESSION_READER_TYPE, UNKNOWN		, -1);

#pragma pack(push, 1)
	struct SExpressionToken {
		int32_t							ParentIndex;
		EXPRESSION_READER_TYPE			Type;
		::gpk::sliceu2_t				Span;
		bool							ClosingCondition				= false;
	};

	struct SExpressionReaderState {
		uint32_t						IndexCurrentChar				= 0;
		int32_t							IndexCurrentElement				= -1;
		::gpk::SExpressionToken			* CurrentElement				= 0;
		int32_t							NestLevel						= 0;
		sc_t							CharCurrent						= 0;
		bool							Escaping						= false;
		bool							ExpectsSeparator				= false;
		bool							InsideString					= false;
		sc_t							ClosingQuotes					= '0';
	};
#pragma pack(pop)

	struct SExpressionNode {
		SExpressionToken				* Token							= 0;
		SExpressionNode					* Parent						= 0;
		::gpk::apobj<SExpressionNode>	Children						= {};
		int32_t							ObjectIndex						= -1;
	};

	struct SExpressionReader {
		::gpk::apod<::gpk::SExpressionToken>	Token;
		::gpk::aobj<::gpk::vcsc_t>				View;
		::gpk::apobj<::gpk::SExpressionNode>	Tree;
		::gpk::SExpressionReaderState			StateRead;
		::gpk::pobj<::gpk::SExpressionReader>	SubExpressionCache;
	};

	::gpk::error_t					expressionReaderParse			(::gpk::SExpressionReader & reader, const ::gpk::vcsc_t & expression);
	::gpk::error_t					expressionReaderParseStep		(::gpk::SExpressionReader & reader, const ::gpk::vcsc_t & expression);
} // namespace

#endif // GPK_EXPRESSION_H_23627
