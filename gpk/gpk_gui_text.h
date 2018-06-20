#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_grid_view.h"
#include "gpk_bit_view.h"
#include "gpk_grid_copy.h"

#ifndef GPK_GUI_TEXT_H_928734982734
#define GPK_GUI_TEXT_H_928734982734

namespace gpk 
{
	struct SGUIText {
									::gpk::view_const_string							Text								= {};
									::gpk::SCoord2<uint32_t>							Metrics								= {};
									::gpk::ALIGN										Align								= {};
	};


	static inline constexpr		::gpk::error_t										textLineCalcSize							(const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0)	{ return (::gpk::error_t)(sizeCharCell.x * (text0.size() - 1)); }

	template<typename _tColor>
	static						::gpk::error_t										textLineDrawFixedSize							(::gpk::grid_view<_tColor>& bmpTarget, const ::gpk::grid_view<_tColor>& viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const ::gpk::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			int32_t																				coordTableX										= text0[iChar] % characterCellsX;
			int32_t																				coordTableY										= text0[iChar] / characterCellsX;
			const ::gpk::SCoord2<int32_t>														coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::gpk::SRectangle2D<int32_t>													dstRect0										= {{sizeCharCell.x * iChar, dstOffsetY}, sizeCharCell};
			const ::gpk::SRectangle2D<int32_t>													srcRect0										= {coordCharTable, sizeCharCell};
			error_if(errored(::gpk::grid_copy_alpha(bmpTarget, viewTextureFont, ::gpk::SRectangle2D<int32_t>{dstTextOffset + dstRect0.Offset, dstRect0.Size}, srcRect0.Offset, {0xFF, 0x00, 0xFF, 0xFF})), "I believe this never fails.");
			//error_if(errored(::gpk::grid_copy(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
		}
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename _tColor>
	static				::gpk::error_t												textLineDrawFixedSizeLit						(::gpk::grid_view<_tColor>& bmpTarget, const ::gpk::bit_view<uint32_t>& viewTextureFont, const ::gpk::SCoord2<uint32_t> & viewMetrics, uint32_t characterCellsX, int32_t dstOffsetY, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const ::gpk::SCoord2<int32_t> dstTextOffset, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::gpk::array_pod<::gpk::SCoord2<uint32_t>>										dstCoords;
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			const int32_t																	coordTableX										= text0[iChar] % characterCellsX;
			const int32_t																	coordTableY										= text0[iChar] / characterCellsX;
			const ::gpk::SCoord2<int32_t>													coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::gpk::SRectangle2D<int32_t>												dstRect0										= {{sizeCharCell.x * iChar, dstOffsetY}, sizeCharCell};
			const ::gpk::SRectangle2D<int32_t>												srcRect0										= {coordCharTable, sizeCharCell};
			//error_if(errored(::gpk::grid_copy_alpha_bit(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, color, srcRect0)), "I believe this never fails.");
			dstCoords.clear();
			error_if(errored(::gpk::grid_raster_alpha_bit(bmpTarget.metrics(), viewTextureFont, viewMetrics, ::gpk::SRectangle2D<int32_t>{dstTextOffset + dstRect0.Offset, sizeCharCell}, srcRect0.Offset, dstCoords)), "I believe this never fails.");
			//error_if(errored(::gpk::grid_raster_alpha_bit(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, srcRect0, dstCoords)), "I believe this never fails.");
			for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
				::gpk::drawPixelLight(bmpTarget, dstCoords[iCoord], color, 0.05f, 0.75);
		}
		return 0;
	}

	template<typename _tColor>
	static				::gpk::error_t												textLineDrawAlignedFixedSizeRGBA			(::gpk::grid_view<_tColor>& targetView, const ::gpk::grid_view<_tColor>& fontAtlas, uint32_t lineOffset, const ::gpk::SCoord2<uint32_t>& targetSize, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0 )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::SCoord2<int32_t>														dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)::gpk::textLineCalcSize(sizeCharCell, text0) / 2, };
		uint32_t																			dstOffsetY									= (int32_t)(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset);
	}

	template<typename _tColor>
	static				::gpk::error_t												textLineDrawAlignedFixedSizeLit					(::gpk::grid_view<_tColor>& targetView, const ::gpk::bit_view<uint32_t>& fontAtlas, const ::gpk::SCoord2<uint32_t> & viewMetrics, uint32_t lineOffset, const ::gpk::SCoord2<uint32_t>& targetSize, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::SCoord2<int32_t>														dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)::gpk::textLineCalcSize(sizeCharCell, text0) / 2, };
		uint32_t																			dstOffsetY									= (int32_t)(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSizeLit(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset, color);
	}
} // namespace

#endif // GPK_GUI_TEXT_H_928734982734
