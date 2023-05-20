#include "gpk_typeint.h"

#ifndef GPK_SIZE_H_928374982374
#define GPK_SIZE_H_928374982374

namespace gpk
{
	template<typename _t>										stincxp	uint32_t	size		(const uint32_t instanceCount)							noexcept	{ return sizeof(_t) * instanceCount;				}
	template<typename _t, size_t _arraySize>					stincxp	uint32_t	size		(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return _arraySize;								}
	template<typename _t, size_t _sizeDepth, size_t _sizeWidth>	stincxp	uint32_t	size		(const _t (&/*staticArray*/)[_sizeDepth][_sizeWidth])	noexcept	{ return _sizeDepth * _sizeWidth;					}
	template<typename _t, size_t _arraySize>					stincxp	uint32_t	byte_count	(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return ::gpk::size<_t>(_arraySize);				}
	template<typename _t, size_t _sizeDepth, size_t _sizeWidth>	stincxp	uint32_t	byte_count	(const _t (&/*staticArray*/)[_sizeDepth][_sizeWidth])	noexcept	{ return ::gpk::size<_t>(_sizeDepth*_sizeWidth);	}
}

#endif // GPK_SIZE_H_928374982374
