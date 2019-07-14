#include "gpk_view_array.h"

#ifndef GPK_KEYVAL_H_298037928374
#define GPK_KEYVAL_H_298037928374

namespace gpk
{
	template<typename _tKey, typename _tVal> 
	struct SKeyVal {
				_tKey									Key;
				_tVal									Val;
	};

	typedef		::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>		
										TKeyValConstString;

				::gpk::error_t							token_split						(char token, const ::gpk::view_const_string& input_string, TKeyValConstString& output_views);
	inline		::gpk::error_t							keyval_split					(const ::gpk::view_const_string& input_string, TKeyValConstString& out_keyval) { return ::gpk::token_split('=', input_string, out_keyval); }

	template<typename _tVal>
				::gpk::error_t							find							(const ::gpk::view_const_string & keyToFind, const ::gpk::view_array<const ::gpk::SKeyVal<::gpk::view_const_string, _tVal>> & keyvals)	{
		for(uint32_t iKeyVal = 0; iKeyVal < keyvals.size(); ++iKeyVal)
			if(keyToFind == keyvals[iKeyVal].Key)
				return iKeyVal;
		return -1;
	}
				::gpk::error_t							find							(const ::gpk::view_const_string & keyToFind, const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyvals, ::gpk::view_const_string& out_val);
				::gpk::error_t							keyValVerify					(const ::gpk::view_array<::gpk::TKeyValConstString> & environViews, const ::gpk::view_const_string & keyToVerify, const ::gpk::view_const_string & valueToVerify);
				::gpk::error_t							keyvalNumeric					(const ::gpk::view_const_string & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber);
	template <typename _tNumeric>
				::gpk::error_t							keyvalNumeric					(const ::gpk::view_const_string & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, _tNumeric & outputNumber)	{
		uint64_t												value							= 0;
		::gpk::error_t											indexKey						= ::gpk::keyvalNumeric(key, keyVals, &value);
		if(-1 != indexKey)
			outputNumber										= *(_tNumeric*)&value;
		return indexKey;
	}

} // namespace

#endif // GPK_KEYVAL_H_298037928374
