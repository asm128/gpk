#include "gpk_json.h"
#include "gpk_expression.h"

#ifndef GPK_JSON_EXPRESSION_9238749283744
#define GPK_JSON_EXPRESSION_9238749283744

namespace gpk
{
	static const ::gpk::vcs					strEmptyObj				= "{}"		;
	static const ::gpk::vcs					strEmptyArr				= "[]"		;
	stacxpr	::gpk::vcc						strNull					= VCC_NULL	;
	stacxpr	::gpk::vcc						strBool		[2]			= {VCC_FALSE, VCC_TRUE};
	static const ::gpk::vcs					strZero					= "0"		;
#pragma pack(push, 1)
	struct SEvaluationStepResult {
		uint32_t							IndexRootJSONNode		;
		int32_t								IndexJSONResult			;
		uint64_t							LastValue				;
		::gpk::pobj<::gpk::SJSONReader>		JSONReader				;
		::gpk::vcc							Output					;
		::gpk::vcc							Expression				;
		bool								LastValueCarry			;

		::gpk::error_t						SetBoolCarry			(bool evalResult, ::gpk::vcc & output)	{
			IndexJSONResult						= -5;
			LastValueCarry						= true	;
			LastValue							= evalResult ? 1 : 0;
			output								= ::gpk::strBool[LastValue];
			return 0;
		}
	};

	struct SJSONExpressionSolver {
		::gpk::aobj<SEvaluationStepResult>	Results;
	};
#pragma pack(pop)

			::gpk::error_t		jsonExpressionResolve		(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::SJSONExpressionSolver& results, ::gpk::vcc & output);
			::gpk::error_t		jsonExpressionResolve		(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output);
	stainli	::gpk::error_t		jsonExpressionResolve		(const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON)	{ ::gpk::vcc dead; return ::gpk::jsonExpressionResolve(reader, inputJSON, indexNodeJSON, dead); }

			::gpk::error_t		jsonExpressionResolve		(const ::gpk::vcs	& expression, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::vcc & output);
	stainli	::gpk::error_t		jsonExpressionResolve		(const ::gpk::vcs	& expression, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON)			{ ::gpk::vcc dead; return ::gpk::jsonExpressionResolve(expression, inputJSON, indexNodeJSON, dead); }

			::gpk::error_t		jsonStringFormat			(const ::gpk::vcs	& format	, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::apod<char>& output);
} // namespace

#endif // GPK_JSON_EXPRESSION_9238749283744
