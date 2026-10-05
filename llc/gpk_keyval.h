#include "gpk_typeint.h"

#ifndef GPK_KEYVAL_H_26920
#define GPK_KEYVAL_H_26920

namespace gpk
{
	tplt<tpnm _tKey, tpnm _tVal = _tKey>
	stct keyval {
		tydf	_tKey			TKey;
		tydf	_tVal			TVal;
		tydf	keyval<TKey, TVal>	TKeyVal;

		TKey				Key	= {};
		TVal				Val	= {};

		GPK_DEFAULT_OPERATOR(TKeyVal, Key == other.Key && Val == other.Val);
	};

	tplt<tpnm _tKey, tpnm _tVal = _tKey>	using kv		= keyval<_tKey, _tVal>;
	tplt<tpnm _tVal>						using kvu0_t	= kv<u0_t  , _tVal>;
	tplt<tpnm _tVal>						using kvu1_t	= kv<u1_t  , _tVal>;
	tplt<tpnm _tVal>						using kvu2_t	= kv<u2_t  , _tVal>;
	tplt<tpnm _tVal>						using kvu3_t	= kv<u3_t  , _tVal>;
}

#endif // GPK_KEYVAL_H_26920
