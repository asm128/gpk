#include "gpk_json.h"
#include "gpk_expression.h"

#ifndef GPK_JSON_EXPRESSION_9238749283744
#define GPK_JSON_EXPRESSION_9238749283744

namespace gpk
{
	static const ::gpk::view_const_string	strEmptyObj								= "{}"		;
	static const ::gpk::view_const_string	strEmptyArr								= "[]"		;
	static const ::gpk::view_const_string	strNull									= "null"	;
	static const ::gpk::view_const_string	strBool		[2]							=
		{ "false"
		, "true"
		};

	static const ::gpk::view_const_string	strZero									= "0"		;
#pragma pack(push, 1)
	struct SEvaluationStepResult {
		uint32_t								IndexRootJSONNode						;
		int32_t									IndexJSONResult							;
		uint64_t								LastValue								;
		::gpk::ptr_obj<::gpk::SJSONReader>		JSONReader								;
		::gpk::view_const_string				Output									;
		::gpk::view_const_string				Expression								;
		bool									LastValueCarry							;

		::gpk::error_t							SetBoolCarry							(bool evalResult, ::gpk::view_const_string & output)	{
			IndexJSONResult							= -5;
			LastValueCarry							= true	;
			LastValue								= evalResult ? 1 : 0;
			output									= ::gpk::strBool[LastValue];
			return 0;
		}
	};

	struct SJSONExpressionSolver {
		::gpk::array_obj<SEvaluationStepResult>			Results;
	};
#pragma pack(pop)

	::gpk::error_t							jsonExpressionResolve		(::gpk::SJSONExpressionSolver	& results	, const ::gpk::SExpressionReader & reader, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output);
	::gpk::error_t							jsonExpressionResolve		(const ::gpk::SExpressionReader & reader	, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output);
	::gpk::error_t							jsonExpressionResolve		(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output);
	::gpk::error_t							jsonStringFormat			(const ::gpk::view_const_string & format	, const ::gpk::SJSONReader & inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output);
} // namespace

#endif // GPK_JSON_EXPRESSION_9238749283744
