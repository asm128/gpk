// Original file created on: 2002/08/30 19:33 by Andreas Hartl. Visit http://www.runicsoft.com for updates and more information
#include "gpk_bitmap_file.h"
#include "gpk_png.h"
#include "gpk_file.h"

GPK_USING_TYPEINT ();
GPK_USING_VIEW    ();
GPK_USING_ASTATIC ();
GPK_USING_APOD    ();


#if defined(GPK_WINDOWS)

#include <Windows.h>

// Minuto	Vistas
// 5		11
// 10		22
// 15		31
// 20		41
// 25		48
// 30		51
// 33		56
// 34		59
// 35		65
// 36		75
// 38		75
// 39		81
// 40		90
// 41		95
// 42		98
// 43		106
// 44		110
// 46		124
// 47		128
// 48		138
// 49		143
// 50		151
// 51		155
// 52		158
// 54		162
// 55		169
// 56		178
// 59		183
// 60		189
// 61		194
#pragma pack(push, 1)


static	::gpk::err_t	LoadBitmapFromBMPFile		(const ::gpk::vcst_t & szFileName, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra& out_ImageView, const ::gpk::bgra & alphaKey, bool* out_alphaFound)		{
	HBITMAP						phBitmap					= (HBITMAP)LoadImageA(NULL, gpk::string(szFileName), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);		// Use LoadImage() to get the image loaded into a DIBSection
	if_true_vef(-1, phBitmap == NULL, "Failed to load bitmap file: %.*s.", GPK_VIEW_ARGS(szFileName));

	BITMAP						bm							= {};
	GetObject(phBitmap, sizeof(BITMAP), &bm);		// Get the color depth of the DIBSection
	if_fail_fef(out_Colors.resize(bm.bmWidth * bm.bmHeight), "Out of memory? Requested size: {x: %i, y: %i}", (s2_t)bm.bmWidth, (s2_t)bm.bmHeight);
	out_ImageView			= {out_Colors.begin(), {(u2_t)bm.bmWidth, (u2_t)bm.bmHeight}};
	HDC							hMemDC						= CreateCompatibleDC(NULL);
	HBITMAP						hOldBitmap;
	hOldBitmap				= (HBITMAP)SelectObject(hMemDC, phBitmap);
	for(u2_t y = 0; y < out_ImageView.metrics().y; ++y)
	for(u2_t x = 0; x < out_ImageView.metrics().x; ++x) {
		const COLORREF				colpix						= GetPixel(hMemDC, x, y); // GetPixel(hMemDC, x, out_ImageView.metrics().y - 1 - y);
		const ::gpk::bgra			toWrite						= {GetBValue(colpix), GetGValue(colpix), GetRValue(colpix), 0xFF};
		out_ImageView[y][x]		= toWrite;
		if(toWrite == alphaKey && out_alphaFound)
			*out_alphaFound			= true;
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC	(hMemDC);

	::gpk::apod<sc_t>			filename					= szFileName;
	filename.resize(filename.size() - 3);
	filename.append_string("png");

	::gpk::vcst_t					pngfn						= {filename};
	::gpk::au0_t					filebytes;
	::gpk::pngFileWrite(out_ImageView, filebytes);
	::gpk::fileFromMemory(pngfn, filebytes);
	//if((bm.bmBitsPixel * bm.bmPlanes) <= 8) { // If the DIBSection is 256 color or less, it has a color table
	//	HDC							hMemDC;
	//	HBITMAP						hOldBitmap;
	//	RGBQUAD						rgb[256];
	//	LPLOGPALETTE				pLogPal;
	//	// Create a memory DC and select the DIBSection into it
	//	hMemDC					= CreateCompatibleDC(NULL);
	//	hOldBitmap				= (HBITMAP)SelectObject(hMemDC, *phBitmap);
	//	// Get the DIBSection's color table
	//	GetDIBColorTable(hMemDC, 0, 256, rgb);
	//	// Create a palette from the color tabl
	//	pLogPal					= (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY)));
	//	pLogPal->palVersion				= 0x300;
	//	pLogPal->palNumEntries			= 256;
	//	for(WORD i = 0; i < 256; ++i) {
	//	  pLogPal->palPalEntry[i].peRed																			= rgb[i].rgbRed;
	//	  pLogPal->palPalEntry[i].peGreen																		= rgb[i].rgbGreen;
	//	  pLogPal->palPalEntry[i].peBlue																		= rgb[i].rgbBlue;
	//	  pLogPal->palPalEntry[i].peFlags																		= 0;
	//	}
	//	phPalette				= CreatePalette( pLogPal );
	//	// Clean up
	//	free( pLogPal );
	//	SelectObject(hMemDC, hOldBitmap);
	//	DeleteDC	(hMemDC);
	//	DeleteObject(phPalette);
	//}
	//else { // It has no color table, so use a halftone palette
	//	HDC							hRefDC;
	//	hRefDC					= GetDC( NULL );
	//	phPalette				= CreateHalftonePalette( hRefDC );
	//	ReleaseDC(NULL, hRefDC);
	//}
	return 0;
}

::gpk::err_t			gpk::bmpFileLoad			(const ::gpk::vcst_t & filename, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)	{ //
#if defined(GPK_WINDOWS)
	bool						isAlpha						= false;
	return ::LoadBitmapFromBMPFile(filename, out_Colors, out_ImageView, {0xFF, 0x00, 0xFF, 0xFF}, &isAlpha);
#else
	FILE						* source					= 0;

	::gpk::fopen_s(&source, filename.begin(), "rb");
	if(0 == source) {
		error_printf("Failed to open file: %s. File not found?", filename.begin());
		return -1;
	}
	if(failed(::gpk::bmpFileLoad(source, out_Colors, out_ImageView))) {
		error_printf("Failed to load file: '%s'. File corrupt?", filename.begin());
		fclose(source);
		return -1;
	}
	fclose(source);
	return 0;
#endif
}

// BMP File header
struct SHeaderFileBMP {
	u0_t			Type[2]		;	// Identifier, must be BM
	u2_t		Size		;	// Size of BMP file
	u2_t		Reserved	;	// 0
	u2_t		Offset		;	//
};

// BMP Information Header
struct SHeaderInfoBMP {
	u2_t		Size		;	// Number of bytes in structure
	::gpk::n2s2_t	Metrics		;	// Width and Height of Image
	u1_t		Planes		;	// Always 1
	u1_t		Bpp			;	// Bits Per Pixel (must be 24 for now)
	u2_t		Compression	;	// Must be 0 (uncompressed)
	::gpk::n2s2_t	PPM			;	// Pixels Per Meter
	u2_t		ClrUsed		;	// 0 for 24 bpp bmps
	u2_t		ClrImp		;	// 0
	u2_t		Dunno		;	// 0
};
#pragma pack( pop )

// Currently supporting only 24-bit bitmaps
::gpk::err_t			gpk::bmpFileLoad			(const s0_t * source, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)					{
	SHeaderFileBMP				& fileHeader				= *(SHeaderFileBMP*)*source;
	if_true_vef(-1, 0 != memcmp(fileHeader.Type, "BM", 2), "%.*s", 2, fileHeader.Type);
	SHeaderInfoBMP				& infoHeader				= *(SHeaderInfoBMP*)*(source + sizeof(SHeaderFileBMP));
	if_true_vef(-1, infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported. Bits per pixel: %i", infoHeader.Bpp);
	u2_t					nPixelCount					= infoHeader.Metrics.x * infoHeader.Metrics.y;
	if_true_vef(-1, 0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data
	out_Colors.resize(nPixelCount);
	u0_t						* srcBytes					= (u0_t*)(source + sizeof(SHeaderFileBMP) + sizeof(SHeaderInfoBMP));
	bool						b32Bit						= false;
	u2_t					colorSize					= 0;
	s2_t						x, y, linearIndexSrc;
	switch(infoHeader.Bpp) {
	default:
		error_printf("Unsupported BMP file! Bits per pixel: %i", infoHeader.Bpp);
		return -1;
	case 32:
		b32Bit					= true;
	case 24:
		colorSize				= b32Bit ? 4 : 3;
		for(y = 0; y < infoHeader.Metrics.y; ++y)
		for(x = 0; x < infoHeader.Metrics.x; ++x) {
			linearIndexSrc								= y * infoHeader.Metrics.x * colorSize + (x * colorSize);
			out_Colors[y * infoHeader.Metrics.x + x]	=
				::gpk::bgra{ srcBytes[linearIndexSrc + 0]
				, srcBytes[linearIndexSrc + 1]
				, srcBytes[linearIndexSrc + 2]
				, u0_t(b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU)
				};
		}
		break;
	case 8 :
		for(y = 0; y < infoHeader.Metrics.y; ++y )
		for(x = 0; x < infoHeader.Metrics.x; ++x ) {
			linearIndexSrc				= y * infoHeader.Metrics.x + x;
			out_Colors[linearIndexSrc]	=
				::gpk::bgra{ srcBytes[linearIndexSrc]
				, srcBytes[linearIndexSrc]
				, srcBytes[linearIndexSrc]
				, 0xFFU
				};
		}
		break;
	}
	out_ImageView			= {out_Colors.begin(), (u2_t)infoHeader.Metrics.x, (u2_t)infoHeader.Metrics.y};
	return 0;
}
//
//// Currently supporting only 24-bit bitmaps
//::gpk::err_t			gpk::bmpFileLoad	(FILE * source, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)					{
//	::SHeaderFileBMP			fileHeader			= {};
//	::SHeaderInfoBMP			infoHeader			= {};
//	if_true_vef(-1, sizeof(::SHeaderFileBMP) != fread(&fileHeader, 1, sizeof(::SHeaderFileBMP), source), "Failed to read file! File corrupt?");
//	if_true_vef(-1, sizeof(::SHeaderInfoBMP) != fread(&infoHeader, 1, sizeof(::SHeaderInfoBMP), source), "Failed to read file! File corrupt?");
//	u2_t					nPixelCount			= infoHeader.Metrics.x * infoHeader.Metrics.y;
//	if_true_vef(-1, 0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data
//	if_true_vef(-1, infoHeader.Compression != BI_RGB, "Unsupported bmp compression!");
//	if_true_vef(-1, infoHeader.Bpp != 24
//		&& infoHeader.Bpp != 32
//		&& infoHeader.Bpp != 8
//		&& infoHeader.Bpp != 1
//		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");
//	u2_t					pixelSize			= infoHeader.Bpp == 1 ? 1 : infoHeader.Bpp / 8;
//	::gpk::au0_t				srcBytes			= {};
//	if_fail_fef(srcBytes.resize(nPixelCount * pixelSize), "Out of memory?");
//	size_t						readResult			= fread(&srcBytes[0], pixelSize, nPixelCount, source);
//	if_true_vef(-1, readResult != (size_t)nPixelCount, "Failed to read file! File corrupt?");
//	if_fail_fef(out_Colors.resize(nPixelCount), "Out of memory?");
//	bool						b32Bit				= false;
//	u2_t					colorSize			= 0;
//	s2_t						x, y, linearIndexSrc;
//	switch(infoHeader.Bpp) {
//	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
//	case 32:
//		b32Bit					= true;
//	case 24:
//		colorSize				= b32Bit ? 4 : 3;
//		for(y = 0; y < infoHeader.Metrics.y; ++y)
//		for(x = 0; x < infoHeader.Metrics.x; ++x) {
//			linearIndexSrc								= y * infoHeader.Metrics.x * colorSize + (x * colorSize);
//			out_Colors[y * infoHeader.Metrics.x + x]	=
//				::gpk::bgra{ srcBytes[linearIndexSrc + 0]
//				, srcBytes[linearIndexSrc + 1]
//				, srcBytes[linearIndexSrc + 2]
//				, u0_t(b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU)
//				};
//		}
//		break;
//	case 8 :
//		for(y = 0; y < infoHeader.Metrics.y; ++y)
//		for(x = 0; x < infoHeader.Metrics.x; ++x) {
//			linearIndexSrc				= y * infoHeader.Metrics.x + x;
//			out_Colors[linearIndexSrc]	=
//				::gpk::bgra{ srcBytes[linearIndexSrc]
//				, srcBytes[linearIndexSrc]
//				, srcBytes[linearIndexSrc]
//				, 0xFFU
//				};
//		}
//		break;
//	case 1 :
//		for(y = 0; y < infoHeader.Metrics.y; ++y)
//		for(x = 0; x < infoHeader.Metrics.x; ++x) {
//			linearIndexSrc				= y * (infoHeader.Metrics.x / 8) + x / 8;
//			s2_t							linearIndexDst		= y *  infoHeader.Metrics.x + x;
//			out_Colors[linearIndexDst]	=
//				::gpk::bgra{ srcBytes[linearIndexSrc] & (1U << (x % 8))
//				, srcBytes[linearIndexSrc] & (1U << (x % 8))
//				, srcBytes[linearIndexSrc] & (1U << (x % 8))
//				, 0xFFU
//				};
//		}
//		break;
//	}
//	out_ImageView			= {out_Colors.begin(), (u2_t)infoHeader.Metrics.x, (u2_t)infoHeader.Metrics.y};
//	return 0;
//}

#endif // defined(GPK_WINDOWS)

