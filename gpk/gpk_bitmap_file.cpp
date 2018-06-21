// Original file created on: 2002/08/30 19:33 by Andreas Hartl. Visit http://www.runicsoft.com for updates and more information
#include "gpk_bitmap_file.h"
#include <Windows.h>

#pragma pack(push, 1)

static				::gpk::error_t																	LoadBitmapFromBMPFile						(const ::gpk::view_const_string& szFileName, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView, const ::gpk::SColorBGRA& alphaKey, bool* out_alphaFound)		{
	HBITMAP																									phBitmap									= (HBITMAP)LoadImageA(NULL, szFileName.begin(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);		// Use LoadImage() to get the image loaded into a DIBSection
	ree_if(phBitmap == NULL, "Failed to load bitmap file: %s.", szFileName.begin());

	BITMAP																									bm											= {};
	GetObject(phBitmap, sizeof(BITMAP), &bm);		// Get the color depth of the DIBSection
	gpk_necall(out_Colors.resize(bm.bmWidth * bm.bmHeight), "Out of memory? Requested size: {x: %i, y: %i}", (int32_t)bm.bmWidth, (int32_t)bm.bmHeight);
	out_ImageView																						= {out_Colors.begin(), {(uint32_t)bm.bmWidth, (uint32_t)bm.bmHeight}};
	HDC																										hMemDC										= CreateCompatibleDC(NULL);
	HBITMAP																									hOldBitmap;
	hOldBitmap																							= (HBITMAP)SelectObject(hMemDC, phBitmap);
	for(uint32_t y = 0; y < out_ImageView.metrics().y; ++y)
	for(uint32_t x = 0; x < out_ImageView.metrics().x; ++x) {
		const COLORREF																							colpix										= GetPixel(hMemDC, x, y); // GetPixel(hMemDC, x, out_ImageView.metrics().y - 1 - y);
		const ::gpk::SColorBGRA																					toWrite										= {GetBValue(colpix), GetGValue(colpix), GetRValue(colpix), 0xFF};
		out_ImageView[y][x]																					= toWrite;
		if(toWrite == alphaKey && out_alphaFound)
			*out_alphaFound																						= true;
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC	(hMemDC);

	const uint32_t																							lenFilename									= (uint32_t)strlen(szFileName.begin());
	::gpk::array_pod<char>																					filename;
	filename.resize(lenFilename + 1);
	memset(&filename[0], 0, filename.size());
	memcpy(&filename[0], &szFileName[0], lenFilename);
	if('p' == filename[lenFilename - 2])
		memcpy(&filename[lenFilename - 4], "bmg", 3);
	else
		memcpy(&filename[lenFilename - 3], "bmg", 3);

	::gpk::bmgFileWrite(::gpk::view_const_string(filename.begin(), lenFilename), out_ImageView);
	//if((bm.bmBitsPixel * bm.bmPlanes) <= 8) { // If the DIBSection is 256 color or less, it has a color table
	//	HDC																										hMemDC;
	//	HBITMAP																									hOldBitmap;
	//	RGBQUAD																									rgb[256];
	//	LPLOGPALETTE																							pLogPal;
	//	// Create a memory DC and select the DIBSection into it
	//	hMemDC																								= CreateCompatibleDC(NULL);
	//	hOldBitmap																							= (HBITMAP)SelectObject(hMemDC, *phBitmap);
	//	// Get the DIBSection's color table
	//	GetDIBColorTable(hMemDC, 0, 256, rgb);
	//	// Create a palette from the color tabl
	//	pLogPal																								= (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY)));
	//	pLogPal->palVersion																					= 0x300;
	//	pLogPal->palNumEntries																				= 256;
	//	for(WORD i = 0; i < 256; ++i) {
	//	  pLogPal->palPalEntry[i].peRed																			= rgb[i].rgbRed;
	//	  pLogPal->palPalEntry[i].peGreen																		= rgb[i].rgbGreen;
	//	  pLogPal->palPalEntry[i].peBlue																		= rgb[i].rgbBlue;
	//	  pLogPal->palPalEntry[i].peFlags																		= 0;
	//	}
	//	phPalette																							= CreatePalette( pLogPal );
	//	// Clean up
	//	free( pLogPal );
	//	SelectObject(hMemDC, hOldBitmap);
	//	DeleteDC	(hMemDC);
	//	DeleteObject(phPalette);
	//}
	//else { // It has no color table, so use a halftone palette
	//	HDC																										hRefDC;
	//	hRefDC																								= GetDC( NULL );
	//	phPalette																							= CreateHalftonePalette( hRefDC );
	//	ReleaseDC(NULL, hRefDC);
	//}
	return 0;
}

					::gpk::error_t																	gpk::bmpFileLoad							(const ::gpk::view_const_string	& filename	, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)	{ // 
#if defined(GPK_WINDOWS)
	bool																									isAlpha										= false;
	return ::LoadBitmapFromBMPFile(filename, out_Colors, out_ImageView, {0xFF, 0x00, 0xFF, 0xFF}, &isAlpha);
#else
	FILE																									* source									= 0; 

	fopen_s(&source, filename.begin(), "rb");
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
					uint8_t																			Type[2]		;	// Identifier, must be BM
					uint32_t																		Size		;	// Size of BMP file
					uint32_t																		Reserved	;	// 0
					uint32_t																		Offset		;	//
};

// BMP Information Header
struct SHeaderInfoBMP {
					uint32_t																		Size		;	// Number of bytes in structure
					::gpk::SCoord2<int32_t>															Metrics		;	// Width and Height of Image
					uint16_t																		Planes		;	// Always 1
					uint16_t																		Bpp			;	// Bits Per Pixel (must be 24 for now)
					uint32_t																		Compression	;	// Must be 0 (uncompressed)
					::gpk::SCoord2<int32_t>															PPM			;	// Pixels Per Meter
					uint32_t																		ClrUsed		;	// 0 for 24 bpp bmps
					uint32_t																		ClrImp		;	// 0
					uint32_t																		Dunno		;	// 0
};
#pragma pack( pop )

// Currently supporting only 24-bit bitmaps
					::gpk::error_t																	gpk::bmpFileLoad							(const byte_t* source, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)					{
	SHeaderFileBMP																							& fileHeader								= *(SHeaderFileBMP*)*source;	
	ree_if(0 != memcmp(fileHeader.Type, "BM", 2), "Invalid magic number for BMP file.");	
	SHeaderInfoBMP																							& infoHeader								= *(SHeaderInfoBMP*)*(source + sizeof(SHeaderFileBMP));
	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");	
	uint32_t																								nPixelCount									= infoHeader.Metrics.x * infoHeader.Metrics.y;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data 
	out_Colors.resize(nPixelCount);
	ubyte_t																									* srcBytes									= (ubyte_t*)(source + sizeof(SHeaderFileBMP) + sizeof(SHeaderInfoBMP));
	bool																									b32Bit										= false;
	uint32_t																								colorSize									= 0;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
	case 32:
		b32Bit																								= true;
	case 24:
		colorSize																							= b32Bit ? 4 : 3;
		for(int32_t y = 0; y < infoHeader.Metrics.y; ++y)
		for(int32_t x = 0; x < infoHeader.Metrics.x; ++x) {
				int32_t																									linearIndexSrc								= y * infoHeader.Metrics.x * colorSize + (x * colorSize);
				out_Colors[y * infoHeader.Metrics.x + x]																= 
					{ srcBytes[linearIndexSrc + 0]
					, srcBytes[linearIndexSrc + 1]
					, srcBytes[linearIndexSrc + 2]
					, b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU
					};
			}
		break;
	case 8 :
		for( int32_t y = 0; y < infoHeader.Metrics.y; ++y )
		for( int32_t x = 0; x < infoHeader.Metrics.x; ++x ) {
				int32_t																									linearIndexSrc								= y * infoHeader.Metrics.x + x;
				out_Colors[linearIndexSrc]																			= 
					{ srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, 0xFFU
					};
			}
		break;
	}
	out_ImageView																						= ::gpk::grid_view<::gpk::SColorBGRA>{out_Colors.begin(), (uint32_t)infoHeader.Metrics.x, (uint32_t)infoHeader.Metrics.y};
	return 0;
}

					::gpk::error_t																	bmpFileLoadPaletted							(FILE* source, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)					{
	source, out_Colors, out_ImageView;
	return 0;
}

// Currently supporting only 24-bit bitmaps
					::gpk::error_t																	gpk::bmpFileLoad							(FILE* source, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)					{
	::SHeaderFileBMP																						fileHeader									= {};	
	::SHeaderInfoBMP																						infoHeader									= {};
	ree_if(fread(&fileHeader, 1, sizeof(::SHeaderFileBMP), source) != sizeof(::SHeaderFileBMP), "Failed to read file! File corrupt?");
	ree_if(fread(&infoHeader, 1, sizeof(::SHeaderInfoBMP), source) != sizeof(::SHeaderInfoBMP), "Failed to read file! File corrupt?");
	uint32_t																								nPixelCount									= infoHeader.Metrics.x * infoHeader.Metrics.y;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Metrics.x, infoHeader.Metrics.y);	// make sure it contains data 
	ree_if(infoHeader.Compression != BI_RGB, "Unsupported bmp compression!");
	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		&& infoHeader.Bpp != 1
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");
	uint32_t																								pixelSize									= infoHeader.Bpp == 1 ? 1 : infoHeader.Bpp / 8;
	::gpk::array_pod<ubyte_t>																				srcBytes									= {};
	gpk_necall(srcBytes.resize(nPixelCount * pixelSize), "Out of memory?");
	size_t																									readResult									= fread(&srcBytes[0], pixelSize, nPixelCount, source);
	ree_if(readResult != (size_t)nPixelCount, "Failed to read file! File corrupt?");
	gpk_necall(out_Colors.resize(nPixelCount), "Out of memory?");
	bool																									b32Bit										= false;
	uint32_t																								colorSize									= 0;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
	case 32:
		b32Bit																								= true;
	case 24:
		colorSize																							= b32Bit ? 4 : 3;
		for(int32_t y = 0; y < infoHeader.Metrics.y; ++y)
		for(int32_t x = 0; x < infoHeader.Metrics.x; ++x) {
				int32_t																									linearIndexSrc								= y * infoHeader.Metrics.x * colorSize + (x * colorSize);
				out_Colors[y * infoHeader.Metrics.x + x]																= 
					{ srcBytes[linearIndexSrc + 0]
					, srcBytes[linearIndexSrc + 1]
					, srcBytes[linearIndexSrc + 2]
					, b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU
					};
			}
		break;
	case 8 :
		for(int32_t y = 0; y < infoHeader.Metrics.y; ++y)
		for(int32_t x = 0; x < infoHeader.Metrics.x; ++x) {
				int32_t																									linearIndexSrc								= y * infoHeader.Metrics.x + x;
				out_Colors[linearIndexSrc]																			= 
					{ srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, 0xFFU
					};
			}
		break;
	case 1 :
		for(int32_t y = 0; y < infoHeader.Metrics.y; ++y)
		for(int32_t x = 0; x < infoHeader.Metrics.x; ++x) {
				int32_t																									linearIndexSrc								= y * (infoHeader.Metrics.x / 8) + x / 8;
				int32_t																									linearIndexDst								= y *  infoHeader.Metrics.x + x;
				out_Colors[linearIndexDst]																			= 
					{ srcBytes[linearIndexSrc] & (1U << (x % 8))
					, srcBytes[linearIndexSrc] & (1U << (x % 8))
					, srcBytes[linearIndexSrc] & (1U << (x % 8))
					, 0xFFU
					};
			}
		break;
	}
	out_ImageView																						= ::gpk::grid_view<::gpk::SColorBGRA>{out_Colors.begin(), (uint32_t)infoHeader.Metrics.x, (uint32_t)infoHeader.Metrics.y};
	return 0;
}

