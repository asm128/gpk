/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_N2_3973498373423617
#define GPK_APOD_N2_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::n2char	>	an2c	, an2char		;
	typedef	::gpk::apod<::gpk::n2uchar	>	an2uc	, an2uchar		;
	typedef	::gpk::apod<::gpk::n2f32	>	an2f32	;
	typedef	::gpk::apod<::gpk::n2f64	>	an2f64	;
	typedef	::gpk::apod<::gpk::n2u8		>	an2u8	;
	typedef	::gpk::apod<::gpk::n2u16	>	an2u16	;
	typedef	::gpk::apod<::gpk::n2u32	>	an2u32	;
	typedef	::gpk::apod<::gpk::n2u64	>	an2u64	;
	typedef	::gpk::apod<::gpk::n2i8		>	an2i8	;
	typedef	::gpk::apod<::gpk::n2i16	>	an2i16	;
	typedef	::gpk::apod<::gpk::n2i32	>	an2i32	;
	typedef	::gpk::apod<::gpk::n2i64	>	an2i64	;

	template<typename _tDimension>	
	using		an2	= ::gpk::apod<::gpk::n2<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_N2_3973498373423617
