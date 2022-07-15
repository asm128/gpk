#include "gpk_view_stream.h"
#include "deflate.h"
#include "gpk_png.h"
#include "gpk_adam7.h"
#include "gpk_io.h"
#include "gpk_storage.h"

static			::gpk::error_t											pngScanLineSizeFromFormat						(int32_t colorType, int32_t bitDepth, int32_t imageWidth)	{
	int32_t																		scanLineWidth									= imageWidth;
	switch(bitDepth) {
	default	  : return -1; // ?? Corrupt file?
	case	 1: { scanLineWidth /= 8;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 2: { scanLineWidth /= 4;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 4: { scanLineWidth /= 2;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 8: {						if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }																						} break;
	case	16: { scanLineWidth *= 2;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }																						} break;
	}

	switch(bitDepth) {
	default	  : return scanLineWidth;
	case	 1: return scanLineWidth + ((imageWidth % 8) ? 1 : 0);
	case	 2: return scanLineWidth + ((imageWidth % 4) ? 1 : 0);
	case	 4: return scanLineWidth + ((imageWidth % 2) ? 1 : 0);
	}
}

static			::gpk::error_t											pngScanlineDecode_2_8							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const ::gpk::color_rgb<uint8_t>>					viewPixels										= {(const ::gpk::color_rgb<uint8_t>*)scanline.begin(), scanline.size() / 3};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			const ::gpk::color_rgb<uint8_t>												& pixelInput									= viewPixels[iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_8							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			uint8_t																		pixelInput										= scanline[iPixel];
			pixelOutput																= {pixelInput, pixelInput, pixelInput, 255};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_2_16							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	static constexpr const double												unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const ::gpk::color_rgb<uint16_t>>					viewPixels										= {(const ::gpk::color_rgb<uint16_t>*)scanline.begin(), scanline.size() / 6};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_rgb<uint16_t>													pixelInput										= viewPixels[iPixel];
			be2le_16(pixelInput.b);
			be2le_16(pixelInput.g);
			be2le_16(pixelInput.r);
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput.b															= (uint8_t)(unitChannel * pixelInput.b);
			pixelOutput.g															= (uint8_t)(unitChannel * pixelInput.g);
			pixelOutput.r															= (uint8_t)(unitChannel * pixelInput.r);
			pixelOutput.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_16							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	static constexpr const double												unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const uint16_t>										viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint16_t																	toReverse										= viewPixels[iPixel];
			be2le_16(toReverse);
			uint8_t																		largeValue										= (uint8_t)(unitChannel * toReverse);
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput																= {largeValue, largeValue, largeValue, 255};
		}
	}
	return 0;
}
static			::gpk::error_t											pngScanlineDecode_6_8							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const ::gpk::color_rgba<uint8_t>>					viewPixels										= {(const ::gpk::color_rgba<uint8_t>*)scanline.begin(), scanline.size() / 4};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			const ::gpk::color_rgba<uint8_t>											& pixelInput									= viewPixels[iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, pixelInput.a};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_6_16							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	static constexpr const double												unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const ::gpk::color_rgba<uint16_t>>					viewPixels										= {(const ::gpk::color_rgba<uint16_t>*)scanline.begin(), scanline.size() / 8};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_rgba<uint16_t>													colorReversed									= viewPixels[iPixel];
			be2le_16(colorReversed.b);
			be2le_16(colorReversed.g);
			be2le_16(colorReversed.r);
			be2le_16(colorReversed.a);
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput.b															= (uint8_t)(unitChannel * colorReversed.b);
			pixelOutput.g															= (uint8_t)(unitChannel * colorReversed.g);
			pixelOutput.r															= (uint8_t)(unitChannel * colorReversed.r);
			pixelOutput.a															= (uint8_t)(unitChannel * colorReversed.a);
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_4_8							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_array<const uint16_t>										viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint16_t																valueInput										= viewPixels[iPixel];
			const uint8_t																valueGrayscale									= valueInput & 0xFFU;
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput																= {valueGrayscale, valueGrayscale, valueGrayscale, (uint8_t)((valueInput & 0xFF00U) >> 8)};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_4_16							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines)		{
	static constexpr const double												unitGreyscale									= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view_array<uint32_t>													viewPixels										= {(uint32_t*)scanline.begin(), scanline.size() / 4};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint32_t																packedValues									= viewPixels[iPixel];
			uint16_t																	valueGrayscale									= (packedValues & 0xFFFFU);
			uint16_t																	valueAlpha										= (packedValues & 0xFFFF0000U) >> 16;
			be2le_16(valueGrayscale	);
			be2le_16(valueAlpha		);
			const uint8_t																greyFinal										= (uint8_t)(unitGreyscale * valueGrayscale);
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput																= {greyFinal, greyFinal, greyFinal, (uint8_t)(unitGreyscale * valueAlpha)};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_3_8							(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines, const ::gpk::view_array<::gpk::color_bgr<uint8_t>>& palette)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			uint8_t																		paletteIndex									= scanline[iPixel];
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			const ::gpk::color_bgr<uint8_t>												& pixelInput									= palette[paletteIndex];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_3_bits						(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines, const ::gpk::view_array<::gpk::color_bgr<uint8_t>>& palette, uint32_t bitDepth)	{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		indexPalette									= 0;
			::gpk::view_bit<uint8_t>													indexPaletteBits								= {&indexPalette, (uint32_t)8};
			for(uint32_t i=0; i < bitDepth; ++i)
				indexPaletteBits[i]														= viewPixels[bitDepth * iPixel + i];

			const ::gpk::color_bgr<uint8_t>												& pixelInput									= (indexPalette >= palette.size()) ? ::gpk::color_bgr<uint8_t>{} : palette[indexPalette];
			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_bits						(::gpk::view_grid<::gpk::SColorBGRA>& out_View, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>> & scanlines, uint32_t bitDepth)	{
	double																		unitGrayscale									= 1.0 / ((1 << bitDepth) - 1) * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		valueGrayscale								= 0;
			::gpk::view_bit<uint8_t>													valueGrayscaleBits							= {&valueGrayscale, (uint32_t)8};
			for(uint32_t i = 0; i < bitDepth; ++i)
				valueGrayscaleBits[i]													= viewPixels[bitDepth * iPixel + i];

			::gpk::SColorBGRA															& pixelOutput									= out_View[iScanline][iPixel];
			pixelOutput.r															=
			pixelOutput.g															=
			pixelOutput.b															= (1 == bitDepth) ? (uint8_t)(valueGrayscale * 255) : (uint8_t)(valueGrayscale * unitGrayscale);
			pixelOutput.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngDecode
	( const int32_t												bitDepth
	, const int32_t												colorType
	, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>>	& scanlines
	, const ::gpk::view_array<::gpk::color_bgr<uint8_t>>		& palette
	, ::gpk::view_grid<::gpk::SColorBGRA>						& out_View
	) {
	switch(colorType) {
	default: error_printf("Invalid color type: %u.", colorType); return -1;
	case  2: ree_if(bitDepth != 8 && bitDepth != 16, "Invalid bit depth: %u.", bitDepth);									return (bitDepth == 8) ? ::pngScanlineDecode_2_8(out_View, scanlines)			: ::pngScanlineDecode_2_16  (out_View, scanlines);
	case  4: ree_if(bitDepth != 8 && bitDepth != 16, "Invalid bit depth: %u.", bitDepth);									return (bitDepth == 8) ? ::pngScanlineDecode_4_8(out_View, scanlines)			: ::pngScanlineDecode_4_16  (out_View, scanlines);
	case  6: ree_if(bitDepth != 8 && bitDepth != 16, "Invalid bit depth: %u.", bitDepth);									return (bitDepth == 8) ? ::pngScanlineDecode_6_8(out_View, scanlines)			: ::pngScanlineDecode_6_16  (out_View, scanlines);
	case  3: ree_if(bitDepth != 1 && bitDepth != 2 && bitDepth != 4 && bitDepth != 8, "Invalid bit depth: %u.", bitDepth);	return (bitDepth == 8) ? ::pngScanlineDecode_3_8(out_View, scanlines, palette)	: ::pngScanlineDecode_3_bits(out_View, scanlines, palette, bitDepth);
	case  0: ree_if(bitDepth != 1 && bitDepth != 2 && bitDepth != 4 && bitDepth != 8 && bitDepth != 16, "Invalid bit depth: %u.", bitDepth);
		 if(bitDepth == 8)
			return ::pngScanlineDecode_0_8(out_View, scanlines);
		else
			return (bitDepth == 16) ? ::pngScanlineDecode_0_16(out_View, scanlines) : ::pngScanlineDecode_0_bits(out_View, scanlines, bitDepth);
	} // switch(colorType
}

static			::gpk::error_t											pngDecodeInterlaced
	( const int32_t												bitDepth
	, const int32_t												colorType
	, const ::gpk::view_array<const ::gpk::array_pod<uint8_t>>	& scanlines
	, const ::gpk::view_array<::gpk::color_bgr<uint8_t>>		& palette
	, const ::gpk::view_array<const ::gpk::SCoord2<uint32_t>>	& imageSizes
	, ::gpk::view_grid<::gpk::SColorBGRA>						& out_View
	) {
	::gpk::SImage<::gpk::SColorBGRA>											adam7			[7]						= {};
	uint32_t																	offsetScanline							= 0;
	for(uint32_t iImage = 0; iImage < imageSizes.size(); ++iImage) {
		const ::gpk::SCoord2<uint32_t>												currentImageSize						= imageSizes[iImage];
		adam7[iImage].resize(currentImageSize);
		if(0 == adam7[iImage].Texels.size())
			continue;
		const ::gpk::view_array<const::gpk::array_pod<uint8_t>>						currentScanlineSet						= {&scanlines[offsetScanline], currentImageSize.y};
		switch(colorType) {
		case 0: // grayscale
			 if(bitDepth == 8)
				 gpk_necall(::pngScanlineDecode_0_8(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			else if(bitDepth == 16)
				gpk_necall(::pngScanlineDecode_0_16(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			else  if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1)
				gpk_necall(::pngScanlineDecode_0_bits(adam7[iImage].View, currentScanlineSet, bitDepth), "%s", "Corrupt file?");
			break;
		case 2: // RGB
			if(bitDepth == 8)
				gpk_necall(::pngScanlineDecode_2_8(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			else if(bitDepth == 16)
				gpk_necall(::pngScanlineDecode_2_16(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			break;
		case 3: // Palette
			if(bitDepth == 8)
				gpk_necall(::pngScanlineDecode_3_8(adam7[iImage].View, currentScanlineSet, palette), "%s", "Corrupt file?");
			else if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1)
				gpk_necall(::pngScanlineDecode_3_bits(adam7[iImage].View, currentScanlineSet, palette, bitDepth), "%s", "Corrupt file?");
			break;
		case 4: // Grayscale + Alpha
			if(bitDepth == 8)
				gpk_necall(::pngScanlineDecode_4_8(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			else if(bitDepth == 16)
				gpk_necall(::pngScanlineDecode_4_16(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			break;
		case 6: // RGBA
			if(bitDepth == 8)
				gpk_necall(::pngScanlineDecode_6_8(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			else if(bitDepth == 16)
				gpk_necall(::pngScanlineDecode_6_16(adam7[iImage].View, currentScanlineSet), "%s", "Corrupt file?");
			break;
		} // switch(colorType)
		offsetScanline															+= currentImageSize.y;
	}
	::gpk::adam7Interlace(::gpk::view_array<::gpk::SImage<::gpk::SColorBGRA>>{adam7}, out_View);
	return 0;
}


static			::gpk::error_t											scanlineBitDecodeOrder
	( const int32_t										bitDepth
	, ::gpk::view_array<::gpk::array_pod<uint8_t>>		scanlines
	) {
	if(bitDepth <= 4)
		for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
			::gpk::array_pod<uint8_t>													& scanline										= scanlines[iScanline];
			for(uint32_t iByte = 0; iByte < scanline.size(); ++iByte) {
				uint8_t																		& pixel											= scanline[iByte];
				pixel																	= ::gpk::reverse_bitfield(pixel, bitDepth);
			}
		}
	return 0;
}

static			::gpk::error_t											pngActualFileLoad								(const ::gpk::view_array<const ubyte_t>& source, ::gpk::SPNGData & pngData, ::gpk::array_pod<uint32_t> & indicesIDAT)	{
	pngData.Feature															= {};
	memset(pngData.Signature, 0, ::gpk::size(pngData.Signature));
	::gpk::clear
		( pngData.Deflated
		, pngData.Inflated
		, pngData.Filters
		, pngData.Chunks
		);
	memset(pngData.Adam7Sizes, 0, ::gpk::size(pngData.Adam7Sizes) * sizeof(::gpk::SCoord2<uint32_t>));
	pngData.Header															= {};

	::gpk::view_stream<const ubyte_t>											png_stream										= {source.begin(), source.size()};
	png_stream.read_pod(pngData.Signature);
	while(png_stream.CursorPosition < (png_stream.size() - 7)) {
		::gpk::SPNGChunk															chunkRead										= {};
		uint32_t																	sizeChunk										= 0;
		gpk_necall(png_stream.read_pod(sizeChunk)		, "%s", "Failed to read PNG! File corrupt?");
		uint32_t																	crcDataStart									= png_stream.CursorPosition;
		gpk_necall(png_stream.read_pod(chunkRead.Type)	, "%s", "Failed to read PNG! File corrupt?");
		be2le_32(sizeChunk);
		ree_if(sizeChunk > (0x7FFFFFFF >> 2), "%s", "Chunk too large! Corrupt file?");
		gpk_necall(chunkRead.Data.resize(sizeChunk), "Out of memory? Chunk size: %u.", sizeChunk);
		gpk_necall(png_stream.read_pod(chunkRead.Data.begin(), sizeChunk)	, "%s", "Failed to read PNG! File corrupt?");
		gpk_necall(png_stream.read_pod(chunkRead.CRC)						, "%s", "Failed to read PNG! File corrupt?");
		be2le_32(chunkRead.CRC);
		uint32_t																	crcGenerated									= ::gpk::get_crc({&source[crcDataStart], sizeChunk + 4});
		gerror_if(crcGenerated != chunkRead.CRC, "Invalid CRC: File: %X, Generated: %X.", chunkRead.CRC, crcGenerated);
		gpk_necall(pngData.Chunks.push_back(chunkRead), "%s", "Out of memory?");
		break_gverbose_if(0 == memcmp(chunkRead.Type, "IEND", 4), "%s", "Found IEND chunk (image end).");
	}

	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	::gpk::array_pod<::gpk::color_bgr<uint8_t>>									& palette										= pngData.Palette;
	for(uint32_t iChunk = 0; iChunk < pngData.Chunks.size(); ++iChunk) {
		const ::gpk::SPNGChunk														& newChunk										= pngData.Chunks[iChunk];
		verbose_printf("Found chunk of type: %c%c%c%c. Data size: %u.", newChunk.Type[0], newChunk.Type[1], newChunk.Type[2], newChunk.Type[3], (uint32_t)newChunk.Data.size());
			 if(0 == memcmp(newChunk.Type, "IDAT", 4)) { indicesIDAT.push_back(iChunk); }
		else if(0 == memcmp(newChunk.Type, "IHDR", 4)) {
			imageHeader																= *(const ::gpk::SPNGIHDR*)newChunk.Data.begin();
			be2le_32(imageHeader.Size.x);
			be2le_32(imageHeader.Size.y);
		}
		else if(0 == memcmp(newChunk.Type, "PLTE", 4)) {
			uint32_t																		colorCount									= newChunk.Data.size() / 3;
			if(colorCount) {
				verbose_printf("Loading palette of %u colors.", colorCount);
				gpk_necall(palette.resize(colorCount), "%s", "Out of memory or corrupt file.");
				::gpk::color_rgb<uint8_t>														* pngPalette								= (::gpk::color_rgb<uint8_t>*)newChunk.Data.begin();
				for(uint32_t iColor = 0; iColor < colorCount; ++iColor) {
					palette[iColor].b															= pngPalette[iColor].b;
					palette[iColor].g															= pngPalette[iColor].g;
					palette[iColor].r															= pngPalette[iColor].r;
				}
			}
		}
		else if(0 == memcmp(newChunk.Type, "tEXt", 4)) { 	// Can store text that can be represented in ISO/IEC 8859-1, with one key-value pair for each chunk. The "key" must be between 1 and 79 characters long. Separator is a null character. The "value" can be any length, including zero up to the maximum permissible chunk size minus the length of the keyword and separator. Neither "key" nor "value" can contain null character. Leading or trailing spaces are also disallowed.
			verbose_printf("Text Key: %s.", newChunk.Data.begin());
			uint32_t																	keyLength										= (uint32_t)strlen((const char*)newChunk.Data.begin());
			uint32_t																	valueLength										= newChunk.Data.size() - (keyLength + 1);
			::gpk::array_pod<char_t>													value;
			gpk_necall(value.resize(valueLength + 1), "%s", "Out of memory or corrupt file.");
			value[valueLength]														= 0;
			memcpy(&value[0], &newChunk.Data[keyLength + 1], valueLength);
			verbose_printf("Text: %s.", value.begin());
			if(-1 == pngData.Feature[::gpk::PNG_TAG_tEXt])
				pngData.Feature[::gpk::PNG_TAG_tEXt] = iChunk;

		}
		else if(0 == memcmp(newChunk.Type, "zTXt", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_zTXt]) pngData.Feature[::gpk::PNG_TAG_zTXt] = iChunk; }	// Contains compressed text (and a compression method marker) with the same limits as tEXt.
		else if(0 == memcmp(newChunk.Type, "bKGD", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_bKGD]) pngData.Feature[::gpk::PNG_TAG_bKGD] = iChunk; }	// Gives the default background color. It is intended for use when there is no better choice available, such as in standalone image viewers (but not web browsers; see below for more details).
		else if(0 == memcmp(newChunk.Type, "cHRM", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_cHRM]) pngData.Feature[::gpk::PNG_TAG_cHRM] = iChunk; }	// Gives the chromaticity coordinates of the display primaries and white point.
		else if(0 == memcmp(newChunk.Type, "dSIG", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_dSIG]) pngData.Feature[::gpk::PNG_TAG_dSIG] = iChunk; }	// Is for storing digital signatures.[13]
		else if(0 == memcmp(newChunk.Type, "eXIf", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_eXIf]) pngData.Feature[::gpk::PNG_TAG_eXIf] = iChunk; }	// Stores Exif metadata.[14]
		else if(0 == memcmp(newChunk.Type, "gAMA", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_gAMA]) pngData.Feature[::gpk::PNG_TAG_gAMA] = iChunk; }	// Specifies gamma.
		else if(0 == memcmp(newChunk.Type, "hIST", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_hIST]) pngData.Feature[::gpk::PNG_TAG_hIST] = iChunk; }	// Can store the histogram, or total amount of each color in the image.
		else if(0 == memcmp(newChunk.Type, "iCCP", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_iCCP]) pngData.Feature[::gpk::PNG_TAG_iCCP] = iChunk; }	// Is an ICC color profile.
		else if(0 == memcmp(newChunk.Type, "iTXt", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_iTXt]) pngData.Feature[::gpk::PNG_TAG_iTXt] = iChunk; }	// Contains a keyword and UTF-8 text, with encodings for possible compression and translations marked with language tag. The Extensible Metadata Platform (XMP) uses this chunk with a keyword 'XML:com.adobe.xmp'
		else if(0 == memcmp(newChunk.Type, "pHYs", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_pHYs]) pngData.Feature[::gpk::PNG_TAG_pHYs] = iChunk; }	// Holds the intended pixel size and/or aspect ratio of the image.
		else if(0 == memcmp(newChunk.Type, "sBIT", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_sBIT]) pngData.Feature[::gpk::PNG_TAG_sBIT] = iChunk; }	// (significant bits) indicates the color-accuracy of the source data.
		else if(0 == memcmp(newChunk.Type, "sPLT", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_sPLT]) pngData.Feature[::gpk::PNG_TAG_sPLT] = iChunk; }	// Suggests a palette to use if the full range of colors is unavailable.
		else if(0 == memcmp(newChunk.Type, "sRGB", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_sRGB]) pngData.Feature[::gpk::PNG_TAG_sRGB] = iChunk; }	// Indicates that the standard sRGB color space is used.
		else if(0 == memcmp(newChunk.Type, "sTER", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_sTER]) pngData.Feature[::gpk::PNG_TAG_sTER] = iChunk; }	// Stereo-image indicator chunk for stereoscopic images.[15]
		else if(0 == memcmp(newChunk.Type, "tIME", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_tIME]) pngData.Feature[::gpk::PNG_TAG_tIME] = iChunk; }	// Stores the time that the image was last changed.
		else if(0 == memcmp(newChunk.Type, "tRNS", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_tRNS]) pngData.Feature[::gpk::PNG_TAG_tRNS] = iChunk; }	// Contains transparency information. For indexed images, it stores alpha channel values for one or more palette entries. For truecolor and grayscale images, it stores a single pixel value that is to be regarded as fully transparent.
		else if(0 == memcmp(newChunk.Type, "fcTL", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_fcTL]) pngData.Feature[::gpk::PNG_TAG_fcTL] = iChunk; }	// The frame control chunk contains several bits of information, the most important of which is the display time of the following frame.
		else if(0 == memcmp(newChunk.Type, "fdAT", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_fdAT]) pngData.Feature[::gpk::PNG_TAG_fdAT] = iChunk; }	// The frame data chunks have the same structure as the IDAT chunks, except preceded by a sequence number.
		else if(0 == memcmp(newChunk.Type, "acTL", 4)) { if(-1 == pngData.Feature[::gpk::PNG_TAG_acTL]) pngData.Feature[::gpk::PNG_TAG_acTL] = iChunk; verbose_printf("%s", "This is an animated PNG."); }	// The animation control chunk is a kind of "marker" chunk, telling the parser that this is an animated png.
	}
	return 0;
}

