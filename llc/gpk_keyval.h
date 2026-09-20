#include "gpk_view.h"

#ifndef GPK_KEYVAL_H_26920
#define GPK_KEYVAL_H_26920

namespace gpk
{
	tplt<tpnm _tKey, tpnm _tVal>
	stct keyval {
		tydf	_tKey			TKey;
		tydf	_tVal			TVal;
		tydf	keyval<TKey, TVal>	TKeyVal;

		TKey				Key	= {};
		TVal				Val	= {};

		GPK_DEFAULT_OPERATOR(TKeyVal, Key == other.Key && Val == other.Val);
	};

	tplt<tpnm _tVal> using kvvcst_t = keyval<vcst_t, _tVal>;
	tplt<tpnm _tVal> using kvu0_t   = keyval<u0_t  , _tVal>;
	tplt<tpnm _tVal> using kvu1_t   = keyval<u1_t  , _tVal>;
	tplt<tpnm _tVal> using kvu2_t   = keyval<u2_t  , _tVal>;
	tplt<tpnm _tVal> using kvu3_t   = keyval<u3_t  , _tVal>;
}

#endif // GPK_KEYVAL_H_26920
