#include "gpk_view.h"

#ifndef GPK_KEYVAL_H_23627
#define GPK_KEYVAL_H_23627

namespace gpk
{
	tydf	keyval<vcst_t, vcst_t>	TKeyValConstChar, TKeyValConstString;

			error_t					token_split			(char valueSeparator, vcst_t input_string, TKeyValConstChar & output_views);
	inln	error_t					keyval_split		(vcst_t input_string, TKeyValConstString & out_keyval) { return token_split('=', input_string, out_keyval); }

			error_t					keyValVerify		(view<cnst TKeyValConstString> environViews, vcst_t keyToVerify, vcst_t valueToVerify);
			error_t					keyvalNumeric		(vcst_t key, view<cnst TKeyValConstString> keyVals, uint64_t & outputNumber);
	tplt <tpnm _tNumeric>
			error_t					keyvalNumeric		(vcst_t key, view<cnst TKeyValConstString> keyVals, _tNumeric & outputNumber)	{
		uint64_t							value				= 0;
		error_t								indexKey			= keyvalNumeric(key, keyVals, value);
		if(-1 != indexKey)
			outputNumber				= value;
		return indexKey;
	}

	tplt <tpnm... _tArgs>
			error_t					keyValVerify		(view<cnst TKeyValConstString> environViews, vcst_t keyToVerify, view<cnst vcst_t> valueToVerify)	{
		for(::gpk::u2_t iKey = 0; iKey < valueToVerify.size(); ++iKey) {
			err_c						val						= keyValVerify(environViews, keyToVerify, valueToVerify[iKey]);
			if(-1 != val)
				return val;
		}
		return -1;
	}


} // namespace

#endif // GPK_KEYVAL_H_23627