// Currently supporting only 24-bit bitmaps
					::gpk::error_t																	gpk::bmgFileLoad							(FILE							* source		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)	{ 
	ree_if(0 == source, "Invalid function usage: destionation file cannot be NULL.");
	uint32_t																								sizeRead									= (uint32_t)(out_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	uint32_t																								elementSize									= 0;  fread(&elementSize, sizeof(uint32_t					), 1, source);
	::gpk::SCoord2<uint32_t>																				gridMetrics									= {}; fread(&gridMetrics, sizeof(::gpk::SCoord2<uint32_t>	), 1, source);
	gpk_necall(out_Colors.resize(gridMetrics.x * gridMetrics.y), "Out of memory");
	ree_if(out_Colors.size() != fread(out_Colors.begin(), ::gpk::min((size_t)elementSize, sizeof(::gpk::SColorBGRA)), out_Colors.size(), source), "Corrupt file?");
	out_ImageView																						= {out_Colors.begin(), gridMetrics};
	return sizeRead; 
}

					::gpk::error_t																	gpk::bmgFileLoad							(const byte_t					* source		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)	{ 
	ree_if(0 == source, "Invalid function usage: source cannot be NULL.");
	uint32_t																								sizeRead									= (uint32_t)(sizeof(uint32_t) + sizeof(uint32_t));
	uint32_t																								& elementSize								= *(uint32_t*)source;
	::gpk::SCoord2<uint32_t>																				& gridMetrics								= *(::gpk::SCoord2<uint32_t>*)(source + sizeof(uint32_t));
	::gpk::SColorBGRA																						* elementGrid								= (::gpk::SColorBGRA*)(source + sizeof(uint32_t) + sizeof(::gpk::SCoord2<uint32_t>));
	gpk_necall(out_Colors.resize(gridMetrics.x * gridMetrics.y), "Out of memory");
	memcpy(out_Colors.begin(), elementGrid, out_Colors.size() * ::gpk::min((size_t)elementSize, sizeof(::gpk::SColorBGRA)));
	out_ImageView																						= {out_Colors.begin(), gridMetrics};
	return sizeRead + out_Colors.size(); 
}

					::gpk::error_t																	gpk::bmgFileWrite							(byte_t							* destination	, const ::gpk::grid_view<::gpk::SColorBGRA>& in_ImageView)							{ 
	uint32_t																								sizeToWrite									= (uint32_t)(in_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	if(0 == destination)
		return sizeToWrite;	// count + element size + (grid size * element size)
	uint32_t																								& elementSize								= *(uint32_t*)destination;
	::gpk::SCoord2<uint32_t>																				& gridMetrics								= *(::gpk::SCoord2<uint32_t>*)(destination + sizeof(uint32_t));
	::gpk::SColorBGRA																						* elementGrid								= (::gpk::SColorBGRA*)(destination + sizeof(uint32_t) + sizeof(::gpk::SCoord2<uint32_t>));
	elementSize																							= (uint32_t)sizeof(::gpk::SColorBGRA);
	gridMetrics																							= in_ImageView.metrics();
	memcpy(elementGrid, in_ImageView.begin(), in_ImageView.size() * (size_t)elementSize);
	return sizeToWrite; 
}

					::gpk::error_t																	gpk::bmgFileWrite							(FILE							* destination	, const ::gpk::grid_view<::gpk::SColorBGRA>& in_ImageView)												{ 
	ree_if(0 == destination, "Invalid function usage: destionation file cannot be NULL.");
	uint32_t																								sizeToWrite									= (uint32_t)(in_ImageView.size() * sizeof(::gpk::SColorBGRA) + sizeof(uint32_t) + sizeof(uint32_t));
	const uint32_t																							elementSize									= (uint32_t)sizeof(::gpk::SColorBGRA);	ree_if(fwrite(&elementSize, sizeof(uint32_t					), 1, destination) != 1, "No space on disk?");
	const ::gpk::SCoord2<uint32_t>																			& gridMetrics								= in_ImageView.metrics();				ree_if(fwrite(&gridMetrics, sizeof(::gpk::SCoord2<uint32_t> ), 1, destination) != 1, "No space on disk?");
	ree_if(fwrite(in_ImageView.begin(), elementSize, in_ImageView.size(), destination) != in_ImageView.size(), "No space on disk?");
	return sizeToWrite; 
}

					::gpk::error_t																	gpk::bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::array_pod<::gpk::SColorBGRA>& out_Colors, ::gpk::grid_view<::gpk::SColorBGRA>& out_ImageView)	{ 
	FILE																									* source									= 0; 
	fopen_s(&source, filename.begin(), "rb");
	if(0 == source) {
		error_printf("Failed to open file: %s. File not found?", filename.begin());
		return -1;
	}
	if errored(::gpk::bmgFileLoad(source, out_Colors, out_ImageView)) {
		error_printf("Failed to load file: '%s'. File corrupt?", filename.begin());
		fclose(source);
		return -1;
	}
	fclose(source);
	return 0; 
}

					::gpk::error_t																	gpk::bmgFileWrite							(const ::gpk::view_const_string	& filename		, const ::gpk::grid_view<::gpk::SColorBGRA>& in_ImageView)												{ 
	FILE																									* source									= 0; 
	fopen_s(&source, filename.begin(), "wb");
	if(0 == source) {
		error_printf("Failed to open file: %s. Forbidden?", filename.begin());
		return -1;
	}
	if errored(::gpk::bmgFileWrite(source, in_ImageView)) {
		error_printf("Failed to store file: '%s'. No space on disk?", filename.begin());
		fclose(source);
		return -1;
	}
	fclose(source);
	return 0; 
}

					::gpk::error_t																	gpk::bmpOrBmgLoad							(::gpk::view_string bmpFileName, ::gpk::STexture<::gpk::SColorBGRA>& loaded)		{
	::gpk::view_const_string																				bmpFileNameC								= {bmpFileName.begin(), bmpFileName.size()};
	if(errored(::gpk::bmpFileLoad(bmpFileNameC, loaded))) {
		error_printf("Failed to load bitmap from file: %s.", bmpFileNameC.begin());
		if('p' == bmpFileName[bmpFileName.size() - 2])
			bmpFileName[bmpFileName.size() - 2]										= 'g';
		else
			bmpFileName[bmpFileName.size() - 1]										= 'g';
		gpk_necall(::gpk::bmgFileLoad(bmpFileNameC, loaded), "Failed to load bitmap from file: %s.", bmpFileNameC.begin());
	}
	return 0;
}
