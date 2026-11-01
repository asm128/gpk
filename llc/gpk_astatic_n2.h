/// Copyright 2009-2023 - asm128
#include "llc_n2.h"
#include "llc_array_static.h"

#ifndef LLC_ASTATIC_N2_H
#define LLC_ASTATIC_N2_H

namespace llc
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension, size_t _nSize>
	using		ast2		= ::llc::astatic<::llc::n2<_tDimension>, _nSize>;

	tplt<size_t _nSize>	using ast2sc_t	= ::llc::ast2<sc_t, _nSize>	;
	tplt<size_t _nSize>	using ast2uc_t	= ::llc::ast2<uc_t, _nSize>	;
	tplt<size_t _nSize>	using ast2f2_t	= ::llc::ast2<f2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2f3_t	= ::llc::ast2<f3_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u0_t	= ::llc::ast2<u0_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u1_t	= ::llc::ast2<u1_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u2_t	= ::llc::ast2<u2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2u3_t	= ::llc::ast2<u3_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s0_t	= ::llc::ast2<s0_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s1_t	= ::llc::ast2<s1_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s2_t	= ::llc::ast2<s2_t, _nSize>	;
	tplt<size_t _nSize>	using ast2s3_t	= ::llc::ast2<s3_t, _nSize>	;
#pragma pack(pop)
} // namespace

#endif // LLC_ASTATIC_N2_H
