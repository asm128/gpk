#include "gpk_keyval.h"
#include "gpk_find.h"
#include "gpk_safe.h"
#include "gpk_array.h"
#include "gpk_parse.h"

::gpk::error_t							gpk::append_quoted					(::gpk::array_pod<char_t>& output, ::gpk::view_const_char text)		{
	gpk_necall(output.push_back('"'), "%s", "Out of memory?");
	gpk_necall(output.append(text)	, "%s", "Out of memory?");
	gpk_necall(output.push_back('"'), "%s", "Out of memory?");
	return 0;
}

::gpk::error_t							gpk::join							(::gpk::array_pod<char_t> & query, char separator, ::gpk::view_array<const gpk::view_const_char> fields)	{
	for(uint32_t iField = 0; iField < fields.size();) {
		gpk_necall(query.append(fields[iField]), "%s", "Out of memory?");
		if(++iField < fields.size())
			gpk_necall(query.push_back(separator), "%s", "Out of memory?");
	}
	return 0;
}

::gpk::array_pod<char_t>				gpk::toString						(const ::gpk::view_const_char& strToLog)	{
	::gpk::array_pod<char_t>					sprintfable							= strToLog;
	e_if(::gpk::failed(sprintfable.push_back(0)), "%s", "Out of memory?")
	else
		e_if(::gpk::failed(sprintfable.resize(sprintfable.size()-1)), "%s", "Out of memory?");
	return sprintfable;
}

::gpk::error_t							gpk::token_split					(char token, const ::gpk::view_const_char& input_string, TKeyValConstString& output_views)	{
	int32_t										indexToken							= ::gpk::find(token, input_string);
	rvi_if(-1, errored(indexToken), "'%c' Token not found.", token);
	output_views.Key						= {input_string.begin(), (uint32_t)indexToken};
	output_views.Val						= (indexToken + 1 < (int32_t)input_string.size())
		? ::gpk::view_const_char{&input_string[indexToken + 1], input_string.size() - (indexToken + 1)}
		: ::gpk::view_const_char{}	// empty view if there's no data after the separator.
		;
	::gpk::trim(output_views.Key, output_views.Key);
	::gpk::trim(output_views.Val, output_views.Val);
	return 0;
}

::gpk::error_t							gpk::find							(const ::gpk::view_const_char & keyToFind, const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyvals, ::gpk::view_const_char& out_val)		{
	::gpk::error_t								index								= ::gpk::find(keyToFind, keyvals);
	out_val									= (-1 == index) ? ::gpk::view_const_string{} : keyvals[index].Val;
	return index;
}

::gpk::error_t							gpk::keyvalNumeric					(const ::gpk::view_const_char & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber)	{
	ree_if(0 == outputNumber, "%s", "Output number cannot point to a null address.");
	::gpk::error_t								indexKey							= ::gpk::find(key, keyVals);
	if(-1 != indexKey) {
		::gpk::parseIntegerDecimal(keyVals[indexKey].Val, outputNumber);
	}
	return indexKey;
}

::gpk::error_t							gpk::keyValVerify					(const ::gpk::view_array<::gpk::TKeyValConstString> & environViews, const ::gpk::view_const_char & keyToVerify, const ::gpk::view_const_char & valueToVerify)	{
	for(uint32_t iKey = 0; iKey < environViews.size(); ++iKey) {
		if(environViews[iKey].Key == keyToVerify)
			return (environViews[iKey].Val == valueToVerify) ? iKey : -1;
	}
	return -1;
}

::gpk::error_t							gpk::keyValConstStringSerialize		(const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyVals, const ::gpk::view_array<const ::gpk::view_const_char> & keysToSave, ::gpk::array_pod<byte_t> & output)	{
	::gpk::array_pod<::gpk::TKeyValConstString>	keyValsToSave						= {};
	for(uint32_t iKey = 0; iKey < keyVals.size(); ++iKey) {
		for(uint32_t iRef = 0; iRef < keysToSave.size(); ++iRef) {
			const ::gpk::TKeyValConstString				& kvToCheck							= keyVals[iKey];
			const ::gpk::view_const_char				& keyToSave							= keysToSave[iRef];
			if(kvToCheck.Key == keyToSave)
				keyValsToSave.push_back(kvToCheck);
		}
	}
	output.append((const char*)&keyValsToSave.size(), sizeof(uint32_t));
	uint32_t												iOffset								= 0;
	for(uint32_t iKey = 0; iKey < keyValsToSave.size(); ++iKey) {
		iOffset												+= ::gpk::viewWrite(keyValsToSave[iKey].Key, output);
		iOffset												+= ::gpk::viewWrite(keyValsToSave[iKey].Val, output);
	}
	return 0;
}

::gpk::error_t							gpk::keyValConstStringDeserialize	(const ::gpk::view_const_byte & input, ::gpk::array_obj<::gpk::TKeyValConstString> & output)	{
	uint32_t									offset								= 0;
	const uint32_t								keysToRead							= *(const uint32_t*)input.begin();
	offset									+= (uint32_t)sizeof(uint32_t);
	output.resize(keysToRead);
	for(uint32_t iKey = 0; iKey < keysToRead; ++iKey) {
		offset									+= ::gpk::viewRead(output[iKey].Key, {&input[offset], input.size() - offset});
		offset									+= ::gpk::viewRead(output[iKey].Val, {&input[offset], input.size() - offset});
	}
	return 0;
}
