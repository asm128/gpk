#include "gpk_json.h"

#ifndef GPK_JSON_FORMATTER_923874928374
#define GPK_JSON_FORMATTER_923874928374

namespace gpk
{
	::gpk::error_t								jsonExpressionResolve		(const ::gpk::view_const_string & expression, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::view_const_string& output);
	::gpk::error_t								jsonStringFormat			(const ::gpk::view_const_string& format, const ::gpk::SJSONReader& inputJSON, uint32_t indexNodeJSON, ::gpk::array_pod<char_t>& output);
} // namespace

#endif // GPK_JSON_FORMATTER_923874928374