static			::gpk::error_t											pngDefilterSub									(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp)															{ for(uint32_t iByte = bpp	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanline[iByte - bpp];	return 0; }
static			::gpk::error_t											pngDefilterUp									(::gpk::view_array<ubyte_t>& scanline, const ::gpk::view_array<const ubyte_t>& scanlinePrevious)				{ for(uint32_t iByte = 0	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanlinePrevious[iByte];	return 0; }
static			::gpk::error_t											pngDefilterAverage								(::gpk::view_array<ubyte_t>& scanline, const ::gpk::view_array<const ubyte_t>& scanlinePrevious, uint32_t bpp)	{
	if(scanlinePrevious.size()) {
		for(uint32_t iByte = 0; iByte < bpp; ++iByte)
			scanline[iByte]															+= ((uint32_t)scanlinePrevious[iByte] / 2) & 0xFFU;
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]															+= (((uint32_t)scanline[iByte - bpp] + scanlinePrevious[iByte]) / 2) & 0xFFU;
	} else {
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]															+= ((uint32_t)scanline[iByte - bpp] / 2) & 0xFFU;
	}
	return 0;
}

static			::gpk::error_t											paethPredictor									(int32_t left, int32_t above, int32_t upperleft)	{
	int32_t																		p												= left + above - upperleft;	// initial estimate
	int32_t																		pa												= ::gpk::abs(p - left		); // distances to a, b, c
	int32_t																		pb												= ::gpk::abs(p - above		); // a = left, b = above, c = upper left
	int32_t																		pc												= ::gpk::abs(p - upperleft	); //
	if (pa <= pb && pa <= pc)	// return nearest of a,b,c, breaking ties in order a,b,c.
		return left;
	return (pb <= pc) ? above : upperleft;
}

