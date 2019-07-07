#include "gpk_cgi_runtime.h"
#include <string>

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

