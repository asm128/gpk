#include "gpk_png.h"

#include "gpk_view_stream.h"
#include "gpk_adam7.h"
#include "gpk_file.h"
#include "gpk_bit.h"
#include "gpk_deflate.h"

#include "gpk_view_color.h"

using gpk::sc_t, gpk::sc_c;

#ifdef abs
#	undef abs
#endif

static	::gpk::error_t	scanLineSizeFromFormat	(int32_t colorType, int32_t bitDepth, int32_t imageWidth)	{
	int32_t																		scanLineWidth									= imageWidth;
	switch(bitDepth) {
	default	  : return -1; // ?? Corrupt file?
	case	 1: { scanLineWidth /= 8;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 2: { scanLineWidth /= 4;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 4: { scanLineWidth /= 2;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); } else if(colorType == 6) { error_printf("Unsupported color type/bit depth combination: %i, %i.", colorType, bitDepth); }	} break;
	case	 8: {						if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }			} break;
	case	16: { scanLineWidth *= 2;	if(colorType == 2) { scanLineWidth *= 3; } else if(colorType == 4) { scanLineWidth *= 2; } else if(colorType == 6) { scanLineWidth *= 4; }			} break;
	}

	switch(bitDepth) {
	default	  : return scanLineWidth;
	case	 1: return scanLineWidth + ((imageWidth % 8) ? 1 : 0);
	case	 2: return scanLineWidth + ((imageWidth % 4) ? 1 : 0);
	case	 4: return scanLineWidth + ((imageWidth % 2) ? 1 : 0);
	}
}

static	::gpk::error_t	pngScanlineDecode_2_8	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb8>					viewPixels										= {(::gpk::crgb8*)scanline.begin(), scanline.size() / 3};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			::gpk::crgb8												& pixelInput									= viewPixels[iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_2_8	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8										& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb8>					viewPixels										= {(::gpk::crgb8*)scanline.begin(), scanline.size() / 3};
		::gpk::vu16												scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::crgb8										& pixelInput									= viewPixels[iPixel];
			scanlineOut[iPixel]									= ::gpk::SColorBGR{pixelInput.b, pixelInput.g, pixelInput.r};
		}
	}
	return 0;
}

static	uint8_t			toGrayscale8			(::gpk::bgra color)	{ return uint8_t (.3f / 255 * color.r + .59f / 255 * color.g + .11f / 255 * color.b) *   255; }
static	uint8_t			toGrayscale8			(::gpk::bgr  color)	{ return uint8_t (.3f / 255 * color.r + .59f / 255 * color.g + .11f / 255 * color.b) *   255; }

static	::gpk::error_t	pngScanlineDecode_2_8	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb8>					viewPixels										= {(::gpk::crgb8*)scanline.begin(), scanline.size() / 3};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::crgb8												& pixelInput									= viewPixels[iPixel];
			scanlineOut[iPixel]														= ::toGrayscale8(::gpk::SColorBGR{pixelInput.b, pixelInput.g, pixelInput.r});
		}
	}
	return 0;
}

// grayscale 8
static	::gpk::error_t	pngScanlineDecode_0_8	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			uint8_t																		pixelInput										= scanline[iPixel];
			pixelOutput																= {pixelInput, pixelInput, pixelInput, 255};
		}
	}
	return 0;
}

// grayscale 8
static	::gpk::error_t	pngScanlineDecode_0_8	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			scanlineOut[iPixel]														= scanline[iPixel];
		}
	}
	return 0;
}