static			::gpk::error_t											pngDefilterPaeth								(::gpk::view_array<ubyte_t>& scanline, const ::gpk::view_array<const ubyte_t>& scanlinePrevious, uint32_t bpp) {
	if(scanlinePrevious.size()) {
		for(uint32_t iByte = 0; iByte < bpp; ++iByte)
			scanline[iByte]															+= paethPredictor(0, scanlinePrevious[iByte], 0) & 0xFFU;
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]															+= paethPredictor(scanline[iByte - bpp], scanlinePrevious[iByte], scanlinePrevious[iByte - bpp]) & 0xFFU;
	} else {
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]															+= paethPredictor(scanline[iByte - bpp], 0, 0) & 0xFFU;
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDefilter								(const ::gpk::view_array<const ubyte_t> & scanlineFilters, const ::gpk::SCoord2<uint32_t> & imageSize, uint32_t bytesPerPixel, ::gpk::view_array<::gpk::array_pod<uint8_t>> scanlines) {
	::gpk::array_pod<bool>														filteredScanlines;
	filteredScanlines.resize(scanlineFilters.size(), false);
	if(scanlineFilters.size())
		switch(scanlineFilters[0]) {
		case 1: ::pngDefilterSub	(scanlines[0], bytesPerPixel);		filteredScanlines[0] = true; break;
		case 2: /* nothing to do when there is no "up" scanline */		filteredScanlines[0] = true; break;
		case 3: ::pngDefilterAverage(scanlines[0], {}, bytesPerPixel);	filteredScanlines[0] = true; break;
		case 4: ::pngDefilterPaeth	(scanlines[0], {}, bytesPerPixel);	filteredScanlines[0] = true; break;
		}

	for(uint32_t iScanline = 0; iScanline < filteredScanlines.size(); ++iScanline)
		if(scanlineFilters[iScanline] == 0)
			filteredScanlines[iScanline]											= true;

	for(uint32_t y = 1; y < imageSize.y; ++y)
		if(scanlineFilters[y] == 1) {
			::pngDefilterSub(scanlines[y], bytesPerPixel);
			filteredScanlines[y]														= true;
		}

	uint32_t																	countPasses										= 0;
	while(true) {
		bool																		filterPass										= false;
		for(uint32_t iScanline = 0; iScanline < filteredScanlines.size(); ++iScanline)
			if(false == filteredScanlines[iScanline]) {
				filterPass																= true;
				break;
			}
		if(false == filterPass)
			break;

		++countPasses;
		for(uint32_t y = 1; y < imageSize.y; ++y) {
			if(false == filteredScanlines[y] && filteredScanlines[y - 1]) {
					 if(scanlineFilters[y] == 2) { ::pngDefilterUp		(scanlines[y], scanlines[y - 1]);					filteredScanlines[y] = true; }
				else if(scanlineFilters[y] == 3) { ::pngDefilterAverage	(scanlines[y], scanlines[y - 1], bytesPerPixel);	filteredScanlines[y] = true; }
				else if(scanlineFilters[y] == 4) { ::pngDefilterPaeth	(scanlines[y], scanlines[y - 1], bytesPerPixel);	filteredScanlines[y] = true; }
				else
					reterr_gerror_if(scanlineFilters[y] > 4, "Invalid filter: %u! Corrupt png file?", (uint32_t)scanlineFilters[y]);
			}
		}
	}
	gwarn_if(countPasses > 1, "Decoding passess executed: %u.", countPasses);
	return 0;
}

