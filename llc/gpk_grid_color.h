#include "gpk_view_color.h"
#include "gpk_view_grid.h"

#ifndef GPK_GRID_COLOR_H_90238749823611
#define GPK_GRID_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::view2d<::gpk::rgbaf	>		v2rgbaf	;
	typedef	::gpk::view2d<::gpk::bgra	>		v2bgra	;
	typedef	::gpk::view2d<::gpk::bgr	>		v2bgr	;
	typedef	::gpk::view2d<::gpk::rgba	>		v2rgba	;
	typedef	::gpk::view2d<::gpk::rgb	>		v2rgb	;

	typedef	::gpk::view2d<const ::gpk::rgbaf>	v2crgbaf;
	typedef	::gpk::view2d<const ::gpk::bgra	>	v2cbgra	;
	typedef	::gpk::view2d<const ::gpk::bgr	>	v2cbgr	;
	typedef	::gpk::view2d<const ::gpk::rgba	>	v2crgba	;
	typedef	::gpk::view2d<const ::gpk::rgb	>	v2crgb	;
} // namespace

#endif // GPK_GRID_COLOR_H_90238749823611
