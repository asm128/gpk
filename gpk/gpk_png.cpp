#include "gpk_bitmap_file.h"
#include "gpk_stream_view.h"
#include "zlib.h"

#define be2le_16(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<ubyte_t>({(ubyte_t*)&number, 8})

static		::gpk::error_t												pngScanLineSizeFromFormat						(int32_t colorType, int32_t bitDepth, int32_t imageWidth)	{
	int32_t																			scanLineWidth									= imageWidth;
	switch(bitDepth) {
	default	  : break;
	case	 1: { scanLineWidth = imageWidth / 8; if(colorType == 0) break; else if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination"); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination"); }	} break; 
	case	 2: { scanLineWidth = imageWidth / 4; if(colorType == 0) break; else if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination"); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination"); }	} break; 
	case	 4: { scanLineWidth = imageWidth / 2; if(colorType == 0) break; else if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination"); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination"); }	} break; 
	case	 8: { scanLineWidth = imageWidth / 1; if(colorType == 0) break; else if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }																							} break; 
	case	16: { scanLineWidth = imageWidth * 2; if(colorType == 0) break; else if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }																							} break; 
	}

	switch(bitDepth) {
	default	  : break;
	case	 1: scanLineWidth += ((imageWidth % 8) ? 1 : 0); break;
	case	 2: scanLineWidth += ((imageWidth % 4) ? 1 : 0); break;
	case	 4: scanLineWidth += ((imageWidth % 2) ? 1 : 0); break;
	}
	return scanLineWidth;
}

static	::gpk::error_t												pngStreamInflate								(const ::gpk::array_view<ubyte_t>& deflated, ::gpk::array_pod<ubyte_t>& inflated)							{
	int																			ret;
	z_stream																	strm											= {};

	ret																		= inflateInit(&strm);	 // allocate inflate state 
	if (ret != Z_OK)
		return ret;

	strm.avail_in															= (uint32_t)deflated.size(); 
	strm.avail_out															= (uint32_t)inflated.size();
	strm.next_in															= (Bytef *)deflated.begin();
	strm.next_out															= (Bytef *)inflated.begin();
	ret																		= ::inflate(&strm, Z_NO_FLUSH);
	ree_if(ret == Z_STREAM_ERROR, "Error");  // state not clobbered 
	switch (ret) {
	case Z_NEED_DICT		:
		ret																			= Z_DATA_ERROR;     // and fall through 
	case Z_VERSION_ERROR	:
	case Z_STREAM_ERROR		:
	case Z_DATA_ERROR		:
	case Z_MEM_ERROR		:
		(void)inflateEnd(&strm);
		return -1;
	}
	if(ret != Z_STREAM_END && ret != Z_OK) {
		(void)inflateEnd(&strm);
		error_printf("Failed to decompress?");
		return -1;
	}
	ret																			= ::inflateEnd(&strm);
	info_printf("inflateEnd: %u.", (uint32_t)ret);
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_2_8							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const ::gpk::color_bgr<uint8_t>>					viewPixels										= {(const ::gpk::color_bgr<uint8_t>*)scanline.begin(), scanline.size() / 3};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			const ::gpk::color_bgr<uint8_t>												& pixelToLoad									= viewPixels[iPixel];
			outputPixel.b															= pixelToLoad.r;
			outputPixel.g															= pixelToLoad.g;
			outputPixel.r															= pixelToLoad.b;
			outputPixel.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_8							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= scanline[iPixel];
			outputPixel.g															= scanline[iPixel];
			outputPixel.r															= scanline[iPixel];
			outputPixel.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_2_16							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const ::gpk::color_bgr<uint16_t>>					viewPixels										= {(const ::gpk::color_bgr<uint16_t>*)scanline.begin(), scanline.size() / 6};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_bgr<uint16_t>													colorInput										= viewPixels[iPixel];
			be2le_16(colorInput.b);
			be2le_16(colorInput.g);
			be2le_16(colorInput.r);
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= (uint8_t)(((1.0 / 65535) * colorInput.r) * 255);
			outputPixel.g															= (uint8_t)(((1.0 / 65535) * colorInput.g) * 255);
			outputPixel.r															= (uint8_t)(((1.0 / 65535) * colorInput.b) * 255);
			outputPixel.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_16							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const uint16_t>										viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		double																		rangeUnit										= 1.0 / 65535;
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint16_t																	toReverse										= viewPixels[iPixel];
			be2le_16(toReverse);
			double																		largeValue										= rangeUnit * toReverse;
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.r															=
			outputPixel.g															=
			outputPixel.b															= (uint8_t)(largeValue * 255);
			outputPixel.a															= 255;
		}
	}
	return 0;
}
static			::gpk::error_t											pngScanlineDecode_6_8							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const ::gpk::color_rgba<uint8_t>>					viewPixels										= {(const ::gpk::color_rgba<uint8_t>*)scanline.begin(), scanline.size() / 4};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= viewPixels[iPixel].b;
			outputPixel.g															= viewPixels[iPixel].g;
			outputPixel.r															= viewPixels[iPixel].r;
			outputPixel.a															= viewPixels[iPixel].a;
		}
	}
	return 0; 
}

