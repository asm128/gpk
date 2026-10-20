/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_array_static.h"

#ifndef GPK_ASTATIC_N2_H
#define GPK_ASTATIC_N2_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension, size_t _nSize>
	using		ast2		= ::gpk::astatic<::gpk::n2<_tDimension>, _nSize>;

	tplt<size_t _nSize>	using ast2sc_t	= ::gpk::ast2<sc_t, _nSize>	;
	tplt<size_t _nSize>	using ast2uc_t	= ::gpk::ast2<uc_t, _nSize>	;
	tplt<size_t _nSize>	using ast2f2_t	= ::gpk::ast2<f2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2f3_t	= ::gpk::ast2<f3_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u0_t	= ::gpk::ast2<u0_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u1_t	= ::gpk::ast2<u1_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u2_t	= ::gpk::ast2<u2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u3_t	= ::gpk::ast2<u3_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s0_t	= ::gpk::ast2<s0_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s1_t	= ::gpk::ast2<s1_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s2_t	= ::gpk::ast2<s2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s3_t	= ::gpk::ast2<s3_t, _nSize>	;
#pragma pack(pop)
} // namespace

#endif // GPK_ASTATIC_N2_H


