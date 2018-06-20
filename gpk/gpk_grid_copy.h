#include "gpk_grid_view.h"
#include "gpk_bit_view.h"

#ifndef GPK_GRID_COPY_20983429834
#define GPK_GRID_COPY_20983429834

namespace gpk
{
	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_ex					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& size_, const ::gpk::SCoord2<_tCoord>& offsetDst, const ::gpk::SCoord2<_tCoord>& offsetSrc)	{
		for(int32_t y = 0; y < size_.y; ++y) {
#if defined(GPK_DEBUG_GRID_COPY_EX) && (defined(DEBUG) || defined(_DEBUG)) 
			for(int32_t x = 0; x < size_.x; ++x)
				dst[y + offsetDst.y][x + offsetDst.x]							= src[y + offsetSrc.y][x + offsetSrc.x];
#else
			const uint32_t													sizeToCopy						= size_.x * sizeof(_tCell);
			memcpy(&dst[y + offsetDst.y][offsetDst.x], &src[y + offsetSrc.y][offsetSrc.x], sizeToCopy);
#endif
		}
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha_ex				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& size_, const ::gpk::SCoord2<_tCoord>& offsetDst, const ::gpk::SCoord2<_tCoord>& offsetSrc, const _tCell& comparand)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if(src[y + offsetSrc.y][x + offsetSrc.x] != comparand)
					dst[y + offsetDst.y][x + offsetDst.x]						= src[y + offsetSrc.y][x + offsetSrc.x];
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend_ex				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& size_, const ::gpk::SCoord2<_tCoord>& offsetDst, const ::gpk::SCoord2<_tCoord>& offsetSrc)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x) {
				_tCell															& srcCell						= src[y + offsetSrc.y][x + offsetSrc.x];
				_tCell															& dstCell						= dst[y + offsetDst.y][x + offsetDst.x];
				double															factor							= srcCell.a / 255.0;
				dstCell														= ::gpk::interpolate_linear(dstCell, srcCell, factor);
			}
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename _tCoord>
						::gpk::error_t							grid_raster_alpha_bit_ex		(const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<uint32_t>& srcMetrics, const ::gpk::SCoord2<_tCoord>& size_, const ::gpk::SCoord2<_tCoord>& offsetDst, const ::gpk::SCoord2<_tCoord>& offsetSrc, ::gpk::array_pod<::gpk::SCoord2<uint32_t>> & dstCoords)	{
		for(int32_t y = 0; y < size_.y; ++y)
			for(int32_t x = 0; x < size_.x; ++x)
				if(src[(y + offsetSrc.y) * srcMetrics.x + (x + offsetSrc.x)])
					dstCoords.push_back({(uint32_t)(x + offsetDst.x), (uint32_t)(y + offsetDst.y)});
		return ::gpk::max(size_.x * size_.y, 0);
	}

	template<typename _tCoord>
						::gpk::error_t							grid_raster_alpha_bit			(const ::gpk::SCoord2<uint32_t>& dstMetrics, const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<uint32_t>& srcMetrics, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SCoord2<_tCoord>& srcOffset, ::gpk::array_pod<::gpk::SCoord2<uint32_t>> & dstCoords)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstRect.Offset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= (int32_t)(srcMetrics.y) + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)(srcMetrics.x) + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dstMetrics.y - offsetDst.y), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dstMetrics.x - offsetDst.x), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::gpk::grid_raster_alpha_bit_ex(src, srcMetrics, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, dstCoords);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SRectangle2D<_tCoord>& srcRect, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcRect.Offset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcRect.Offset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SRectangle2D<_tCoord>& srcRect)		{
		const int32_t													offsetYSrc						= srcRect.Offset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcRect.Offset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return ::gpk::grid_copy_blend_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SRectangle2D<_tCoord>& srcRect)		{
		const int32_t													offsetYSrc						= srcRect.Offset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcRect.Offset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcRect.Offset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)srcRect.Size.y);
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcRect.Offset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)srcRect.Size.x);
		return ::gpk::grid_copy_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SCoord2<_tCoord>& srcOffset, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstRect.Offset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstRect.Offset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= ((int32_t)src.metrics().y) + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= ((int32_t)src.metrics().x) + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::gpk::grid_copy_blend_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstRect.Offset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstRect.Offset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= ((int32_t)src.metrics().y) + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= ((int32_t)src.metrics().x) + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y/* - dstRect.Offset.y*/));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x/* - dstRect.Offset.x*/));
		return ::gpk::grid_copy_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<_tCoord>& srcOffset, const _tCell& comparand)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t													maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc}, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t													maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_blend_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const int32_t													offsetYSrc						= srcOffset.y + -::gpk::min(0, (int32_t)dstOffset.y) ;
		const int32_t													offsetXSrc						= srcOffset.x + -::gpk::min(0, (int32_t)dstOffset.x) ;
		const ::gpk::SCoord2<int32_t>									offsetDst						= {::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		const int32_t													stopYDst						= (int32_t)(src.metrics().y) + ::gpk::min(0, (int32_t)dstOffset.y);
		const int32_t													stopXDst						= (int32_t)(src.metrics().x) + ::gpk::min(0, (int32_t)dstOffset.x);
		const int32_t													maxY							= ::gpk::min(stopYDst - (int32_t)srcOffset.y, (int32_t)dst.metrics().y - offsetDst.y);
		const int32_t													maxX							= ::gpk::min(stopXDst - (int32_t)srcOffset.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, ::gpk::SCoord2<int32_t>{offsetXSrc, offsetYSrc});
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const _tCell& comparand)	{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstRect.Offset.x), -::gpk::min(0, (int32_t)dstRect.Offset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= (int32_t)src.metrics().y + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)src.metrics().x + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x));
		return ::gpk::grid_copy_alpha_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, offsetSrc, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect)		{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstRect.Offset.x), -::gpk::min(0, (int32_t)dstRect.Offset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= (int32_t)src.metrics().y + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)src.metrics().x + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x));
		return ::gpk::grid_copy_blend_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, offsetSrc);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect)		{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstRect.Offset.x), -::gpk::min(0, (int32_t)dstRect.Offset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstRect.Offset.x),  ::gpk::max(0, (int32_t)dstRect.Offset.y)};
		const int32_t													stopYDst						= (int32_t)src.metrics().y + ::gpk::min(0, (int32_t)dstRect.Offset.y);
		const int32_t													stopXDst						= (int32_t)src.metrics().x + ::gpk::min(0, (int32_t)dstRect.Offset.x);
		const int32_t													maxY							= ::gpk::min(::gpk::min(stopYDst, (int32_t)dst.metrics().y - offsetDst.y), (int32_t)(dstRect.Size.y));
		const int32_t													maxX							= ::gpk::min(::gpk::min(stopXDst, (int32_t)dst.metrics().x - offsetDst.x), (int32_t)(dstRect.Size.x));
		return ::gpk::grid_copy_ex(dst, src, ::gpk::SCoord2<int32_t>{maxX, maxY}, offsetDst, offsetSrc);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const _tCell& comparand)		{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstOffset.x), -::gpk::min(0, (int32_t)dstOffset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		::gpk::SCoord2<int32_t>											stopDst							= src.metrics().Cast<int32_t>() + ::gpk::SCoord2<int32_t>{::gpk::min(0, (int32_t)dstOffset.x), ::gpk::min(0, (int32_t)dstOffset.y)};
		stopDst.y													= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_alpha_ex(dst, src, stopDst, offsetDst, offsetSrc, comparand);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset)		{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstOffset.x), -::gpk::min(0, (int32_t)dstOffset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		::gpk::SCoord2<int32_t>											stopDst							= src.metrics().Cast<int32_t>() + ::gpk::SCoord2<int32_t>{::gpk::min(0, (int32_t)dstOffset.x), ::gpk::min(0, (int32_t)dstOffset.y)};
		stopDst.y													= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_blend_ex(dst, src, stopDst, offsetDst, offsetSrc);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset)		{
		const ::gpk::SCoord2<int32_t>									offsetSrc						= {-::gpk::min(0, (int32_t)dstOffset.x), -::gpk::min(0, (int32_t)dstOffset.y)};
		const ::gpk::SCoord2<int32_t>									offsetDst						= { ::gpk::max(0, (int32_t)dstOffset.x),  ::gpk::max(0, (int32_t)dstOffset.y)};
		::gpk::SCoord2<int32_t>											stopDst							= src.metrics().Cast<int32_t>() + ::gpk::SCoord2<int32_t>{::gpk::min(0, (int32_t)dstOffset.x), ::gpk::min(0, (int32_t)dstOffset.y)};
		stopDst.y													= ::gpk::min(stopDst.y, (int32_t)dst.metrics().y - offsetDst.y);
		stopDst.x													= ::gpk::min(stopDst.x, (int32_t)dst.metrics().x - offsetDst.x);
		return ::gpk::grid_copy_ex(dst, src, stopDst, offsetDst, offsetSrc);
	}

	template<typename _tCell>	::gpk::error_t					grid_copy_alpha					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const _tCell& comparand)		{ const ::gpk::SCoord2<int32_t> metrics = {(int32_t)::gpk::min(dst.metrics().x, src.metrics().x), (int32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_alpha_ex	(dst, src, metrics, {}, {}, comparand); }
	template<typename _tCell>	::gpk::error_t					grid_copy_blend					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)								{ const ::gpk::SCoord2<int32_t> metrics = {(int32_t)::gpk::min(dst.metrics().x, src.metrics().x), (int32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_blend_ex	(dst, src, metrics, {}, {}); }
	template<typename _tCell>	::gpk::error_t					grid_copy						(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)								{ const ::gpk::SCoord2<int32_t> metrics = {(int32_t)::gpk::min(dst.metrics().x, src.metrics().x), (int32_t)::gpk::min(dst.metrics().y, src.metrics().y)}; return ::gpk::grid_copy_ex		(dst, src, metrics, {}, {}); }

	template<typename _tCell>
						::gpk::error_t							grid_mirror_x					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)		{
		const ::gpk::SCoord2<int32_t>									metrics							= 
			{ (int32_t)::gpk::min(dst.metrics().x, src.metrics().x)
			, (int32_t)::gpk::min(dst.metrics().y, src.metrics().y)
			};
		uint32_t														srcMaxX							= src.metrics().x - 1;
		for(uint32_t y = 0; y < metrics.y; ++y) 
		for(uint32_t x = 0; x < metrics.x; ++x) 
			dst[y][x]													= src[y][srcMaxX - x];
		return metrics.y * metrics.x;
	}

	template<typename _tCell>
						::gpk::error_t							grid_mirror_y					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)		{
		const ::gpk::SCoord2<int32_t>									metrics							= 
			{ (int32_t)::gpk::min(dst.metrics().x, src.metrics().x)
			, (int32_t)::gpk::min(dst.metrics().y, src.metrics().y)
			};
		uint32_t														srcMaxY							= src.metrics().y - 1;
		for(uint32_t y = 0; y < metrics.y; ++y) 
			memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(_tCell) * metrics.x);
		return metrics.y * metrics.x;
	}

} // namespace

#endif // GPK_GRID_COPY_20983429834
