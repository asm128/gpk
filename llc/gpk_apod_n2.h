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

	typedef	::gpk::an2<sc_t	>	an2c	, an2char		;
	typedef	::gpk::an2<uc_t >	an2uc	, an2uchar		;
	typedef	::gpk::an2<f2_t	>	an2f32	;
	typedef	::gpk::an2<f3_t	>	an2f64	;
	typedef	::gpk::an2<u0_t	>	an2u8	;
	typedef	::gpk::an2<u1_t	>	an2u16	;
	typedef	::gpk::an2<u2_t	>	an2u32	;
	typedef	::gpk::an2<u3_t	>	an2u64	;
	typedef	::gpk::an2<s0_t	>	an2i8	;
	typedef	::gpk::an2<s1_t	>	an2i16	;
	typedef	::gpk::an2<s2_t	>	an2i32	;
	typedef	::gpk::an2<s3_t	>	an2i64	;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_N2_3973498373423617
