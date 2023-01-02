#include "gpk_view_grid.h"
#include "gpk_color.h"

#ifndef GPK_GRID_SCALE_20654834
#define GPK_GRID_SCALE_20654834

namespace gpk
{
	template<typename _tCell>
						::gpk::error_t							grid_scale_fast					(::gpk::view2d<_tCell>& dst, const ::gpk::view2d<_tCell>& src, const float xFactor, const float yFactor)		{
		for(uint32_t y = 0; y < dst.metrics().y; ++y)
		for(uint32_t x = 0; x < dst.metrics().x; ++x)
			dst[y][x]													= src[(uint32_t)(y * yFactor)][(uint32_t)(x * xFactor)];
		return 0;
	}

	template<typename _tCell>
						::gpk::error_t							grid_scale					(::gpk::view2d<_tCell>& dst, const ::gpk::view2d<_tCell>& src)													{
		const float														xFactor						= src.metrics().x / (float)dst.metrics().x;
		const float														yFactor						= src.metrics().y / (float)dst.metrics().y;
		return grid_scale_fast(dst, src, xFactor, yFactor);
	}

	template<typename _tCell>
						::gpk::error_t							grid_scale					(::gpk::view2d<_tCell>& dst, const ::gpk::view2d<_tCell>& src, const float xFactor, const float yFactor)			{
		for(uint32_t y = 0; y < dst.metrics().y; ++y)
		for(uint32_t x = 0; x < dst.metrics().x; ++x) {
			const uint32_t													xSrc						= (uint32_t)(x * yFactor);
			const uint32_t													ySrc						= (uint32_t)(y * xFactor);
			if( ::gpk::in_range(ySrc, 0, src.metrics().y)
			 && ::gpk::in_range(xSrc, 0, src.metrics().x)
			 )
				dst[y][x]													= src[ySrc][xSrc];
		}
		return 0;
	}

	template<typename _tCell>
						::gpk::error_t							grid_scale_alpha			(::gpk::view2d<_tCell>& dst, const ::gpk::view2d<_tCell>& src, const ::gpk::SCoord2<int32_t>& dstPos, const ::gpk::SCoord2<int32_t>& dstSize) {
		const ::gpk::SCoord2<int32_t>									srcSize						= {(int)src.metrics().x	, (int)src.metrics().y	};
		for(int y = dstPos.y, yStop = ::gpk::min(dstPos.y + dstSize.y, (int32_t)dst.metrics().y); y < yStop; ++y) {
			::gpk::SCoord2<int32_t>											sourcePixelCoord			= {0, (int)((y - (double)dstPos.y) / dstSize.y * (double)srcSize.y)};
			for(int x = dstPos.x, xStop = ::gpk::min(dstPos.x + dstSize.x, (int32_t)dst.metrics().x); x < xStop; ++x) {
				sourcePixelCoord.x											= (int)((x - (double)dstPos.x) / dstSize.x * (double)srcSize.x);
				::gpk::SColorBGRA												colorDst					= dst[y][x];
				const ::gpk::SColorBGRA											colorSrc					= src[sourcePixelCoord.y][sourcePixelCoord.x];
				dst[y][x]													= ::gpk::interpolate_linear(colorDst, colorSrc, colorSrc.a / 255.0);
			}
		}
		return 0;
	}
	template<typename _tCell>
						::gpk::error_t							grid_scale					(::gpk::view2d<_tCell>& dst, const ::gpk::view2d<_tCell>& src, const ::gpk::SCoord2<int32_t>& dstPos, const ::gpk::SCoord2<int32_t>& dstSize) {
		const ::gpk::SCoord2<int32_t>									srcSize						= {(int)src.metrics().x	, (int)src.metrics().y	};
		for(int y = dstPos.y, yStop = ::gpk::min(dstPos.y + dstSize.y, (int32_t)dst.metrics().y); y < yStop; ++y) {
			::gpk::SCoord2<int32_t>											sourcePixelCoord			= {0, (int)((y - (double)dstPos.y) / dstSize.y * (double)srcSize.y)};
			for(int x = dstPos.x, xStop = ::gpk::min(dstPos.x + dstSize.x, (int32_t)dst.metrics().x); x < xStop; ++x) {
				sourcePixelCoord.x											= (int)((x - (double)dstPos.x) / dstSize.x * (double)srcSize.x);
				dst[y][x]													= src[sourcePixelCoord.y][sourcePixelCoord.x];
			}
		}
		return 0;
	}

}

#endif // GPK_GRID_SCALE_20983429834
