#include "gpk_array.h"
#include "gpk_grid.h"

#ifndef GPK_GRID_H_23627
#define GPK_GRID_H_23627

namespace gpk
{
	tplt<tpnm T, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public grid<T> {
		typedef	T			TCell;
		typedef	grid<T>		TGridView;

		stxp	u2_t	Width									= (u2_t)_sizeWidth;
		stxp	u2_t	Depth									= (u2_t)_sizeDepth;

				TCell		Cells	[_sizeDepth][_sizeWidth]		= {};

		inxp				grid_static								()																		: grid<T>(&Cells[0][0], _sizeWidth, _sizeDepth)	{}

	};
}

#endif // GPK_GRID_H_23627
