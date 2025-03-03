#include "gpk_view.h"

#ifndef GPK_KEYVAL_H_23627
#define GPK_KEYVAL_H_23627

namespace gpk
{
	tplt<tpnm _tKey, tpnm _tVal>
	struct SKeyVal {
					_tKey									Key;
					_tVal									Val;
	};

	typedef		::gpk::SKeyVal<::gpk::vcc, ::gpk::vcc>
														TKeyValConstChar, TKeyValConstString;

	//typedef		::gpk::SKeyVal<::gpk::vcs, ::gpk::vcs> TKeyValConstString;

				::gpk::error_t							token_split						(sc_t token, const ::gpk::vcs& input_string, TKeyValConstChar& output_views);
	inline		::gpk::error_t							keyval_split					(const ::gpk::vcs& input_string, TKeyValConstString& out_keyval) { return ::gpk::token_split('=', input_string, out_keyval); }

	tplt<tpnm _tVal>
				::gpk::error_t							find			(const ::gpk::vcs & keyToFind, const ::gpk::view<const ::gpk::SKeyVal<::gpk::vcc, _tVal>> & keyvals)	{
		for(uint32_t iKeyVal = 0; iKeyVal < keyvals.size(); ++iKeyVal) {
			const ::gpk::vcc							& keyCurrent					= keyvals[iKeyVal].Key;
			if(keyToFind == keyCurrent)
				return iKeyVal;
		}
		return -1;
	}

	tplt<tpnm _tVal>
				::gpk::error_t							find			(const ::gpk::vcs & keyToFind, const ::gpk::view<const ::gpk::TKeyValConstString> & keyvals, ::gpk::view<const _tVal>& out_val)		{
		::gpk::error_t					index							= ::gpk::find(keyToFind, keyvals);
		out_val								= (-1 == index) ? ::gpk::view<const _tVal>{} : ::gpk::view<const _tVal>{(const _tVal*)keyvals[index].Val.begin(), keyvals[index].Val.size()};
		return index;
	}

				::gpk::error_t							find			(const ::gpk::vcs & keyToFind, const ::gpk::view<const ::gpk::TKeyValConstString> & keyvals, ::gpk::vcc & out_val);
				::gpk::error_t							keyValVerify					(const ::gpk::view<::gpk::TKeyValConstString> & environViews, const ::gpk::vcc & keyToVerify, const ::gpk::vcc & valueToVerify);
				::gpk::error_t							keyvalNumeric					(const ::gpk::vcs & key, const ::gpk::view<const ::gpk::TKeyValConstString> keyVals, uint64_t * outputNumber);
	tplt <tpnm _tNumeric>
				::gpk::error_t							keyvalNumeric					(const ::gpk::vcs & key, const ::gpk::view<const ::gpk::TKeyValConstString> keyVals, _tNumeric & outputNumber)	{
		uint64_t												value							= 0;
		::gpk::error_t					indexKey						= ::gpk::keyvalNumeric(key, keyVals, &value);
		if(-1 != indexKey)
			outputNumber										= *(_tNumeric*)&value;
		return indexKey;
	}

	tplt <tpnm... _tArgs>
	::gpk::error_t				keyValVerify					(const ::gpk::view<::gpk::TKeyValConstString> & environViews, const ::gpk::vcc & keyToVerify, const ::gpk::view<const ::gpk::vcc>& valueToVerify)	{
		for(uint32_t iKey = 0; iKey < valueToVerify.size(); ++iKey) {
			const ::gpk::error_t			val								= ::gpk::keyValVerify(environViews, keyToVerify, valueToVerify[iKey]);
			if(-1 != val)
				return val;
		}
		return -1;
	}


} // namespace

#endif // GPK_KEYVAL_H_23627
