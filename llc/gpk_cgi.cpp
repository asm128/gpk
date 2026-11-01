#include "gpk_cgi_runtime.h"
#include "gpk_file.h"
#include "gpk_apod_serialize.h"
#include "gpk_slice.h"
#include "gpk_keyval.h"

#ifdef GPK_ATMEL
#	include <time.h>
#else
#	include <ctime>
#endif

namespace gpk
{
	err_t	keyValConstStringSerialize		(view<cnst kv<vcst_t>> keyVals, view<cnst vcst_t> keysToSave, au0_t & output);
	err_t	keyValConstStringDeserialize	(vcu0_c input, aobj<kv<vcst_t>> & output);
} // namespace 

gpk::err_t	gpk::keyValConstStringSerialize	(view<cnst kv<vcst_t>> keyVals, view<cnst vcst_t> keysToSave, au0_t & output)	{
	::gpk::apod<kv<vcst_t>>	keyValsToSave			= {};
	for(uint32_t iKey = 0; iKey < keyVals.size(); ++iKey) {
		for(uint32_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
			const kv<vcst_t>	& kvToCheck				= keyVals[iKey];
			::gpk::vcst_c		& keyToSave				= keysToSave[iRef];
			if(kvToCheck.Key == keyToSave)
				keyValsToSave.push_back(kvToCheck);
		}
	}
	output.append((const uint8_t*)&keyValsToSave.size(), szof(uint32_t));
	uint32_t					iOffset								= 0;
	for(uint32_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
		iOffset					+= saveView(output, keyValsToSave[iKey].Key);
		iOffset					+= saveView(output, keyValsToSave[iKey].Val);
	}
	return 0;
}

gpk::err_t			gpk::keyValConstStringDeserialize	(vcu0_c input, ::gpk::aobj<kv<vcst_t>> & output)	{
	uint32_t				offset								= 0;
	u2_c					keysToRead							= *(u2_c*)input.begin();
	offset				+= (uint32_t)szof(uint32_t);
	output.resize(keysToRead);
	for(uint32_t iKey = 0; iKey < keysToRead; ++iKey) {
		offset					+= ::gpk::viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
		offset					+= ::gpk::viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
	}
	return 0;
}
gpk::err_t			gpk::querystring_split	(vcst_t input_string, aobj<vcst_t> & output_views) {
	if(0 == input_string.size())
		return 0;
	u2_t						start				= 0;
	apod<sliceu2_t>	slices;
	for(u2_t iChar = 0; iChar < input_string.size(); ++iChar) {
		if('&' == input_string[iChar]) {
			if(start != iChar)
				slices.push_back({start, iChar});
			start						= iChar + 1;
		}
	}
	slices.push_back({start, input_string.size()});
	for(u2_t iSlice = 0; iSlice < slices.size(); ++iSlice) {
		sliceu2_c		& slice				= slices[iSlice];
		const u2_t			slicelen			= slice.End - slice.Begin;
		if(slicelen) {
			view<sc_c>			keyval				= {&input_string[slice.Begin], slicelen};
			output_views.push_back(keyval);
		}
	}
	return 0;
}

gpk::err_t			gpk::writeCGIEnvironToFile		(view<const kv<vcst_t>> environViews)	{
	au0_t				environmentBlockToSave			= {};
	keyValConstStringSerialize(environViews, cgi_environ, environmentBlockToSave);
	if(environmentBlockToSave.size()) {
		au0_t					temp;
		apod<sc_t>				tempName						= {};
		u3_t						curTime							= time(0);
		for(u2_t iKey = 0; iKey < environViews.size(); ++iKey)
			if(environViews[iKey].Key == "REMOTE_ADDR") {
				tempName							= environViews[iKey].Val;
				if(0 == tempName.size() || tempName[0] == ':')
					tempName = "localhost";
				tempName.append(".trace");
			}
		if(0 == tempName.size())
			return 1;

		fileToMemory({tempName}, temp);
		temp.append((const u0_t*)&curTime, sizeof(u3_t));
		temp.append(environmentBlockToSave);
		temp.push_back('\r');
		temp.push_back('\n');
		fileFromMemory({tempName}, temp);
		aobj<kv<vcst_t>>	environViewsTest;
		keyValConstStringDeserialize(environmentBlockToSave, environViewsTest);
		//for(u2_t iEnviron = 0; iEnviron < environViews.size(); ++iEnviron)
		//	info_printf("CGI Environ (original): '%s = %s'.", GPK_VIEW_ARGS(environViews[iEnviron].Key).begin(), GPK_VIEW_ARGS(environViews[iEnviron].Val).begin());
		for(u2_t iEnviron = 0; iEnviron < environViewsTest.size(); ++iEnviron)
			info_printf("CGI Environ: '%.*s = %.*s'.", GPK_VIEW_ARGS(environViewsTest[iEnviron].Key), GPK_VIEW_ARGS(environViewsTest[iEnviron].Val));
	}
	return 0;
}
