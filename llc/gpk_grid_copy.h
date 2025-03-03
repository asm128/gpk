#include "gpk_grid.h"
#include "gpk_view_bit.h"
#include "gpk_array_pod.h"

#ifndef GPK_GRID_COPY_20983429834
#define GPK_GRID_COPY_20983429834

namespace gpk
{
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_ex					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc)	{
		for(uint32_t y = 0; y < (uint32_t)size_.y; ++y) {
#if defined(GPK_DEBUG_GRID_COPY_EX) && (defined(DEBUG) || defined(_DEBUG))
			for(uint32_t x = 0; x < (uint32_t)size_.x; ++x)
				dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
#else
			const uint32_t													sizeToCopy						= size_.x * sizeof(T);
			memcpy(&dst[y + offsetDst.y][offsetDst.x], &src[y + offsetSrc.y][offsetSrc.x], sizeToCopy);
#endif
		}
		return ::gpk::max(int32_t(size_.x * size_.y), (int32_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha_ex				(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, const T& comparand)	{
		for(uint32_t y = 0; y < (uint32_t)size_.y; ++y)
			for(uint32_t x = 0; x < (uint32_t)size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
		return ::gpk::max(int32_t(size_.x * size_.y), (int32_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha_ex				(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, const T& comparand, const T& color)	{
		for(uint32_t y = 0; y < (uint32_t)size_.y; ++y)
			for(uint32_t x = 0; x < (uint32_t)size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= color;
		return ::gpk::max(int32_t(size_.x * size_.y), (int32_t)0);
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend_ex				(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc)	{
		::gpk::n2i32 count = {int32_t(size_.x + ((offsetSrc.x < 0) ? offsetSrc.x : 0)), int32_t(size_.y + ((offsetSrc.y < 0) ? offsetSrc.y : 0))};
		for(int32_t y = 0; y < (int32_t)count.y; ++y)
			for(int32_t x = 0; x < (int32_t)count.x; ++x) {
				const ::gpk::n2i32									coordSrcCell					= {int32_t(x + offsetSrc.x), int32_t(y + offsetSrc.y)};
				if(coordSrcCell.x < 0 || coordSrcCell.y < 0)
					continue;
				const T													& srcCell						= src[coordSrcCell.y][coordSrcCell.x];
				if(srcCell.a) {
					T															& dstCell						= dst[y + offsetDst.y][x + offsetDst.x];
					double															factor							= srcCell.a / 255.0;
					dstCell														= ::gpk::interpolate_linear(dstCell, srcCell, factor);
				}
			}
		return ::gpk::max(int32_t(count.x * count.y), (int32_t)0);
	}

	tplt<tpnm _tCoord>
	::gpk::error_t			grid_raster_alpha_bit_ex		(const ::gpk::view_bit<uint64_t> & src, const ::gpk::n2u32 & srcMetrics, const ::gpk::n2u32 & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, ::gpk::apod<::gpk::n2u16> & dstCoords)	{
		for(uint16_t y = 0; y < size_.y; ++y)
			for(uint16_t x = 0; x < size_.x; ++x)
				if(src[(y + offsetSrc.y) * srcMetrics.x + (x + offsetSrc.x)])
					dstCoords.push_back({(uint16_t)(x + offsetDst.x), (uint16_t)(y + offsetDst.y)});
		return ::gpk::max(size_.x * size_.y, (uint32_t)0U);
	}

	tplt<tpnm _tCoord>
	::gpk::error_t			grid_raster_alpha_bit			(const ::gpk::n2u32 & dstMetrics, const ::gpk::view_bit<uint64_t> & src, const ::gpk::n2u32 & srcMetrics, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset, ::gpk::apod<::gpk::n2u16> & dstCoords)		{
		const ::gpk::n2i32			offsetSrc					= srcOffset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst					= {::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst					= int32_t(srcMetrics.y) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y) - ::gpk::min<_tCoord>(srcOffset.y, 0);
		const int32_t				stopXDst					= int32_t(srcMetrics.x) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x) - ::gpk::min<_tCoord>(srcOffset.x, 0);
		const int32_t				maxY						= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dstMetrics.y - offsetDst.y), int32_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t				maxX						= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dstMetrics.x - offsetDst.x), int32_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_raster_alpha_bit_ex(src, srcMetrics, ::gpk::n2i32{maxX, maxY}.Clamp({}, dstMetrics.s2_t()).u2_t(), offsetDst, offsetSrc, dstCoords) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect, const T& comparand)		{
		const ::gpk::n2i32			offsetSrc						= srcRect.Offset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect)									{
		const ::gpk::n2i32			offsetSrc						= srcRect.Offset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect)									{
		const ::gpk::n2i32			offsetSrc						= srcRect.Offset.s2_t() + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset, const T& comparand)		{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t() + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= ((int32_t)src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= ((int32_t)src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t() + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= ((int32_t)src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= ((int32_t)src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset, const T& comparand)			{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t				maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t				maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2i32			offsetSrc						= srcOffset.s2_t()  + ::gpk::n2i32{-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= {::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		const int32_t				stopYDst						= int32_t(src.metrics().y) + ::gpk::min((int32_t)0, (int32_t)dstOffset.y);
		const int32_t				stopXDst						= int32_t(src.metrics().x) + ::gpk::min((int32_t)0, (int32_t)dstOffset.x);
		const int32_t				maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t				maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const T& comparand)													{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= (int32_t)src.metrics().y + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= (int32_t)src.metrics().x + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::max((int32_t)0, ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y)));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect)																			{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= (int32_t)src.metrics().y + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= (int32_t)src.metrics().x + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::max((int32_t)0, ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y)));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::rect2<_tCoord>& dstRect)																			{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x), -::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y)};
		::gpk::n2i32				offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.x),  ::gpk::max((int32_t)0, (int32_t)dstRect.Offset.y)};
		const int32_t				stopYDst						= (int32_t)src.metrics().y + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.y);
		const int32_t				stopXDst						= (int32_t)src.metrics().x + ::gpk::min((int32_t)0, (int32_t)dstRect.Offset.x);
		const int32_t				maxY							= ::gpk::max((int32_t)0, ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), int32_t(dstRect.Size.y)));
		const int32_t				maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), int32_t(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2i32{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}
	// -----------------------------------------------
	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const T& comparand)														{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		::gpk::n2i32				stopDst							= src.metrics().s2_t() + ::gpk::n2i32{::gpk::min((int32_t)0, (int32_t)dstOffset.x), ::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const T& comparand, const T& color)														{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		::gpk::n2i32				stopDst							= src.metrics().s2_t() + ::gpk::n2i32{::gpk::min((int32_t)0, (int32_t)dstOffset.x), ::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand, color) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset)			{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		::gpk::n2i32				stopDst							= src.metrics().s2_t() + ::gpk::n2i32{::gpk::min((int32_t)0, (int32_t)dstOffset.x), ::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_blend_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T, tpnm _tCoord>
	::gpk::error_t			grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T>& src, const ::gpk::n2<_tCoord> & dstOffset)			{
		const ::gpk::n2i32			offsetSrc						= {-::gpk::min((int32_t)0, (int32_t)dstOffset.x), -::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		const ::gpk::n2i32			offsetDst						= { ::gpk::max((int32_t)0, (int32_t)dstOffset.x),  ::gpk::max((int32_t)0, (int32_t)dstOffset.y)};
		::gpk::n2i32				stopDst							= src.metrics().s2_t() + ::gpk::n2i32{::gpk::min((int32_t)0, (int32_t)dstOffset.x), ::gpk::min((int32_t)0, (int32_t)dstOffset.y)};
		stopDst.y				= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x				= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	tplt<tpnm T> ::gpk::error_t		grid_copy_alpha					(::gpk::grid<T> & dst, const ::gpk::grid<T> & src, const T & comparand)	{ const ::gpk::n2u32 metrics = {(uint32_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_alpha_ex	(dst, src, metrics, {}, {}, comparand); }
	tplt<tpnm T> ::gpk::error_t		grid_copy_blend					(::gpk::grid<T> & dst, const ::gpk::grid<T> & src)						{ const ::gpk::n2u32 metrics = {(uint32_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_blend_ex	(dst, src, metrics, {}, {}); }
	tplt<tpnm T> ::gpk::error_t		grid_copy						(::gpk::grid<T> & dst, const ::gpk::grid<T> & src)						{ const ::gpk::n2u32 metrics = {(uint32_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_ex			(dst, src, metrics, {}, {}); }

	tplt<tpnm T>
	::gpk::error_t			grid_mirror_x					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src)											{
		const ::gpk::n2i32			metrics							=
			{ ::gpk::min((int32_t)dst.metrics().x, (int32_t)src.metrics().x)
			, ::gpk::min((int32_t)dst.metrics().y, (int32_t)src.metrics().y)
			};
		uint32_t					srcMaxX							= src.metrics().x - 1;
		for(int32_t y = 0; y < metrics.y; ++y)
		for(int32_t x = 0; x < metrics.x; ++x)
			dst[y][x]				= src[y][srcMaxX - x];
		return metrics.y * metrics.x;
	}

	tplt<tpnm T>
	::gpk::error_t			grid_mirror_y					(::gpk::grid<T> & dst, const ::gpk::grid<T>& src)		{
		const ::gpk::n2i32			metrics							=
			{ ::gpk::min((int32_t)dst.metrics().x, (int32_t)src.metrics().x)
			, ::gpk::min((int32_t)dst.metrics().y, (int32_t)src.metrics().y)
			};
		uint32_t					srcMaxY							= src.metrics().y - 1;
		if(metrics.x > 0)
			for(uint32_t y = 0; y < (uint32_t)metrics.y; ++y)
				memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(T) * metrics.x);
		return metrics.y * metrics.x;
	}
} // namespace

#endif // GPK_GRID_COPY_20983429834
