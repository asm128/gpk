#include "gpk_color.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_COLOR_H_23627
#define GPK_VIEW_COLOR_H_23627

nmsp gpk
{
	tydf	view<frgba	>		vrgbaf	, vfrgba	;
	tydf	view<u8bgra	>		vbgra	, v8bgra	;
	tydf	view<u8bgr	>		vbgr	, v8bgr		;
	tydf	view<u8rgba	>		vrgba	, v8rgba	;
	tydf	view<u8rgb	>		vrgb	, v8rgb		;

	tydf	view<cnst frgba	>	vcrgbaf	, vcfrgba	;
	tydf	view<cnst u8bgra>	vcbgra	, vc8bgra	;
	tydf	view<cnst u8bgr	>	vcbgr	, vc8bgr		;
	tydf	view<cnst u8rgba>	vcrgba	, vc8rgba	;
	tydf	view<cnst u8rgb	>	vcrgb	, vc8rgb		;

	err_t 	vcu82bgra8	(vcu0_t bytes, bgra8 & colorToSet);
	err_t 	vcu82bgr8	(vcu0_t bytes, bgr8 & colorToSet);
} // nmsp

#endif // GPK_VIEW_COLOR_H_23627