static			::gpk::error_t											pngBytesPerPixel								(int32_t colorType, int32_t bitDepth)					{
	switch(colorType) {
	default: return -1;
	case  3: return 1;
	case  0: return (bitDepth == 16) ? 2 : 1;
	case  2: return (bitDepth == 16) ? 6 : 3;
	case  4: return (bitDepth == 16) ? 4 : 2;
	case  6: return (bitDepth == 16) ? 8 : 4;
	}
}

static			::gpk::error_t											pngDefilterScanlinesInterlaced					(::gpk::SPNGData& pngData)									{
	const ::gpk::SPNGIHDR														& imageHeader									= pngData.Header;
	const ::gpk::SCoord2<uint32_t>												& imageSize										= imageHeader.Size;
	::gpk::array_obj<::gpk::array_pod<ubyte_t>>									& scanlines										= pngData.Scanlines;
	::gpk::view_array<::gpk::SCoord2<uint32_t>>									imageSizes										= pngData.Adam7Sizes;
	::gpk::adam7Sizes(imageSizes, imageSize);

	uint32_t																	totalScanlines									= 0;
	for(uint32_t iImage = 0; iImage < 7; ++iImage)
		totalScanlines															+= imageSizes[iImage].y;
	gpk_necall(scanlines.resize(totalScanlines)			, "%s", "Out of memory or corrupt file.");
	gpk_necall(pngData.Filters.resize(scanlines.size())	, "%s", "Out of memory or corrupt file.");
	uint32_t																	offsetByte										= 0;
	uint32_t																	offsetScanline									= 0;
	const uint32_t																bytesPerPixel									= ::pngBytesPerPixel(imageHeader.ColorType, imageHeader.BitDepth);
	ree_if(errored(bytesPerPixel), "Invalid format! ColorType: %u. Bit Depth: %u.", bytesPerPixel);
	for(uint32_t iImage = 0; iImage < 7; ++iImage) {
		uint32_t																	widthScanlineCurrent							= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageSizes[iImage].x);
		verbose_printf("Image: %u. Scanline size: %u.", iImage, widthScanlineCurrent);
		const ::gpk::SCoord2<uint32_t>												currentImageSize								= imageSizes[iImage];
		for(uint32_t y = 0; y < currentImageSize.y; ++y) {
			const int32_t																currentScanline									= offsetScanline + y;
			pngData.Filters[currentScanline]										= pngData.Inflated[offsetByte + y * widthScanlineCurrent + y];
			verbose_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[currentScanline]);
			gpk_necall	(scanlines[currentScanline].resize(widthScanlineCurrent), "%s", "Out of memory or corrupt file.");
			memcpy		(scanlines[currentScanline].begin(), &pngData.Inflated[offsetByte + y * widthScanlineCurrent + y + 1], widthScanlineCurrent);
		}

		if(currentImageSize.y) {
			gpk_necall(::pngScanlineDefilter({&pngData.Filters[offsetScanline], currentImageSize.y}, currentImageSize, bytesPerPixel, {&scanlines[offsetScanline], currentImageSize.y}), "%s", "Corrupt file?");
			if(imageHeader.BitDepth < 8 && (imageHeader.ColorType == 3 || imageHeader.ColorType == 0)) // Decode pixel ordering for bit depths of 1, 2 and 4 bits
				if(widthScanlineCurrent) {
					::scanlineBitDecodeOrder(imageHeader.BitDepth, {&scanlines[offsetScanline], currentImageSize.y});
				}
			if(widthScanlineCurrent)
				offsetByte																+= (widthScanlineCurrent + 1) * currentImageSize.y;
			offsetScanline															+= currentImageSize.y;
		}
	}
	return 0;
}

