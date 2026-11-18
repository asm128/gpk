#include "gpk_expression_core.h"
#include "gpk_array_ptr.h"
#include "gpk_parse.h"
#include "gpk_functional.h"

#ifndef GPK_EXPRESSION_H_23627
#define GPK_EXPRESSION_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	struct SExpressionReaderState {
		u2_t					IndexCurrentChar				= 0;
		s2_t					IndexCurrentElement				= -1;
		SExpressionToken		* CurrentElement				= 0;
		s2_t					NestLevel						= 0;
		sc_t					CharCurrent						= 0;
		b8_t					Escaping						= false;
		b8_t					ExpectsSeparator				= false;
		b8_t					InsideString					= false;
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
	err_t					expressionReaderLoad			(SExpressionReader & reader, view<cnst SExpressionToken> tokens, vcst_t expression);

	// Token spans refer to format. Resolved views must survive the callback until the formatter copies them.
	err_t					expressionStringFormat		(vcst_t format, view<cnst SStripLiteralToken> tokens, cnst FError<vcst_c &, vcst_t &> & resolve, asc_t & output);
	err_t					expressionStringFormat		(vcst_t format, cnst FError<vcst_c &, vcst_t &> & resolve, asc_t & output);
} // nmsp

#endif // GPK_EXPRESSION_H_23627
