#include "gpk_keyval.h"
#include "gpk_view.h"

#ifndef GPK_KEYVAL_VIEW_H_261022
#define GPK_KEYVAL_VIEW_H_261022

namespace gpk
{
			err_t		token_split			(char valueSeparator, vcst_t input_string, keyval<vcst_t> & output_views);
	stin	err_t		keyval_split		(vcst_t input_string, keyval<vcst_t> & out_keyval)			{ return token_split('=', input_string, out_keyval); }

	ndsc	err_t		keyValVerify		(view<cnst keyval<vcst_t>> environViews, vcst_t keyToVerify, vcst_t valueToVerify);
			err_t		keyvalNumeric		(vcst_t key, view<cnst keyval<vcst_t>> keyVals, u3_t & outputNumber);
	tplt<tpnm _tNumeric>
			err_t		keyvalNumeric		(vcst_t key, view<cnst keyval<vcst_t>> keyVals, _tNumeric & outputNumber)	{
		u3_t				value				= 0;
		err_t					indexKey			= keyvalNumeric(key, keyVals, value);
		if(-1 != indexKey)
			outputNumber		= value;
		return indexKey;
	}
	tplt <tpnm... _tArgs>
			err_t		keyValVerify		(view<cnst keyval<vcst_t>> environViews, vcst_t keyToVerify, view<cnst vcst_t> valueToVerify)	{
		for(u2_t iKey = 0; iKey < valueToVerify.size(); ++iKey) {
			err_c					val					= keyValVerify(environViews, keyToVerify, valueToVerify[iKey]);
			if(-1 != val)
				return val;
		}
		return -1;
	}
} // namespace

#endif // GPK_KEYVAL_VIEW_H_261022
