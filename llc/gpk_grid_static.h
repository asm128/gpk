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

		gpk::err_t 	read					(const s0_t* input, u2_t* inout_bytesRead)						{
			if_true_vef(-1, 0 == input, "%s", "Invalid input pointer!");
			TGridView															inputGrid								= {(T*)input, Width, Depth};
			*inout_bytesRead												+= sizeof(T) * Width * Depth;
			for(u2_t y = 0; y < Depth; ++y) {
				for(u2_t x = 0; x < Width; ++x)
					::gpk::podcpy(&Cells[y][x], inputGrid[y][x]);
			}
			return 0;
		}

		gpk::err_t 	write									(s0_t* output, u2_t* inout_bytesWritten)		const				{
			if_true_vef(-1, 0 == output && 0 == inout_bytesWritten, "%s", "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten												+= sizeof(T) * Width * Depth;	// Just return the size required to store this.

			if(0 == output)
				return 0;

			TGridView															newStorage								= {(T*)output, Width, Depth};
			for(u2_t y = 0; y < Depth; ++y) {
				for(u2_t x = 0; x < Width; ++x)
					::gpk::podcpy(output[y * Width + x], &Cells[y][x]);
			}
			return 0;
		}
	};
}

#endif // GPK_GRID_H_23627