static			::gpk::error_t											pngScanlineDecode_6_16							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const ::gpk::color_bgra<uint16_t>>					viewPixels										= {(const ::gpk::color_bgra<uint16_t>*)scanline.begin(), scanline.size() / 8};
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_bgra<uint16_t>													colorReversed									= viewPixels[iPixel];
			be2le_16(colorReversed.b);
			be2le_16(colorReversed.g);
			be2le_16(colorReversed.r);
			be2le_16(colorReversed.a);
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= (uint8_t)(((1.0 / 65535) * colorReversed.r) * 255);
			outputPixel.g															= (uint8_t)(((1.0 / 65535) * colorReversed.g) * 255);
			outputPixel.r															= (uint8_t)(((1.0 / 65535) * colorReversed.b) * 255);
			outputPixel.a															= (uint8_t)(((1.0 / 65535) * colorReversed.a) * 255);
		}
	}
	return 0; 
}

static			::gpk::error_t											pngScanlineDecode_4_8							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::array_view<const uint16_t>										viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		double																		rangeUnit										= 1.0 / 255;
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint8_t																		grayscaleValue									= (viewPixels[iPixel] & 0xFFU);
			uint8_t																		alphaComponent									= (viewPixels[iPixel] & 0xFF00U) >> 8;
			const double																grayscaleWeigth									= rangeUnit * grayscaleValue;
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= (uint8_t)(grayscaleWeigth * 255);
			outputPixel.g															= (uint8_t)(grayscaleWeigth * 255);
			outputPixel.r															= (uint8_t)(grayscaleWeigth * 255);
			outputPixel.a															= (uint8_t)((1.0 / 255.0 * alphaComponent) * 255);
		}
	}
	return 0; 
}

static			::gpk::error_t											pngScanlineDecode_4_16							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::array_view<uint32_t>													viewPixels										= {(uint32_t*)scanline.begin(), scanline.size() / 4};
		double																		unitRange										= 1.0 / 65535;//maxGrey;
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint32_t																packedValues									= viewPixels[iPixel];
			uint16_t																	grayscaleValue									= (packedValues & 0xFFFFU);
			uint16_t																	alphacomponent									= (packedValues & 0xFFFF0000U) >> 16;
			be2le_16(grayscaleValue);
			be2le_16(alphacomponent);
			const double																grayscaleWeight									= unitRange * grayscaleValue;
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.b															= (uint8_t)(grayscaleWeight * 255);
			outputPixel.g															= (uint8_t)(grayscaleWeight * 255);
			outputPixel.r															= (uint8_t)(grayscaleWeight * 255);
			outputPixel.a															= (uint8_t)((1.0 / 65535 * alphacomponent) * 255);
		}
	}
	return 0; 
}

static			::gpk::error_t											pngScanlineDecode_3_8							(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, const ::gpk::array_view<::gpk::color_bgr<uint8_t>>& palette, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)		{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			uint8_t																		paletteIndex									= scanline[iPixel];
			::gpk::SColorBGRA															& outputPixel									= out_View[iScanline][iPixel];
			const ::gpk::color_bgr<uint8_t>												& paletteColor									= palette[paletteIndex];
			outputPixel.r															= paletteColor.r;
			outputPixel.g															= paletteColor.g;
			outputPixel.b															= paletteColor.b;
			outputPixel.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_3_bits						(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, const ::gpk::array_view<::gpk::color_bgr<uint8_t>>& palette, uint32_t bitDepth, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)	{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {	
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::bit_view<const uint8_t>										viewPixels										= ::gpk::bit_view<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		paletteIndex									= 0;
			::gpk::bit_view<uint8_t>													paletteIndexBits								= {&paletteIndex, (uint32_t)8};
			for(uint32_t i=0; i < bitDepth; ++i)
				paletteIndexBits[i]														= viewPixels[bitDepth * iPixel + i];

			const ::gpk::color_bgr<uint8_t>												& paletteColor									= (paletteIndex >= palette.size()) ? ::gpk::color_bgr<uint8_t>{} : palette[paletteIndex];
			::gpk::SColorBGRA															& outputPixel										= out_View[iScanline][iPixel];
			outputPixel.r															= paletteColor.r;
			outputPixel.g															= paletteColor.g;
			outputPixel.b															= paletteColor.b;
			outputPixel.a															= 255;
		}
	}
	return 0;
}

static			::gpk::error_t											pngScanlineDecode_0_bits						(const ::gpk::array_view<const ::gpk::array_pod<uint8_t>> & scanlines, uint32_t bitDepth, ::gpk::grid_view<::gpk::SColorBGRA>& out_View)	{ 
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::array_pod<uint8_t>												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::bit_view<const uint8_t>										viewPixels										= ::gpk::bit_view<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		grayscaleIndex									= 0;
			::gpk::bit_view<uint8_t>													grayscaleIndexBits								= {&grayscaleIndex, (uint32_t)8};
			for(uint32_t i = 0; i < bitDepth; ++i) 
				grayscaleIndexBits[i]													= viewPixels[bitDepth * iPixel + i];

			double																	grayscaleWeight									= 1.0 / ((1 << bitDepth) - 1);
			::gpk::SColorBGRA														& outputPixel									= out_View[iScanline][iPixel];
			outputPixel.r														=
			outputPixel.g														=
			outputPixel.b														= (1 == bitDepth) ? (uint8_t)(grayscaleIndex * 255) : (uint8_t)(grayscaleWeight * grayscaleIndex * 255);
			outputPixel.a														= 255;
		}
	}		
	return 0;
}

namespace gpk
{
#pragma pack(push, 1)
	struct SPNGChunk {
						uint8_t													Type				[4]	= {};
						uint32_t												CRC						= {};
						::gpk::array_pod<uint8_t>								Data					= {};
	};

