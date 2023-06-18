#include "gpk_view_color.h"
#include "gpk_grid.h"

#ifndef GPK_GRID_COLOR_H_90238749823611
#define GPK_GRID_COLOR_H_90238749823611

namespace gpk
{
	typedef	::gpk::grid<::gpk::rgbaf>	gfrgba	;
	typedef	::gpk::grid<::gpk::bgra	>	g8bgra	;
	typedef	::gpk::grid<::gpk::bgr	>	g8bgr	;
	typedef	::gpk::grid<::gpk::rgba	>	g8rgba	;
	typedef	::gpk::grid<::gpk::rgb	>	g8rgb	;

	typedef	::gpk::grid<const ::gpk::rgbaf	>	gcfrgba	;
	typedef	::gpk::grid<const ::gpk::bgra	>	gc8bgra	;
	typedef	::gpk::grid<const ::gpk::bgr	>	gc8bgr	;
	typedef	::gpk::grid<const ::gpk::rgba	>	gc8rgba	;
	typedef	::gpk::grid<const ::gpk::rgb	>	gc8rgb	;
} // namespace

#endif // GPK_GRID_COLOR_H_90238749823611