// grayscale 8
static	::gpk::error_t	pngScanlineDecode_0_8	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 255 * 65535;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			scanlineOut[iPixel]														= (uint16_t)(unitChannel * scanline[iPixel]);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_2_16	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb16>					viewPixels										= {(::gpk::crgb16*)scanline.begin(), scanline.size() / 6};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::rgb16													pixelInput										= viewPixels[iPixel];
			be2le_16(pixelInput.b);
			be2le_16(pixelInput.g);
			be2le_16(pixelInput.r);
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			pixelOutput.b															= (uint8_t)(unitChannel * pixelInput.b);
			pixelOutput.g															= (uint8_t)(unitChannel * pixelInput.g);
			pixelOutput.r															= (uint8_t)(unitChannel * pixelInput.r);
			pixelOutput.a															= 255;
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_2_16	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb16>					viewPixels										= {(::gpk::crgb16*)scanline.begin(), scanline.size() / 6};
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::rgb16													pixelInput										= viewPixels[iPixel];
			be2le_16(pixelInput.b);
			be2le_16(pixelInput.g);
			be2le_16(pixelInput.r);
			::gpk::bgra															pixelOutput									= 
			{ (uint8_t)(unitChannel * pixelInput.b)
			, (uint8_t)(unitChannel * pixelInput.g)
			, (uint8_t)(unitChannel * pixelInput.r)
			, 255
			};
			scanlineOut[iPixel]														= ::gpk::SColor16(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_2_16	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgb16>					viewPixels										= {(::gpk::crgb16*)scanline.begin(), scanline.size() / 6};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::rgb16													pixelInput										= viewPixels[iPixel];
			be2le_16(pixelInput.b);
			be2le_16(pixelInput.g);
			be2le_16(pixelInput.r);
			::gpk::bgra															pixelOutput									= 
				{ (uint8_t)(unitChannel * pixelInput.b)
				, (uint8_t)(unitChannel * pixelInput.g)
				, (uint8_t)(unitChannel * pixelInput.r)
				, 255
				};
			scanlineOut[iPixel]														= ::toGrayscale8(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_16	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	const double									unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8										& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::vcu16										viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::view<::gpk::bgra>								scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint16_t												toReverse										= viewPixels[iPixel];
			be2le_16(toReverse);
			uint8_t													largeValue										= (uint8_t)(unitChannel * toReverse);
			::gpk::bgra												& pixelOutput									= scanlineOut[iPixel];
			pixelOutput											= {largeValue, largeValue, largeValue, 255};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_16	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())	
			continue;
		const ::gpk::vcu16			viewPixels				= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::vu16					scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint16_t					toReverse				= viewPixels[iPixel];
			be2le_16(toReverse);
			scanlineOut[iPixel]		= toReverse;
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_16	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 255 * 65535;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::vcu16			viewPixels										= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::vu8					scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			uint16_t					toReverse										= viewPixels[iPixel];
			be2le_16(toReverse);
			scanlineOut[iPixel]		= uint8_t(toReverse * unitChannel);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_8	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::vc8rgba		viewPixels				= {(const ::gpk::rgba*)scanline.begin(), scanline.size() / 4};
		::gpk::v8bgra				scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			const ::gpk::rgba8											& pixelInput									= viewPixels[iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, pixelInput.a};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_8	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<const ::gpk::rgba8>					viewPixels										= {(const ::gpk::rgba8*)scanline.begin(), scanline.size() / 4};
		::gpk::vu16					scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const ::gpk::rgba8											& pixelInput									= viewPixels[iPixel];
			scanlineOut[iPixel]														= ::gpk::SColorBGR{pixelInput.b, pixelInput.g, pixelInput.r};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_8	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgba8>					viewPixels										= {(::gpk::crgba8*)scanline.begin(), scanline.size() / 4};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const ::gpk::rgba8											& pixelInput									= viewPixels[iPixel];
			scanlineOut[iPixel]														= ::toGrayscale8(::gpk::SColorBGR{pixelInput.b, pixelInput.g, pixelInput.r});
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_16	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<::gpk::crgba16>					viewPixels										= {(::gpk::crgba16*)scanline.begin(), scanline.size() / 8};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_rgba<uint16_t>													colorReversed									= viewPixels[iPixel];
			be2le_16(colorReversed.b);
			be2le_16(colorReversed.g);
			be2le_16(colorReversed.r);
			be2le_16(colorReversed.a);
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			pixelOutput.b															= (uint8_t)(unitChannel * colorReversed.b);
			pixelOutput.g															= (uint8_t)(unitChannel * colorReversed.g);
			pixelOutput.r															= (uint8_t)(unitChannel * colorReversed.r);
			pixelOutput.a															= (uint8_t)(unitChannel * colorReversed.a);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_16	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitChannel										= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view<const ::gpk::color_rgba<uint16_t>>					viewPixels										= {(const ::gpk::color_rgba<uint16_t>*)scanline.begin(), scanline.size() / 8};
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_rgba<uint16_t>													colorReversed									= viewPixels[iPixel];
			be2le_16(colorReversed.b);
			be2le_16(colorReversed.g);
			be2le_16(colorReversed.r);
			be2le_16(colorReversed.a);
			::gpk::bgra															pixelOutput									= 
				{ (uint8_t)(unitChannel * colorReversed.b)
				, (uint8_t)(unitChannel * colorReversed.g)
				, (uint8_t)(unitChannel * colorReversed.r)
				, (uint8_t)(unitChannel * colorReversed.a)
				};
			scanlineOut[iPixel]														= ::gpk::SColor16(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_6_16	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	const double		unitChannel				= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::v1<const ::gpk::rgba16>	viewPixels			= {(const ::gpk::rgba16*)scanline.begin(), scanline.size() / 8};
		::gpk::vu8					scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			::gpk::color_rgba<uint16_t>	colorReversed			= viewPixels[iPixel];
			be2le_16(colorReversed.b);
			be2le_16(colorReversed.g);
			be2le_16(colorReversed.r);
			be2le_16(colorReversed.a);
			::gpk::bgra					pixelOutput				= 
				{ (uint8_t)(unitChannel * colorReversed.b)
				, (uint8_t)(unitChannel * colorReversed.g)
				, (uint8_t)(unitChannel * colorReversed.r)
				, (uint8_t)(unitChannel * colorReversed.a)
				};
			scanlineOut[iPixel]		= ::toGrayscale8(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_8	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::vcu16			viewPixels				= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::view<::gpk::bgra>	scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint16_t				valueInput				= viewPixels[iPixel];
			const uint8_t				valueGrayscale			= valueInput & 0xFFU;
			::gpk::bgra					& pixelOutput			= scanlineOut[iPixel];
			pixelOutput				= {valueGrayscale, valueGrayscale, valueGrayscale, (uint8_t)((valueInput & 0xFF00U) >> 8)};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_8	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())
			continue;

		const ::gpk::vcu16			viewPixels				= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::vu16					scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel)
			scanlineOut[iPixel]		= viewPixels[iPixel];
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_8	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8			& scanline				= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::vcu16			viewPixels				= {(const uint16_t*)scanline.begin(), scanline.size() / 2};
		::gpk::vu8					scanlineOut				= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint16_t				valueInput				= viewPixels[iPixel];
			const uint8_t				valueGrayscale			= valueInput & 0xFFU;
			scanlineOut[iPixel]		= uint8_t(valueGrayscale / 255.0 * 15) | uint8_t(((valueInput & 0xFF00U) >> 8) / 255.0 * 15);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_16	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitGreyscale									= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view<uint32_t>													viewPixels										= {(uint32_t*)scanline.begin(), scanline.size() / 4};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint32_t																packedValues									= viewPixels[iPixel];
			uint16_t																	valueGrayscale									= (packedValues & 0xFFFFU);
			uint16_t																	valueAlpha										= (packedValues & 0xFFFF0000U) >> 16;
			be2le_16(valueGrayscale	);
			be2le_16(valueAlpha		);
			const uint8_t																greyFinal										= (uint8_t)(unitGreyscale * valueGrayscale);
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			pixelOutput																= {greyFinal, greyFinal, greyFinal, (uint8_t)(unitGreyscale * valueAlpha)};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_16	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitGreyscale									= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view<uint32_t>													viewPixels										= {(uint32_t*)scanline.begin(), scanline.size() / 4};
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint32_t																packedValues									= viewPixels[iPixel];
			uint16_t																	valueGrayscale									= (packedValues & 0xFFFFU);
			uint16_t																	valueAlpha										= (packedValues & 0xFFFF0000U) >> 16;
			be2le_16(valueGrayscale	);
			be2le_16(valueAlpha		);
			const uint8_t																greyFinal										= (uint8_t)(unitGreyscale * valueGrayscale);
			scanlineOut[iPixel]														= uint8_t(greyFinal) | (uint8_t)(unitGreyscale * valueAlpha);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_4_16	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines)		{
	stacxpr	double				unitGreyscale									= 1.0 / 65535 * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view<uint32_t>													viewPixels										= {(uint32_t*)scanline.begin(), scanline.size() / 4};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < viewPixels.size(); ++iPixel) {
			const uint32_t																packedValues									= viewPixels[iPixel];
			uint16_t																	valueGrayscale									= (packedValues & 0xFFFFU);
			uint16_t																	valueAlpha										= (packedValues & 0xFFFF0000U) >> 16;
			be2le_16(valueGrayscale	);
			be2le_16(valueAlpha		);
			const uint8_t																greyFinal										= (uint8_t)(unitGreyscale * valueGrayscale);
			scanlineOut[iPixel]														= uint8_t(greyFinal / 255.0 * 15) | (uint8_t)(unitGreyscale * valueAlpha / 255.0 * 15);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_8	(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::view<::gpk::bgr>& palette)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			uint8_t																		paletteIndex									= scanline[iPixel];
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			const ::gpk::bgr												& pixelInput									= palette[paletteIndex];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_8	(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::vbgr & palette)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			uint8_t																		paletteIndex									= scanline[iPixel];
			const ::gpk::bgr												& pixelInput									= palette[paletteIndex];
			::gpk::SColorBGR															pixelOutput										= {pixelInput.b, pixelInput.g, pixelInput.r};
			scanlineOut[iPixel]														= ::gpk::SColor16{pixelOutput};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_8	(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::vbgr & palette)		{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < scanline.size(); ++iPixel) {
			uint8_t																		paletteIndex									= scanline[iPixel];
			const ::gpk::bgr												& pixelInput									= palette[paletteIndex];
			::gpk::SColorBGR															pixelOutput										= {pixelInput.b, pixelInput.g, pixelInput.r};
			scanlineOut[iPixel]														= ::toGrayscale8(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_bits(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::vbgr & palette, uint32_t bitDepth)	{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		indexPalette									= 0;
			::gpk::view_bit<uint8_t>													indexPaletteBits								= {&indexPalette, (uint32_t)8};
			for(uint32_t i=0; i < bitDepth; ++i)
				indexPaletteBits[i]														= viewPixels[bitDepth * iPixel + i];

			const ::gpk::bgr												& pixelInput									= (indexPalette >= palette.size()) ? ::gpk::bgr{} : palette[indexPalette];
			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			pixelOutput																= {pixelInput.b, pixelInput.g, pixelInput.r, 255};
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_bits(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::vbgr & palette, uint32_t bitDepth)	{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		indexPalette									= 0;
			::gpk::view_bit<uint8_t>													indexPaletteBits								= {&indexPalette, (uint32_t)8};
			for(uint32_t i=0; i < bitDepth; ++i)
				indexPaletteBits[i]														= viewPixels[bitDepth * iPixel + i];

			const ::gpk::bgr												& pixelInput									= (indexPalette >= palette.size()) ? ::gpk::bgr{} : palette[indexPalette];
			::gpk::SColorBGR															pixelOutput									= {pixelInput.b, pixelInput.g, pixelInput.r};
			scanlineOut[iPixel]														= ::gpk::SColor16(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_3_bits(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, const ::gpk::vbgr & palette, uint32_t bitDepth)	{
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint8_t																		indexPalette									= 0;
			::gpk::view_bit<uint8_t>													indexPaletteBits								= {&indexPalette, (uint32_t)8};
			for(uint32_t i=0; i < bitDepth; ++i)
				indexPaletteBits[i]														= viewPixels[bitDepth * iPixel + i];

			const ::gpk::bgr												& pixelInput									= (indexPalette >= palette.size()) ? ::gpk::bgr{} : palette[indexPalette];
			::gpk::SColorBGR															pixelOutput									= {pixelInput.b, pixelInput.g, pixelInput.r};
			scanlineOut[iPixel]														= ::toGrayscale8(pixelOutput);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_bits(::gpk::g8bgra & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, uint32_t bitDepth)	{
	double																		unitGrayscale									= 1.0 / ((1 << bitDepth) - 1) * 255;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::view<::gpk::bgra>										scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint64_t																	valueGrayscale								= 0;
			::gpk::view_bit<uint64_t>													valueGrayscaleBits							= {&valueGrayscale, (uint32_t)64};
			for(uint32_t i = 0; i < bitDepth; ++i)
				valueGrayscaleBits[i]													= viewPixels[bitDepth * iPixel + i];

			::gpk::bgra															& pixelOutput									= scanlineOut[iPixel];
			pixelOutput.r															=
			pixelOutput.g															=
			pixelOutput.b															= (1 == bitDepth) ? (uint8_t)(valueGrayscale * 255) : (uint8_t)(valueGrayscale * unitGrayscale);
			pixelOutput.a															= 255;
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_bits(::gpk::gu16 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, uint32_t bitDepth)	{
	double																		unitGrayscale									= 1.0 / ((1 << bitDepth) - 1) * 65535;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::vu16													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint64_t																	valueGrayscale								= 0;
			::gpk::view_bit<uint64_t>													valueGrayscaleBits							= {&valueGrayscale, (uint32_t)64};
			for(uint32_t i = 0; i < bitDepth; ++i)
				valueGrayscaleBits[i]													= viewPixels[bitDepth * iPixel + i];

			scanlineOut[iPixel]														= (1 == bitDepth) ? (uint16_t)valueGrayscale : (uint16_t)(valueGrayscale * unitGrayscale);
		}
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDecode_0_bits(::gpk::gu8 & out_View, const ::gpk::view<const ::gpk::au8> & scanlines, uint32_t bitDepth)	{
	double																		unitGrayscale									= 1.0 / ((1 << bitDepth) - 1) * 65535;
	for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
		const ::gpk::au8												& scanline										= scanlines[iScanline];
		if(0 == scanline.size())
			continue;
		const ::gpk::view_bit<const uint8_t>										viewPixels										= ::gpk::view_bit<const uint8_t>{scanline.begin(), out_View.metrics().x * bitDepth};
		::gpk::vu8													scanlineOut										= out_View[iScanline];
		for(uint32_t iPixel = 0; iPixel < out_View.metrics().x; ++iPixel) {
			uint64_t																	valueGrayscale								= 0;
			::gpk::view_bit<uint64_t>													valueGrayscaleBits							= {&valueGrayscale, (uint32_t)64};
			for(uint32_t i = 0; i < bitDepth; ++i)
				valueGrayscaleBits[i]													= viewPixels[bitDepth * iPixel + i];

			scanlineOut[iPixel]														= (1 == bitDepth) ? (uint8_t)valueGrayscale : (uint8_t)(valueGrayscale * unitGrayscale);
		}
	}
	return 0;
}

// const gpk::function & pngScanlineDecode_2_8;
// const gpk::function & pngScanlineDecode_4_8;
// const gpk::function & pngScanlineDecode_6_8;
// const gpk::function & pngScanlineDecode_3_8;

tplt<tpnm _tPixel>
static	::gpk::error_t	pngDecode
	( const int32_t							bitDepth
	, const int32_t							colorType
	, const ::gpk::view<const ::gpk::au8>	& scanlines
	, const ::gpk::vbgr						& palette
	, ::gpk::grid<_tPixel>				& out_View
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

tplt<tpnm _tPixel>
static	::gpk::error_t	pngDecodeInterlaced
	( const int32_t							bitDepth
	, const int32_t							colorType
	, const ::gpk::view<const ::gpk::au8>	& scanlines
	, const ::gpk::vbgr						& palette
	, const ::gpk::view<const ::gpk::n2u32>	& imageSizes
	, ::gpk::grid<_tPixel>				& out_View
	) {
	::gpk::img<_tPixel>			adam7			[7]		= {};
	uint32_t					offsetScanline			= 0;
	for(uint32_t iImage = 0; iImage < imageSizes.size(); ++iImage) {
		const ::gpk::n2u32			currentImageSize		= imageSizes[iImage];
		adam7[iImage].resize(currentImageSize);
		if(0 == adam7[iImage].Texels.size())
			continue;
		const ::gpk::view<const::gpk::au8>	currentScanlineSet	= {&scanlines[offsetScanline], currentImageSize.y};
		switch(colorType) {
		case 0: // grayscale
			 if(bitDepth == 8)
				 gpk_necs(::pngScanlineDecode_0_8(adam7[iImage].View, currentScanlineSet));
			else if(bitDepth == 16)
				gpk_necs(::pngScanlineDecode_0_16(adam7[iImage].View, currentScanlineSet));
			else  if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1)
				gpk_necs(::pngScanlineDecode_0_bits(adam7[iImage].View, currentScanlineSet, bitDepth));
			break;
		case 2: // RGB
			if(bitDepth == 8)
				gpk_necs(::pngScanlineDecode_2_8(adam7[iImage].View, currentScanlineSet));
			else if(bitDepth == 16)
				gpk_necs(::pngScanlineDecode_2_16(adam7[iImage].View, currentScanlineSet));
			break;
		case 3: // Palette
			if(bitDepth == 8)
				gpk_necs(::pngScanlineDecode_3_8(adam7[iImage].View, currentScanlineSet, palette));
			else if(bitDepth == 4 || bitDepth == 2 || bitDepth == 1)
				gpk_necs(::pngScanlineDecode_3_bits(adam7[iImage].View, currentScanlineSet, palette, bitDepth));
			break;
		case 4: // Grayscale + Alpha
			if(bitDepth == 8)
				gpk_necs(::pngScanlineDecode_4_8(adam7[iImage].View, currentScanlineSet));
			else if(bitDepth == 16)
				gpk_necs(::pngScanlineDecode_4_16(adam7[iImage].View, currentScanlineSet));
			break;
		case 6: // RGBA
			if(bitDepth == 8)
				gpk_necs(::pngScanlineDecode_6_8(adam7[iImage].View, currentScanlineSet));
			else if(bitDepth == 16)
				gpk_necs(::pngScanlineDecode_6_16(adam7[iImage].View, currentScanlineSet));
			break;
		} // switch(colorType)
		offsetScanline			+= currentImageSize.y;
	}
	return ::gpk::adam7Interlace(::gpk::view<::gpk::img<_tPixel>>{adam7}, out_View);
}


static	::gpk::error_t	scanlineBitDecodeOrder
	( const int32_t				bitDepth
	, ::gpk::view<::gpk::au8>	scanlines
	) {
	if(bitDepth <= 4)
		for(uint32_t iScanline = 0; iScanline < scanlines.size(); ++iScanline) {
			::gpk::au8				& scanline					= scanlines[iScanline];
			for(uint32_t iByte = 0; iByte < scanline.size(); ++iByte) {
				uint8_t					& pixel						= scanline[iByte];
				pixel				= ::gpk::reverse_bitfield(pixel, bitDepth);
			}
		}
	return 0;
}

static	::gpk::error_t	pngActualFileLoad		(const ::gpk::vcu8 & source, ::gpk::SPNGData & pngData, ::gpk::au32 & indicesIDAT)	{
	pngData.Feature			= {};
	memset(pngData.Signature, 0, ::gpk::size(pngData.Signature));
	::gpk::clear
		( pngData.Deflated
		, pngData.Inflated
		, pngData.Filters
		, pngData.Chunks
		);
	memset(pngData.Adam7Sizes, 0, ::gpk::size(pngData.Adam7Sizes) * sizeof(::gpk::n2u32));
	pngData.Header			= {};

	::gpk::view_stream<const uint8_t>	png_stream		= {source.begin(), source.size()};
	png_stream.read_pod(pngData.Signature);
	while(png_stream.CursorPosition < (png_stream.size() - 7)) {
		::gpk::SPNGChunk			chunkRead				= {};
		uint32_t					sizeChunk				= 0;
		gpk_necall(png_stream.read_pod(sizeChunk)		, "%s", "Failed to read PNG! File corrupt?");
		uint32_t					crcDataStart			= png_stream.CursorPosition;
		gpk_necall(png_stream.read_pod(chunkRead.Type)	, "%s", "Failed to read PNG! File corrupt?");
		be2le_32(sizeChunk);
		ree_if(sizeChunk > (0x7FFFFFFF >> 2), "%s", "Chunk too large! Corrupt file?");
		gpk_necall(chunkRead.Data.resize(sizeChunk), "Out of memory? Chunk size: %u.", sizeChunk);
		gpk_necall(png_stream.read_pod(chunkRead.Data.begin(), sizeChunk)	, "%s", "Failed to read PNG! File corrupt?");
		gpk_necall(png_stream.read_pod(chunkRead.CRC)						, "%s", "Failed to read PNG! File corrupt?");
		be2le_32(chunkRead.CRC);
		uint32_t					crcGenerated			= ::gpk::get_crc({&source[crcDataStart], sizeChunk + 4});
		ef_if(crcGenerated != chunkRead.CRC, "Invalid CRC: File: %X, Generated: %X.", chunkRead.CRC, crcGenerated);
		gpk_necs(pngData.Chunks.push_back(chunkRead));
		break_gverbose_if(0 == memcmp(chunkRead.Type, "IEND", 4), "%s", "Found IEND chunk (image end).");
	}

	::gpk::SPNGIHDR						& imageHeader	= pngData.Header;
	::gpk::a8bgr						& palette		= pngData.Palette;
	for(uint32_t iChunk = 0; iChunk < pngData.Chunks.size(); ++iChunk) {
		const ::gpk::SPNGChunk				& newChunk		= pngData.Chunks[iChunk];
		verbose_printf("Found chunk of type: %c%c%c%c. Data size: %u.", newChunk.Type[0], newChunk.Type[1], newChunk.Type[2], newChunk.Type[3], (uint32_t)newChunk.Data.size());
			 if(0 == memcmp(newChunk.Type, "IDAT", 4)) { indicesIDAT.push_back(iChunk); }
		else if(0 == memcmp(newChunk.Type, "IHDR", 4)) {
			imageHeader						= *(const ::gpk::SPNGIHDR*)newChunk.Data.begin();
			be2le_32(imageHeader.Size.x);
			be2le_32(imageHeader.Size.y);
		}
		else if(0 == memcmp(newChunk.Type, "PLTE", 4)) {
			uint32_t							colorCount		= newChunk.Data.size() / 3;
			if(colorCount) {
				verbose_printf("Loading palette of %u colors.", colorCount);
				gpk_necs(palette.resize(colorCount));
				const ::gpk::rgb					* pngPalette	= (::gpk::rgb*)newChunk.Data.begin();
				for(uint32_t iColor = 0; iColor < colorCount; ++iColor) {
					palette[iColor].b				= pngPalette[iColor].b;
					palette[iColor].g				= pngPalette[iColor].g;
					palette[iColor].r				= pngPalette[iColor].r;
				}
			}
		}
		else if(0 == memcmp(newChunk.Type, "tEXt", 4)) { 	// Can store text that can be represented in ISO/IEC 8859-1, with one key-value pair for each chunk. The "key" must be between 1 and 79 characters long. Separator is a null character. The "value" can be any length, including zero up to the maximum permissible chunk size minus the length of the keyword and separator. Neither "key" nor "value" can contain null character. Leading or trailing spaces are also disallowed.
			verbose_printf("Text Key: %s.", newChunk.Data.begin());
			uint32_t							keyLength										= (uint32_t)strlen((const sc_t*)newChunk.Data.begin());
			uint32_t							valueLength										= newChunk.Data.size() - (keyLength + 1);
			::gpk::apod<sc_t>					value;
			gpk_necs(value.resize(valueLength + 1));
			value[valueLength]				= 0;
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

static	::gpk::error_t	pngDefilterSub			(::gpk::vu8 & scanline, uint32_t bpp)										{ for(uint32_t iByte = bpp	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanline[iByte - bpp];	return 0; }
static	::gpk::error_t	pngDefilterUp			(::gpk::vu8 & scanline, const ::gpk::vcu8 & scanlinePrevious)				{ for(uint32_t iByte = 0	; iByte < scanline.size(); ++iByte) scanline[iByte] += scanlinePrevious[iByte];	return 0; }
static	::gpk::error_t	pngDefilterAverage		(::gpk::vu8 & scanline, const ::gpk::vcu8 & scanlinePrevious, uint32_t bpp)	{
	if(scanlinePrevious.size()) {
		for(uint32_t iByte = 0; iByte < bpp; ++iByte)
			scanline[iByte]			+= ((uint32_t)scanlinePrevious[iByte] / 2) & 0xFFU;
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]			+= (((uint32_t)scanline[iByte - bpp] + scanlinePrevious[iByte]) / 2) & 0xFFU;
	} else {
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]			+= ((uint32_t)scanline[iByte - bpp] / 2) & 0xFFU;
	}
	return 0;
}

static	::gpk::error_t	paethPredictor			(int32_t left, int32_t above, int32_t upperleft)	{
	int32_t						p						= left + above - upperleft;	// initial estimate
	int32_t						pa						= ::gpk::abs(int(p - left		)); // distances to a, b, c
	int32_t						pb						= ::gpk::abs(int(p - above		)); // a = left, b = above, c = upper left
	int32_t						pc						= ::gpk::abs(int(p - upperleft	)); //
	if (pa <= pb && pa <= pc)	// return nearest of a,b,c, breaking ties in order a,b,c.
		return left;
	return (pb <= pc) ? above : upperleft;
}

static	::gpk::error_t	pngDefilterPaeth		(::gpk::vu8 & scanline, const ::gpk::vcu8 & scanlinePrevious, uint32_t bpp) {
	if(scanlinePrevious.size()) {
		for(uint32_t iByte = 0; iByte < bpp; ++iByte)
			scanline[iByte]			+= ::paethPredictor(0, scanlinePrevious[iByte], 0) & 0xFFU;
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]			+= ::paethPredictor(scanline[iByte - bpp], scanlinePrevious[iByte], scanlinePrevious[iByte - bpp]) & 0xFFU;
	} else {
		for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte)
			scanline[iByte]			+= ::paethPredictor(scanline[iByte - bpp], 0, 0) & 0xFFU;
	}
	return 0;
}

static	::gpk::error_t	pngScanlineDefilter		(const ::gpk::vcu8 & scanlineFilters, const ::gpk::n2u32 & imageSize, uint32_t bytesPerPixel, ::gpk::view<::gpk::au8> scanlines) {
	::gpk::apod<bool>			filteredScanlines;
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
			filteredScanlines[iScanline]	= true;

	for(uint32_t y = 1; y < imageSize.y; ++y)
		if(scanlineFilters[y] == 1) {
			::pngDefilterSub(scanlines[y], bytesPerPixel);
			filteredScanlines[y]	= true;
		}

	uint32_t					countPasses				= 0;
	while(true) {
		bool						filterPass				= false;
		for(uint32_t iScanline = 0; iScanline < filteredScanlines.size(); ++iScanline)
			if(false == filteredScanlines[iScanline]) {
				filterPass				= true;
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
	wf_if(countPasses > 1, "Decoding passess executed: %u.", countPasses);
	return 0;
}

static	::gpk::error_t	pngBytesPerPixel			(int32_t colorType, int32_t bitDepth)					{
	switch(colorType) {
	default: return -1;					
	case  3: return 1;							// palette 8-bit 
	case  0: return (bitDepth == 16) ? 2 : 1;	// grayscale 8 or 16 bit per channel
	case  2: return (bitDepth == 16) ? 6 : 3;	// rgb 8 or 16 bit per channel
	case  4: return (bitDepth == 16) ? 4 : 2;	// grayscale w/alpha 8 or 16 bit per channel
	case  6: return (bitDepth == 16) ? 8 : 4;	// rgba 8 or 16 bit per channel
	}
}

static	::gpk::error_t	defilterInterlaced			(::gpk::SPNGData & pngData)									{
	const ::gpk::SPNGIHDR		& imageHeader				= pngData.Header;
	const ::gpk::n2u32			& imageSize					= imageHeader.Size;
	::gpk::aobj<::gpk::au8>		& scanlines					= pngData.Scanlines;
	::gpk::view<::gpk::n2u32>	imageSizes					= pngData.Adam7Sizes;
	::gpk::adam7Sizes(imageSizes, imageSize);

	uint32_t					totalScanlines				= 0;
	for(uint32_t iImage = 0; iImage < 7; ++iImage)
		totalScanlines			+= imageSizes[iImage].y;
	gpk_necs(scanlines.resize(totalScanlines)			);
	gpk_necs(pngData.Filters.resize(scanlines.size())	);
	uint32_t					offsetByte					= 0;
	uint32_t					offsetScanline				= 0;
	const uint32_t				bytesPerPixel				= ::pngBytesPerPixel(imageHeader.ColorType, imageHeader.BitDepth);
	ree_if(errored(bytesPerPixel), "Invalid format! ColorType: %u. Bit Depth: %u.", bytesPerPixel);
	for(uint32_t iImage = 0; iImage < 7; ++iImage) {
		uint32_t					widthScanlineCurrent		= ::scanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageSizes[iImage].x);
		verbose_printf("Image: %u. Scanline size: %u.", iImage, widthScanlineCurrent);
		const ::gpk::n2u32			currentImageSize			= imageSizes[iImage];
		for(uint32_t y = 0; y < currentImageSize.y; ++y) {
			const int32_t				currentScanline				= offsetScanline + y;
			pngData.Filters[currentScanline]	= pngData.Inflated[offsetByte + y * widthScanlineCurrent + y];
			verbose_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[currentScanline]);
			gpk_necs	(scanlines[currentScanline].resize(widthScanlineCurrent));
			memcpy		(scanlines[currentScanline].begin(), &pngData.Inflated[offsetByte + y * widthScanlineCurrent + y + 1], widthScanlineCurrent);
		}

		if(currentImageSize.y) {
			gpk_necs(::pngScanlineDefilter({&pngData.Filters[offsetScanline], currentImageSize.y}, currentImageSize, bytesPerPixel, {&scanlines[offsetScanline], currentImageSize.y}));
			if(imageHeader.BitDepth < 8 && (imageHeader.ColorType == 3 || imageHeader.ColorType == 0)) { // Decode pixel ordering for bit depths of 1, 2 and 4 bits
				if(widthScanlineCurrent)
					::scanlineBitDecodeOrder(imageHeader.BitDepth, {&scanlines[offsetScanline], currentImageSize.y});
			}
			if(widthScanlineCurrent)
				offsetByte				+= (widthScanlineCurrent + 1) * currentImageSize.y;
			offsetScanline			+= currentImageSize.y;
		}
	}
	return 0;
}

stainli	::gpk::error_t	pngFilePrintInfo			(::gpk::SPNGData & pngData) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
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

static	::gpk::error_t	defilterNonInterlaced		(::gpk::SPNGData & pngData, uint32_t bytesPerPixel) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	::gpk::aobj<::gpk::au8>		& scanlines					= pngData.Scanlines;
	pngData.Filters.clear();
	scanlines.resize(imageHeader.Size.y);
	uint32_t					widthScanline				= ::scanLineSizeFromFormat(imageHeader.ColorType, imageHeader.BitDepth, imageHeader.Size.x);
	verbose_printf("Scanline size: %u.", widthScanline);
	for(uint32_t y = 0; y < imageHeader.Size.y; ++y) {
		pngData.Filters.push_back(pngData.Inflated[y * widthScanline + y]);
		verbose_printf("Filter for scanline %u: %u", y, (uint32_t)pngData.Filters[y]);
		gpk_necs(scanlines[y].resize(widthScanline));
		memcpy(scanlines[y].begin(), &pngData.Inflated[y * widthScanline + y + 1], widthScanline);
	}
	gpk_necs(::pngScanlineDefilter(pngData.Filters, imageHeader.Size, bytesPerPixel, scanlines));
	if(imageHeader.ColorType == 3 || imageHeader.ColorType == 0) // Decode pixel ordering for bit depths of 1, 2 and 4 bits
		return ::scanlineBitDecodeOrder(imageHeader.BitDepth, pngData.Scanlines);

	return 0;
}

::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::img8bgra & out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	gpk_necs(out_Texture.resize(imageHeader.Size));
	return gpk::pngDecode(pngData, out_Texture.View);
}
::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::imgu16 & out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	gpk_necs(out_Texture.resize(imageHeader.Size));
	return gpk::pngDecode(pngData, out_Texture.View);
}
::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::imgu8 & out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	gpk_necs(out_Texture.resize(imageHeader.Size));
	return gpk::pngDecode(pngData, out_Texture.View);
}

::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::g8bgra out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	if(imageHeader.MethodInterlace)
		return ::pngDecodeInterlaced
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, pngData.Adam7Sizes
			, out_Texture
			);
	else
		return ::pngDecode
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, out_Texture
			);
}

