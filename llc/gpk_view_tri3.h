/// Copyright 2009-2023 - asm128
#include "gpk_tri3.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_TRI3_2973498273423617
#define GPK_VIEW_TRI3_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::tri3char	>	vtri3c	, vtri3char ;
	typedef	::gpk::view<::gpk::tri3uchar>	vtri3uc	, vtri3uchar;
	typedef	::gpk::view<::gpk::tri3f32	>	vtri3f32	;
	typedef	::gpk::view<::gpk::tri3f64	>	vtri3f64	;
	typedef	::gpk::view<::gpk::tri3u8	>	vtri3ui8	;
	typedef	::gpk::view<::gpk::tri3u16	>	vtri3ui16	;
	typedef	::gpk::view<::gpk::tri3u32	>	vtri3ui32	;
	typedef	::gpk::view<::gpk::tri3u64	>	vtri3ui64	;
	typedef	::gpk::view<::gpk::tri3i8	>	vtri3i8		;
	typedef	::gpk::view<::gpk::tri3i16	>	vtri3i16	;
	typedef	::gpk::view<::gpk::tri3i32	>	vtri3i32	;
	typedef	::gpk::view<::gpk::tri3i64	>	vtri3i64	;

	typedef	::gpk::view<const ::gpk::tri3char	>	vctri3c		;
	typedef	::gpk::view<const ::gpk::tri3uchar	>	vctri3uc	;
	typedef	::gpk::view<const ::gpk::tri3f32	>	vctri3f32	;
	typedef	::gpk::view<const ::gpk::tri3f64	>	vctri3f64	;
	typedef	::gpk::view<const ::gpk::tri3u8		>	vctri3ui8	;
	typedef	::gpk::view<const ::gpk::tri3u16	>	vctri3ui16	;
	typedef	::gpk::view<const ::gpk::tri3u32	>	vctri3ui32	;
	typedef	::gpk::view<const ::gpk::tri3u64	>	vctri3ui64	;
	typedef	::gpk::view<const ::gpk::tri3i8		>	vctri3i8	;
	typedef	::gpk::view<const ::gpk::tri3i16	>	vctri3i16	;
	typedef	::gpk::view<const ::gpk::tri3i32	>	vctri3i32	;
	typedef	::gpk::view<const ::gpk::tri3i64	>	vctri3i64	;

	template<typename _tDimension>	
	using	vtri3	= ::gpk::view<::gpk::tri3<_tDimension>>;

	template<typename _tDimension>	
	using	vctri3	= ::gpk::view<const ::gpk::tri3<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_TRI3_2973498273423617
