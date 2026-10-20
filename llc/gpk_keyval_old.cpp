#include "gpk_keyval_old.h"
#include "gpk_safe.h"
#include "gpk_parse.h"
#include "gpk_apod_serialize.h"

gpk::err_t			gpk::token_split		(char valueSeparator, vcst_t input_string, TKeyValConstChar & output_views)	{
	int32_t						indexSeparator;
	if_fail_wf(indexSeparator = find(valueSeparator, input_string), "'%c' character not found.", valueSeparator);
	output_views.Key		= {input_string.begin(), (::gpk::u2_t)indexSeparator};

	gpk::b8_c					hasValue			= indexSeparator > 0 && ::gpk::u2_t(indexSeparator + 1) < input_string.size();
	output_views.Val		= hasValue
		? vcst_t{&input_string[indexSeparator + 1U], input_string.size() - (indexSeparator + 1U)}
		: vcst_t{}	// empty view if there's no data after the separator.
		;
	trim(output_views.Key, output_views.Key);
	trim(output_views.Val, output_views.Val);
	return output_views.Val.size();
}

gpk::err_t			gpk::keyvalNumeric		(vcst_t key, cnst view<cnst TKeyValConstString> keyVals, uint64_t & outputNumber)	{
	gpk::err_t				indexKey;
	if_fail_fwf(indexKey = find(key, keyVals), "key:\"%.*s\"", (int)key.size(), key.begin());
	if_fail_vef(-2, parseIntegerDecimal(keyVals[indexKey].Val, outputNumber), "%.*s", (int)keyVals[indexKey].Val.size(), keyVals[indexKey].Val.begin());
	return indexKey;
}

gpk::err_t			gpk::keyValVerify		(view<cnst TKeyValConstString> environViews, vcst_t keyToVerify, vcst_t valueToVerify)	{
	for(::gpk::u2_t iKey = 0; iKey < environViews.size(); ++iKey) {
		if(environViews[iKey].Key == keyToVerify)
			return (environViews[iKey].Val == valueToVerify) ? iKey : -1;
	}
	return -1;
}

gpk::err_t			gpk::keyValConstStringSerialize		(view<cnst TKeyValConstString> keyVals, view<cnst vcst_t> keysToSave, au0_t & output)	{
	apod<TKeyValConstString>	keyValsToSave					= {};
	for(::gpk::u2_t iKey = 0; iKey < keyVals.size(); ++iKey) {
		for(::gpk::u2_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
			cnst TKeyValConstString	& kvToCheck						= keyVals[iKey];
			vcst_c				& keyToSave						= keysToSave[iRef];
			if(kvToCheck.Key == keyToSave)
				keyValsToSave.push_back(kvToCheck);
		}
	}
	output.append((cnst u0_t*)&keyValsToSave.size(), szof(::gpk::u2_t));
	::gpk::u2_t					iOffset								= 0;
	for(::gpk::u2_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
		iOffset					+= saveView(output, keyValsToSave[iKey].Key);
		iOffset					+= saveView(output, keyValsToSave[iKey].Val);
	}
	return 0;
}

gpk::err_t			gpk::keyValConstStringDeserialize	(vcu0_t input, aobj<TKeyValConstString> & output)	{
	::gpk::u2_t					offset								= 0;
	u2_c				keysToRead							= *(u2_c*)input.begin();
	offset					+= (::gpk::u2_t)szof(::gpk::u2_t);
	output.resize(keysToRead);
	for(::gpk::u2_t iKey = 0; iKey < keysToRead; ++iKey) {
		offset					+= viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
		offset					+= viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
	}
	return 0;
}