::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::gu16 out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	if(imageHeader.MethodInterlace)
		return ::pngDecodeInterlaced
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, pngData.Adam7Sizes
			, out_Texture
			);
	else
		return ::pngDecode
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, out_Texture
			);
}

::gpk::error_t			gpk::pngDecode				(::gpk::SPNGData & pngData, ::gpk::gu8 out_Texture) {
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	if(imageHeader.MethodInterlace)
		return ::pngDecodeInterlaced
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, pngData.Adam7Sizes
			, out_Texture
			);
	else
		return ::pngDecode
			( pngData.Header.BitDepth
			, pngData.Header.ColorType
			, pngData.Scanlines
			, pngData.Palette
			, out_Texture
			);
}

::gpk::error_t	gpk::pngDecode	(::gpk::SPNGData & pngData, ::gpk::au8 & out_Data, ::gpk::g8bgra & out_View) { gpk_necs(out_Data.resize(pngData.Header.Size.Area() * sizeof(::gpk::bgra))); return ::gpk::pngDecode(pngData, out_View = {(::gpk::bgra*)out_Data.begin(), pngData.Header.Size}); }
::gpk::error_t	gpk::pngDecode	(::gpk::SPNGData & pngData, ::gpk::au8 & out_Data, ::gpk::gu16   & out_View) { gpk_necs(out_Data.resize(pngData.Header.Size.Area() * sizeof(::gpk::u1_t ))); return ::gpk::pngDecode(pngData, out_View = {(::gpk::u1_t *)out_Data.begin(), pngData.Header.Size}); }
::gpk::error_t	gpk::pngDecode	(::gpk::SPNGData & pngData, ::gpk::au8 & out_Data, ::gpk::gu8    & out_View) { gpk_necs(out_Data.resize(pngData.Header.Size.Area() * sizeof(::gpk::u0_t  ))); return ::gpk::pngDecode(pngData, out_View = {(::gpk::u0_t  *)out_Data.begin(), pngData.Header.Size}); }
::gpk::error_t	gpk::pngDecode	(::gpk::SPNGData & pngData, ::gpk::au8 & out_Data) { gpk_necs(out_Data.resize(pngData.Header.Size.Area() * sizeof(::gpk::bgra))); return ::gpk::pngDecode(pngData, {(::gpk::bgra*)out_Data.begin(), pngData.Header.Size}); }


