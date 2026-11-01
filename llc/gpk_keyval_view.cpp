#include "gpk_keyval_view.h"
#include "gpk_parse.h"

gpk::err_t			gpk::keyvalNumeric		(vcst_t key, cnst view<cnst keyval<vcst_t>> keyVals, u3_t & outputNumber)	{
	err_t					indexKey;
	if_fail_fwf(indexKey = find(key, keyVals), "key:\"%.*s\"", (int)key.size(), key.begin());
	cnst	kv<vcst_t>		keyVal					= keyVals[indexKey];
	if_fail_vef(-2, parseIntegerDecimal(keyVal.Val, outputNumber), "%.*s", GPK_VIEW_ARGS(keyVal.Val));
	return indexKey;
}
ndsc	gpk::err_t	gpk::keyValVerify		(view<cnst keyval<vcst_t>> environViews, vcst_t keyToVerify, vcst_t valueToVerify)	{
	for(u2_t iKey = 0; iKey < environViews.size(); ++iKey) {
		if(environViews[iKey].Key == keyToVerify)
			return (environViews[iKey].Val == valueToVerify) ? iKey : -1;
	}
	return -1;
}
gpk::err_t			gpk::token_split		(char valueSeparator, vcst_t input_string, keyval<vcst_t> & output_views)	{
	s2_t					indexSeparator;
	if_fail_wf(indexSeparator = find(valueSeparator, input_string), "'%c' character not found.", valueSeparator);
	output_views.Key		= {input_string.begin(), (u2_t)indexSeparator};

	b8_c					hasValue							= indexSeparator > 0 && u2_t(indexSeparator + 1) < input_string.size();
	output_views.Val	= hasValue
		? vcst_t{&input_string[indexSeparator + 1U], input_string.size() - (indexSeparator + 1U)}
		: vcst_t{}	// empty view if there's no data after the separator.
		;
	trim(output_views.Key, output_views.Key);
	trim(output_views.Val, output_views.Val);
	return output_views.Val.size();
}

// gpk::err_t			gpk::keyValConstStringDeserialize	(vcu0_t input, aobj<keyval<vcst_t>> & output)	{
// 	u2_t					offset								= 0;
// 	u2_c					keysToRead							= *(u2_c*)input.begin();
// 	offset				+= (u2_t)szof(u2_t);
// 	if_fail_fe(output.resize(keysToRead));
// 	for(u2_t iKey = 0; iKey < keysToRead; ++iKey) {
// 		offset				+= viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
// 		offset				+= viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
// 	}
// 	return 0;
// }
//gpk::err_t			gpk::keyValConstStringSerialize		(view<cnst keyval<vcst_t>> keyVals, view<cnst vcst_t> keysToSave, au0_t & output)	{
//	apod<keyval<vcst_t>>	keyValsToSave						= {};
//	for(u2_t iKey = 0; iKey < keyVals.size(); ++iKey) {
//		for(u2_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
//			cnst keyval<vcst_t>	& kvToCheck							= keyVals[iKey];
//			vcst_c				& keyToSave							= keysToSave[iRef];
//			if(kvToCheck.Key == keyToSave)
//				if_fail_fe(keyValsToSave.push_back(kvToCheck));
//		}
//	}
//	if_fail_fe(output.append((cnst u0_t*)&keyValsToSave.size(), szof(u2_t)));
//	u2_t					iOffset								= 0;
//	for(u2_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
//		iOffset				+= saveView(output, keyValsToSave[iKey].Key);
//		iOffset				+= saveView(output, keyValsToSave[iKey].Val);
//	}
//	return 0;
//}
