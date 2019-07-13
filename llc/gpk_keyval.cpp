#include "gpk_keyval.h"
#include "gpk_find.h"
#include "gpk_safe.h"

#include <string>

::gpk::error_t						gpk::token_split			(char token, const ::gpk::view_const_string& input_string, TKeyValConstString& output_views)	{
	int32_t									indexToken					= ::gpk::find(token, input_string);
	rni_if(errored(indexToken), "'%c' Token not found.", token);
	output_views.Key					= {input_string.begin(), (uint32_t)indexToken};
	output_views.Val					= (indexToken + 1 < (int32_t)input_string.size()) 
		? ::gpk::view_const_string{&input_string[indexToken + 1], input_string.size() - (indexToken + 1)} 
		: ::gpk::view_const_string{}	// empty view if there's no data after the separator.
		;
	return 0;
}

::gpk::error_t						gpk::find					(const ::gpk::view_const_string & keyToFind, const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyvals, ::gpk::view_const_string& out_val)		{
	::gpk::error_t							index						= ::gpk::find(keyToFind, keyvals);
	out_val								= (-1 == index) ? ::gpk::view_const_string{} : keyvals[index].Val;
	return index;
}

::gpk::error_t						gpk::keyvalNumeric			(const ::gpk::view_const_string & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber)	{
	ree_if(0 == outputNumber, "%s", "Output number cannot point to a null address.");
	::gpk::error_t							indexKey					= ::gpk::find(key, keyVals);
	if(-1 != indexKey) {
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
		gpk_safe_assign(outputNumber, ::std::stoull(keyVals[indexKey].Val.begin())); 
#else
		try {
			gpk_safe_assign(outputNumber, ::std::stoull(keyVals[indexKey].Val.begin())); 
		} 
		catch(...) {
			return -1;
		}
#endif
	}
	return indexKey;
}
