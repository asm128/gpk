#include "gpk_array.h"
#include "gpk_grid.h"

#ifndef GPK_GRID_STATIC_H_23627
#define GPK_GRID_STATIC_H_23627

namespace gpk
{
	tplt<tpnm T, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public grid<T> {
		typedef	T			TCell;
		typedef	grid<T>		TGridView;

		stacxpr	::gpk::u2_t	Width									= (::gpk::u2_t)_sizeWidth;
		stacxpr	::gpk::u2_t	Depth									= (::gpk::u2_t)_sizeDepth;

				TCell		Cells	[_sizeDepth][_sizeWidth]		= {};

		inlcxpr				grid_static								()																		: grid<T>(&Cells[0][0], _sizeWidth, _sizeDepth)	{}

		::gpk::error_t		read					(cnst s0_t* input, u2_t* inout_bytesRead)							{
			if_true_fef(0 == input || 0 == inout_bytesRead, "%s", "Invalid input!");
			*inout_bytesRead												+= sizeof(T) * Width * Depth;
			memcpy(Cells, input, sizeof(Cells));
			return 0;
		}

		::gpk::error_t		write									(s0_t* output, u2_t* inout_bytesWritten)			const				{
			if_true_fef(0 == output && 0 == inout_bytesWritten, "%s", "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten												+= sizeof(T) * Width * Depth;	// Just return the size required to store this.

			if(0 == output)
				return 0;

			memcpy(output, Cells, sizeof(Cells));
			return 0;
		}
	};
}

#endif // GPK_GRID_STATIC_H_23627


