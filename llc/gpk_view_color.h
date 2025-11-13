#include "gpk_color.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_COLOR_H_23627
#define GPK_VIEW_COLOR_H_23627

namespace gpk
{
	tydf	::gpk::view<::gpk::frgba	>		vrgbaf	, vfrgba	;
	tydf	::gpk::view<::gpk::u8bgra	>		vbgra	, v8bgra	;
	tydf	::gpk::view<::gpk::u8bgr	>		vbgr	, v8bgr		;
	tydf	::gpk::view<::gpk::u8rgba	>		vrgba	, v8rgba	;
	tydf	::gpk::view<::gpk::u8rgb	>		vrgb	, v8rgb		;

	tydf	::gpk::view<const ::gpk::frgba	>	vcrgbaf	, vcfrgba	;
	tydf	::gpk::view<const ::gpk::u8bgra	>	vcbgra	, vc8bgra	;
	tydf	::gpk::view<const ::gpk::u8bgr	>	vcbgr	, vc8bgr		;
	tydf	::gpk::view<const ::gpk::u8rgba	>	vcrgba	, vc8rgba	;
	tydf	::gpk::view<const ::gpk::u8rgb	>	vcrgb	, vc8rgb		;

	::gpk::error_t 		vcu82bgra8	(vcu0_c & bytes, ::gpk::bgra8 & colorToSet);
	::gpk::error_t 		vcu82bgr8	(vcu0_c & bytes, ::gpk::bgr8 & colorToSet);
} // namespace

#endif // GPK_VIEW_COLOR_H_23627
