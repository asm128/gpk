/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_N2_2973498273423617
#define GPK_VIEW_N2_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::n2char	>	vn2c	, vn2char	;
	typedef	::gpk::view<::gpk::n2uchar	>	vn2uc	, vn2uchar	;
	typedef	::gpk::view<::gpk::n2f32	>	vn2f32	;
	typedef	::gpk::view<::gpk::n2f64	>	vn2f64	;
	typedef	::gpk::view<::gpk::n2u8		>	vn2u8	;
	typedef	::gpk::view<::gpk::n2u16	>	vn2u16	;
	typedef	::gpk::view<::gpk::n2u32	>	vn2u32	;
	typedef	::gpk::view<::gpk::n2u64	>	vn2u64	;
	typedef	::gpk::view<::gpk::n2i8		>	vn2i8	;
	typedef	::gpk::view<::gpk::n2i16	>	vn2i16	;
	typedef	::gpk::view<::gpk::n2i32	>	vn2i32	;
	typedef	::gpk::view<::gpk::n2i64	>	vn2i64	;

	typedef	::gpk::view<const ::gpk::n2char	>	vc2c	;
	typedef	::gpk::view<const ::gpk::n2uchar>	vc2uc	;
	typedef	::gpk::view<const ::gpk::n2f32	>	vc2f32	;
	typedef	::gpk::view<const ::gpk::n2f64	>	vc2f64	;
	typedef	::gpk::view<const ::gpk::n2u8	>	vc2u8	;
	typedef	::gpk::view<const ::gpk::n2u16	>	vc2u16	;
	typedef	::gpk::view<const ::gpk::n2u32	>	vc2u32	;
	typedef	::gpk::view<const ::gpk::n2u64	>	vc2u64	;
	typedef	::gpk::view<const ::gpk::n2i8	>	vc2i8	;
	typedef	::gpk::view<const ::gpk::n2i16	>	vc2i16	;
	typedef	::gpk::view<const ::gpk::n2i32	>	vc2i32	;
	typedef	::gpk::view<const ::gpk::n2i64	>	vc2i64	;

	tplt<tpnm _tDimension>	
	using	vn2		= ::gpk::view<::gpk::n2<_tDimension>>;

	tplt<tpnm _tDimension>	
	using	vcn2	= ::gpk::view<const ::gpk::n2<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_N2_2973498273423617