static			::gpk::error_t											pngInflate										(const ::gpk::view_array<ubyte_t>& deflated, ::gpk::array_pod<ubyte_t>& inflated)							{
	z_stream																	strm											= {};
	int																			ret												= inflateInit(&strm);	 // allocate inflate state
	if (ret != Z_OK)
		return ret;

	strm.avail_in															= deflated.size();
	strm.avail_out															= inflated.size();
	strm.next_in															= (Bytef *)deflated.begin();
	strm.next_out															= (Bytef *)inflated.begin();
	ret																		= ::inflate(&strm, Z_NO_FLUSH);
	ree_if(ret == Z_STREAM_ERROR, "%s", "ZIP Error");  // state not clobbered
	switch (ret) {
	case Z_NEED_DICT		:
		ret																		= Z_DATA_ERROR;     // and fall through
	case Z_VERSION_ERROR	:
	case Z_STREAM_ERROR		:
	case Z_DATA_ERROR		:
	case Z_MEM_ERROR		:
		(void)inflateEnd(&strm);
		return -1;
	}
	if(ret != Z_STREAM_END && ret != Z_OK) {
		(void)inflateEnd(&strm);
		error_printf("%s", "Failed to decompress?");
		return -1;
	}
	inflated.resize((uint32_t)((ptrdiff_t)strm.next_out - (ptrdiff_t)inflated.begin()));
	ret																			= ::inflateEnd(&strm);
	verbose_printf("inflateEnd: %u.", (uint32_t)ret);
	return 0;
}

