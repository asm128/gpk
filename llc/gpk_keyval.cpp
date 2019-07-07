#include "gpk_keyval.h"
#include "gpk_find.h"

::gpk::error_t										gpk::token_split			(char token, const ::gpk::view_const_string& input_string, TKeyValConstString& output_views)	{
	int32_t													indexToken					= ::gpk::find(token, input_string);
	rni_if(errored(indexToken), "'%c' Token not found.", token);
	output_views.Key									= {input_string.begin(), (uint32_t)indexToken};
	output_views.Val									= (indexToken + 1 < (int32_t)input_string.size()) ? ::gpk::view_const_string{&input_string[indexToken + 1], input_string.size() - (indexToken + 1)} : ::gpk::view_const_string{};
	return 0;
}

