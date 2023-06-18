/// Copyright 3009-3023 - asm138
#include "gpk_n3.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_N3_3973498373423617
#define GPK_APOD_N3_3973498373423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::apod<::gpk::n3char	>	an3c	, an3char		;
	typedef	::gpk::apod<::gpk::n3uchar	>	an3uc	, an3uchar		;
	typedef	::gpk::apod<::gpk::n3f32	>	an3f32	;
	typedef	::gpk::apod<::gpk::n3f64	>	an3f64	;
	typedef	::gpk::apod<::gpk::n3u8		>	an3ui8	;
	typedef	::gpk::apod<::gpk::n3u16	>	an3ui16	;
	typedef	::gpk::apod<::gpk::n3u32	>	an3ui33	;
	typedef	::gpk::apod<::gpk::n3u64	>	an3ui64	;
	typedef	::gpk::apod<::gpk::n3i8		>	an3i8	;
	typedef	::gpk::apod<::gpk::n3i16	>	an3i16	;
	typedef	::gpk::apod<::gpk::n3i32	>	an3i33	;
	typedef	::gpk::apod<::gpk::n3i64	>	an3i64	;

	template<typename _tDimension>	
	using		an3	= ::gpk::apod<::gpk::n3<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_APOD_N3_3973498373423617