#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
stacxpr	uint32_t		DEFLATE_CHUNK_SIZE			= 1024 * 1;
stacxpr	uint32_t		INFLATE_CHUNK_SIZE			= DEFLATE_CHUNK_SIZE;
#else
stacxpr	uint32_t		DEFLATE_CHUNK_SIZE			= 1024 * 1024 * 1;
stacxpr	uint32_t		INFLATE_CHUNK_SIZE			= DEFLATE_CHUNK_SIZE;
#endif
::gpk::error_t			gpk::pngFileLoad			(::gpk::SPNGData & pngData, const ::gpk::vcu8 & source	)	{
	::gpk::au32					indicesIDAT;
	gpk_necall(::pngActualFileLoad(source, pngData, indicesIDAT), "%s", "Failed to read png stream! Corrupt file?");
	::gpk::au8					& imageDeflated				= pngData.Deflated;
	for(uint32_t iChunk = 0; iChunk < indicesIDAT.size(); ++iChunk) {
		const ::gpk::SPNGChunk		& loadedChunk				= pngData.Chunks[indicesIDAT[iChunk]];
		if(loadedChunk.Data.size()) {
			uint32_t					dataOffset					= imageDeflated.size();
			gpk_necall(imageDeflated.resize(imageDeflated.size() + loadedChunk.Data.size()), "Out of memory? Requested size: %u.", imageDeflated.size() + loadedChunk.Data.size());
			memcpy(&imageDeflated[dataOffset], loadedChunk.Data.begin(), loadedChunk.Data.size());
		}
	}
	::gpk::SPNGIHDR				& imageHeader				= pngData.Header;
	pngData.Inflated.clear();
	gpk_necs(gpk::arrayInflate(pngData.Deflated.cu8(), pngData.Inflated, ::INFLATE_CHUNK_SIZE));
	::pngFilePrintInfo(pngData);

	uint32_t					bytesPerPixel				= ::pngBytesPerPixel(imageHeader.ColorType, imageHeader.BitDepth);
	gpk_necall(bytesPerPixel, "%s", "Invalid format! Color Type: %u. Bit Depth: %u.");
	gpk_necs(imageHeader.MethodInterlace 
		? ::defilterInterlaced   (pngData)
		: ::defilterNonInterlaced(pngData, bytesPerPixel)
		);
	return 0;
}

