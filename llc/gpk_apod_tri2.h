/// Copyright 2009-2023 - asm138
#include "gpk_tri2.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_TRI2_3973498373423617
#define GPK_APOD_TRI2_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::tri2char	>	atri2c	, atri2char		;
	typedef	::gpk::apod<::gpk::tri2uchar>	atri2uc	, atri2uchar		;
	typedef	::gpk::apod<::gpk::tri2f32	>	atri2f32	;
	typedef	::gpk::apod<::gpk::tri2f64	>	atri2f64	;
	typedef	::gpk::apod<::gpk::tri2u8	>	atri2u8		;
	typedef	::gpk::apod<::gpk::tri2u16	>	atri2u16	;
	typedef	::gpk::apod<::gpk::tri2u32	>	atri2u32	;
	typedef	::gpk::apod<::gpk::tri2u64	>	atri2u64	;
	typedef	::gpk::apod<::gpk::tri2i8	>	atri2i8		;
	typedef	::gpk::apod<::gpk::tri2i16	>	atri2i16	;
	typedef	::gpk::apod<::gpk::tri2i32	>	atri2i32	;
	typedef	::gpk::apod<::gpk::tri2i64	>	atri2i64	;

	template<typename _tDimension>	
	using		atri2	= ::gpk::apod<::gpk::tri2<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_TRI2_3973498373423617
