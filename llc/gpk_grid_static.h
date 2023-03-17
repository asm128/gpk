#include "gpk_array.h"
#include "gpk_view_grid.h"

#ifndef GPK_GRID_H_9802374982374
#define GPK_GRID_H_9802374982374

namespace gpk
{
	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public view2d<_tCell> {
							typedef										_tCell									TCell;
							typedef										view2d<_tCell>							TGridView;

		stacxpr	const uint32_t								Width									= (uint32_t)_sizeWidth;
		stacxpr	const uint32_t								Depth									= (uint32_t)_sizeDepth;

							TCell										Cells	[_sizeDepth][_sizeWidth]		= {};

		inlcxpr												grid_static								()																		: view2d<_tCell>(&Cells[0][0], _sizeWidth, _sizeDepth)	{}

							::gpk::error_t								read									(const byte_t* input, uint32_t* inout_bytesRead)						{
			ree_if(0 == input, "%s", "Invalid input pointer!");
			TGridView															inputGrid								= {(_tCell*)input, Width, Depth};
			*inout_bytesRead												+= sizeof(_tCell) * Width * Depth;
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::gpk::podcpy(&Cells[y][x], inputGrid[y][x]);
			}
			return 0;
		}

							::gpk::error_t								write									(byte_t* output, uint32_t* inout_bytesWritten)		const				{
			ree_if(0 == output && 0 == inout_bytesWritten, "%s", "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten												+= sizeof(_tCell) * Width * Depth;	// Just return the size required to store this.

			if(0 == output)
				return 0;

			TGridView															newStorage								= {(_tCell*)output, Width, Depth};
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::gpk::podcpy(output[y * Width + x], &Cells[y][x]);
			}
			return 0;
		}
	};
}

#endif // GPK_GRID_H_9802374982374
