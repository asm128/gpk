#include "gpk_color.h"
#include "gpk_grid.h"

#ifndef GPK_GRID_COLOR_H_23627
#define GPK_GRID_COLOR_H_23627

nmsp gpk
{
	tydf	::gpk::grid<::gpk::rgbaf>	gfrgba	;
	tydf	::gpk::grid<::gpk::bgra	>	g8bgra	;
	tydf	::gpk::grid<::gpk::bgr	>	g8bgr	;
	tydf	::gpk::grid<::gpk::rgba	>	g8rgba	;
	tydf	::gpk::grid<::gpk::rgb	>	g8rgb	;

	tydf	::gpk::grid<cnst ::gpk::rgbaf	>	gcfrgba	;
	tydf	::gpk::grid<cnst ::gpk::bgra	>	gc8bgra	;
	tydf	::gpk::grid<cnst ::gpk::bgr	>	gc8bgr	;
	tydf	::gpk::grid<cnst ::gpk::rgba	>	gc8rgba	;
	tydf	::gpk::grid<cnst ::gpk::rgb	>	gc8rgb	;
} // nmsp

#endif // GPK_GRID_COLOR_H_23627
