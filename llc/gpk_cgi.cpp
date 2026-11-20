#include "gpk_cgi_runtime.h"
#include "gpk_file.h"
#include "gpk_apod_serialize.h"
#include "gpk_slice.h"
#include "gpk_keyval_old.h"

#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <ctime>
#endif

gpk::err_t			gpk::querystring_split	(::gpk::vcsc_c & input_string, ::gpk::aobj<::gpk::vcsc_t>& output_views) {
	if(0 == input_string.size())
		return 0;
	u2_t						start				= 0;
	::gpk::apod<::gpk::sliceu2_t>	slices;
	for(u2_t iChar = 0; iChar < input_string.size(); ++iChar) {
		if('&' == input_string[iChar]) {
			if(start != iChar)
				slices.push_back({start, iChar});
			start						= iChar + 1;
		}
	}
	slices.push_back({start, input_string.size()});
	for(u2_t iSlice = 0; iSlice < slices.size(); ++iSlice) {
		::gpk::sliceu2_c		& slice				= slices[iSlice];
		const u2_t			slicelen			= slice.End - slice.Begin;
		if(slicelen) {
			::gpk::vcsc_c			keyval				= {&input_string[slice.Begin], slicelen};
			output_views.push_back(keyval);
		}
	}
	return 0;
}

gpk::err_t			gpk::writeCGIEnvironToFile		(::gpk::view<const ::gpk::kv<vcsc_t, vcsc_t>> environViews)	{
	::gpk::au0_t				environmentBlockToSave			= {};
	::gpk::keyValConstStringSerialize(environViews, ::gpk::cgi_environ, environmentBlockToSave);
	if(environmentBlockToSave.size()) {
		::gpk::au0_t					temp;
		::gpk::apod<sc_t>				tempName						= {};
		u3_t						curTime							= time(0);
		for(u2_t iKey = 0; iKey < environViews.size(); ++iKey)
			if(environViews[iKey].Key == ::gpk::vcs{"REMOTE_ADDR"}) {
				tempName							= environViews[iKey].Val;
				if(0 == tempName.size() || tempName[0] == ':')
					tempName = ::gpk::vcs{"localhost"};
				tempName.append(".trace");
			}
		if(0 == tempName.size())
			return 1;

		::gpk::fileToMemory({tempName}, temp);
		temp.append((const uint8_t*)&curTime, sizeof(u3_t));
		temp.append(environmentBlockToSave);
		temp.push_back('\r');
		temp.push_back('\n');
		::gpk::fileFromMemory({tempName}, temp);
		::gpk::aobj<::gpk::kv<vcsc_t, vcsc_t>>	environViewsTest;
		::gpk::keyValConstStringDeserialize(environmentBlockToSave, environViewsTest);
		//for(u2_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron)
		//	info_printf("CGI Environ (original): '%.*s = %.*s'.", GPK_VIEW_ARGS(environViews[iEnviron].Key), GPK_VIEW_ARGS(environViews[iEnviron].Val));
		for(u2_t iEnviron = 0; iEnviron < environViewsTest.size(); ++iEnviron)
			info_printf("CGI Environ: '%.*s = %.*s'.", GPK_VIEW_ARGS(environViewsTest[iEnviron].Key), GPK_VIEW_ARGS(environViewsTest[iEnviron].Val));
	}
	return 0;
}
