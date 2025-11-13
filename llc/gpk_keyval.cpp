#include "gpk_keyval.h"
#include "gpk_safe.h"
#include "gpk_parse.h"
#include "gpk_apod_serialize.h"

::gpk::error_t			gpk::join				(::gpk::asc_t & output, sc_t separator, ::gpk::vcvsc_t fields)	{
	for(uint32_t iField = 0; iField < fields.size();) {
		gpk_necall(output.append(fields[iField]), "%i", iField);
		if(++iField < fields.size())
			gpk_necall(output.push_back(separator), "%i", iField);
	}
	return 0;
}

::gpk::asc_t			gpk::toString			(::gpk::vcsc_c & strToLog)	{
	::gpk::asc_t		sprintfable				= strToLog;
	if(sprintfable.size() && sprintfable[sprintfable.size() - 1] == 0) { // it already contains a null, so resize it to avoid counting it as part of the array.
		sprintfable.resize(sprintfable.size() - 1);
		return sprintfable;
	}

	es_if(::gpk::failed(sprintfable.push_back(0)))
	else
		es_if(::gpk::failed(sprintfable.resize(sprintfable.size()-1)));

	return sprintfable;
}

::gpk::error_t			gpk::token_split		(sc_t token, const ::gpk::vcs & input_string, TKeyValConstChar & output_views)	{
	uint32_t					indexToken;
	gpk_necall(indexToken = ::gpk::find(token, input_string), "'%c' not found.", token);
	output_views.Key		= {input_string.begin(), (uint32_t)indexToken};
	output_views.Val		= (indexToken + 1 < (int32_t)input_string.size())
		? ::gpk::vcs{&input_string[indexToken + 1], input_string.size() - (indexToken + 1)}
		: ::gpk::vcs{}	// empty view if there's no data after the separator.
		;
	::gpk::trim(output_views.Key, output_views.Key);
	::gpk::trim(output_views.Val, output_views.Val);
	return 0;
}

::gpk::error_t			gpk::find				(::gpk::vcst_c & keyToFind, const ::gpk::view<const ::gpk::TKeyValConstString> & keyvals, ::gpk::vcsc_t & out_val)		{
	::gpk::error_t				index					= ::gpk::find(keyToFind, keyvals);
	out_val					= (-1 == index) ? ::gpk::vcs{} : keyvals[index].Val;
	return index;
}

::gpk::error_t			gpk::keyvalNumeric		(::gpk::vcst_c & key, const ::gpk::view<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber)	{
	ree_if(0 == outputNumber, "%s", "Output number cannot point to a null address.");
	::gpk::error_t				indexKey				= ::gpk::find(key, keyVals);
	if(-1 != indexKey)
		::gpk::parseIntegerDecimal(keyVals[indexKey].Val, outputNumber);

	return indexKey;
}

::gpk::error_t			gpk::keyValVerify		(const ::gpk::view<::gpk::TKeyValConstString> & environViews, const ::gpk::vcsc_t & keyToVerify, const ::gpk::vcsc_t & valueToVerify)	{
	for(uint32_t iKey = 0; iKey < environViews.size(); ++iKey) {
		if(environViews[iKey].Key == keyToVerify)
			return (environViews[iKey].Val == valueToVerify) ? iKey : -1;
	}
	return -1;
}

::gpk::error_t			gpk::keyValConstStringSerialize		(const ::gpk::view<const ::gpk::TKeyValConstString> & keyVals, const ::gpk::view<vcsc_c> & keysToSave, ::gpk::au0_t & output)	{
	::gpk::apod<::gpk::TKeyValConstString>	keyValsToSave					= {};
	for(uint32_t iKey = 0; iKey < keyVals.size(); ++iKey) {
		for(uint32_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
			const ::gpk::TKeyValConstString	& kvToCheck						= keyVals[iKey];
			const ::gpk::vcsc_t				& keyToSave						= keysToSave[iRef];
			if(kvToCheck.Key == keyToSave)
				keyValsToSave.push_back(kvToCheck);
		}
	}
	output.append((const uint8_t*)&keyValsToSave.size(), sizeof(uint32_t));
	uint32_t					iOffset								= 0;
	for(uint32_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
		iOffset					+= ::gpk::saveView(output, keyValsToSave[iKey].Key);
		iOffset					+= ::gpk::saveView(output, keyValsToSave[iKey].Val);
	}
	return 0;
}

::gpk::error_t			gpk::keyValConstStringDeserialize	(const ::gpk::vcu0_t & input, ::gpk::aobj<::gpk::TKeyValConstString> & output)	{
	uint32_t					offset								= 0;
	const uint32_t				keysToRead							= *(const uint32_t*)input.begin();
	offset					+= (uint32_t)sizeof(uint32_t);
	output.resize(keysToRead);
	for(uint32_t iKey = 0; iKey < keysToRead; ++iKey) {
		offset					+= ::gpk::viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
		offset					+= ::gpk::viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
	}
	return 0;
}
