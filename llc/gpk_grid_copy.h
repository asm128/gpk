#include "gpk_grid.h"
#include "gpk_view_bit.h"
#include "gpk_array_pod.h"

#ifndef GPK_GRID_COPY_20983429834
#define GPK_GRID_COPY_20983429834

nmsp gpk
{
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_ex					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & size_, cnst ::gpk::n2<_tCoord> & offsetDst, cnst ::gpk::n2<_tCoord> & offsetSrc)	{
		for(u2_t y = 0; y < (u2_t)size_.y; ++y) {
#if defined(GPK_DEBUG_GRID_COPY_EX) && (defined(DEBUG) || defined(_DEBUG))
			for(u2_t x = 0; x < (u2_t)size_.x; ++x)
				dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
#else
			u2_c													sizeToCopy						= size_.x * sizeof(T);
			memcpy(&dst[y + offsetDst.y][offsetDst.x], &src[y + offsetSrc.y][offsetSrc.x], sizeToCopy);
#endif
		}
		return ::gpk::max(s2_t(size_.x * size_.y), (s2_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha_ex				(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & size_, cnst ::gpk::n2<_tCoord> & offsetDst, cnst ::gpk::n2<_tCoord> & offsetSrc, cnst T& comparand)	{
		for(u2_t y = 0; y < (u2_t)size_.y; ++y)
			for(u2_t x = 0; x < (u2_t)size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
		return ::gpk::max(s2_t(size_.x * size_.y), (s2_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha_ex				(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & size_, cnst ::gpk::n2<_tCoord> & offsetDst, cnst ::gpk::n2<_tCoord> & offsetSrc, cnst T& comparand, cnst T& color)	{
		for(u2_t y = 0; y < (u2_t)size_.y; ++y)
			for(u2_t x = 0; x < (u2_t)size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= color;
		return ::gpk::max(s2_t(size_.x * size_.y), (s2_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend_ex				(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & size_, cnst ::gpk::n2<_tCoord> & offsetDst, cnst ::gpk::n2<_tCoord> & offsetSrc)	{
		::gpk::n2s2_t count = {s2_t(size_.x + ((offsetSrc.x < 0) ? offsetSrc.x : 0)), s2_t(size_.y + ((offsetSrc.y < 0) ? offsetSrc.y : 0))};
		for(s2_t y = 0; y < (s2_t)count.y; ++y)
			for(s2_t x = 0; x < (s2_t)count.x; ++x) {
				cnst ::gpk::n2s2_t									coordSrcCell					= {s2_t(x + offsetSrc.x), s2_t(y + offsetSrc.y)};
				if(coordSrcCell.x < 0 || coordSrcCell.y < 0)
					continue;
				cnst T													& srcCell						= src[coordSrcCell.y][coordSrcCell.x];
				if(srcCell.a) {
					T															& dstCell						= dst[y + offsetDst.y][x + offsetDst.x];
					f3_t															factor							= srcCell.a / 255.0;
					dstCell														= ::gpk::interpolate_linear(dstCell, srcCell, factor);
				}
			}
		return ::gpk::max(s2_t(count.x * count.y), (s2_t)0);
	}

	tplt<tpnm _tCoord>
	gpk::err_t 		grid_raster_alpha_bit_ex		(cnst ::gpk::view_bit<u3_t> & src, cnst ::gpk::n2u2_t & srcMetrics, cnst ::gpk::n2u2_t & size_, cnst ::gpk::n2<_tCoord> & offsetDst, cnst ::gpk::n2<_tCoord> & offsetSrc, ::gpk::apod<::gpk::n2u1_t> & dstCoords) {
		for(u1_t y = 0; y < size_.y; ++y)
			for(u1_t x = 0; x < size_.x; ++x)
				if(src[(y + offsetSrc.y) * srcMetrics.x + (x + offsetSrc.x)])
					dstCoords.push_back({(u1_t)(x + offsetDst.x), (u1_t)(y + offsetDst.y)});
		return ::gpk::max(size_.x * size_.y, (u2_t)0U);
	}
	tplt<tpnm _tCoord>
	gpk::err_t 		grid_raster_alpha_bit			(cnst ::gpk::n2u2_t & dstMetrics, cnst ::gpk::view_bit<u3_t> & src, cnst ::gpk::n2u2_t & srcMetrics, cnst ::gpk::rect2<_tCoord>& dstRect, cnst ::gpk::n2<_tCoord> & srcOffset, ::gpk::apod<::gpk::n2u1_t> & dstCoords) {
		cnst ::gpk::n2s2_t			offsetSrc					= srcOffset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst					= {::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst					= s2_t(srcMetrics.y) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y) - ::gpk::min<_tCoord>(srcOffset.y, 0);
		cnst s2_t				stopXDst					= s2_t(srcMetrics.x) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x) - ::gpk::min<_tCoord>(srcOffset.x, 0);
		cnst s2_t				maxY						= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dstMetrics.y - offsetDst.y), s2_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		cnst s2_t				maxX						= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dstMetrics.x - offsetDst.x), s2_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_raster_alpha_bit_ex(src, srcMetrics, ::gpk::n2s2_t{maxX, maxY}.Clamp({}, dstMetrics.s2()).u2(), offsetDst, offsetSrc, dstCoords) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::rect2<_tCoord>& srcRect, cnst T& comparand)		{
		cnst ::gpk::n2s2_t			offsetSrc						= srcRect.Offset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcRect.Offset.y, (s2_t)dst.metrics().y - offsetDst.y), (s2_t)srcRect.Size.y);
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcRect.Offset.x, (s2_t)dst.metrics().x - offsetDst.x), (s2_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::rect2<_tCoord>& srcRect)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcRect.Offset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcRect.Offset.y, (s2_t)dst.metrics().y - offsetDst.y), (s2_t)srcRect.Size.y);
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcRect.Offset.x, (s2_t)dst.metrics().x - offsetDst.x), (s2_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::rect2<_tCoord>& srcRect)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcRect.Offset.s2() + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcRect.Offset.y, (s2_t)dst.metrics().y - offsetDst.y), (s2_t)srcRect.Size.y);
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcRect.Offset.x, (s2_t)dst.metrics().x - offsetDst.x), (s2_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect, cnst ::gpk::n2<_tCoord> & srcOffset, cnst T& comparand)		{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect, cnst ::gpk::n2<_tCoord> & srcOffset)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2() + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= ((s2_t)src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= ((s2_t)src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect, cnst ::gpk::n2<_tCoord> & srcOffset)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2() + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= ((s2_t)src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= ((s2_t)src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::n2<_tCoord> & srcOffset, cnst T& comparand)			{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y);
		cnst s2_t				maxX							= ::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::n2<_tCoord> & srcOffset)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y);
		cnst s2_t				maxX							= ::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst ::gpk::n2<_tCoord> & srcOffset)									{
		cnst ::gpk::n2s2_t			offsetSrc						= srcOffset.s2()  + ::gpk::n2s2_t{-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= {::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		cnst s2_t				stopYDst						= s2_t(src.metrics().y) + ::gpk::min((s2_t)0, (s2_t)dstOffset.y);
		cnst s2_t				stopXDst						= s2_t(src.metrics().x) + ::gpk::min((s2_t)0, (s2_t)dstOffset.x);
		cnst s2_t				maxY							= ::gpk::min(stopYDst - (s2_t)srcOffset.y, (s2_t)dst.metrics().y - offsetDst.y);
		cnst s2_t				maxX							= ::gpk::min(stopXDst - (s2_t)srcOffset.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect, cnst T& comparand)													{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= (s2_t)src.metrics().y + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= (s2_t)src.metrics().x + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::max((s2_t)0, ::gpk::min(::gpk::min(stopYDst, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y)));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect)																			{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= (s2_t)src.metrics().y + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= (s2_t)src.metrics().x + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::max((s2_t)0, ::gpk::min(::gpk::min(stopYDst, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y)));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::rect2<_tCoord>& dstRect)																			{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x), -::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y)};
		::gpk::n2s2_t				offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.x),  ::gpk::max((s2_t)0, (s2_t)dstRect.Offset.y)};
		cnst s2_t				stopYDst						= (s2_t)src.metrics().y + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.y);
		cnst s2_t				stopXDst						= (s2_t)src.metrics().x + ::gpk::min((s2_t)0, (s2_t)dstRect.Offset.x);
		cnst s2_t				maxY							= ::gpk::max((s2_t)0, ::gpk::min(::gpk::min(stopYDst, (s2_t)dst.metrics().y - offsetDst.y), s2_t(dstRect.Size.y)));
		cnst s2_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (s2_t)dst.metrics().x - offsetDst.x), s2_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2s2_t{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}
	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst T& comparand)														{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		::gpk::n2s2_t				stopDst							= src.metrics().s2() + ::gpk::n2s2_t{::gpk::min((s2_t)0, (s2_t)dstOffset.x), ::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (s2_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset, cnst T& comparand, cnst T& color)														{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		::gpk::n2s2_t				stopDst							= src.metrics().s2() + ::gpk::n2s2_t{::gpk::min((s2_t)0, (s2_t)dstOffset.x), ::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (s2_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand, color) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset)			{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		::gpk::n2s2_t				stopDst							= src.metrics().s2() + ::gpk::n2s2_t{::gpk::min((s2_t)0, (s2_t)dstOffset.x), ::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (s2_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_blend_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	gpk::err_t 		grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src, cnst ::gpk::n2<_tCoord> & dstOffset)			{
		cnst ::gpk::n2s2_t			offsetSrc						= {-::gpk::min((s2_t)0, (s2_t)dstOffset.x), -::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		cnst ::gpk::n2s2_t			offsetDst						= { ::gpk::max((s2_t)0, (s2_t)dstOffset.x),  ::gpk::max((s2_t)0, (s2_t)dstOffset.y)};
		::gpk::n2s2_t				stopDst							= src.metrics().s2() + ::gpk::n2s2_t{::gpk::min((s2_t)0, (s2_t)dstOffset.x), ::gpk::min((s2_t)0, (s2_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (s2_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (s2_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T> gpk::err_t 	grid_copy_alpha					(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src, cnst T & comparand)	{ cnst ::gpk::n2u2_t metrics = {(u2_t)::gpk::min(dst.metrics().x, src.metrics().x), (u2_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_alpha_ex	(dst, src, metrics, {}, {}, comparand); }
	tplt<tpnm T> gpk::err_t 	grid_copy_blend					(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src)						{ cnst ::gpk::n2u2_t metrics = {(u2_t)::gpk::min(dst.metrics().x, src.metrics().x), (u2_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_blend_ex	(dst, src, metrics, {}, {}); }
	tplt<tpnm T> gpk::err_t 	grid_copy						(::gpk::grid<T> & dst, cnst ::gpk::grid<T> & src)						{ cnst ::gpk::n2u2_t metrics = {(u2_t)::gpk::min(dst.metrics().x, src.metrics().x), (u2_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_ex		(dst, src, metrics, {}, {}); }

	tplt<tpnm T>
	gpk::err_t 		grid_mirror_x					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src)											{
		cnst ::gpk::n2s2_t			metrics							=
			{ ::gpk::min((s2_t)dst.metrics().x, (s2_t)src.metrics().x)
			, ::gpk::min((s2_t)dst.metrics().y, (s2_t)src.metrics().y)
			};
		u2_t					srcMaxX							= src.metrics().x - 1;
		for(s2_t y = 0; y < metrics.y; ++y)
		for(s2_t x = 0; x < metrics.x; ++x)
			dst[y][x]				= src[y][srcMaxX - x];
		return metrics.y * metrics.x;
	}

	tplt<tpnm T>
	gpk::err_t 		grid_mirror_y					(::gpk::grid<T> & dst, cnst ::gpk::grid<T>& src)		{
		cnst ::gpk::n2s2_t			metrics							=
			{ ::gpk::min((s2_t)dst.metrics().x, (s2_t)src.metrics().x)
			, ::gpk::min((s2_t)dst.metrics().y, (s2_t)src.metrics().y)
			};
		u2_t					srcMaxY							= src.metrics().y - 1;
		if(metrics.x > 0)
			for(u2_t y = 0; y < (u2_t)metrics.y; ++y)
				memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(T) * metrics.x);
		return metrics.y * metrics.x;
	}
} // nmsp

#endif // GPK_GRID_COPY_20983429834
