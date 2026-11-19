#include "gpk_grid.h"

#ifndef GPK_GRID_SCALE_20654834
#define GPK_GRID_SCALE_20654834

nmsp gpk
{
	tplt<tpnm T>
	gpk::err_t 				grid_scale_fast				(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src, cnst f2_t xFactor, cnst f2_t yFactor) {
		for(u2_t y = 0; y < dst.metrics().y; ++y)
		for(u2_t x = 0; x < dst.metrics().x; ++x)
			dst[y][x]						= src[(u2_t)(y * yFactor)][(u2_t)(x * xFactor)];
		return 0;
	}

	tplt<tpnm T>
	gpk::err_t 				grid_scale					(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src) {
		cnst f2_t							xFactor						= src.metrics().x / (f2_t)dst.metrics().x;
		cnst f2_t							yFactor						= src.metrics().y / (f2_t)dst.metrics().y;
		return grid_scale_fast(dst, src, xFactor, yFactor);
	}

	tplt<tpnm T>
	gpk::err_t 				grid_scale					(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src, cnst f2_t xFactor, cnst f2_t yFactor) {
		for(u2_t y = 0; y < dst.metrics().y; ++y)
		for(u2_t x = 0; x < dst.metrics().x; ++x) {
			u2_c						xSrc						= (u2_t)(x * yFactor);
			u2_c						ySrc						= (u2_t)(y * xFactor);
			if( ::gpk::in_range(ySrc, 0, src.metrics().y)
			 && ::gpk::in_range(xSrc, 0, src.metrics().x)
			 )
				dst[y][x]						= src[ySrc][xSrc];
		}
		return 0;
	}

	tplt<tpnm T>
	gpk::err_t 				grid_scale_alpha			(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src, cnst ::gpk::n2s2_t & dstPos, cnst ::gpk::n2s2_t & dstSize) {
		cnst ::gpk::n2s2_t				srcSize						= {(int)src.metrics().x	, (int)src.metrics().y	};
		for(int y = dstPos.y, yStop = ::gpk::min(dstPos.y + dstSize.y, (s2_t)dst.metrics().y); y < yStop; ++y) {
			::gpk::n2s2_t						sourcePixelCoord			= {0, (int)((y - (f3_t)dstPos.y) / dstSize.y * (f3_t)srcSize.y)};
			for(int x = dstPos.x, xStop = ::gpk::min(dstPos.x + dstSize.x, (s2_t)dst.metrics().x); x < xStop; ++x) {
				sourcePixelCoord.x				= (int)((x - (f3_t)dstPos.x) / dstSize.x * (f3_t)srcSize.x);
				T									& colorDst					= dst[y][x];
				cnst T								& colorSrc					= src[sourcePixelCoord.y][sourcePixelCoord.x];
				dst[y][x]						= ::gpk::interpolate_linear(colorDst, colorSrc, colorSrc.a / 255.0);
			}
		}
		return 0;
	}

	tplt<tpnm T>
	gpk::err_t 				grid_scale					(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src, cnst ::gpk::n2s2_t & dstPos, cnst ::gpk::n2s2_t & dstSize) {
		cnst ::gpk::n2s2_t				srcSize						= {(int)src.metrics().x	, (int)src.metrics().y	};
		for(int y = dstPos.y, yStop = ::gpk::min(dstPos.y + dstSize.y, (s2_t)dst.metrics().y); y < yStop; ++y) {
			::gpk::n2s2_t						sourcePixelCoord			= {0, (int)((y - (f3_t)dstPos.y) / dstSize.y * (f3_t)srcSize.y)};
			for(int x = dstPos.x, xStop = ::gpk::min(dstPos.x + dstSize.x, (s2_t)dst.metrics().x); x < xStop; ++x) {
				sourcePixelCoord.x				= (int)((x - (f3_t)dstPos.x) / dstSize.x * (f3_t)srcSize.x);
				dst[y][x]						= src[sourcePixelCoord.y][sourcePixelCoord.x];
			}
		}
		return 0;
	}
} // nmsp

#endif // GPK_GRID_SCALE_20983429834
