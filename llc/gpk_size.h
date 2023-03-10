#include "gpk_typeint.h"

#ifndef GPK_SIZE_H_928374982374
#define GPK_SIZE_H_928374982374

namespace gpk
{
	template <typename _t, size_t _arraySize>						stainli constexpr uint32_t	size		(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return _arraySize;							}
	template <typename _t, size_t _sizeDepth, size_t _sizeWidth>	stainli constexpr uint32_t	size		(const _t (&/*staticArray*/)[_sizeDepth][_sizeWidth])	noexcept	{ return _sizeDepth * _sizeWidth;				}
	template <typename _t, size_t _arraySize>						stainli constexpr uint32_t	byte_count	(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return sizeof(_t) * _arraySize;				}
	template <typename _t, size_t _sizeDepth, size_t _sizeWidth>	stainli constexpr uint32_t	byte_count	(const _t (&/*staticArray*/)[_sizeDepth][_sizeWidth])	noexcept	{ return sizeof(_t) * _sizeDepth*_sizeWidth;	}
}

#endif // GPK_SIZE_H_928374982374
