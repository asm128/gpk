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

	tplt<size_t _nSize>	using ast2char	= ::gpk::ast2<char	, _nSize>	;
	tplt<size_t _nSize>	using ast2uchar	= ::gpk::ast2<uchar	, _nSize>	;
	tplt<size_t _nSize>	using ast2c		= ::gpk::ast2<char	, _nSize>	;
	tplt<size_t _nSize>	using ast2uc	= ::gpk::ast2<uchar	, _nSize>	;
	tplt<size_t _nSize>	using ast2f32	= ::gpk::ast2<f32	, _nSize>	;
	tplt<size_t _nSize>	using ast2f64	= ::gpk::ast2<f64	, _nSize>	;
	tplt<size_t _nSize>	using ast2u8	= ::gpk::ast2<u8	, _nSize>	;
	tplt<size_t _nSize>	using ast2u16	= ::gpk::ast2<u16	, _nSize>	;
	tplt<size_t _nSize>	using ast2u32	= ::gpk::ast2<u32	, _nSize>	;
	tplt<size_t _nSize>	using ast2u64	= ::gpk::ast2<u64	, _nSize>	;
	tplt<size_t _nSize>	using ast2i8	= ::gpk::ast2<i8	, _nSize>	;
	tplt<size_t _nSize>	using ast2i16	= ::gpk::ast2<i16	, _nSize>	;
	tplt<size_t _nSize>	using ast2i32	= ::gpk::ast2<i32	, _nSize>	;
	tplt<size_t _nSize>	using ast2i64	= ::gpk::ast2<i64	, _nSize>	;
#pragma pack(pop)
} // namespace

#endif // GPK_ASTATIC_N2_H
