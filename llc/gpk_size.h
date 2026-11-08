#include "gpk_typeint.h"

#ifndef GPK_SIZE_H_23627
#define GPK_SIZE_H_23627

nmsp gpk
{
	tplt<tpnm T>										sinx	::gpk::u2_t	size		(const T & sized)										nxpt	{ return sized.size();							}
	tplt<tpnm _t, size_t _arraySize>					sinx	::gpk::u2_t	size		(const _t (&/*staticArray*/)[_arraySize])				nxpt	{ return _arraySize;							}
	tplt<tpnm _t, size_t _arraySize>					sinx	::gpk::u2_t	byte_count	(const _t (&/*staticArray*/)[_arraySize])				nxpt	{ return szof(_t) * (_arraySize);				}
}

#endif // GPK_SIZE_H_23627
