#include "gpk_json.h"
#include "gpk_expression.h"

#ifndef GPK_JSON_EXPRESSION_9238749283744
#define GPK_JSON_EXPRESSION_9238749283744

namespace gpk
{
	stxp	vcst_c			strEmptyObj				= GPK_CXS("{}");
	stxp	vcst_c			strEmptyArr				= GPK_CXS("[]");
	stxp	vcst_c			strNull					= GPK_CXS("null");
	stxp	vcst_c			strBool		[2]			= {GPK_CXS("false"), GPK_CXS("true")};
	stxp	vcst_c			strZero					= GPK_CXS("0");
	struct SEvaluationStepResult {
		u2_t					IndexRootJSONNode		;
		s2_t					IndexJSONResult			;
		u3_t					LastValue				;
		vcst_t					Output					;
		vcst_t					Expression				;
		bool					LastValueCarry			;

		error_t					SetBoolCarry			(bool evalResult, vcst_t & output)	{
			IndexJSONResult			= -5;
			LastValueCarry			= true	;
			LastValue				= evalResult ? 1 : 0;
			output					= strBool[LastValue];
			return 0;
		}
	};

	struct SJSONExpressionSolver {
		aobj<SEvaluationStepResult>	Results;
	};

			error_t		jsonExpressionResolve		(const SExpressionReader & reader, const SJSONReader & inputJSON, u2_t indexNodeJSON, SJSONExpressionSolver & results, vcst_t & output);
			error_t		jsonExpressionResolve		(const SExpressionReader & reader, const SJSONReader & inputJSON, u2_t indexNodeJSON, vcst_t & output);
	stainli	error_t		jsonExpressionResolve		(const SExpressionReader & reader, const SJSONReader & inputJSON, u2_t indexNodeJSON)	{ vcst_t dead; return jsonExpressionResolve(reader, inputJSON, indexNodeJSON, dead); }

			error_t		jsonExpressionResolve		(vcst_t expression, const SJSONReader & inputJSON, u2_t indexNodeJSON, vcst_t & output);
	stainli	error_t		jsonExpressionResolve		(vcst_t expression, const SJSONReader & inputJSON, u2_t indexNodeJSON) { vcst_t dead; return jsonExpressionResolve(expression, inputJSON, indexNodeJSON, dead); }

			error_t		jsonStringFormat			(vcst_t format, const SJSONReader & inputJSON, u2_t indexNodeJSON, asc_t & output);
} // namespace

#endif // GPK_JSON_EXPRESSION_9238749283744


