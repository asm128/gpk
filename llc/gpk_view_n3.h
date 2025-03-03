/// Copyright 2009-2023 - asm128
#include "gpk_n3.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_N3_2973498273423617
#define GPK_VIEW_N3_2973498273423617

namespace gpk
{
#pragma pack(push, 1)

	tplt<tpnm _tDimension>	
	using	vn3		= ::gpk::view<::gpk::n3<_tDimension>>;

	tplt<tpnm _tDimension>	
	using	vcn3	= ::gpk::view<const ::gpk::n3<_tDimension>>;

	typedef	::gpk::vn3<sc_t >	vn3c	, vn3char	;
	typedef	::gpk::vn3<uc_t>	vn3uc	, vn3uchar	;
	typedef	::gpk::vn3<f2_t  >	vn3f32	;
	typedef	::gpk::vn3<f3_t  >	vn3f64	;
	typedef	::gpk::vn3<u0_t   > 	vn3u8	;
	typedef	::gpk::vn3<u1_t  >	vn3u16	;
	typedef	::gpk::vn3<u2_t  >	vn3u32	;
	typedef	::gpk::vn3<u3_t  >	vn3u64	;
	typedef	::gpk::vn3<s0_t   > 	vn3i8	;
	typedef	::gpk::vn3<s1_t  >	vn3i16	;
	typedef	::gpk::vn3<s2_t  >	vn3i32	;
	typedef	::gpk::vn3<s3_t  >	vn3i64	;

	typedef	::gpk::vcn3<sc_t >	vc3c	;
	typedef	::gpk::vcn3<uc_t>	vc3uc	;
	typedef	::gpk::vcn3<f2_t  >	vc3f32	;
	typedef	::gpk::vcn3<f3_t  >	vc3f64	;
	typedef	::gpk::vcn3<u0_t   > 	vc3u8	;
	typedef	::gpk::vcn3<u1_t  >	vc3u16	;
	typedef	::gpk::vcn3<u2_t  >	vc3u32	;
	typedef	::gpk::vcn3<u3_t  >	vc3u64	;
	typedef	::gpk::vcn3<s0_t   > 	vc3i8	;
	typedef	::gpk::vcn3<s1_t  >	vc3i16	;
	typedef	::gpk::vcn3<s2_t  >	vc3i32	;
	typedef	::gpk::vcn3<s3_t  >	vc3i64	;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_N3_2973498273423617
