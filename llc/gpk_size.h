#include "gpk_typeint.h"

#ifndef GPK_SIZE_H_23627
#define GPK_SIZE_H_23627

namespace gpk
{
	tplt<tpnm _t>										stincxp	uint32_t	size		(const uint32_t instanceCount)							noexcept	{ return sizeof(_t) * instanceCount;				}
	tplt<tpnm _t, size_t _arraySize>					stincxp	uint32_t	size		(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return _arraySize;								}
	tplt<tpnm _t, size_t _sizeDepth, size_t _sizeWidth>	stincxp	uint32_t	size		(const _t (&/*staticArray*/)[_sizeWidth][_sizeDepth])	noexcept	{ return _sizeDepth * _sizeWidth;					}
	tplt<tpnm _t, size_t _arraySize>					stincxp	uint32_t	byte_count	(const _t (&/*staticArray*/)[_arraySize])				noexcept	{ return ::gpk::size<_t>(_arraySize);				}
	tplt<tpnm _t, size_t _sizeDepth, size_t _sizeWidth>	stincxp	uint32_t	byte_count	(const _t (&/*staticArray*/)[_sizeWidth][_sizeDepth])	noexcept	{ return ::gpk::size<_t>(_sizeDepth*_sizeWidth);	}
}

#endif // GPK_SIZE_H_23627
