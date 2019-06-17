#include "gpk_cgi_runtime.h"
#include <string>

::gpk::error_t										gpk::token_split			(char token, const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views)	{
	uint32_t												iChar				= 0;
	for(iChar = 0; iChar < input_string.size(); ++iChar) {
		if(token == input_string[iChar]) {
			output_views.Key									= {input_string.begin(), iChar};
			output_views.Val									= (iChar + 1 < input_string.size()) ? ::gpk::view_const_string{&input_string[iChar + 1], input_string.size() - (iChar + 1)} : ::gpk::view_const_string{};
			break;
		}
	}
	return 0;
}

::gpk::error_t										gpk::keyval_split		(const ::gpk::view_const_string& input_string, ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& output_views)		{
	return ::gpk::token_split('=', input_string, output_views);
}

::gpk::error_t										gpk::querystring_split	(const ::gpk::view_const_string& input_string, ::gpk::array_obj<::gpk::view_const_string>& output_views)		{
	if(0 == input_string.size())
		return 0;
	uint32_t												start				= 0;
	::gpk::array_pod<::gpk::SSlice<uint32_t>>				slices;
	for(uint32_t iChar = 0; iChar < input_string.size(); ++iChar) {
		if('&' == input_string[iChar]) {
			if(start != iChar)
				slices.push_back({start, iChar});
			start												= iChar + 1;
		}
	}
	slices.push_back({start, input_string.size()});
	for(uint32_t iSlice = 0; iSlice < slices.size(); ++iSlice) {
		const ::gpk::SSlice<uint32_t>							& slice				= slices[iSlice];
		const uint32_t											slicelen			= slice.End - slice.Begin;
		if(slicelen) {
			const ::gpk::view_const_string							keyval				= {&input_string[slice.Begin], slicelen};
			output_views.push_back(keyval);
		}
	}
	return 0;
}

