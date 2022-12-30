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

	typedef		::gpk::SKeyVal<::gpk::vcc, ::gpk::vcc>
														TKeyValConstChar, TKeyValConstString;

	//typedef		::gpk::SKeyVal<::gpk::vcs, ::gpk::vcs> TKeyValConstString;

				::gpk::error_t							token_split						(char token, const ::gpk::vcs& input_string, TKeyValConstChar& output_views);
	inline		::gpk::error_t							keyval_split					(const ::gpk::vcs& input_string, TKeyValConstString& out_keyval) { return ::gpk::token_split('=', input_string, out_keyval); }

	template<typename _tVal>
				::gpk::error_t							find							(const ::gpk::vcs & keyToFind, const ::gpk::view_array<const ::gpk::SKeyVal<::gpk::vcc, _tVal>> & keyvals)	{
		for(uint32_t iKeyVal = 0; iKeyVal < keyvals.size(); ++iKeyVal) {
			const ::gpk::vcc							& keyCurrent					= keyvals[iKeyVal].Key;
			if(keyToFind == keyCurrent)
				return iKeyVal;
		}
		return -1;
	}

	template<typename _tVal>
				::gpk::error_t							find							(const ::gpk::vcs & keyToFind, const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyvals, ::gpk::view_array<const _tVal>& out_val)		{
		::gpk::error_t											index							= ::gpk::find(keyToFind, keyvals);
		out_val								= (-1 == index) ? ::gpk::view_array<const _tVal>{} : ::gpk::view_array<const _tVal>{(const _tVal*)keyvals[index].Val.begin(), keyvals[index].Val.size()};
		return index;
	}

				::gpk::error_t							find							(const ::gpk::vcs & keyToFind, const ::gpk::view_array<const ::gpk::TKeyValConstString> & keyvals, ::gpk::vcc & out_val);
				::gpk::error_t							keyValVerify					(const ::gpk::view_array<::gpk::TKeyValConstString> & environViews, const ::gpk::vcc & keyToVerify, const ::gpk::vcc & valueToVerify);
				::gpk::error_t							keyvalNumeric					(const ::gpk::vcs & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber);
	template <typename _tNumeric>
				::gpk::error_t							keyvalNumeric					(const ::gpk::vcs & key, const ::gpk::view_array<const ::gpk::TKeyValConstString> keyVals, _tNumeric & outputNumber)	{
		uint64_t												value							= 0;
		::gpk::error_t											indexKey						= ::gpk::keyvalNumeric(key, keyVals, &value);
		if(-1 != indexKey)
			outputNumber										= *(_tNumeric*)&value;
		return indexKey;
	}

	template <typename... _tArgs>
	::gpk::error_t										keyValVerify					(const ::gpk::view_array<::gpk::TKeyValConstString> & environViews, const ::gpk::vcc & keyToVerify, const ::gpk::view_array<const ::gpk::vcc>& valueToVerify)	{
		for(uint32_t iKey = 0; iKey < valueToVerify.size(); ++iKey) {
			const ::gpk::error_t									val								= ::gpk::keyValVerify(environViews, keyToVerify, valueToVerify[iKey]);
			if(-1 != val)
				return val;
		}
		return -1;
	}


} // namespace

#endif // GPK_KEYVAL_H_298037928374
