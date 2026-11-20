#include "gpk_ctti_enum.h"
#include "gpk_reader_core.h"
#include "gpk_slice.h"

#ifndef GPK_EXPRESSION_CORE_H
#define GPK_EXPRESSION_CORE_H

nmsp gpk
{
	CTTI_ENUM(EXPRESSION_READER_TYPE,
	enum EXPRESSION_READER_TYPE : s0_t
		{ EXPRESSION_READER_TYPE_TERM_ANY		=  0
		, EXPRESSION_READER_TYPE_TERM_KEY		=  1
		, EXPRESSION_READER_TYPE_TERM_INDEX		=  2
		, EXPRESSION_READER_TYPE_TERM_BOOL		=  3
		, EXPRESSION_READER_TYPE_TERM_AND		=  4
		, EXPRESSION_READER_TYPE_TERM_OR		=  5
		, EXPRESSION_READER_TYPE_TERM_EQUALS	=  6
		, EXPRESSION_READER_TYPE_KEY			=  7
		, EXPRESSION_READER_TYPE_LITERAL		=  8
		, EXPRESSION_READER_TYPE_UNARY_NOT		=  9
		, EXPRESSION_READER_TYPE_CODEPOINT		= 10
		, EXPRESSION_READER_TYPE_UNKNOWN		= -1
		};
	);

#pragma pack(push, 1)
	stct SExpressionToken {
		s2_t					ParentIndex;
		EXPRESSION_READER_TYPE	Type;
		sliceu2_t				Span;
		b8_t					ClosingCondition = false;
	};
	stct SExpressionReaderState : SReaderState<SExpressionToken> {
		b8_t					Escaping						= false;
		b8_t					ExpectsSeparator				= false;
		b8_t					InsideString					= false;
		sc_t					ClosingQuotes					= '0';
	};
#pragma pack(pop)
	tydf SReaderView<SExpressionToken> SExpressionReaderView;
} // nmsp

#endif // GPK_EXPRESSION_CORE_H
