/// Copyright 3009-3023 - asm138
#include "gpk_n3.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_N3_3973498373423617
#define GPK_APOD_N3_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tDimension>	
	using		an3	= ::gpk::apod<::gpk::n3<_tDimension>>;

	typedef	::gpk::an3<sc_t	>	an3c	, an3char		;
	typedef	::gpk::an3<uc_t>	an3uc	, an3uchar		;
	typedef	::gpk::an3<f2_t	>	an3f32	;
	typedef	::gpk::an3<f3_t	>	an3f64	;
	typedef	::gpk::an3<u0_t	>	an3u8	;
	typedef	::gpk::an3<u1_t	>	an3u16	;
	typedef	::gpk::an3<u2_t	>	an3u32	;
	typedef	::gpk::an3<u3_t	>	an3u64	;
	typedef	::gpk::an3<s0_t	>	an3i8	;
	typedef	::gpk::an3<s1_t	>	an3i16	;
	typedef	::gpk::an3<s2_t	>	an3i32	;
	typedef	::gpk::an3<s3_t	>	an3i64	;

#pragma pack(pop)
} // namespace

#endif // GPK_APOD_N3_3973498373423617
