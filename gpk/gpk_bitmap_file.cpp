// Original file created on: 2002/08/30 19:33 by Andreas Hartl. Visit http://www.runicsoft.com for updates and more information
#include "gpk_bitmap_file.h"
#include "gpk_png.h"
#include "gpk_file.h"

using ::gpk::sc_t;

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


static	::gpk::error_t	LoadBitmapFromBMPFile		(const ::gpk::vcs & szFileName, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra& out_ImageView, const ::gpk::bgra & alphaKey, bool* out_alphaFound)		{
	HBITMAP						phBitmap					= (HBITMAP)LoadImageA(NULL, ::gpk::toString(szFileName).begin(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);		// Use LoadImage() to get the image loaded into a DIBSection
	ree_if(phBitmap == NULL, "Failed to load bitmap file: %s.", ::gpk::toString(szFileName).begin());

	BITMAP						bm							= {};
	GetObject(phBitmap, sizeof(BITMAP), &bm);		// Get the color depth of the DIBSection
	gpk_necall(out_Colors.resize(bm.bmWidth * bm.bmHeight), "Out of memory? Requested size: {x: %i, y: %i}", (int32_t)bm.bmWidth, (int32_t)bm.bmHeight);
	out_ImageView			= {out_Colors.begin(), {(uint32_t)bm.bmWidth, (uint32_t)bm.bmHeight}};
	HDC							hMemDC						= CreateCompatibleDC(NULL);
	HBITMAP						hOldBitmap;
	hOldBitmap				= (HBITMAP)SelectObject(hMemDC, phBitmap);
	for(uint32_t y = 0; y < out_ImageView.metrics().y; ++y)
	for(uint32_t x = 0; x < out_ImageView.metrics().x; ++x) {
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

	::gpk::vcs					pngfn						= {filename};
	::gpk::au8					filebytes;
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

::gpk::error_t			gpk::bmpFileLoad			(const ::gpk::vcs & filename, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)	{ //
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
	if errored(::gpk::bmpFileLoad(source, out_Colors, out_ImageView)) {
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
	uint8_t			Type[2]		;	// Identifier, must be BM
	uint32_t		Size		;	// Size of BMP file
	uint32_t		Reserved	;	// 0
	uint32_t		Offset		;	//
};

// BMP Information Header
struct SHeaderInfoBMP {
	uint32_t		Size		;	// Number of bytes in structure
	::gpk::n2i32	Metrics		;	// Width and Height of Image
	uint16_t		Planes		;	// Always 1
	uint16_t		Bpp			;	// Bits Per Pixel (must be 24 for now)
	uint32_t		Compression	;	// Must be 0 (uncompressed)
	::gpk::n2i32	PPM			;	// Pixels Per Meter
	uint32_t		ClrUsed		;	// 0 for 24 bpp bmps
	uint32_t		ClrImp		;	// 0
	uint32_t		Dunno		;	// 0
};
#pragma pack( pop )

// Currently supporting only 24-bit bitmaps
::gpk::error_t			gpk::bmpFileLoad			(const int8_t * source, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)					{
	SHeaderFileBMP				& fileHeader				= *(SHeaderFileBMP*)*source;
	ree_if(0 != memcmp(fileHeader.Type, "BM", 2), "Invalid magic number for BMP file.");
	SHeaderInfoBMP				& infoHeader				= *(SHeaderInfoBMP*)*(source + sizeof(SHeaderFileBMP));
	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");
	uint32_t					nPixelCount					= infoHeader.Metrics.x * infoHeader.Metrics.y;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data
	out_Colors.resize(nPixelCount);
	uint8_t						* srcBytes					= (uint8_t*)(source + sizeof(SHeaderFileBMP) + sizeof(SHeaderInfoBMP));
	bool						b32Bit						= false;
	uint32_t					colorSize					= 0;
	int32_t						x, y, linearIndexSrc;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
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
				, uint8_t(b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU)
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
	out_ImageView			= {out_Colors.begin(), (uint32_t)infoHeader.Metrics.x, (uint32_t)infoHeader.Metrics.y};
	return 0;
}

// Currently supporting only 24-bit bitmaps
::gpk::error_t			gpk::bmpFileLoad	(FILE * source, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView)					{
	::SHeaderFileBMP			fileHeader			= {};
	::SHeaderInfoBMP			infoHeader			= {};
	ree_if(fread(&fileHeader, 1, sizeof(::SHeaderFileBMP), source) != sizeof(::SHeaderFileBMP), "Failed to read file! File corrupt?");
	ree_if(fread(&infoHeader, 1, sizeof(::SHeaderInfoBMP), source) != sizeof(::SHeaderInfoBMP), "Failed to read file! File corrupt?");
	uint32_t					nPixelCount			= infoHeader.Metrics.x * infoHeader.Metrics.y;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data
	ree_if(infoHeader.Compression != BI_RGB, "Unsupported bmp compression!");
	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		&& infoHeader.Bpp != 1
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");
	uint32_t					pixelSize			= infoHeader.Bpp == 1 ? 1 : infoHeader.Bpp / 8;
	::gpk::au8					srcBytes			= {};
	gpk_necall(srcBytes.resize(nPixelCount * pixelSize), "Out of memory?");
	size_t						readResult			= fread(&srcBytes[0], pixelSize, nPixelCount, source);
	ree_if(readResult != (size_t)nPixelCount, "Failed to read file! File corrupt?");
	gpk_necall(out_Colors.resize(nPixelCount), "Out of memory?");
	bool						b32Bit				= false;
	uint32_t					colorSize			= 0;
	int32_t						x, y, linearIndexSrc;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
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
				, uint8_t(b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU)
				};
		}
		break;
	case 8 :
		for(y = 0; y < infoHeader.Metrics.y; ++y)
		for(x = 0; x < infoHeader.Metrics.x; ++x) {
			linearIndexSrc				= y * infoHeader.Metrics.x + x;
			out_Colors[linearIndexSrc]	=
				::gpk::bgra{ srcBytes[linearIndexSrc]
				, srcBytes[linearIndexSrc]
				, srcBytes[linearIndexSrc]
				, 0xFFU
				};
		}
		break;
	case 1 :
		for(y = 0; y < infoHeader.Metrics.y; ++y)
		for(x = 0; x < infoHeader.Metrics.x; ++x) {
			linearIndexSrc				= y * (infoHeader.Metrics.x / 8) + x / 8;
			int32_t							linearIndexDst		= y *  infoHeader.Metrics.x + x;
			out_Colors[linearIndexDst]	=
				::gpk::bgra{ srcBytes[linearIndexSrc] & (1U << (x % 8))
				, srcBytes[linearIndexSrc] & (1U << (x % 8))
				, srcBytes[linearIndexSrc] & (1U << (x % 8))
				, 0xFFU
				};
		}
		break;
	}
	out_ImageView			= {out_Colors.begin(), (uint32_t)infoHeader.Metrics.x, (uint32_t)infoHeader.Metrics.y};
	return 0;
}

#endif // defined(GPK_WINDOWS)

