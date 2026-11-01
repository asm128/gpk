#include "gpk_typeint.h"

#ifndef GPK_SIZE_H_23627
#define GPK_SIZE_H_23627

namespace gpk
{
	tplt<tpnm T>										sinx	::gpk::u2_t	size		(const T & sized)										noexcept	{ return sized.size();							}
	tplt<tpnm _t, size_t _arraySize>					sinx	::gpk::u2_t	size		(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return _arraySize;							}
	tplt<tpnm _t, size_t _arraySize>					sinx	::gpk::u2_t	byte_count	(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return szof(_t) * (_arraySize);				}
}

#endif // GPK_SIZE_H_23627
