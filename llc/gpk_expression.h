#include "gpk_enum.h"
#include "gpk_array_ptr.h"
#include "gpk_slice.h"
#include "gpk_parse.h"
#include "gpk_functional.h"

#ifndef GPK_EXPRESSION_H_23627
#define GPK_EXPRESSION_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE (EXPRESSION_READER_TYPE, s0_t);
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
		s2_t					ParentIndex;
		EXPRESSION_READER_TYPE	Type;
		sliceu2_t				Span;
		bool					ClosingCondition				= false;
	};

	struct SExpressionReaderState {
		u2_t					IndexCurrentChar				= 0;
		s2_t					IndexCurrentElement				= -1;
		SExpressionToken		* CurrentElement				= 0;
		s2_t					NestLevel						= 0;
		sc_t					CharCurrent						= 0;
		bool					Escaping						= false;
		bool					ExpectsSeparator				= false;
		bool					InsideString					= false;
		sc_t					ClosingQuotes					= '0';
	};
#pragma pack(pop)

	struct SExpressionNode {
		SExpressionToken		* Token							= 0;
		SExpressionNode			* Parent						= 0;
		apobj<SExpressionNode>	Children						= {};
		s2_t					ObjectIndex						= -1;
	};

	struct SExpressionReader {
		apod<SExpressionToken>	Token;
		aobj<vcst_t>			View;
		apobj<SExpressionNode>	Tree;
		SExpressionReaderState	StateRead;
		pobj<SExpressionReader>	SubExpressionCache;
	};

	err_t					expressionReaderParse			(SExpressionReader & reader, vcst_t expression);
	err_t					expressionReaderParseStep		(SExpressionReader & reader, vcst_t expression);

	// Token spans refer to format. Resolved views must survive the callback until the formatter copies them.
	err_t					expressionStringFormat		(vcst_t format, view<cnst SStripLiteralType> tokens, cnst FError<vcst_c &, vcst_t &> & resolve, asc_t & output);
	err_t					expressionStringFormat		(vcst_t format, cnst FError<vcst_c &, vcst_t &> & resolve, asc_t & output);
} // namespace

#endif // GPK_EXPRESSION_H_23627
