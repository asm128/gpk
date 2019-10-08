#include "gpk_cgi_runtime.h"
#include "gpk_storage.h"

#include <ctime>
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

::gpk::error_t										gpk::writeCGIEnvironToFile		(::gpk::view_array<const ::gpk::TKeyValConstString> environViews)	{
	::gpk::array_pod<char_t>								environmentBlockToSave			= {};
	::gpk::keyValConstStringSerialize(environViews, ::gpk::cgi_environ, environmentBlockToSave);
	if(environmentBlockToSave.size()) {
		::gpk::array_pod<char_t>								temp;
		::gpk::array_pod<char_t>								tempName						= {};
		uint64_t												curTime							= time(0);
		for(uint32_t iKey = 0; iKey < environViews.size(); ++iKey)
			if(environViews[iKey].Key == ::gpk::view_const_string{"REMOTE_ADDR"}) {
				tempName											= environViews[iKey].Val;
				if(0 == tempName.size() || tempName[0] == ':')
					tempName = ::gpk::view_const_string{"localhost"};
				tempName.append(".trace");
			}
		if(0 == tempName.size())
			return 1;

		::gpk::fileToMemory({tempName.begin(), tempName.size()}, temp);
		temp.append((const char*)&curTime, sizeof(uint64_t));
		temp.append(environmentBlockToSave);
		temp.push_back('\r');
		temp.push_back('\n');
		::gpk::fileFromMemory({tempName.begin(), tempName.size()}, temp);
		::gpk::array_obj<::gpk::TKeyValConstString>				environViewsTest;
		::gpk::keyValConstStringDeserialize(environmentBlockToSave, environViewsTest);
		for(uint32_t iEnviron = 0; iEnviron < environViewsTest.size(); ++iEnviron)
			info_printf("CGI Environ: '%s = %s'.", ::gpk::toString(environViewsTest[iEnviron].Key).begin(), ::gpk::toString(environViewsTest[iEnviron].Val).begin());
	}
	return 0;
}
