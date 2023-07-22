/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_N2_3973498373423617
#define GPK_APOD_N2_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension>	
	using		an2		= ::gpk::apod<::gpk::n2<_tDimension>>;

	typedef	::gpk::an2<char	>	an2c	, an2char		;
	typedef	::gpk::an2<uchar>	an2uc	, an2uchar		;
	typedef	::gpk::an2<f32	>	an2f32	;
	typedef	::gpk::an2<f64	>	an2f64	;
	typedef	::gpk::an2<u8	>	an2u8	;
	typedef	::gpk::an2<u16	>	an2u16	;
	typedef	::gpk::an2<u32	>	an2u32	;
	typedef	::gpk::an2<u64	>	an2u64	;
	typedef	::gpk::an2<i8	>	an2i8	;
	typedef	::gpk::an2<i16	>	an2i16	;
	typedef	::gpk::an2<i32	>	an2i32	;
	typedef	::gpk::an2<i64	>	an2i64	;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_N2_3973498373423617
