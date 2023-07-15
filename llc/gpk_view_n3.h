/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_N3_2973498273423617
#define GPK_VIEW_N3_2973498273423617

namespace gpk
{
#pragma pack(push, 1)

	template<typename _tDimension>	
	using	vn3		= ::gpk::view<::gpk::n3<_tDimension>>;

	template<typename _tDimension>	
	using	vcn3	= ::gpk::view<const ::gpk::n3<_tDimension>>;

	typedef	::gpk::vn3<char >	vn3c	, vn3char	;
	typedef	::gpk::vn3<uchar>	vn3uc	, vn3uchar	;
	typedef	::gpk::vn3<f32  >	vn3f32	;
	typedef	::gpk::vn3<f64  >	vn3f64	;
	typedef	::gpk::vn3<u8   > 	vn3u8	;
	typedef	::gpk::vn3<u16  >	vn3u16	;
	typedef	::gpk::vn3<u32  >	vn3u32	;
	typedef	::gpk::vn3<u64  >	vn3u64	;
	typedef	::gpk::vn3<i8   > 	vn3i8	;
	typedef	::gpk::vn3<i16  >	vn3i16	;
	typedef	::gpk::vn3<i32  >	vn3i32	;
	typedef	::gpk::vn3<i64  >	vn3i64	;

	typedef	::gpk::vcn3<char >	vc3c	;
	typedef	::gpk::vcn3<uchar>	vc3uc	;
	typedef	::gpk::vcn3<f32  >	vc3f32	;
	typedef	::gpk::vcn3<f64  >	vc3f64	;
	typedef	::gpk::vcn3<u8   > 	vc3u8	;
	typedef	::gpk::vcn3<u16  >	vc3u16	;
	typedef	::gpk::vcn3<u32  >	vc3u32	;
	typedef	::gpk::vcn3<u64  >	vc3u64	;
	typedef	::gpk::vcn3<i8   > 	vc3i8	;
	typedef	::gpk::vcn3<i16  >	vc3i16	;
	typedef	::gpk::vcn3<i32  >	vc3i32	;
	typedef	::gpk::vcn3<i64  >	vc3i64	;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_N3_2973498273423617
