#include "gpk_array.h"
#include "gpk_view_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"

#ifndef GPK_GUI_TEXT_H_928734982734
#define GPK_GUI_TEXT_H_928734982734

namespace gpk
{
	stincxp	::gpk::error_t	textLineCalcWidth				(const ::gpk::n2u8 & sizeCharCell, const ::gpk::vcs & text0)		{ return (::gpk::error_t)(sizeCharCell.x * text0.size()); }

	template<typename _tColor>
	static	::gpk::error_t	textLineDrawFixedSize			(::gpk::view2d<_tColor> & target, const ::gpk::view2d<_tColor> & viewTextureFont, uint16_t characterCellsX, int16_t dstOffsetY, const ::gpk::n2u8 & sizeCharCell, const ::gpk::vcs & text0, const ::gpk::n2i16 dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			const unsigned char			charToDraw						= (unsigned char)text0[iChar];
			int16_t						coordTableX						= charToDraw % characterCellsX;
			int16_t						coordTableY						= charToDraw / characterCellsX;
			const ::gpk::n2i16			coordCharTable					= {int16_t(coordTableX * sizeCharCell.x), int16_t(coordTableY * sizeCharCell.y)};
			const ::gpk::rect2i16		dstRect0						= {{int16_t(sizeCharCell.x * iChar), dstOffsetY}, sizeCharCell.i16()};
			const ::gpk::rect2i16		srcRect0						= {coordCharTable, sizeCharCell.i16()};
			gerror_if(errored(::gpk::grid_copy_alpha(target, viewTextureFont, ::gpk::rect2i16{dstTextOffset + dstRect0.Offset, dstRect0.Size}, srcRect0.Offset, {0xFF, 0x00, 0xFF, 0xFF})), "%s", "I believe this never fails.");
			//gerror_if(errored(::gpk::grid_copy(target, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)), "I believe this never fails.");
		}
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<typename _tColor>
	static	::gpk::error_t	textLineDrawFixedSizeLit		(::gpk::view2d<_tColor> & target, const ::gpk::view_bit<uint64_t> & viewTextureFont, const ::gpk::n2u32 & viewMetrics, uint16_t characterCellsX, int16_t dstOffsetY, const ::gpk::n2u8 & sizeCharCell, const ::gpk::vcs & text0, const ::gpk::n2i16 dstTextOffset, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::gpk::apod<::gpk::n2u16>	dstCoords;
		for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
			const unsigned char			charToDraw						= (unsigned char)text0[iChar];
			const int16_t				coordTableX						= charToDraw % characterCellsX;
			const int16_t				coordTableY						= charToDraw / characterCellsX;
			const ::gpk::n2i16			coordCharTable					= {int16_t(coordTableX * sizeCharCell.x), int16_t(coordTableY * sizeCharCell.y)};
			const ::gpk::rect2i16		dstRect0						= {{int16_t(sizeCharCell.x * iChar), dstOffsetY}, sizeCharCell.i16()};
			const ::gpk::rect2i16		srcRect0						= {coordCharTable, sizeCharCell.i16()};
			//gerror_if(errored(::gpk::grid_copy_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, color, srcRect0)), "I believe this never fails.");
			gerror_if(errored(::gpk::grid_raster_alpha_bit(target.metrics(), viewTextureFont, viewMetrics, ::gpk::rect2i16{dstTextOffset + dstRect0.Offset, sizeCharCell.i16()}, srcRect0.Offset, dstCoords)), "%s", "I believe this never fails.");
			//gerror_if(errored(::gpk::grid_raster_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, srcRect0, dstCoords)), "I believe this never fails.");
		}
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
			::gpk::drawPixelLight(target, dstCoords[iCoord], color, 0.05f, 0.75);
		return 0;
	}

	template<typename _tColor>
	static	::gpk::error_t	textLineDrawAlignedFixedSize	(::gpk::view2d<_tColor> & targetView, const ::gpk::view2d<_tColor> & fontAtlas, uint16_t lineOffset, const ::gpk::n2u16 & targetSize, const ::gpk::n2u8 & sizeCharCell, const ::gpk::vcs & text0)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::n2i16			dstTextOffset					= {int16_t((targetSize.x >> 1) - (::gpk::textLineCalcWidth(sizeCharCell, text0) >> 1)), };
		uint16_t					dstOffsetY						= int16_t(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset);
	}

	template<typename _tColor>
	static	::gpk::error_t	textLineDrawAlignedFixedSizeLit	(::gpk::view2d<_tColor> & targetView, const ::gpk::view_bit<uint64_t> & fontAtlas, const ::gpk::n2u32 & viewMetrics, uint16_t lineOffset, const ::gpk::n2u16 & targetSize, const ::gpk::n2u8 & sizeCharCell, const ::gpk::vcs & text0, const _tColor & color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const ::gpk::n2i16			dstTextOffset					= {int16_t((targetSize.x >> 1) - (::gpk::textLineCalcWidth(sizeCharCell, text0) >> 1)), };
		uint16_t					dstOffsetY						= int16_t(lineOffset * sizeCharCell.y);
		return ::gpk::textLineDrawFixedSizeLit(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset, color);
	}

	template<typename _tColor>
	static	::gpk::error_t	textLineRaster					(const ::gpk::n2u32 & targetMetrics, const ::gpk::n2u8 & fontCharSize, const ::gpk::rect2<int16_t> & targetRect, const ::gpk::SImageMonochrome<_tColor> & fontTexture, const ::gpk::vcs & textToRaster, ::gpk::apod<::gpk::n2u16> & dstCoords)		{
		const int32_t				charsPerRow						= fontTexture.Pitch / fontCharSize.x;
		for(uint32_t iChar = 0, countChars = textToRaster.size(); iChar < countChars; ++iChar) {
			const unsigned char			charToDraw						= (unsigned char)textToRaster[iChar];
			const int32_t				coordTableX						= charToDraw % charsPerRow;
			const int32_t				coordTableY						= charToDraw / charsPerRow;
			const ::gpk::n2i32			coordCharTable					= {coordTableX * fontCharSize.x, coordTableY * fontCharSize.y};
			const ::gpk::rect2i32		srcRect0						= {coordCharTable, fontCharSize.i32()};
			::gpk::rect2i32				dstRect1						= {{int32_t(targetRect.Offset.x + fontCharSize.x * iChar), targetRect.Offset.y}, fontCharSize.i32()};
			//dstRect1.Size.x		+= ::gpk::min(0, (int32_t)targetRect.Offset.x);
			dstRect1.Size.y			+= ::gpk::min<int16_t>(0, targetRect.Offset.y);
			gerror_if(errored(::gpk::grid_raster_alpha_bit(targetMetrics, fontTexture.View, {uint32_t(charsPerRow * fontCharSize.x), 256U * fontCharSize.y}, dstRect1, srcRect0.Offset, dstCoords)), "%s", "I believe this never fails.");
		}
		return 0;
	}
} // namespace

#endif // GPK_GUI_TEXT_H_928734982734
