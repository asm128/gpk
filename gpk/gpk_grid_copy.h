#include "gpk_grid_view.h"
#include "gpk_bit_view.h"

#ifndef GPK_GRID_COPY_20983429834
#define GPK_GRID_COPY_20983429834

namespace gpk
{
	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::gpk::max
			( 0
			, ::gpk::min
				( ::gpk::min
					( ::gpk::min
						((int32_t)(src.metrics().x - xSrcOffset), (int32_t)(dst.metrics().x - xDstOffset))
						, (int32_t)srcRect.Size.x
						)
				, (int32_t)dstRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x, 0, (int32_t)src.metrics().x);			// 
		const int32_t													yDstLimit					= ::gpk::min((int32_t)(dstRect.Offset.y + dstRect.Size.y), (int32_t)dst.metrics().y);
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y), (int32_t)src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, dstRect, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstRect.Offset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= yDstLimit || ySrc >= ySrcLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::gpk::grid_view<_tCell>& dst, const ::gpk::SCoord2<uint32_t>& srcMetrics, const ::gpk::SRectangle2D<_tCoord>& srcRect, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return (uint32_t)::gpk::max		// Make sure everything is in range.
			( 0
			, ::gpk::min
				( ::gpk::min((int32_t)(srcMetrics.x - xSrcOffset), (int32_t)(dst.metrics().x - xDstOffset))
				, (int32_t)srcRect.Size.x
				)
			);	// 
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha_bit			(::gpk::grid_view<_tCell>& dst, const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<uint32_t> & srcMetrics, const _tCell& color, const ::gpk::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)srcMetrics.x);			// 
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)srcMetrics.y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, srcMetrics, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= ySrcLimit) 
				break;
			for(uint32_t x = 0, xMax = xCopyCells; x < xMax; ++x) {
				const uint32_t xSrc = xSrcOffset + x;
				if(true == src[ySrc * srcMetrics.x + xSrc])
					dst[yDst][xDstOffset + x]									= color;
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCoord>
						::gpk::error_t							check_bit_raster_contour				(const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<uint32_t> & srcMetrics, const ::gpk::SRectangle2D<_tCoord>& srcRect, const ::gpk::SCoord2<_tCoord> & cellToCheck)		{
		::gpk::SCoord2<_tCoord>											cellToCheckLeft							= {cellToCheck.x - 1, cellToCheck.y};
		::gpk::SCoord2<_tCoord>											cellToCheckRight						= {cellToCheck.x + 1, cellToCheck.y};
		::gpk::SCoord2<_tCoord>											cellToCheckTop							= {cellToCheck.x, cellToCheck.y + 1};
		::gpk::SCoord2<_tCoord>											cellToCheckBottom						= {cellToCheck.x, cellToCheck.y - 1};
		::gpk::SCoord2<_tCoord>											rectLimit								= srcRect.Offset + srcRect.Size;
		bool															result									
			=  ((uint32_t)cellToCheckLeft	.x) >= srcMetrics.x || ((uint32_t)cellToCheckLeft	.y) >= srcMetrics.y
			|| ((uint32_t)cellToCheckRight	.x) >= srcMetrics.x || ((uint32_t)cellToCheckRight	.y) >= srcMetrics.y
			|| ((uint32_t)cellToCheckTop	.x) >= srcMetrics.x || ((uint32_t)cellToCheckTop	.y) >= srcMetrics.y
			|| ((uint32_t)cellToCheckBottom	.x) >= srcMetrics.x || ((uint32_t)cellToCheckBottom	.y) >= srcMetrics.y
			|| (false == ::gpk::in_range(cellToCheckLeft	.y, srcRect.Offset.y, rectLimit.y))
			|| (false == ::gpk::in_range(cellToCheckRight	.y, srcRect.Offset.y, rectLimit.y))
			|| (false == ::gpk::in_range(cellToCheckTop		.y, srcRect.Offset.y, rectLimit.y))
			|| (false == ::gpk::in_range(cellToCheckBottom	.y, srcRect.Offset.y, rectLimit.y))
			;
			 if(false == result) {
				 result 
					=  (false == src[cellToCheckLeft	.y * srcMetrics.x + cellToCheckLeft		.x])
					|| (false == src[cellToCheckRight	.y * srcMetrics.x + cellToCheckRight	.x])
					|| (false == src[cellToCheckTop		.y * srcMetrics.x + cellToCheckTop		.x])
					|| (false == src[cellToCheckBottom	.y * srcMetrics.x + cellToCheckBottom	.x])
					;
			 }
		return result ? 1 : 0;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_raster_alpha_bit_contour	(::gpk::grid_view<_tCell>& dst, const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<uint32_t> & srcMetrics, const ::gpk::SRectangle2D<_tCoord>& srcRect, ::gpk::array_pod<::gpk::SCoord2<uint32_t>> & dstCoords)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)srcMetrics.x);			// 
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)srcMetrics.y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, srcMetrics, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= ySrcLimit) 
				break;
			for(uint32_t x = 0, xMax = xCopyCells; x < xMax; ++x) {
				const uint32_t													xSrc						= xSrcOffset + x;
				if((true == src[ySrc * srcMetrics.x + xSrc]) && check_bit_raster_contour(src, srcMetrics, srcRect, {(_tCoord)xSrc, (_tCoord)ySrc}))
					dstCoords.push_back({xDstOffset + x, (uint32_t)yDst});
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_raster_alpha_bit			(::gpk::grid_view<_tCell>& dst, const ::gpk::bit_view<uint32_t>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<uint32_t> & srcMetrics, const ::gpk::SRectangle2D<_tCoord>& srcRect, ::gpk::array_pod<::gpk::SCoord2<uint32_t>> & dstCoords)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)srcMetrics.x);			// 
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)srcMetrics.y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, srcMetrics, srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= ySrcLimit) 
				break;
			for(uint32_t x = 0, xMax = xCopyCells; x < xMax; ++x) {
				const uint32_t													xSrc						= xSrcOffset + x;
				if(true == src[ySrc * srcMetrics.x + xSrc])
					dstCoords.push_back({xDstOffset + x, (uint32_t)yDst});
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SRectangle2D<_tCoord>& srcRect, const _tCell& comparand)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)src.metrics().x);			// 
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src.metrics(), srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= ySrcLimit) 
				break;
			for(uint32_t x = 0, xMax = xCopyCells; x < xMax; ++x) {
				const uint32_t xSrc = xSrcOffset + x;
				if(comparand != src[ySrc][xSrc])
					dst[yDst][xDstOffset + x]									= src[ySrc][xSrc];
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SRectangle2D<_tCoord>& srcRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x			, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcRect.Offset.x		, 0, (int32_t)src.metrics().x);			// 
		const int32_t													ySrcLimit					= ::gpk::min((int32_t)(srcRect.Offset.y + srcRect.Size.y),  (int32_t)src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src.metrics(), srcRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)srcRect.Size.y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcRect.Offset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= ySrcLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstRect.Offset.x		, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcOffset.x			, 0, (int32_t)src.metrics().x);			// 
		const int32_t													yDstLimit					= ::gpk::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src.metrics(), dstRect, xDstOffset, xSrcOffset);
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = src.metrics().y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstRect.Offset.y;
			const int32_t													ySrc						= y + (int32_t)srcOffset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= yDstLimit || ySrc >= (int32_t)src.metrics().y) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						uint32_t								grid_copy_row_calc			(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, uint32_t xDstOffset)		{
		return (uint32_t)::gpk::min	// Make sure everything is in range.
			( ::gpk::min
				( (int32_t)src.metrics().x
				, ::gpk::max(0, (int32_t)(dst.metrics().x - xDstOffset))
				)
			, (int32_t)dstRect.Size.x);
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect, const _tCell& comparand)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.metrics().x);			// 
		const int32_t													yDstLimit					= ::gpk::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, dstRect, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.metrics().y; ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstRect.Offset.y;
			if(yDst < 0)
				continue;
			if(yDst >= yDstLimit) 
				break;
			for(uint32_t x = 0, xStop = xCopyCells; x < xStop; ++x)
				if(comparand != src[ySrc][x])
					dst[yDst][x + xDstOffset]									= src[ySrc][x];
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SRectangle2D<_tCoord>& dstRect)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstRect.Offset.x, 0, (int32_t)dst.metrics().x);			// 
		const int32_t													yDstLimit					= ::gpk::min((int32_t)(dstRect.Offset.y + dstRect.Size.y),  (int32_t)dst.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, dstRect, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.metrics().y; ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstRect.Offset.y;
			if(yDst < 0)
				continue;
			if(yDst >= yDstLimit) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, uint32_t xDstOffset, uint32_t xSrcOffset)		{
		return ::gpk::max(0, (int32_t)::gpk::min((int32_t)src.metrics().x - xSrcOffset, (int32_t)dst.metrics().x - xDstOffset));	// Make sure everything is in range.
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const ::gpk::SCoord2<_tCoord>& srcOffset)		{
		const uint32_t													xDstOffset					= (uint32_t)::gpk::clamp((int32_t)dstOffset.x, 0, (int32_t)dst.metrics().x);			// 
		const uint32_t													xSrcOffset					= (uint32_t)::gpk::clamp((int32_t)srcOffset.x, 0, (int32_t)src.metrics().x);			// 
		const uint32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, xDstOffset, xSrcOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t y = 0, yMax = (int32_t)src.metrics().y; y < yMax; ++y) {
			const int32_t													yDst						= y + (int32_t)dstOffset.y;
			const int32_t													ySrc						= y + (int32_t)srcOffset.y;
			if(yDst < 0 || ySrc < 0)
				continue;
			if(yDst >= (int32_t)dst.metrics().y || ySrc >= (int32_t)src.metrics().y) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][xSrcOffset], sizeof(_tCell) * xCopyCells);	// memcpy the row from src to dst.
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						uint32_t								grid_copy_row_calc			(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, uint32_t xDstOffset)		{
		return (uint32_t)::gpk::max(0, (int32_t)::gpk::min((int32_t)src.metrics().x, (int32_t)dst.metrics().x - (int32_t)xDstOffset));
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset)		{
		const int32_t													xDstOffset					= (int32_t)::gpk::clamp(0, (int32_t)dstOffset.x, (int32_t)dst.metrics().x);			// 
		const int32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.metrics().y; ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstOffset.y;
			if(yDst < 0) 
				continue;
			if(yDst >= (int32_t)dst.metrics().y) 
				break;
			memcpy(&dst[yDst][xDstOffset], &src[ySrc][0], sizeof(_tCell) * xCopyCells);
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell, typename _tCoord>
						::gpk::error_t							grid_copy_alpha				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src, const ::gpk::SCoord2<_tCoord>& dstOffset, const _tCell& comparand)		{
		const int32_t													xDstOffset					= ::gpk::min((int32_t)dstOffset.x, (int32_t)dst.metrics().x);			// 
		const int32_t													xCopyCells					= ::gpk::grid_copy_row_calc(dst, src, xDstOffset);	// 
		uint32_t														elementsCopied				= 0;
		for(int32_t ySrc = 0, yMax = (int32_t)src.metrics().y; ySrc < yMax; ++ySrc) {
			const int32_t													yDst						= ySrc + (int32_t)dstOffset.y;
			if(yDst < 0) 
				continue;
			if(yDst >= (int32_t)dst.metrics().y) 
				break;
			for(int32_t x = 0, xStop = xCopyCells; x < xStop; ++x) {
				if((x + xDstOffset) >= (int32_t)dst.metrics().x)
					break;
				if((x + xDstOffset) < 0)
					continue;
				if(comparand != src[ySrc][x])
					dst[yDst][x + xDstOffset]									= src[ySrc][x];
			}
			elementsCopied												+= xCopyCells;
		}
		return elementsCopied;
	}

	template<typename _tCell>
						::gpk::error_t							grid_copy					(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows					= ::gpk::min(dst.metrics().y, src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::min(dst.width (), src.width ());
		for(uint32_t y = 0; y < yCopyRows; ++y) 
			memcpy(&dst[y][0], &src[y][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells * yCopyRows;
	}

	template<typename _tCell>
						::gpk::error_t							grid_mirror_x				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows					= ::gpk::min(dst.metrics().y, src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::min(dst.width (), src.width ());
		uint32_t														srcMaxX						= src.metrics().x - 1;
		for(uint32_t y = 0; y < yCopyRows; ++y) 
		for(uint32_t x = 0; x < xCopyCells; ++x) 
			dst[y][x]													= src[y][srcMaxX - x];
		return xCopyCells * yCopyRows;
	}

	template<typename _tCell>
						::gpk::error_t							grid_mirror_y				(::gpk::grid_view<_tCell>& dst, const ::gpk::grid_view<_tCell>& src)		{
		const uint32_t													yCopyRows					= ::gpk::min(dst.metrics().y, src.metrics().y);
		const uint32_t													xCopyCells					= ::gpk::min(dst.width (), src.width ());
		uint32_t														srcMaxY						= src.metrics().y - 1;
		for(uint32_t y = 0; y < yCopyRows; ++y) 
			memcpy(&dst[y][0], &src[srcMaxY - y][0], sizeof(_tCell) * xCopyCells);
		return xCopyCells * yCopyRows;
	}
} // namespace

#endif // GPK_GRID_COPY_20983429834
