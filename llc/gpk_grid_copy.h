#include "gpk_view_grid.h"
#include "gpk_view_bit.h"

#ifndef GPK_GRID_COPY_20983429834
#define GPK_GRID_COPY_20983429834

namespace gpk
{
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_ex					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc)	{
		for(int32_t y = 0; y < size_.y; ++y) {
#if defined(GPK_DEBUG_GRID_COPY_EX) && (defined(DEBUG) || defined(_DEBUG))
			for(int32_t x = 0; x < size_.x; ++x)
				dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
#else
			const uint32_t													sizeToCopy						= size_.x * sizeof(T);
			memcpy(&dst[y + offsetDst.y][offsetDst.x], &src[y + offsetSrc.y][offsetSrc.x], sizeToCopy);
#endif
		}
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha_ex				(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, const T& comparand)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha_ex				(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, const T& comparand, const T& color)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if (src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= color;
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend_ex				(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc)	{
		::gpk::n2i16 count = {int16_t(size_.x + ((offsetSrc.x < 0) ? offsetSrc.x : 0)), int16_t(size_.y + ((offsetSrc.y < 0) ? offsetSrc.y : 0))};
		for(int32_t y = 0; y < (int16_t)count.y; ++y)
			for(int32_t x = 0; x < (int16_t)count.x; ++x) {
				const ::gpk::n2i16									coordSrcCell					= {int16_t(x + offsetSrc.x), int16_t(y + offsetSrc.y)};
				if(coordSrcCell.x < 0 || coordSrcCell.y < 0)
					continue;
				const T													& srcCell						= src[coordSrcCell.y][coordSrcCell.x];
				if(srcCell.a) {
					T															& dstCell						= dst[y + offsetDst.y][x + offsetDst.x];
					double															factor							= srcCell.a / 255.0;
					dstCell														= ::gpk::interpolate_linear(dstCell, srcCell, factor);
				}
			}
		return ::gpk::max<int16_t>(int16_t(count.x * count.y), 0);
	}

	template<typename _tCoord>
	::gpk::error_t							grid_raster_alpha_bit_ex		(const ::gpk::view_bit<uint64_t>& src, const ::gpk::n2<uint16_t>& srcMetrics, const ::gpk::n2<uint16_t>& size_, const ::gpk::n2<_tCoord> & offsetDst, const ::gpk::n2<_tCoord> & offsetSrc, ::gpk::apod<::gpk::n2u16> & dstCoords)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if(src[(y + offsetSrc.y) * srcMetrics.x + (x + offsetSrc.x)])
					dstCoords.push_back({(uint16_t)(x + offsetDst.x), (uint16_t)(y + offsetDst.y)});
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename _tCoord>
	::gpk::error_t							grid_raster_alpha_bit			(const ::gpk::n2<uint16_t>& dstMetrics, const ::gpk::view_bit<uint64_t>& src, const ::gpk::n2<uint16_t>& srcMetrics, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset, ::gpk::apod<::gpk::n2<uint16_t>> & dstCoords)		{
		const ::gpk::n2i16							offsetSrc						= srcOffset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2i16							offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t								stopYDst						= (int16_t)(srcMetrics.y) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y) - ::gpk::min<_tCoord>(srcOffset.y, 0);
		const int16_t								stopXDst						= (int16_t)(srcMetrics.x) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x) - ::gpk::min<_tCoord>(srcOffset.x, 0);
		const int16_t								maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dstMetrics.y - offsetDst.y), (int16_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int16_t								maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dstMetrics.x - offsetDst.x), (int16_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_raster_alpha_bit_ex(src, srcMetrics, ::gpk::n2i16{maxX, maxY}.Cast<uint16_t>(), offsetDst, offsetSrc, dstCoords) : 0;
	}

	// -----------------------------------------------
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect, const T& comparand)		{
		const ::gpk::n2<int16_t>									offsetSrc						= srcRect.Offset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcRect.Offset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)srcRect.Size.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcRect.Offset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2i16{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcRect.Offset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcRect.Offset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)srcRect.Size.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcRect.Offset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2i16{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy						(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::rect2<_tCoord>& srcRect)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcRect.Offset.template Cast<int16_t>() + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcRect.Offset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)srcRect.Size.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcRect.Offset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)srcRect.Size.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset, const T& comparand)		{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>() + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= ((int16_t)src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= ((int16_t)src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy						(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>() + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= ((int16_t)src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= ((int16_t)src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset, const T& comparand)			{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy						(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const ::gpk::n2<_tCoord> & srcOffset)									{
		const ::gpk::n2<int16_t>									offsetSrc						= srcOffset.template Cast<int16_t>()  + ::gpk::n2<int16_t>{-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= {::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		const int16_t													stopYDst						= (int16_t)(src.metrics().y) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.y);
		const int16_t													stopXDst						= (int16_t)(src.metrics().x) + ::gpk::min<int16_t>(0, (int16_t)dstOffset.x);
		const int16_t													maxY							= ::gpk::min<int16_t>(stopYDst - (int16_t)srcOffset.y, (int16_t)dst.metrics().y - offsetDst.y);
		const int16_t													maxX							= ::gpk::min<int16_t>(stopXDst - (int16_t)srcOffset.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	// -----------------------------------------------
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect, const T& comparand)													{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= (int16_t)src.metrics().y + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= (int16_t)src.metrics().x + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		const int16_t													maxY							= ::gpk::max<int16_t>(0, ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y)));
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc, comparand) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect)																			{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= (int16_t)src.metrics().y + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= (int16_t)src.metrics().x + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		const int16_t													maxY							= ::gpk::max<int16_t>(0, ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y)));
		const int16_t													maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_blend_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy						(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::rect2<_tCoord>& dstRect)																			{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x), -::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y)};
		::gpk::n2<int16_t>											offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.x),  ::gpk::max<int16_t>(0, (int16_t)dstRect.Offset.y)};
		const int16_t													stopYDst						= (int16_t)src.metrics().y + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.y);
		const int16_t													stopXDst						= (int16_t)src.metrics().x + ::gpk::min<int16_t>(0, (int16_t)dstRect.Offset.x);
		int16_t															maxY							= ::gpk::max<int16_t>(0, ::gpk::min<int16_t>(::gpk::min<int16_t>(stopYDst, (int16_t)dst.metrics().y - offsetDst.y), (int16_t)(dstRect.Size.y)));
		int16_t															maxX							= ::gpk::min<int16_t>(::gpk::min<int16_t>(stopXDst, (int16_t)dst.metrics().x - offsetDst.x), (int16_t)(dstRect.Size.x));
		return (maxX > 0) ? ::gpk::grid_copy_ex(dst, src, ::gpk::n2<int16_t>{maxX, maxY}, offsetDst, offsetSrc) : 0;
	}
	// -----------------------------------------------
	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const T& comparand)														{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		::gpk::n2<int16_t>											stopDst							= src.metrics().template Cast<int16_t>() + ::gpk::n2<int16_t>{::gpk::min<int16_t>(0, (int16_t)dstOffset.x), ::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		stopDst.y													= ::gpk::min<int16_t>(stopDst.y, (int16_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min<int16_t>(stopDst.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_alpha					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset, const T& comparand, const T& color)														{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		::gpk::n2<int16_t>											stopDst							= src.metrics().template Cast<int16_t>() + ::gpk::n2<int16_t>{::gpk::min<int16_t>(0, (int16_t)dstOffset.x), ::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		stopDst.y													= ::gpk::min<int16_t>(stopDst.y, (int16_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min<int16_t>(stopDst.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand, color) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy_blend					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset)																				{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		::gpk::n2<int16_t>											stopDst							= src.metrics().template Cast<int16_t>() + ::gpk::n2<int16_t>{::gpk::min<int16_t>(0, (int16_t)dstOffset.x), ::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		stopDst.y													= ::gpk::min<int16_t>(stopDst.y, (int16_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min<int16_t>(stopDst.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_blend_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	template<typename T, typename _tCoord>
	::gpk::error_t							grid_copy						(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src, const ::gpk::n2<_tCoord> & dstOffset)																				{
		const ::gpk::n2<int16_t>									offsetSrc						= {-::gpk::min<int16_t>(0, (int16_t)dstOffset.x), -::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		const ::gpk::n2<int16_t>									offsetDst						= { ::gpk::max<int16_t>(0, (int16_t)dstOffset.x),  ::gpk::max<int16_t>(0, (int16_t)dstOffset.y)};
		::gpk::n2<int16_t>											stopDst							= src.metrics().template Cast<int16_t>() + ::gpk::n2<int16_t>{::gpk::min<int16_t>(0, (int16_t)dstOffset.x), ::gpk::min<int16_t>(0, (int16_t)dstOffset.y)};
		stopDst.y													= ::gpk::min<int16_t>(stopDst.y, (int16_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min<int16_t>(stopDst.x, (int16_t)dst.metrics().x - offsetDst.x);
		return (stopDst.x > 0) ? ::gpk::grid_copy_ex(dst, src, stopDst, offsetDst, offsetSrc) : 0;
	}

	template<typename T>	::gpk::error_t					grid_copy_alpha					(::gpk::view2d<T> & dst, const ::gpk::view2d<T> & src, const T & comparand)	{ const ::gpk::n2u16 metrics = {(uint16_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint16_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_alpha_ex	(dst, src, metrics, {}, {}, comparand); }
	template<typename T>	::gpk::error_t					grid_copy_blend					(::gpk::view2d<T> & dst, const ::gpk::view2d<T> & src)						{ const ::gpk::n2u16 metrics = {(uint16_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint16_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_blend_ex	(dst, src, metrics, {}, {}); }
	template<typename T>	::gpk::error_t					grid_copy						(::gpk::view2d<T> & dst, const ::gpk::view2d<T> & src)						{ const ::gpk::n2u16 metrics = {(uint16_t)::gpk::min(dst.metrics().x, src.metrics().x), (uint16_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_ex			(dst, src, metrics, {}, {}); }

	template<typename T>
	::gpk::error_t							grid_mirror_x					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src)																														{
		const ::gpk::n2i16									metrics							=
			{ ::gpk::min((int16_t)dst.metrics().x, (int16_t)src.metrics().x)
			, ::gpk::min((int16_t)dst.metrics().y, (int16_t)src.metrics().y)
			};
		uint32_t														srcMaxX							= src.metrics().x - 1;
		for(int32_t y = 0; y < metrics.y; ++y)
		for(int32_t x = 0; x < metrics.x; ++x)
			dst[y][x]													= src[y][srcMaxX - x];
		return metrics.y * metrics.x;
	}

	template<typename T>
	::gpk::error_t							grid_mirror_y					(::gpk::view2d<T>& dst, const ::gpk::view2d<T>& src)		{
		const ::gpk::n2i16									metrics							=
			{ ::gpk::min((int16_t)dst.metrics().x, (int16_t)src.metrics().x)
			, ::gpk::min((int16_t)dst.metrics().y, (int16_t)src.metrics().y)
			};
		uint32_t														srcMaxY							= src.metrics().y - 1;
		if(metrics.x > 0)
			for(uint32_t y = 0; y < (uint32_t)metrics.y; ++y)
				memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(T) * metrics.x);
		return metrics.y * metrics.x;
	}
} // namespace

#endif // GPK_GRID_COPY_20983429834
