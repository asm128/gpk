#include "gpk_expression_core.h"
#include "gpk_reader.h"
#include "gpk_array_ptr.h"
#include "gpk_parse.h"
#include "gpk_functional.h"

#ifndef GPK_EXPRESSION_H_23627
#define GPK_EXPRESSION_H_23627

nmsp gpk
{
	struct SExpressionNode {
		SExpressionToken		* Token							= 0;
		SExpressionNode			* Parent						= 0;
		apobj<SExpressionNode>	Children						= {};
		s2_t					ObjectIndex						= -1;
	};

	struct SExpressionReader : SReader<SExpressionToken> {
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
