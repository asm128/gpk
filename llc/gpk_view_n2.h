/// Copyright 2009-2023 - asm128
#include "gpk_n2.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_N2_2973498273423617
#define GPK_VIEW_N2_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::n2sc_t>	vn2c	, vn2char	;
	typedef	::gpk::view<::gpk::n2uc_t>	vn2uc	, vn2uchar	;
	typedef	::gpk::view<::gpk::n2f2_t>	vn2f32	;
	typedef	::gpk::view<::gpk::n2f3_t>	vn2f64	;
	typedef	::gpk::view<::gpk::n2u0_t>	vn2u8	;
	typedef	::gpk::view<::gpk::n2u1_t>	vn2u16	;
	typedef	::gpk::view<::gpk::n2u2_t>	vn2u32	;
	typedef	::gpk::view<::gpk::n2u3_t>	vn2u64	;
	typedef	::gpk::view<::gpk::n2s0_t>	vn2i8	;
	typedef	::gpk::view<::gpk::n2s1_t>	vn2i16	;
	typedef	::gpk::view<::gpk::n2s2_t>	vn2i32	;
	typedef	::gpk::view<::gpk::n2s3_t>	vn2i64	;

	typedef	::gpk::view<const ::gpk::n2sc_t>	vc2c	;
	typedef	::gpk::view<const ::gpk::n2uc_t>	vc2uc	;
	typedef	::gpk::view<const ::gpk::n2f2_t>	vc2f32	;
	typedef	::gpk::view<const ::gpk::n2f3_t>	vc2f64	;
	typedef	::gpk::view<const ::gpk::n2u0_t>	vc2u8	;
	typedef	::gpk::view<const ::gpk::n2u1_t>	vc2u16	;
	typedef	::gpk::view<const ::gpk::n2u2_t>	vc2u32	;
	typedef	::gpk::view<const ::gpk::n2u3_t>	vc2u64	;
	typedef	::gpk::view<const ::gpk::n2s0_t>	vc2i8	;
	typedef	::gpk::view<const ::gpk::n2s1_t>	vc2i16	;
	typedef	::gpk::view<const ::gpk::n2s2_t>	vc2i32	;
	typedef	::gpk::view<const ::gpk::n2s3_t>	vc2i64	;

	tplt<tpnm _tDimension>	
	using	vn2		= ::gpk::view<::gpk::n2<_tDimension>>;

	tplt<tpnm _tDimension>	
	using	vcn2	= ::gpk::view<const ::gpk::n2<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_N2_2973498273423617