	struct SPNGIHDR {
						::gpk::SCoord2<uint32_t>								Size					= {};
						int8_t													BitDepth				= {};
						int8_t													ColorType				= {};
						int8_t													MethodCompression		= {};
						int8_t													MethodFilter			= {};
						int8_t													MethodInterlace			= {};
	};

	struct SPNGFeature {
						int32_t													tEXt					= -1;
						int32_t													zTXt					= -1;
						int32_t													fcTL					= -1;
						int32_t													fdAT					= -1;
						int32_t													bKGD					= -1;
						int32_t													cHRM					= -1;
						int32_t													dSIG					= -1;
						int32_t													eXIf					= -1;
						int32_t													gAMA					= -1;
						int32_t													hIST					= -1;
						int32_t													iCCP					= -1;
						int32_t													iTXt					= -1;
						int32_t													pHYs					= -1;
						int32_t													sBIT					= -1;
						int32_t													sPLT					= -1;
						int32_t													sRGB					= -1;
						int32_t													sTER					= -1;
						int32_t													tIME					= -1;
						int32_t													tRNS					= -1;
						int32_t													acTL					= -1;
	};

	struct SPNGData {
						char													Signature			[8]	= {};
						::gpk::array_obj<SPNGChunk>								Chunks					;
						::gpk::array_pod<ubyte_t>								Deflated				;
						::gpk::array_pod<ubyte_t>								Inflated				;
						::gpk::array_pod<ubyte_t>								Filters					;
						::gpk::array_obj<::gpk::array_pod<ubyte_t>>				Scanlines				;
						::gpk::array_pod<::gpk::color_bgr<uint8_t>>				Palette					;
						::gpk::SCoord2<uint32_t>								Adam7Sizes			[7]	= {};
						::gpk::SPNGFeature										Feature					= {};
						::gpk::SPNGIHDR											Header					= {};
	};
#pragma pack(pop)
}

static			::gpk::error_t												pngDecode										
	( const int32_t												bitDepth	
	, const int32_t												colorType	
	, const ::gpk::array_view<const ::gpk::array_pod<uint8_t>>	& scanlines	
	, const ::gpk::array_view<::gpk::color_bgr<uint8_t>>		& palette	
	, ::gpk::grid_view<::gpk::SColorBGRA>						& out_View
	) {
	switch(colorType) {
	case 0: // grayscale
		 if(bitDepth == 8) 
			 gpk_necall(::pngScanlineDecode_0_8(scanlines, out_View), "Corrupt file?");
		else if(bitDepth == 16) 
			gpk_necall(::pngScanlineDecode_0_16(scanlines, out_View), "Corrupt file?");
		else  if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1) 
			gpk_necall(::pngScanlineDecode_0_bits(scanlines, bitDepth, out_View), "Corrupt file?");
		break;
	case 2: // RGB
		if(bitDepth == 8) 
			gpk_necall(::pngScanlineDecode_2_8(scanlines, out_View), "Corrupt file?");
		else if(bitDepth == 16) 
			gpk_necall(::pngScanlineDecode_2_16(scanlines, out_View), "Corrupt file?");
		break;
	case 3: // Palette
		if(bitDepth == 8) 
			gpk_necall(::pngScanlineDecode_3_8(scanlines, palette, out_View), "Corrupt file?");
		else if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1) 
			gpk_necall(::pngScanlineDecode_3_bits(scanlines, palette, bitDepth, out_View), "Corrupt file?");
		break;
	case 4: // Grayscale + Alpha
		if(bitDepth == 8) 
			gpk_necall(::pngScanlineDecode_4_8(scanlines, out_View), "Corrupt file?");
		else if(bitDepth == 16) 
			gpk_necall(::pngScanlineDecode_4_16(scanlines, out_View), "Corrupt file?");
		break;
	case 6: // RGBA 
		if(bitDepth == 8) 
			gpk_necall(::pngScanlineDecode_6_8(scanlines, out_View), "Corrupt file?");
		else if(bitDepth == 16) 
			gpk_necall(::pngScanlineDecode_6_16(scanlines, out_View), "Corrupt file?");
		break;
	} // switch(colorType
	return 0;
}

