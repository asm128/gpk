#include "gpk_json_ctti.h"
#include "gpk_expression_ctti.h"

#ifndef GPK_JSON_EXPRESSION_CTTI_H
#define GPK_JSON_EXPRESSION_CTTI_H

nmsp gpk
{
	tplt<size_t NExpression, size_t NJSON>
	stct SCTTIJSONExpressionReader {
		SCTTIExpressionReader<NExpression>	Expression	= {};
		SCTTIJSONReader<NJSON>			JSON		= {};

		inxp b8_t Success() cnst nxpt { rtrn Expression.Success() && JSON.Success(); }
	};

	tplt<size_t NExpression, size_t NJSON>
	inxp SCTTIJSONExpressionReader<NExpression, NJSON> jsonExpressionParseCTTI(sc_c (&expression)[NExpression], sc_c (&json)[NJSON]) nxpt {
		SCTTIJSONExpressionReader<NExpression, NJSON> result = {};
		result.Expression = expressionParseCTTI(expression);
		result.JSON = jsonParseCTTI(json);
		rtrn result;
	}
} // nmsp

#endif // GPK_JSON_EXPRESSION_CTTI_H
