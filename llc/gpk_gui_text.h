#include "gpk_array.h"
#include "gpk_grid.h"
#include "gpk_view_bit.h"
#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"

#ifndef GPK_GUI_TEXT_H_23627
#define GPK_GUI_TEXT_H_23627

namespace gpk
{
	sinx	err_t	textLineCalcWidth				(n2u0_c & sizeCharCell, vcst_c & text0)		{ return (err_t)(sizeCharCell.x * text0.size()); }

	tplt<tpnm _tColor>
	static	err_t	textLineDrawFixedSize			(grid<_tColor> & target, const grid<_tColor> & viewTextureFont, u1_t characterCellsX, s1_t dstOffsetY, const n2u0_t & sizeCharCell, vcst_c & text0, const n2s1_t dstTextOffset)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		for(s2_t iChar = 0, charCount = (s2_t)text0.size(); iChar < charCount; ++iChar) {
			const uc_t					charToDraw						= (uc_t)text0[iChar];
			s1_t						coordTableX						= charToDraw % characterCellsX;
			s1_t						coordTableY						= charToDraw / characterCellsX;
			const n2s1_t			coordCharTable					= {s1_t(coordTableX * sizeCharCell.x), s1_t(coordTableY * sizeCharCell.y)};
			const rect2s1_t		dstRect0						= {{s1_t(sizeCharCell.x * iChar), dstOffsetY}, sizeCharCell.s1()};
			const rect2s1_t		srcRect0						= {coordCharTable, sizeCharCell.s1()};
			if_true_e(failed(grid_copy_alpha(target, viewTextureFont, rect2s1_t{dstTextOffset + dstRect0.Offset, dstRect0.Size}, srcRect0.Offset, {0xFF, 0x00, 0xFF, 0xFF})));
			//if_true_e(failed(grid_copy(target, viewTextureFont, dstTextOffset + dstOffset1, srcRect0)));
		}
		return 0;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	tplt<tpnm _tColor>
	static	err_t	textLineDrawFixedSizeLit		(grid<_tColor> & target, const view_bit<u3_t> & viewTextureFont, const n2u2_t & viewMetrics, u1_t characterCellsX, s1_t dstOffsetY, const n2u0_t & sizeCharCell, const vcst_t & text0, const n2s1_t dstTextOffset, const _tColor& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		apod<n2u1_t>	dstCoords;
		for(s2_t iChar = 0, charCount = (s2_t)text0.size(); iChar < charCount; ++iChar) {
			const uc_t					charToDraw						= (uc_t)text0[iChar];
			const s1_t				coordTableX						= charToDraw % characterCellsX;
			const s1_t				coordTableY						= charToDraw / characterCellsX;
			const n2s1_t			coordCharTable					= {s1_t(coordTableX * sizeCharCell.x), s1_t(coordTableY * sizeCharCell.y)};
			const rect2s1_t		dstRect0						= {{s1_t(sizeCharCell.x * iChar), dstOffsetY}, sizeCharCell.s1()};
			const rect2s1_t		srcRect0						= {coordCharTable, sizeCharCell.s1()};
			//if_true_e(failed(grid_copy_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, color, srcRect0)));
			if_true_e(failed(grid_raster_alpha_bit(target.metrics(), viewTextureFont, viewMetrics, rect2s1_t{dstTextOffset + dstRect0.Offset, sizeCharCell.s1()}, srcRect0.Offset, dstCoords)));
			//if_true_e(failed(grid_raster_alpha_bit(target, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, srcRect0, dstCoords)));
		}
		for(u2_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
			drawPixelLight(target, dstCoords[iCoord], color, 0.05f, 0.75);
		return 0;
	}

	tplt<tpnm _tColor>
	static	err_t	textLineDrawAlignedFixedSize	(grid<_tColor> & targetView, const grid<_tColor> & fontAtlas, u1_t lineOffset, const n2u1_t & targetSize, const n2u0_t & sizeCharCell, const vcst_t & text0)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const n2s1_t			dstTextOffset					= {s1_t((targetSize.x >> 1) - (textLineCalcWidth(sizeCharCell, text0) >> 1)), };
		u1_t					dstOffsetY						= s1_t(lineOffset * sizeCharCell.y);
		return textLineDrawFixedSize(targetView, fontAtlas, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset);
	}

	tplt<tpnm _tColor>
	static	err_t	textLineDrawAlignedFixedSizeLit	(grid<_tColor> & targetView, const view_bit<u3_t> & fontAtlas, const n2u2_t & viewMetrics, u1_t lineOffset, const n2u1_t & targetSize, const n2u0_t & sizeCharCell, const vcst_t & text0, const _tColor & color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		const n2s1_t			dstTextOffset					= {s1_t((targetSize.x >> 1) - (textLineCalcWidth(sizeCharCell, text0) >> 1)), };
		u1_t					dstOffsetY						= s1_t(lineOffset * sizeCharCell.y);
		return textLineDrawFixedSizeLit(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, text0, dstTextOffset, color);
	}

	tplt<tpnm _tColor>
	static	err_t	textLineRaster					(const n2u2_t & targetMetrics, const n2u0_t & fontCharSize, const rect2<s1_t> & targetRect, const imgmono<_tColor> & fontTexture, vcst_c & textToRaster, apod<n2u1_t> & dstCoords)		{
		const s2_t				charsPerRow						= fontTexture.Pitch / fontCharSize.x;
		for(u2_t iChar = 0, countChars = textToRaster.size(); iChar < countChars; ++iChar) {
			const uc_t					charToDraw						= (uc_t)textToRaster[iChar];
			const s2_t				coordTableX						= charToDraw % charsPerRow;
			const s2_t				coordTableY						= charToDraw / charsPerRow;
			const n2s2_t			coordCharTable					= {coordTableX * fontCharSize.x, coordTableY * fontCharSize.y};
			const rect2s2_t		srcRect0						= {coordCharTable, fontCharSize.s2()};
			rect2s2_t				dstRect1						= {{s2_t(targetRect.Offset.x + fontCharSize.x * iChar), targetRect.Offset.y}, fontCharSize.s2()};
			//dstRect1.Size.x		+= min(0, (s2_t)targetRect.Offset.x);
			dstRect1.Size.y			+= min<s1_t>(0, targetRect.Offset.y);
			if_true_e(failed(grid_raster_alpha_bit(targetMetrics, fontTexture.View, {u2_t(charsPerRow * fontCharSize.x), 256U * fontCharSize.y}, dstRect1, srcRect0.Offset, dstCoords)));
		}
		return 0;
	}
} // namespace

#endif // GPK_GUI_TEXT_H_23627
