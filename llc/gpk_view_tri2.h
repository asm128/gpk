/// Copyright 2009-2023 - asm128
#include "gpk_tri2.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_TRI2_2973498273423617
#define GPK_VIEW_TRI2_2973498273423617

namespace gpk
{
#pragma pack(push, 1)
	typedef	::gpk::view<::gpk::tri2char	>	vtri2c  , vtri2char ;
	typedef	::gpk::view<::gpk::tri2uchar>	vtri2uc , vtri2uchar;
	typedef	::gpk::view<::gpk::tri2f32	>	vtri2f32;
	typedef	::gpk::view<::gpk::tri2f64	>	vtri2f64;
	typedef	::gpk::view<::gpk::tri2u8	>	vtri2u8 ;
	typedef	::gpk::view<::gpk::tri2u16	>	vtri2u16;
	typedef	::gpk::view<::gpk::tri2u32	>	vtri2u32;
	typedef	::gpk::view<::gpk::tri2u64	>	vtri2u64;
	typedef	::gpk::view<::gpk::tri2i8	>	vtri2i8 ;
	typedef	::gpk::view<::gpk::tri2i16	>	vtri2i16;
	typedef	::gpk::view<::gpk::tri2i32	>	vtri2i32;
	typedef	::gpk::view<::gpk::tri2i64	>	vtri2i64;

	typedef	::gpk::view<const ::gpk::tri2char	>	vctri2c  ;
	typedef	::gpk::view<const ::gpk::tri2uchar	>	vctri2uc ;
	typedef	::gpk::view<const ::gpk::tri2f32	>	vctri2f32;
	typedef	::gpk::view<const ::gpk::tri2f64	>	vctri2f64;
	typedef	::gpk::view<const ::gpk::tri2u8		>	vctri2u8 ;
	typedef	::gpk::view<const ::gpk::tri2u16	>	vctri2u16;
	typedef	::gpk::view<const ::gpk::tri2u32	>	vctri2u32;
	typedef	::gpk::view<const ::gpk::tri2u64	>	vctri2u64;
	typedef	::gpk::view<const ::gpk::tri2i8		>	vctri2i8 ;
	typedef	::gpk::view<const ::gpk::tri2i16	>	vctri2i16;
	typedef	::gpk::view<const ::gpk::tri2i32	>	vctri2i32;
	typedef	::gpk::view<const ::gpk::tri2i64	>	vctri2i64;

	tplt<tpnm _tDimension>	
	using	vtri2		= ::gpk::view<::gpk::tri2<_tDimension>>;

	tplt<tpnm _tDimension>	
	using	vctri2	= ::gpk::view<const ::gpk::tri2<_tDimension>>;
#pragma pack(pop)
} // namespace

#endif // GPK_VIEW_TRI2_2973498273423617