static inline		::gpk::error_t										pngFilePrintInfo								(::gpk::SPNGData& pngData) {
	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	verbose_printf("----- PNG File Info summary: "
		"\nSize                 : {%u,  %u}."
		"\nBit Depth            : 0x%X."
		"\nColor Type           : 0x%X."
		"\nCompression          : 0x%X."
		"\nFilter               : 0x%X."
		"\nInterlace            : 0x%X."
		"\nInflated size        : %u."
		"\nDeflated size        : %u."
		, imageHeader.Size.x, imageHeader.Size.y
		, (uint32_t)imageHeader.BitDepth
		, (uint32_t)imageHeader.ColorType
		, (uint32_t)imageHeader.MethodCompression
		, (uint32_t)imageHeader.MethodFilter
		, (uint32_t)imageHeader.MethodInterlace
		, pngData.Inflated.size()
		, pngData.Deflated.size()
		);
	return 0;
}

				::gpk::error_t											pngDefilterScanlinesNonInterlaced				(::gpk::SPNGData& pngData, uint32_t bytesPerPixel) {
	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	::gpk::array_obj<::gpk::array_pod<ubyte_t>>									& scanlines										= pngData.Scanlines;
	pngData.Filters.clear();
	scanlines.resize(imageHeader.Size.y);
	uint32_t																	widthScanline									= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageHeader.Size.x);
	verbose_printf("Scanline size: %u.", widthScanline);
	for(uint32_t y = 0; y < imageHeader.Size.y; ++y) {
		pngData.Filters.push_back(pngData.Inflated[y * widthScanline + y]);
		verbose_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[y]);
		gpk_necall(scanlines[y].resize(widthScanline), "%s", "Out of memory or corrupt file.");
		memcpy(scanlines[y].begin(), &pngData.Inflated[y * widthScanline + y + 1], widthScanline);
	}
	gpk_necall(::pngScanlineDefilter(pngData.Filters, imageHeader.Size, bytesPerPixel, scanlines), "%s", "Corrupt file?");
	if(imageHeader.ColorType == 3 || imageHeader.ColorType == 0) // Decode pixel ordering for bit depths of 1, 2 and 4 bits
		::scanlineBitDecodeOrder(imageHeader.BitDepth, pngData.Scanlines);

	return 0;
}

