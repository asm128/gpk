/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_N3_2973498273423617
#define GPK_VIEW_N3_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::n3char	>	vn3c	, vn3char	;
	typedef	::gpk::view<::gpk::n3uchar	>	vn3uc	, vn3uchar	;
	typedef	::gpk::view<::gpk::n3f32	>	vn3f32	;
	typedef	::gpk::view<::gpk::n3f64	>	vn3f64	;
	typedef	::gpk::view<::gpk::n3u8		>	vn3u8	;
	typedef	::gpk::view<::gpk::n3u16	>	vn3u16	;
	typedef	::gpk::view<::gpk::n3u32	>	vn3u32	;
	typedef	::gpk::view<::gpk::n3u64	>	vn3u64	;
	typedef	::gpk::view<::gpk::n3i8		>	vn3i8	;
	typedef	::gpk::view<::gpk::n3i16	>	vn3i16	;
	typedef	::gpk::view<::gpk::n3i32	>	vn3i32	;
	typedef	::gpk::view<::gpk::n3i64	>	vn3i64	;

	typedef	::gpk::view<const ::gpk::n3char	>	vc3c	;
	typedef	::gpk::view<const ::gpk::n3uchar>	vc3uc	;
	typedef	::gpk::view<const ::gpk::n3f32	>	vc3f32	;
	typedef	::gpk::view<const ::gpk::n3f64	>	vc3f64	;
	typedef	::gpk::view<const ::gpk::n3u8	>	vc3u8	;
	typedef	::gpk::view<const ::gpk::n3u16	>	vc3u16	;
	typedef	::gpk::view<const ::gpk::n3u32	>	vc3u32	;
	typedef	::gpk::view<const ::gpk::n3u64	>	vc3u64	;
	typedef	::gpk::view<const ::gpk::n3i8	>	vc3i8	;
	typedef	::gpk::view<const ::gpk::n3i16	>	vc3i16	;
	typedef	::gpk::view<const ::gpk::n3i32	>	vc3i32	;
	typedef	::gpk::view<const ::gpk::n3i64	>	vc3i64	;

	template<typename _tDimension>	
	using	vn3		= ::gpk::view<::gpk::n3<_tDimension>>;

	template<typename _tDimension>	
	using	vcn3	= ::gpk::view<const ::gpk::n3<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_N3_2973498273423617
