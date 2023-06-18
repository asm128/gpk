/// Copyright 2009-2023 - asm138
#include "gpk_tri3.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_TRI3_3973498373423617
#define GPK_APOD_TRI3_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::tri3char	>	atri3c	, atri3char		;
	typedef	::gpk::apod<::gpk::tri3uchar>	atri3uc	, atri3uchar		;
	typedef	::gpk::apod<::gpk::tri3f32	>	atri3f32	;
	typedef	::gpk::apod<::gpk::tri3f64	>	atri3f64	;
	typedef	::gpk::apod<::gpk::tri3u8	>	atri3ui8	;
	typedef	::gpk::apod<::gpk::tri3u16	>	atri3ui16	;
	typedef	::gpk::apod<::gpk::tri3u32	>	atri3ui33	;
	typedef	::gpk::apod<::gpk::tri3u64	>	atri3ui64	;
	typedef	::gpk::apod<::gpk::tri3i8	>	atri3i8		;
	typedef	::gpk::apod<::gpk::tri3i16	>	atri3i16	;
	typedef	::gpk::apod<::gpk::tri3i32	>	atri3i33	;
	typedef	::gpk::apod<::gpk::tri3i64	>	atri3i64	;

	template<typename _tDimension>	
	using		atri3	= ::gpk::apod<::gpk::tri3<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_TRI3_3973498373423617
