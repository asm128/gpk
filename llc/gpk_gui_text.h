#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"

#ifndef GPK_GUI_TEXT_H_928734982734
#define GPK_GUI_TEXT_H_928734982734

namespace gpk
{
	static inline constexpr	::gpk::error_t										textLineCalcWidth								(const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0)		{ return (::gpk::error_t)(sizeCharCell.x * text0.size()); }

	template<typename _tColor>
	static					::gpk::error_t										textLineDrawFixedSize							(::gpk::view_grid<_tColor>& target, const ::gpk::view_grid<_tColor>& viewTextureFont, uint32_t characterCellsX, int32_t dstOffsetY, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const ::gpk::SCoord2<int32_t> dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			int32_t																			coordTableX										= text0[iChar] % characterCellsX;
			int32_t																			coordTableY										= text0[iChar] / characterCellsX;
			const ::gpk::SCoord2<int32_t>													coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::gpk::SRectangle2D<int32_t>												dstRect0										= {{sizeCharCell.x * iChar, dstOffsetY}, sizeCharCell};
			const ::gpk::SRectangle2D<int32_t>												srcRect0										= {coordCharTable, sizeCharCell};
			gerror_if(errored(::gpk::grid_copy_alpha(target, viewTextureFont, ::gpk::SRectangle2D<int32_t>{dstTextOffset + dstRect0.Offset, dstRect0.Size}, srcRect0.Offset, {0xFF, 0x00, 0xFF, 0xFF})), "%s", "I believe this never fails.");
			//gerror_if(errored(::gpk::grid_copy(target, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
		}
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename _tColor>
	static					::gpk::error_t										textLineDrawFixedSizeLit						(::gpk::view_grid<_tColor>& target, const ::gpk::view_bit<uint32_t>& viewTextureFont, const ::gpk::SCoord2<uint32_t> & viewMetrics, uint32_t characterCellsX, int32_t dstOffsetY, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const ::gpk::SCoord2<int32_t> dstTextOffset, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::gpk::array_pod<::gpk::SCoord2<int32_t>>										dstCoords;
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			const int32_t																	coordTableX										= text0[iChar] % characterCellsX;
			const int32_t																	coordTableY										= text0[iChar] / characterCellsX;
			const ::gpk::SCoord2<int32_t>													coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
			const ::gpk::SRectangle2D<int32_t>												dstRect0										= {{sizeCharCell.x * iChar, dstOffsetY}, sizeCharCell};
			const ::gpk::SRectangle2D<int32_t>												srcRect0										= {coordCharTable, sizeCharCell};
			//gerror_if(errored(::gpk::grid_copy_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, color, srcRect0)), "I believe this never fails.");
			gerror_if(errored(::gpk::grid_raster_alpha_bit(target.metrics(), viewTextureFont, viewMetrics, ::gpk::SRectangle2D<int32_t>{dstTextOffset + dstRect0.Offset, sizeCharCell}, srcRect0.Offset, dstCoords)), "%s", "I believe this never fails.");
			//gerror_if(errored(::gpk::grid_raster_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, srcRect0, dstCoords)), "I believe this never fails.");
		}
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
			::gpk::drawPixelLight(target, dstCoords[iCoord], color, 0.05f, 0.75);
		return 0;
	}

	template<typename _tColor>
	static					::gpk::error_t										textLineDrawAlignedFixedSizeRGBA				(::gpk::view_grid<_tColor>& targetView, const ::gpk::view_grid<_tColor>& fontAtlas, uint32_t lineOffset, const ::gpk::SCoord2<uint32_t>& targetSize, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0 )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::SCoord2<int32_t>													dstTextOffset									= {(int32_t)targetSize.x / 2 - (int32_t)::gpk::textLineCalcWidth(sizeCharCell, text0) / 2, };
		uint32_t																		dstOffsetY										= (int32_t)(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset);
	}

	template<typename _tColor>
	static					::gpk::error_t										textLineDrawAlignedFixedSizeLit					(::gpk::view_grid<_tColor>& targetView, const ::gpk::view_bit<uint32_t>& fontAtlas, const ::gpk::SCoord2<uint32_t> & viewMetrics, uint32_t lineOffset, const ::gpk::SCoord2<uint32_t>& targetSize, const ::gpk::SCoord2<int32_t>& sizeCharCell, const ::gpk::view_const_string& text0, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::SCoord2<int32_t>													dstTextOffset									= {(int32_t)targetSize.x / 2 - (int32_t)::gpk::textLineCalcWidth(sizeCharCell, text0) / 2, };
		uint32_t																		dstOffsetY										= (int32_t)(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSizeLit(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset, color);
	}

	template<typename _tColor>
	static					::gpk::error_t										textLineRaster									(const ::gpk::SCoord2<uint32_t> & targetMetrics, const ::gpk::SCoord2<uint16_t> & fontCharSize, const ::gpk::SRectangle2D<int16_t> & targetRect, const ::gpk::SImageMonochrome<_tColor>& fontTexture, const ::gpk::view_const_string& textToRaster, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & dstCoords)		{
		const uint32_t																	charsPerRow										= fontTexture.Pitch / fontCharSize.x;
		for(uint32_t iChar = 0, countChars = (uint32_t)textToRaster.size(); iChar < countChars; ++iChar) {
			char																			charToDraw										= textToRaster[iChar];
			const int32_t																	coordTableX										= charToDraw % (int32_t)charsPerRow;
			const int32_t																	coordTableY										= charToDraw / (int32_t)charsPerRow;
			const ::gpk::SCoord2<int32_t>													coordCharTable									= ::gpk::SCoord2<int32_t>{coordTableX * fontCharSize.x, coordTableY * fontCharSize.y};
			const ::gpk::SRectangle2D<int32_t>												srcRect0										= {coordCharTable, fontCharSize.Cast<int32_t>()};
			::gpk::SRectangle2D<int32_t>													dstRect1										= {{((int32_t)targetRect.Offset.x + (int32_t)fontCharSize.x * (int32_t)iChar), targetRect.Offset.y}, fontCharSize.Cast<int32_t>()};
			//dstRect1.Size.x																+= ::gpk::min(0, (int32_t)targetRect.Offset.x);
			dstRect1.Size.y																+= ::gpk::min(0, (int32_t)targetRect.Offset.y);
			gerror_if(errored(::gpk::grid_raster_alpha_bit(targetMetrics, fontTexture.View, {charsPerRow * fontCharSize.x, 256U * fontCharSize.y}, dstRect1, srcRect0.Offset, dstCoords)), "%s", "I believe this never fails.");
		}
		return 0;
	}
} // namespace

#endif // GPK_GUI_TEXT_H_928734982734
