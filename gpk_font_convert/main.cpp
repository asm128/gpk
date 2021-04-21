#define GPK_CONSOLE_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_array.h"
#include "gpk_png.h"
#include "gpk_storage.h"
#include "gpk_base64.h"
#include "gpk_parse.h"

::gpk::error_t											pngToFont
	( ::gpk::SImage<::gpk::SColorBGRA>	& imageCache
	, ::gpk::SImage<::gpk::SColorBGRA>	& verticalAtlas
	, ::gpk::SImageMonochrome<uint32_t>	& fontTexture
	, ::gpk::vcc						filenameOutput
	, const ::gpk::SCoord2<uint32_t>	fontCharSize
	, bool								negateBit
	) {
	verticalAtlas.resize(fontCharSize.x, fontCharSize.y * 256);
	for(uint32_t iChar = 0; iChar < 256; ++iChar) {
		const uint32_t												srcOffsetY				= iChar / 32 * fontCharSize.y;
		const uint32_t												dstOffsetY				= iChar * fontCharSize.y;
		for(uint32_t y = 0; y < fontCharSize.y; ++y) {
			for(uint32_t x = 0; x < fontCharSize.x; ++x) {
				const uint32_t											srcOffsetX				= iChar % 32 * fontCharSize.x;
				const uint32_t											dstOffsetX				= 0;
				const ::gpk::SColorBGRA									& srcColor				= imageCache.View[srcOffsetY + y][srcOffsetX + x];
				verticalAtlas.View[dstOffsetY + y][dstOffsetX + x]	= srcColor;
			}
		}
	}
	const ::gpk::SCoord2<uint32_t>								& textureFontMetrics	= verticalAtlas.View.metrics();// app.TextureFont.View.metrics();
	gpk_necall(fontTexture.resize(textureFontMetrics), "%s", "");
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x) {
		const ::gpk::SColorBGRA										& srcColor				= verticalAtlas.View[y][x];//app.TextureFont.View[y][x];
		const uint32_t												linearIndex				= y * textureFontMetrics.x + x;
		fontTexture.View[linearIndex]
			=	0 != srcColor.r
			||	0 != srcColor.g
			||	0 != srcColor.b
			;
		if(negateBit)
			fontTexture.View[linearIndex] = !fontTexture.View[linearIndex];
	}

	::gpk::array_pod<char_t>	encoded;
	::gpk::base64Encode({(const byte_t*)fontTexture.Texels.begin(), fontTexture.Texels.size() * 4}, encoded);
	FILE						* fp = 0;
	const int nul = 0;
	fopen_s(&fp, ::gpk::toString(filenameOutput).begin(), "wb");
	fwrite(encoded.begin(), 1, encoded.size(), fp);
	fwrite(&nul, 1, 1, fp);
	fclose(fp);
	return 0;
}

int														main					() {
	::gpk::array_obj<::gpk::array_pod<char_t>>					fontFiles				= {};
	::gpk::vcs													pathToSearch			= {"../gpk_data/fonts/cp437"};
	gpk_necall(::gpk::pathList(pathToSearch, fontFiles), "Path not found: '%s'", ::gpk::toString(pathToSearch).begin());
	::gpk::SPNGData												pngCache				= {};
	::gpk::SImage<::gpk::SColorBGRA>							imageCache				= {};
	::gpk::SImage<::gpk::SColorBGRA>							imageFixed				= {};
	::gpk::SImage<::gpk::SColorBGRA>							verticalAtlas			= {};
	::gpk::SImageMonochrome<uint32_t>							fontTexture				= {};
	::gpk::array_pod<char_t>									filenameOutput			= {};
	for(uint32_t iFile = 0; iFile < fontFiles.size(); ++iFile) {
		::gpk::vcc													filenameInput			= fontFiles[iFile];
		if(filenameInput.size() <= 4)
			continue;
		filenameOutput											= filenameInput;
		filenameOutput.append_string("fix.png");
		gerror_if(errored(::gpk::pngFileLoad(pngCache, filenameInput, imageCache)), "%s", "")
		else {
			::gpk::array_obj<::gpk::vcc>								splitFileName;
			::gpk::split(filenameInput, '_', splitFileName);
			::gpk::array_obj<::gpk::vcc>								splitFontMetrics;
			::gpk::split(splitFileName[3], 'x', splitFontMetrics);
			::gpk::SCoord2<uint32_t>									fontCharSize			= {};
			::gpk::parseIntegerDecimal(splitFontMetrics[0], &fontCharSize.x);
			::gpk::parseIntegerDecimal(splitFontMetrics[1], &fontCharSize.y);
			imageFixed.resize
				( imageCache.metrics().x / 3
				, imageCache.metrics().y
				);
			for(uint32_t y = 0; y < imageFixed.metrics().y; ++y)
			for(uint32_t x = 0; x < imageFixed.metrics().x; ++x) {
				const uint32_t										offsetX					= fontCharSize.x + fontCharSize.x * 3 * (x / fontCharSize.x);
				const uint32_t										dstX					= offsetX + (x % fontCharSize.x);
				::gpk::SColorBGRA									& dstPixel				= imageFixed[y][x];
				dstPixel										= imageCache[y][dstX].r ? ::gpk::BLACK : ::gpk::WHITE;
			}
			::gpk::array_pod<ubyte_t> pngBytes;
			::gpk::pngFileWrite(imageFixed, pngBytes);
			::gpk::fileFromMemory(filenameOutput, {(const byte_t*)pngBytes.begin(), pngBytes.size()});

			filenameOutput[filenameOutput.size() - 3]		= 'b';
			filenameOutput[filenameOutput.size() - 2]		= '6';
			filenameOutput[filenameOutput.size() - 1]		= '4';
			::pngToFont(imageFixed, verticalAtlas, fontTexture, filenameOutput, fontCharSize, false);
		}
	}
	return 0;
}

