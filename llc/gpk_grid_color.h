#include "gpk_view_color.h"
#include "gpk_view_grid.h"

#ifndef GPK_GRID_COLOR_H_90238749823611
#define GPK_GRID_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::view2d<::gpk::rgbaf	>		v2rgbaf	, v2frgba	;
	typedef	::gpk::view2d<::gpk::bgra	>		v2bgra	, v28bgra	;
	typedef	::gpk::view2d<::gpk::bgr	>		v2bgr	, v28bgr	;
	typedef	::gpk::view2d<::gpk::rgba	>		v2rgba	, v28rgba	;
	typedef	::gpk::view2d<::gpk::rgb	>		v2rgb	, v28rgb	;

	typedef	::gpk::view2d<const ::gpk::rgbaf>	v2crgbaf, v2cfrgba	;
	typedef	::gpk::view2d<const ::gpk::bgra	>	v2cbgra	, v2c8bgra	;
	typedef	::gpk::view2d<const ::gpk::bgr	>	v2cbgr	, v2c8bgr	;
	typedef	::gpk::view2d<const ::gpk::rgba	>	v2crgba	, v2c8rgba	;
	typedef	::gpk::view2d<const ::gpk::rgb	>	v2crgb	, v2c8rgb	;
} // namespace

#endif // GPK_GRID_COLOR_H_90238749823611