/*static*/			::gpk::error_t											pngDecodeInterlaced
	( const int32_t												bitDepth	
	, const int32_t												colorType	
	, const ::gpk::array_view<const ::gpk::array_pod<uint8_t>>	& scanlines	
	, const ::gpk::array_view<::gpk::color_bgr<uint8_t>>		& palette
	, const ::gpk::array_view<const ::gpk::SCoord2<uint32_t>>	& imageSizes
	, ::gpk::grid_view<::gpk::SColorBGRA>						& out_View
	) {
	out_View;
	::gpk::STexture<::gpk::SColorBGRA>						adam7			[7]						= {};
	uint32_t												offsetScanline							= 0;
	for(uint32_t iImage = 0; iImage < imageSizes.size(); ++iImage) {
		const ::gpk::SCoord2<uint32_t> currentImageSize = imageSizes[iImage];
		adam7[iImage].resize(currentImageSize);
		if(0 == adam7[iImage].Texels.size())
			continue;
		const ::gpk::array_view<const::gpk::array_pod<uint8_t>>	currentScanlineSet						= {&scanlines[offsetScanline], currentImageSize.y};
		switch(colorType) {
		case 0: // grayscale
			 if(bitDepth == 8) 
				 gpk_necall(::pngScanlineDecode_0_8(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			else if(bitDepth == 16) 
				gpk_necall(::pngScanlineDecode_0_16(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			else  if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1) 
				gpk_necall(::pngScanlineDecode_0_bits(currentScanlineSet, bitDepth, adam7[iImage].View), "Corrupt file?");
			break;
		case 2: // RGB
			if(bitDepth == 8) 
				gpk_necall(::pngScanlineDecode_2_8(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			else if(bitDepth == 16) 
				gpk_necall(::pngScanlineDecode_2_16(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			break;
		case 3: // Palette
			if(bitDepth == 8) 
				gpk_necall(::pngScanlineDecode_3_8(currentScanlineSet, palette, adam7[iImage].View), "Corrupt file?");
			else if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1) 
				gpk_necall(::pngScanlineDecode_3_bits(currentScanlineSet, palette, bitDepth, adam7[iImage].View), "Corrupt file?");
			break;
		case 4: // Grayscale + Alpha
			if(bitDepth == 8) 
				gpk_necall(::pngScanlineDecode_4_8(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			else if(bitDepth == 16) 
				gpk_necall(::pngScanlineDecode_4_16(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			break;
		case 6: // RGBA 
			if(bitDepth == 8) 
				gpk_necall(::pngScanlineDecode_6_8(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			else if(bitDepth == 16) 
				gpk_necall(::pngScanlineDecode_6_16(currentScanlineSet, adam7[iImage].View), "Corrupt file?");
			break;
		} // switch(colorType)
		offsetScanline															+= currentImageSize.y;
	}
	for(uint32_t iImage = 0; iImage < imageSizes.size(); ++iImage) {
		::gpk::SCoord2<uint32_t>													offsetMultiplier								= {1, 1};
		::gpk::SCoord2<uint32_t>													offsetBase										= {0, 0};
		switch(iImage) {
		case 0: offsetMultiplier.x = 8; offsetMultiplier.y = 8; offsetBase.x = 0; offsetBase.y = 0; break;
		case 1: offsetMultiplier.x = 8; offsetMultiplier.y = 8; offsetBase.x = 4; offsetBase.y = 0; break;
		case 2: offsetMultiplier.x = 4; offsetMultiplier.y = 8; offsetBase.x = 0; offsetBase.y = 4; break;
		case 3: offsetMultiplier.x = 4; offsetMultiplier.y = 4; offsetBase.x = 2; offsetBase.y = 0; break;
		case 4: offsetMultiplier.x = 2; offsetMultiplier.y = 4; offsetBase.x = 0; offsetBase.y = 2; break;
		case 5: offsetMultiplier.x = 2; offsetMultiplier.y = 2; offsetBase.x = 1; offsetBase.y = 0; break;
		case 6: offsetMultiplier.x = 1; offsetMultiplier.y = 2; offsetBase.x = 0; offsetBase.y = 1; break;
		}
		for(uint32_t y = 0; y < imageSizes[iImage].y; ++y)
		for(uint32_t x = 0; x < imageSizes[iImage].x; ++x) {
			/*if(iImage == 0) */{
				::gpk::SCoord2<uint32_t>												targetCell										= {x * offsetMultiplier.x + offsetBase.x, y * offsetMultiplier.y + offsetBase.y};
				::gpk::SColorBGRA														inputColor										= adam7[iImage].View[y][x];
				if(targetCell.y < out_View.metrics().y)
					if(targetCell.x < out_View[targetCell.y].size())
						out_View[targetCell.y][targetCell.x]								= inputColor;
			}
		}
	}
	return 0;
}

static			::gpk::error_t											scanlineBitDecodeOrder2							(::gpk::array_view<::gpk::array_pod<uint8_t>> & scanlines)			{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) 
		for(uint32_t iByte = 0; iByte < scanlines[iScanline].size(); ++iByte) {
			::gpk::bit_view<uint8_t>													scanlineBits									= {&scanlines[iScanline][iByte], 8};
			bool																		_0												= scanlineBits[0];
			bool																		_1												= scanlineBits[1];
			bool																		_2												= scanlineBits[2];
			bool																		_3												= scanlineBits[3];
			scanlineBits[0]															= (bool)scanlineBits[6];
			scanlineBits[1]															= (bool)scanlineBits[7];
			scanlineBits[2]															= (bool)scanlineBits[4];
			scanlineBits[3]															= (bool)scanlineBits[5];
			scanlineBits[4]															= _2;
			scanlineBits[5]															= _3;
			scanlineBits[6]															= _0;
			scanlineBits[7]															= _1;
		}
	return 0;
}

static			::gpk::error_t											scanlineBitDecodeOrder4							(::gpk::array_view<::gpk::array_pod<uint8_t>> & scanlines)			{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) 
		for(uint32_t iByte = 0; iByte < scanlines[iScanline].size(); ++iByte) {
			::gpk::bit_view<uint8_t>													scanlineBits									= {&scanlines[iScanline][iByte], 8};
			bool																		_0												= scanlineBits[0] ;
			bool																		_1												= scanlineBits[1] ;
			bool																		_2												= scanlineBits[2] ;
			bool																		_3												= scanlineBits[3] ;
			scanlineBits[0]															= (bool)scanlineBits[4];
			scanlineBits[1]															= (bool)scanlineBits[5];
			scanlineBits[2]															= (bool)scanlineBits[6];
			scanlineBits[3]															= (bool)scanlineBits[7];
			scanlineBits[4]															= _0;
			scanlineBits[5]															= _1;
			scanlineBits[6]															= _2;
			scanlineBits[7]															= _3;
		}
	return 0;
}

static			::gpk::error_t											scanlineBitDecodeOrder								
	( const int32_t										bitDepth	
	, ::gpk::array_view<::gpk::array_pod<uint8_t>>		scanlines	
	) {
	switch(bitDepth) {
	case 4: return ::scanlineBitDecodeOrder4(scanlines);
	case 2: return ::scanlineBitDecodeOrder2(scanlines);
	case 1:
		for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
			::gpk::array_pod<uint8_t>													& scanline										= scanlines[iScanline];
			for(uint32_t iByte = 0; iByte < scanline.size(); ++iByte) 
				::gpk::reverse_bits<uint8_t>(::gpk::bit_view<uint8_t>{&scanline[iByte], 8});
		}
	}
	return 0;
}

static			::gpk::error_t											pngActualFileLoad								(const ::gpk::array_view<const ubyte_t>& source, ::gpk::SPNGData & pngData, ::gpk::array_pod<uint32_t> & IDATIndices)	{
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

	::gpk::stream_view<const ubyte_t>											png_stream										= {source.begin(), source.size()};
	png_stream.read_pod(pngData.Signature);
	while(png_stream.CursorPosition < (png_stream.size() - 7)) {
		::gpk::SPNGChunk															chunkRead										= {};
		uint32_t																	sizeChunk										= 0;
		gpk_necall(png_stream.read_pod(sizeChunk)		, "Failed to read PNG! File corrupt?");
		gpk_necall(png_stream.read_pod(chunkRead.Type)	, "Failed to read PNG! File corrupt?");
		be2le_32(sizeChunk);
		ree_if(sizeChunk > (0x7FFFFFFF >> 2), "Chunk too large! Corrupt file?");
		gpk_necall(chunkRead.Data.resize(sizeChunk), "Out of memory?", sizeChunk);
		gpk_necall(png_stream.read_pod(chunkRead.Data.begin(), sizeChunk)	, "Failed to read PNG! File corrupt?");
		gpk_necall(png_stream.read_pod(chunkRead.CRC)						, "Failed to read PNG! File corrupt?");
		be2le_32(chunkRead.CRC);
		gpk_necall(pngData.Chunks.push_back(chunkRead), "Out of memory?");
		break_info_if(0 == memcmp(chunkRead.Type, "IEND", 4), "Found IEND chunk (image end).");
	}

	::gpk::SPNGIHDR																& imageHeader									= pngData.Header;
	::gpk::array_pod<::gpk::color_bgr<uint8_t>>									& palette										= pngData.Palette;
	for(uint32_t iChunk = 0; iChunk < pngData.Chunks.size(); ++iChunk) {
		const ::gpk::SPNGChunk														& newChunk										= pngData.Chunks[iChunk];
		info_printf("Processing chunk of type: %c%c%c%c. Size: %u."
			, newChunk.Type[0]
			, newChunk.Type[1]
			, newChunk.Type[2]
			, newChunk.Type[3]
			, (uint32_t)newChunk.Data.size()
			);
		// ---------------------------------------
			 if(0 == memcmp(newChunk.Type, "IDAT", 4)) { IDATIndices.push_back(iChunk); }	// We're mostly interested in IDAT chunks.
		else if(0 == memcmp(newChunk.Type, "IHDR", 4)) {
			imageHeader																= *(const ::gpk::SPNGIHDR*)newChunk.Data.begin();
			be2le_32(imageHeader.Size.x);
			be2le_32(imageHeader.Size.y);
		}
		else if(0 == memcmp(newChunk.Type, "PLTE", 4)) { 
			uint32_t																		colorCount									= newChunk.Data.size() / 3;
			if(colorCount) {
				info_printf("Loading palette of %u colors.", colorCount);
				gpk_necall(palette.resize(colorCount), "Out of memory or corrupt file.");
				::gpk::color_rgb<uint8_t>														* pngPalette								= (::gpk::color_rgb<uint8_t>*)newChunk.Data.begin();
				for(uint32_t iColor = 0; iColor < colorCount; ++iColor) {
					palette[iColor].b															= pngPalette[iColor].b;
					palette[iColor].g															= pngPalette[iColor].g;
					palette[iColor].r															= pngPalette[iColor].r;
				}
			}
		}
		else if(0 == memcmp(newChunk.Type, "tEXt", 4)) { 	// Can store text that can be represented in ISO/IEC 8859-1, with one key-value pair for each chunk. The "key" must be between 1 and 79 characters long. Separator is a null character. The "value" can be any length, including zero up to the maximum permissible chunk size minus the length of the keyword and separator. Neither "key" nor "value" can contain null character. Leading or trailing spaces are also disallowed.
			info_printf("Text Key: %s.", newChunk.Data.begin());
			uint32_t																	keyLength										= (uint32_t)strlen((const char*)newChunk.Data.begin());
			uint32_t																	valueLength										= newChunk.Data.size() - (keyLength + 1);
			::gpk::array_pod<char_t>													value;
			gpk_necall(value.resize(valueLength + 1), "Out of memory or corrupt file.");
			value[valueLength]														= 0;
			memcpy(&value[0], &newChunk.Data[keyLength + 1], valueLength);
			info_printf("Text: %s.", value.begin());
			if(-1 == pngData.Feature.tEXt)
				pngData.Feature.tEXt													= iChunk;
			
		}
		else if(0 == memcmp(newChunk.Type, "zTXt", 4)) { if(-1 == pngData.Feature.zTXt) pngData.Feature.zTXt = iChunk; }	// Contains compressed text (and a compression method marker) with the same limits as tEXt.
		else if(0 == memcmp(newChunk.Type, "fcTL", 4)) { if(-1 == pngData.Feature.fcTL) pngData.Feature.fcTL = iChunk; }	// The frame control chunk contains several bits of information, the most important of which is the display time of the following frame. 
		else if(0 == memcmp(newChunk.Type, "fdAT", 4)) { if(-1 == pngData.Feature.fdAT) pngData.Feature.fdAT = iChunk; }	// The frame data chunks have the same structure as the IDAT chunks, except preceded by a sequence number. 
		else if(0 == memcmp(newChunk.Type, "bKGD", 4)) { if(-1 == pngData.Feature.bKGD) pngData.Feature.bKGD = iChunk; }	// Gives the default background color. It is intended for use when there is no better choice available, such as in standalone image viewers (but not web browsers; see below for more details).
		else if(0 == memcmp(newChunk.Type, "cHRM", 4)) { if(-1 == pngData.Feature.cHRM) pngData.Feature.cHRM = iChunk; }	// Gives the chromaticity coordinates of the display primaries and white point.
		else if(0 == memcmp(newChunk.Type, "dSIG", 4)) { if(-1 == pngData.Feature.dSIG) pngData.Feature.dSIG = iChunk; }	// Is for storing digital signatures.[13]
		else if(0 == memcmp(newChunk.Type, "eXIf", 4)) { if(-1 == pngData.Feature.eXIf) pngData.Feature.eXIf = iChunk; }	// Stores Exif metadata.[14]
		else if(0 == memcmp(newChunk.Type, "gAMA", 4)) { if(-1 == pngData.Feature.gAMA) pngData.Feature.gAMA = iChunk; }	// Specifies gamma.
		else if(0 == memcmp(newChunk.Type, "hIST", 4)) { if(-1 == pngData.Feature.hIST) pngData.Feature.hIST = iChunk; }	// Can store the histogram, or total amount of each color in the image.
		else if(0 == memcmp(newChunk.Type, "iCCP", 4)) { if(-1 == pngData.Feature.iCCP) pngData.Feature.iCCP = iChunk; }	// Is an ICC color profile.
		else if(0 == memcmp(newChunk.Type, "iTXt", 4)) { if(-1 == pngData.Feature.iTXt) pngData.Feature.iTXt = iChunk; }	// Contains a keyword and UTF-8 text, with encodings for possible compression and translations marked with language tag. The Extensible Metadata Platform (XMP) uses this chunk with a keyword 'XML:com.adobe.xmp'
		else if(0 == memcmp(newChunk.Type, "pHYs", 4)) { if(-1 == pngData.Feature.pHYs) pngData.Feature.pHYs = iChunk; }	// Holds the intended pixel size and/or aspect ratio of the image.
		else if(0 == memcmp(newChunk.Type, "sBIT", 4)) { if(-1 == pngData.Feature.sBIT) pngData.Feature.sBIT = iChunk; }	// (significant bits) indicates the color-accuracy of the source data.
		else if(0 == memcmp(newChunk.Type, "sPLT", 4)) { if(-1 == pngData.Feature.sPLT) pngData.Feature.sPLT = iChunk; }	// Suggests a palette to use if the full range of colors is unavailable.
		else if(0 == memcmp(newChunk.Type, "sRGB", 4)) { if(-1 == pngData.Feature.sRGB) pngData.Feature.sRGB = iChunk; }	// Indicates that the standard sRGB color space is used.
		else if(0 == memcmp(newChunk.Type, "sTER", 4)) { if(-1 == pngData.Feature.sTER) pngData.Feature.sTER = iChunk; }	// Stereo-image indicator chunk for stereoscopic images.[15]
		else if(0 == memcmp(newChunk.Type, "tIME", 4)) { if(-1 == pngData.Feature.tIME) pngData.Feature.tIME = iChunk; }	// Stores the time that the image was last changed.
		else if(0 == memcmp(newChunk.Type, "tRNS", 4)) { if(-1 == pngData.Feature.tRNS) pngData.Feature.tRNS = iChunk; }	// Contains transparency information. For indexed images, it stores alpha channel values for one or more palette entries. For truecolor and grayscale images, it stores a single pixel value that is to be regarded as fully transparent.
		else if(0 == memcmp(newChunk.Type, "acTL", 4)) { if(-1 == pngData.Feature.acTL) pngData.Feature.acTL = iChunk; info_printf("This is an animated PNG."); }	// The animation control chunk is a kind of "marker" chunk, telling the parser that this is an animated png. 
	}
	return 0;
}

static			::gpk::error_t											pngDefilterSub									(::gpk::array_view<ubyte_t>& scanline, uint32_t bpp)										{ for(uint32_t iByte = bpp	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanline[iByte - bpp];	return 0; }
static			::gpk::error_t											pngDefilterUp									(::gpk::array_view<ubyte_t>& scanline, const ::gpk::array_view<ubyte_t>& scanlinePrevious)	{ for(uint32_t iByte = 0	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanlinePrevious[iByte];		return 0; }
static			::gpk::error_t											pngDefilterAverage								(::gpk::array_view<ubyte_t>& scanline, const ::gpk::array_view<ubyte_t>& scanlinePrevious, uint32_t bpp) {
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
	int32_t																		pa												= abs(p - left		); // distances to a, b, c
	int32_t																		pb												= abs(p - above		); // a = left, b = above, c = upper left
	int32_t																		pc												= abs(p - upperleft	); // 
	// return nearest of a,b,c, breaking ties in order a,b,c.
	if (pa <= pb && pa <= pc)
		return left;
	else if (pb <= pc)
		return above;
	else 
		return upperleft;
}

static			::gpk::error_t											pngDefilterPaeth								(::gpk::array_view<ubyte_t>& scanline, const ::gpk::array_view<ubyte_t>& scanlinePrevious, uint32_t bpp) {
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

static			::gpk::error_t											pngScanlineDefilter								(const ::gpk::array_view<ubyte_t> & scanlineFilters, const ::gpk::SCoord2<uint32_t> & imageSize, uint32_t bytesPerPixel, ::gpk::array_view<::gpk::array_pod<uint8_t>> scanlines) {
	::gpk::array_pod<bool>														filteredScanlines;
	filteredScanlines.resize(scanlineFilters.size(), false);
	if(scanlineFilters.size())
		switch(scanlineFilters[0]) {
		case 1: ::pngDefilterSub	(scanlines[0], bytesPerPixel);		filteredScanlines[0] = true; break;
		case 2:															filteredScanlines[0] = true; break; // nothing to do when there is no "up" scanline
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
					reterr_error_if(scanlineFilters[y] > 4, "Invalid filter! Corrupt png file?");
			}
		}
	}

	info_printf("Decoding passess executed: %u.", countPasses);
	return 0;
}
				::gpk::error_t											gpk::pngFileLoad								(const ::gpk::array_view<const ubyte_t>& source, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_View) {
	::gpk::stream_view<const ubyte_t>											png_stream										= {source.begin(), source.size()};
	::gpk::SPNGData																pngData											;
	::gpk::array_pod<uint32_t>													indicesIDAT;
	gpk_necall(::pngActualFileLoad(source, pngData, indicesIDAT), "Failed to read png stream! Corrupt file?");
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
	info_printf("----- PNG File Info summary: "
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
		, imageDeflated.size()
		);
	uint32_t																	maxScanLineWidth							= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageHeader.Size.x);
	pngData.Inflated.resize(maxScanLineWidth * 2 * imageHeader.Size.y + imageHeader.Size.y);
	gpk_necall(::pngStreamInflate(pngData.Deflated, pngData.Inflated), "Failed to decompress!");

	::gpk::array_obj<::gpk::array_pod<ubyte_t>>									& scanlines									= pngData.Scanlines;
	uint32_t																	bytesPerPixel								= 1;
	switch(imageHeader.ColorType) {
	case 0: bytesPerPixel = (imageHeader.BitDepth == 16) ? 2 : 1; break;
	case 2: bytesPerPixel = (imageHeader.BitDepth == 16) ? 6 : 3; break;
	case 4: bytesPerPixel = (imageHeader.BitDepth == 16) ? 4 : 2; break;
	case 6: bytesPerPixel = (imageHeader.BitDepth == 16) ? 8 : 4; break;
	}

	if(imageHeader.MethodInterlace) {
		::gpk::array_view<::gpk::SCoord2<uint32_t>>									imageSizes									= pngData.Adam7Sizes;
		imageSizes	[0]															= // 1
			{ (imageHeader.Size.x <= 4)	? 0 : imageHeader.Size.x / 8 + one_if(imageHeader.Size.x % 8)
			, (imageHeader.Size.y <= 4)	? 0 : imageHeader.Size.y / 8 + one_if(imageHeader.Size.y % 8)
			};
		imageSizes	[1]															= // 2
			{ (imageHeader.Size.x <= 4)	? 0 : (uint32_t)::gpk::max(((int32_t)imageHeader.Size.x - 4) / 8, 0) + one_if(((int32_t)imageHeader.Size.x - 4) % 8)
			, (imageHeader.Size.y <= 4)	? 0 : imageHeader.Size.y / 8 + one_if((imageHeader.Size.y) % 8)
			};
		imageSizes	[2]															= // 3
			{ (imageHeader.Size.x <= 4)	? 0 : imageHeader.Size.x / 4 + one_if(imageHeader.Size.x % 4)
			, (imageHeader.Size.y <= 4)	? 0 : (uint32_t)::gpk::max(((int32_t)imageHeader.Size.y - 4) / 8, 0) + one_if(((int32_t)imageHeader.Size.y - 4) % 8)
			};
		imageSizes	[3]															= // 4
			{ (imageHeader.Size.x <= 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageHeader.Size.x - 2) / 4, 0) + one_if(((int32_t)imageHeader.Size.x - 2) % 4)
			, (imageHeader.Size.y <= 4)	? 0 : imageHeader.Size.y / 4 + one_if(imageHeader.Size.y % 4)
			};
		imageSizes	[4]															= // 5
			{ (imageHeader.Size.x <  2)	? 0 : (imageHeader.Size.x) / 2 + one_if(imageHeader.Size.x % 2)
			, (imageHeader.Size.y <= 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageHeader.Size.y - 2) / 4, 0) + one_if(((int32_t)imageHeader.Size.y - 2) % 4)
			};
		imageSizes	[5]															= // 6
			{ (imageHeader.Size.x < 2)	? 0 : (uint32_t)::gpk::max(((int32_t)imageHeader.Size.x - 1) / 2, 0) + one_if(((int32_t)imageHeader.Size.x - 1) % 2)
			, (imageHeader.Size.y < 2)	? 0 : (imageHeader.Size.y) / 2 + one_if(imageHeader.Size.y % 2)
			};
		imageSizes	[6]															= // Adam 7
			{ imageHeader.Size.x
			, imageHeader.Size.y		/ 2 
			};

		//return -1;
		uint32_t																	totalScanlines									= 0;
		for(uint32_t iImage = 0; iImage < 7; ++iImage) 
			totalScanlines															+= imageSizes[iImage].y;
		gpk_necall(scanlines.resize(totalScanlines)			, "Out of memory or corrupt file.");
		gpk_necall(pngData.Filters.resize(scanlines.size())	, "Out of memory or corrupt file.");
		uint32_t																	offsetByte										= 0;
		uint32_t																	offsetScanline									= 0;
		for(uint32_t iImage = 0; iImage < 7; ++iImage) {
			uint32_t																	widthScanlineCurrent							= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageSizes[iImage].x);
			info_printf("Image: %u. Scanline size: %u.", iImage, widthScanlineCurrent);
			const ::gpk::SCoord2<uint32_t>												currentImageSize								= imageSizes[iImage];
			for(uint32_t y = 0; y < currentImageSize.y; ++y) {
				int32_t																		currentScanline									= offsetScanline + y;
				pngData.Filters[currentScanline]										= pngData.Inflated[offsetByte + y * widthScanlineCurrent + y];
				info_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[currentScanline]);
				gpk_necall(scanlines[currentScanline].resize(widthScanlineCurrent), "Out of memory or corrupt file.");
				memcpy(scanlines[currentScanline].begin(), &pngData.Inflated[offsetByte + y * widthScanlineCurrent + y + 1], widthScanlineCurrent);
			}

			if(currentImageSize.y) {
				gpk_necall(::pngScanlineDefilter({&pngData.Filters[offsetScanline], currentImageSize.y}, currentImageSize, bytesPerPixel, {&scanlines[offsetScanline], currentImageSize.y}), "Corrupt file?");
				if(imageHeader.ColorType == 3 || imageHeader.ColorType == 0) // Decode pixel ordering for bit depths of 1, 2 and 4 bits
					if(widthScanlineCurrent)
						::scanlineBitDecodeOrder(imageHeader.BitDepth, {&scanlines[offsetScanline], currentImageSize.y});
				if(widthScanlineCurrent)
					offsetByte																+= (widthScanlineCurrent + 1) * currentImageSize.y;
				offsetScanline															+= currentImageSize.y;
			}
		}
	}
	else {
		pngData.Filters.clear();
		scanlines.resize(imageHeader.Size.y);
		uint32_t																	widthScanline									= ::pngScanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageHeader.Size.x);
		info_printf("Scanline size: %u.", widthScanline);
		for(uint32_t y = 0; y < imageHeader.Size.y; ++y) {
			pngData.Filters.push_back(pngData.Inflated[y * widthScanline + y]);
			info_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[y]);
			gpk_necall(scanlines[y].resize(widthScanline), "Out of memory or corrupt file.");
			memcpy(scanlines[y].begin(), &pngData.Inflated[y * widthScanline + y + 1], widthScanline);
		}
		gpk_necall(::pngScanlineDefilter(pngData.Filters, imageHeader.Size, bytesPerPixel, scanlines), "Corrupt file?");
		if(imageHeader.ColorType == 3 || imageHeader.ColorType == 0) // Decode pixel ordering for bit depths of 1, 2 and 4 bits
			::scanlineBitDecodeOrder(imageHeader.BitDepth, pngData.Scanlines);
	}

	gpk_necall(out_Colors.resize(imageHeader.Size.x * imageHeader.Size.y), "Invalid image size?");
	out_View																= {out_Colors.begin(), imageHeader.Size};
	if(imageHeader.MethodInterlace)
		return ::pngDecodeInterlaced
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, pngData.Adam7Sizes
			, out_View
			);
	else
		return ::pngDecode
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, out_View
			);
}

				::gpk::error_t											gpk::pngFileLoad								(const ::gpk::view_const_string	& filename, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)	{
	FILE																		* fp											= 0;
	ree_if(0 != fopen_s(&fp, filename.begin(), "rb") || 0 == fp, "Failed to open file: %s.", filename.begin());
	info_printf("Loading png file: %s.", filename.begin());
	if(0 == fp) // don't do anything with a null pointer
		return -1;
	fseek(fp, 0, SEEK_END);
	const int32_t																fileSize										= (int32_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	::gpk::array_pod<ubyte_t>													fileInMemory									= 0;
	gpk_necall(fileInMemory.resize(fileSize), "Failet to load file in memory. File too large? : %llu.", (uint64_t)fileSize);
	if(fileSize != (int32_t)fread(fileInMemory.begin(), sizeof(byte_t), fileSize, fp)) {
		fclose(fp);
		error_printf("Failet to read file: '%s'. File corrupt?", filename.begin());
		return -1;
	}
	fclose(fp);
	return ::gpk::pngFileLoad(fileInMemory, out_Colors, out_ImageView);
}

