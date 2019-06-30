#include "gpk_json.h"

#ifndef GPK_JSON_EXPRESSION_9238749283744
#define GPK_JSON_EXPRESSION_9238749283744

namespace gpk
{
	::gpk::error_t								jsonExpressionResolve		(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output);
	::gpk::error_t								jsonStringFormat			(const ::gpk::view_const_string& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output);
} // namespace

#endif // GPK_JSON_EXPRESSION_9238749283744