::gpk::error_t			gpk::pngFileLoad			(::gpk::SPNGData & pngData, const ::gpk::vcs & filename	)	{
	::gpk::au8					fileInMemory				= {};
	gpk_necs(gpk::fileToMemory(filename, fileInMemory));
	return ::gpk::pngFileLoad(pngData, fileInMemory);
}

::gpk::error_t	gpk::pngFileLoad(::gpk::SPNGData & pngData, const ::gpk::vcu8 &   source, ::gpk::au8 & out_Data) { 
	gpk_necs(gpk::pngFileLoad(pngData, source)); 
	return ::gpk::pngDecode(pngData, out_Data); 
}

::gpk::error_t	gpk::pngFileLoad(::gpk::SPNGData & pngData, const ::gpk::vcs  & filename, ::gpk::au8 & out_Data) { 
	::gpk::au8					fileInMemory				= {};
	gpk_necs(gpk::fileToMemory(filename, fileInMemory));
	return ::gpk::pngFileLoad(pngData, fileInMemory.cu8(), out_Data);
}

::gpk::error_t	gpk::pngFileLoad(::gpk::SPNGData & pngData, const ::gpk::vcu8 &   source, ::gpk::au8 & out_Data, ::gpk::g8bgra & out_View) { 
	gpk_necs(gpk::pngFileLoad(pngData, source)); 
	return ::gpk::pngDecode(pngData, out_Data, out_View); 
}

::gpk::error_t	gpk::pngFileLoad(::gpk::SPNGData & pngData, const ::gpk::vcs  & filename, ::gpk::au8 & out_Data, ::gpk::g8bgra & out_View) { 
	::gpk::au8					fileInMemory				= {};
	gpk_necs(gpk::fileToMemory(filename, fileInMemory));
	return ::gpk::pngFileLoad(pngData, fileInMemory.cu8(), out_Data, out_View);
}

::gpk::error_t			gpk::pngFileLoad			(::gpk::SPNGData & pngData, const ::gpk::vcu8 & source, ::gpk::img8bgra & out_Texture) {
	gpk_necs(gpk::pngFileLoad(pngData, source));
	gpk_necs(out_Texture.resize(pngData.Header.Size));
	return ::gpk::pngDecode(pngData, out_Texture.View);
}

::gpk::error_t			gpk::pngFileLoad	(::gpk::SPNGData & pngData, const ::gpk::vcs & filename, ::gpk::img8bgra & out_Texture)	{
	gpk_necall(gpk::pngFileLoad(pngData, filename), "%s", ::gpk::toString(filename).begin());
	gpk_necs(out_Texture.resize(pngData.Header.Size));
	return ::gpk::pngDecode(pngData, out_Texture.View);
}
