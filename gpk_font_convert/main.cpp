#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_array.h"
#include "gpk_png.h"
#include "gpk_storage.h"
#include "gpk_base64.h"
#include "gpk_parse.h"
#include "gpk_stdstring.h"

::gpk::error_t											pngToFont
	( ::gpk::SImage<::gpk::bgra>		& imageCache
	, ::gpk::SImage<::gpk::bgra>		& verticalAtlas
	, ::gpk::SImageMonochrome<uint32_t>	& fontTexture
	, ::gpk::vcc						filenameOutput
	, const ::gpk::n2u32				fontCharSize
	, bool								negateBit
	) {
	const uint32_t						ROW_WIDTH					= (imageCache.View.metrics().x == imageCache.View.metrics().y) ? 16 : 32;
	verticalAtlas.resize(::gpk::n2u32{fontCharSize.x, fontCharSize.y * 256});
	for(uint32_t iChar = 0; iChar < 256; ++iChar) {
		const uint32_t												srcOffsetY				= iChar / ROW_WIDTH * fontCharSize.y;
		const uint32_t												dstOffsetY				= iChar * fontCharSize.y;
		for(uint32_t y = 0; y < fontCharSize.y; ++y) {
			for(uint32_t x = 0; x < fontCharSize.x; ++x) {
				const uint32_t											srcOffsetX				= iChar % ROW_WIDTH * fontCharSize.x;
				const uint32_t											dstOffsetX				= 0;
				const ::gpk::SColorBGRA									& srcColor				= imageCache.View[srcOffsetY + y][srcOffsetX + x];
				verticalAtlas.View[dstOffsetY + y][dstOffsetX + x]	= srcColor;
			}
		}
	}
	const ::gpk::n2<uint32_t>								& textureFontMetrics	= verticalAtlas.View.metrics();// app.TextureFont.View.metrics();
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

	::gpk::au8	encoded;
	::gpk::base64Encode({(const uint8_t*)fontTexture.Texels.begin(), fontTexture.Texels.size() * 4}, encoded);
	FILE						* fp = 0;
	const int nul = 0;
	fopen_s(&fp, ::gpk::toString(filenameOutput).begin(), "wb");
	fwrite(encoded.begin(), 1, encoded.size(), fp);
	fwrite(&nul, 1, 1, fp);
	fclose(fp);
	return 0;
}

int							main					() {
	::gpk::aobj<::gpk::ac>			fontFiles				= {};
	::gpk::vcs						pathToSearch			= {"../gpk_data/fonts/cp437"};
	gpk_necall(::gpk::pathList(pathToSearch, fontFiles), "Path not found: '%s'", ::gpk::toString(pathToSearch).begin());
	::gpk::SPNGData						pngCache				= {};
	::gpk::img<::gpk::bgra>				imageCache				= {};
	::gpk::img<::gpk::bgra>				imageFixed				= {};
	::gpk::img<::gpk::bgra>				verticalAtlas			= {};
	::gpk::SImageMonochrome<uint32_t>							fontTexture				= {};
	::gpk::apod<char>									filenameOutput			= {};
	::gpk::vcc													extension				= {};
	for(uint32_t iFile = 0; iFile < fontFiles.size(); ++iFile) {
		::gpk::vcc													filenameInput			= fontFiles[iFile];
		ci_if(errored(filenameInput.slice(extension, filenameInput.size() - 4, 4)), "Skipping '%s'", ::gpk::toString(filenameInput).begin());
		::gpk::apod<char>									lowercaseExtension		= ::gpk::toString(extension);
		::gpk::tolower(lowercaseExtension);
		if(lowercaseExtension != ::gpk::vcs{".png"})
			continue;
		ce_if(errored(::gpk::pngFileLoad(pngCache, filenameInput, imageCache)), "%s", "");
		filenameOutput											= filenameInput;
		filenameOutput.append_string("fix.png");
		::gpk::array_obj<::gpk::vcc>								splitFileName;
		::gpk::split(filenameInput, '_', splitFileName);
		::gpk::array_obj<::gpk::vcc>								splitFontMetrics;
		::gpk::split(splitFileName[3], 'x', splitFontMetrics);
		::gpk::n2<uint32_t>									fontCharSize			= {};
		::gpk::parseIntegerDecimal(splitFontMetrics[0], &fontCharSize.x);
		::gpk::parseIntegerDecimal(splitFontMetrics[1], &fontCharSize.y);
		imageFixed.resize(imageCache.metrics());
		for(uint32_t y = 0; y < imageFixed.metrics().y; ++y)
		for(uint32_t x = 0; x < imageFixed.metrics().x; ++x) {
			const uint32_t					offsetX					= (fontCharSize.x + fontCharSize.x * (x / fontCharSize.x)) % imageCache.metrics().x;
			const uint32_t					dstX					= offsetX + (x % fontCharSize.x);
			::gpk::SColorBGRA				& dstPixel				= imageFixed[y][x];
			dstPixel					= imageCache[y][dstX].r ? ::gpk::BLACK : ::gpk::WHITE;
		}
		::gpk::au8						pngBytes;
		::gpk::pngFileWrite(imageFixed, pngBytes);
		::gpk::fileFromMemory(filenameOutput, pngBytes);

		filenameOutput[filenameOutput.size() - 3]		= 'b';
		filenameOutput[filenameOutput.size() - 2]		= '6';
		filenameOutput[filenameOutput.size() - 1]		= '4';
		::pngToFont(imageFixed, verticalAtlas, fontTexture, filenameOutput, fontCharSize, false);
	}
	return 0;
}

