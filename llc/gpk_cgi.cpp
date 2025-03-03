#include "gpk_cgi_runtime.h"
#include "gpk_file.h"
#include "gpk_apod_serialize.h"
#include "gpk_slice.h"

#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <ctime>
#endif

::gpk::error_t			gpk::querystring_split	(const ::gpk::vcc & input_string, ::gpk::aobj<::gpk::vcc>& output_views)		{
	if(0 == input_string.size())
		return 0;
	uint32_t						start				= 0;
	::gpk::apod<::gpk::sliceu32>	slices;
	for(uint32_t iChar = 0; iChar < input_string.size(); ++iChar) {
		if('&' == input_string[iChar]) {
			if(start != iChar)
				slices.push_back({start, iChar});
			start						= iChar + 1;
		}
	}
	slices.push_back({start, input_string.size()});
	for(uint32_t iSlice = 0; iSlice < slices.size(); ++iSlice) {
		const ::gpk::sliceu32			& slice				= slices[iSlice];
		const uint32_t					slicelen			= slice.End - slice.Begin;
		if(slicelen) {
			const ::gpk::vcc				keyval				= {&input_string[slice.Begin], slicelen};
			output_views.push_back(keyval);
		}
	}
	return 0;
}

::gpk::error_t			gpk::writeCGIEnvironToFile		(::gpk::view<const ::gpk::TKeyValConstString> environViews)	{
	::gpk::au8						environmentBlockToSave			= {};
	::gpk::keyValConstStringSerialize(environViews, ::gpk::cgi_environ, environmentBlockToSave);
	if(environmentBlockToSave.size()) {
		::gpk::au8						temp;
		::gpk::apod<sc_t>				tempName						= {};
		uint64_t						curTime							= time(0);
		for(uint32_t iKey = 0; iKey < environViews.size(); ++iKey)
			if(environViews[iKey].Key == ::gpk::vcs{"REMOTE_ADDR"}) {
				tempName							= environViews[iKey].Val;
				if(0 == tempName.size() || tempName[0] == ':')
					tempName = ::gpk::vcs{"localhost"};
				tempName.append(".trace");
			}
		if(0 == tempName.size())
			return 1;

		::gpk::fileToMemory({tempName}, temp);
		temp.append((const uint8_t*)&curTime, sizeof(uint64_t));
		temp.append(environmentBlockToSave);
		temp.push_back('\r');
		temp.push_back('\n');
		::gpk::fileFromMemory({tempName}, temp);
		::gpk::aobj<::gpk::TKeyValConstString>	environViewsTest;
		::gpk::keyValConstStringDeserialize(environmentBlockToSave, environViewsTest);
		//for(uint32_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron)
		//	info_printf("CGI Environ (original): '%s = %s'.", ::gpk::toString(environViews[iEnviron].Key).begin(), ::gpk::toString(environViews[iEnviron].Val).begin());
		for(uint32_t iEnviron = 0; iEnviron < environViewsTest.size(); ++iEnviron)
			info_printf("CGI Environ: '%s = %s'.", ::gpk::toString(environViewsTest[iEnviron].Key).begin(), ::gpk::toString(environViewsTest[iEnviron].Val).begin());
	}
	return 0;
}
