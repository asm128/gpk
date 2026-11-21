#include "gpk_keyval_old.h"
#include "gpk_safe.h"
#include "gpk_parse.h"
#include "gpk_apod_serialize.h"

gpk::err_t			gpk::join				(::gpk::asc_t & output, char separator, ::gpk::vcvsc_t fields)	{
	for(::gpk::u2_t iField = 0; iField < fields.size();) {
		if_fail_fef(output.append(fields[iField]), "%" GPK_FMT_U2 "", iField);
		if(++iField < fields.size())
			if_fail_fef(output.push_back(separator), "%" GPK_FMT_U2 "", iField);
	}
	return 0;
}

::gpk::asc_t		gpk::toString			(::gpk::vcsc_c & strToLog)	{
	::gpk::asc_t			sprintfable				= strToLog;
	if(sprintfable.size() && sprintfable[sprintfable.size() - 1] == 0) { // it already contains a null, so resize it to avoid counting it as part of the array.
		sprintfable.resize(sprintfable.size() - 1);
		return sprintfable;
	}

	if_true_e(::gpk::failed(sprintfable.push_back(0)))
	else
		if_true_e(::gpk::failed(sprintfable.resize(sprintfable.size()-1)));

	return sprintfable;
}

gpk::err_t			gpk::token_split		(char valueSeparator, const ::gpk::vcst_t & input_string, kv<vcsc_t, vcsc_t> & output_views)	{
	::gpk::s2_t						indexSeparator;
	if_fail_wf(indexSeparator = ::gpk::find(valueSeparator, input_string), "'%c' character not found.", valueSeparator);
	output_views.Key		= {input_string.begin(), (::gpk::u2_t)indexSeparator};

	gpk::b8_c					hasValue			= indexSeparator > 0 && ::gpk::u2_t(indexSeparator + 1) < input_string.size();
	output_views.Val		= hasValue
		? ::gpk::vcst_t{&input_string[indexSeparator + 1U], input_string.size() - (indexSeparator + 1U)}
		: ::gpk::vcst_t{}	// empty view if there's no data after the separator.
		;
	::gpk::trim(output_views.Key, output_views.Key);
	::gpk::trim(output_views.Val, output_views.Val);
	return output_views.Val.size();
}

gpk::err_t			gpk::find				(const ::gpk::vcst_t & keyToFind, const ::gpk::view<const ::gpk::kv<vcsc_t, vcsc_t>> & keyvals, ::gpk::vcsc_t & out_val)		{
	gpk::err_t				index					= ::gpk::find(keyToFind, keyvals);
	out_val					= (-1 == index) ? ::gpk::vcst_t{} : keyvals[index].Val;
	return index;
}

gpk::err_t			gpk::keyvalNumeric		(const ::gpk::vcst_t & key, const ::gpk::view<const ::gpk::kv<vcsc_t, vcsc_t>> keyVals, ::gpk::u3_t * outputNumber)	{
	if_true_fef(0 == outputNumber, "%s", "Output number cannot point to a null address.");
	gpk::err_t				indexKey				= ::gpk::find(key, keyVals);
	if(-1 != indexKey)
		::gpk::parseIntegerDecimal(keyVals[indexKey].Val, outputNumber);

	return indexKey;
}

gpk::err_t			gpk::keyValVerify		(const ::gpk::view<::gpk::kv<vcsc_t, vcsc_t>> & environViews, ::gpk::vcsc_c & keyToVerify, ::gpk::vcsc_c & valueToVerify)	{
	for(::gpk::u2_t iKey = 0; iKey < environViews.size(); ++iKey) {
		if(environViews[iKey].Key == keyToVerify)
			return (environViews[iKey].Val == valueToVerify) ? iKey : -1;
	}
	return -1;
}

gpk::err_t			gpk::keyValConstStringSerialize		(const ::gpk::view<const ::gpk::kv<vcsc_t, vcsc_t>> & keyVals, const view<vcsc_c> & keysToSave, ::gpk::au0_t & output)	{
	::gpk::apod<::gpk::kv<vcsc_t, vcsc_t>>	keyValsToSave					= {};
	for(::gpk::u2_t iKey = 0; iKey < keyVals.size(); ++iKey) {
		for(::gpk::u2_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
			const ::gpk::kv<vcsc_t, vcsc_t>	& kvToCheck						= keyVals[iKey];
			::gpk::vcsc_c				& keyToSave						= keysToSave[iRef];
			if(kvToCheck.Key == keyToSave)
				keyValsToSave.push_back(kvToCheck);
		}
	}
	output.append((const ::gpk::u0_t*)&keyValsToSave.size(), szof(::gpk::u2_t));
	::gpk::u2_t					iOffset								= 0;
	for(::gpk::u2_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
		iOffset					+= ::gpk::saveView(output, keyValsToSave[iKey].Key);
		iOffset					+= ::gpk::saveView(output, keyValsToSave[iKey].Val);
	}
	return 0;
}

gpk::err_t			gpk::keyValConstStringDeserialize	(vcu0_c & input, ::gpk::aobj<::gpk::kv<vcsc_t, vcsc_t>> & output)	{
	::gpk::u2_t					offset								= 0;
	u2_c				keysToRead							= *(u2_c*)input.begin();
	offset					+= (::gpk::u2_t)szof(::gpk::u2_t);
	output.resize(keysToRead);
	for(::gpk::u2_t iKey = 0; iKey < keysToRead; ++iKey) {
		offset					+= ::gpk::viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
		offset					+= ::gpk::viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
	}
	return 0;
}