static			::gpk::error_t											pngProcess										(::gpk::SPNGData& pngData, ::gpk::SImage<::gpk::SColorBGRA>& out_Texture) {
	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	uint32_t																	bytesPerPixel									= ::pngBytesPerPixel(imageHeader.ColorType, imageHeader.BitDepth);
	ree_if(errored(bytesPerPixel), "%s", "Invalid format! Color Type: %u. Bit Depth: %u.");
	if(imageHeader.MethodInterlace)
		gpk_necall(::pngDefilterScanlinesInterlaced(pngData), "%s", "Corrupt file?");
	else
		gpk_necall(::pngDefilterScanlinesNonInterlaced(pngData, bytesPerPixel), "%s", "Corrupt file?");

	gpk_necall(out_Texture.resize(imageHeader.Size), "%s", "Invalid image size?");
	if(imageHeader.MethodInterlace)
		return ::pngDecodeInterlaced
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, pngData.Adam7Sizes
			, out_Texture.View
			);
	else
		return ::pngDecode
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, out_Texture.View
			);
}

				::gpk::error_t											gpk::pngFileLoad								(::gpk::SPNGData & pngData, const ::gpk::view_const_string& filename, ::gpk::SImage<::gpk::SColorBGRA>& out_Texture)	{
	::gpk::array_pod<byte_t>													fileInMemory									= {};
	gpk_necall(::gpk::fileToMemory(filename, fileInMemory), "Failed to load .png file: %s", filename.begin());
	return ::gpk::pngFileLoad(pngData, ::gpk::view_ubyte{(ubyte_t*)fileInMemory.begin(), fileInMemory.size()}, out_Texture);
}

				::gpk::error_t											gpk::pngFileLoad								(::gpk::SPNGData& pngData, const ::gpk::view_const_ubyte& source, ::gpk::SImage<::gpk::SColorBGRA>& out_Texture) {
	//::gpk::view_stream<const ubyte_t>											png_stream										= {source.begin(), source.size()};
	::gpk::array_pod<uint32_t>													indicesIDAT;
	gpk_necall(::pngActualFileLoad(source, pngData, indicesIDAT), "%s", "Failed to read png stream! Corrupt file?");
	::gpk::array_pod<ubyte_t>													& imageDeflated									= pngData.Deflated;
	for(uint32_t iChunk = 0; iChunk < indicesIDAT.size(); ++iChunk) {
		const ::gpk::SPNGChunk														& loadedChunk									= pngData.Chunks[indicesIDAT[iChunk]];
		if(loadedChunk.Data.size()) {
			uint32_t																	dataOffset										= imageDeflated.size();
			gpk_necall(imageDeflated.resize(imageDeflated.size() + loadedChunk.Data.size()), "Out of memory? Requested size: %u.", imageDeflated.size() + loadedChunk.Data.size());
			memcpy(&imageDeflated[dataOffset], loadedChunk.Data.begin(), loadedChunk.Data.size());
		}
	}
	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	const uint32_t																aSuitableWidth									= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageHeader.Size.x);
	gpk_necall(pngData.Inflated.resize(aSuitableWidth * 2 * imageHeader.Size.y + imageHeader.Size.y), "Failed to resize. Requested size: %u.", aSuitableWidth * 2 * imageHeader.Size.y + imageHeader.Size.y);
	gpk_necall(::pngInflate(pngData.Deflated, pngData.Inflated), "%s", "Failed to decompress!");
	::pngFilePrintInfo(pngData);
	return ::pngProcess(pngData, out_Texture);
}
