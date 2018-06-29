#include "gpk_view_grid.h"

#ifndef GPK_GRID_SCALE_20654834
#define GPK_GRID_SCALE_20654834

namespace gpk
{
	template<typename _tCell>
						::gpk::error_t							grid_scale_fast					(::gpk::view_grid<_tCell>& dst, const ::gpk::view_grid<_tCell>& src, const float xFactor, const float yFactor)		{
		for(uint32_t y = 0; y < dst.height(); ++y) 
			for(uint32_t x = 0; x < dst.width(); ++x) 
				dst[y][x]													= src[(uint32_t)(y * yFactor)][(uint32_t)(x * xFactor)];
		return 0;
	}

	template<typename _tCell>
						::gpk::error_t							grid_scale					(::gpk::view_grid<_tCell>& dst, const ::gpk::view_grid<_tCell>& src)													{
		const float														xFactor						= src.width () / (float)dst.width ();
		const float														yFactor						= src.height() / (float)dst.height();
		return grid_scale_fast(dst, src, xFactor, yFactor);
	}

	template<typename _tCell>
						::gpk::error_t							grid_scale					(::gpk::view_grid<_tCell>& dst, const ::gpk::view_grid<_tCell>& src, const float xFactor, const float yFactor)			{
		for(uint32_t y = 0; y < dst.height(); ++y) 
			for(uint32_t x = 0; x < dst.width(); ++x) { 
				const uint32_t													xSrc						= (uint32_t)(x * yFactor);
				const uint32_t													ySrc						= (uint32_t)(y * xFactor);
				if( ::gpk::in_range(ySrc, 0, src.height())
				 && ::gpk::in_range(xSrc, 0, src.width ())
				 )
					dst[y][x]													= src[ySrc][xSrc];
			}
		return 0;
	}
}

#endif // GPK_GRID_SCALE_20983429834
