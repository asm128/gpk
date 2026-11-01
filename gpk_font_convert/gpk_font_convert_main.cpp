#define GPK_STDOUT_LOG_ENABLED
#define GPK_ERROR_PRINTF_ENABLED
#define GPK_WARNING_PRINTF_ENABLED
#define GPK_INFO_PRINTF_ENABLED

#include "gpk_png.h"
#include "gpk_base64.h"
#include "gpk_parse.h"
#include "gpk_path.h"
#include "gpk_img_color.h"

#include "gpk_stdstring.h"
#include "gpk_file.h"
#include "gpk_string.h"

using namespace gpk;

::gpk::err_t				pngToFont
	( ::gpk::img8bgra			& imageCache
	, ::gpk::img8bgra			& verticalAtlas
	, ::gpk::imgmonou3_t			& fontTexture
	, ::gpk::vcst_t				filenameOutput
	, const ::gpk::n2u2_t		fontCharSize
	, bool						negateBit
	) {
	const uint32_t					ROW_WIDTH					= (imageCache.View.metrics().x == imageCache.View.metrics().y) ? 16 : 32;
	verticalAtlas.resize(::gpk::n2u2_t{fontCharSize.x, fontCharSize.y * 256});
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
	if_fail_fef(fontTexture.resize(textureFontMetrics), "%s", "");
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

	::gpk::au0_t	encoded;
	::gpk::base64Encode({(const uint8_t*)fontTexture.Texels.begin(), fontTexture.Texels.size() * 4}, encoded);
	FILE						* fp = 0;
	const int nul = 0;
	if_fail_fe(gpk::fopen_s(fp, filenameOutput, "wb"));
	fwrite(encoded.begin(), 1, encoded.size(), fp);
	fwrite(&nul, 1, 1, fp);
	fclose(fp);
	return 0;
}

int						main					() {
	::gpk::aobj<::gpk::string>	fontFiles				= {};
	::gpk::vcst_t					pathToSearch			= {"../gpk_data/fonts/cp437"};
	if_fail_fef(gpk::pathList(pathToSearch, fontFiles), "Path not found: '%.*s'", GPK_VIEW_ARGS(pathToSearch));
	::gpk::SPNGData				pngCache				= {};
	::gpk::img8bgra				imageCache				= {};
	::gpk::img8bgra				imageFixed				= {};
	::gpk::img8bgra				verticalAtlas			= {};
	::gpk::imgmonou3_t			fontTexture				= {};
	::gpk::string				filenameOutput			= {};
	::gpk::vcst_t				extension				= {};
	for(uint32_t iFile = 0; iFile < fontFiles.size(); ++iFile) {
		::gpk::vcst_t				filenameInput			= fontFiles[iFile];
		if_fail_cif(filenameInput.slice(extension, filenameInput.size() - 4, 4), "Skipping '%.*s'", GPK_VIEW_ARGS(filenameInput));
		::gpk::string			lowercaseExtension		= extension;
		::gpk::tolower(lowercaseExtension);
		if(lowercaseExtension != ".png")
			continue;
		if_fail_ce(::gpk::pngFileLoad(pngCache, filenameInput, imageCache));
		filenameOutput			= filenameInput;
		filenameOutput.append_string("fix.png");
		::gpk::aobj<::gpk::vcst_t>		splitFileName;
		::gpk::split(filenameInput, '_', splitFileName);
		::gpk::aobj<::gpk::vcst_t>		splitFontMetrics;
		::gpk::split(splitFileName[3], 'x', splitFontMetrics);
		::gpk::n2u2_t				fontCharSize			= {};
		::gpk::parseIntegerDecimal(splitFontMetrics[0], fontCharSize.x);
		::gpk::parseIntegerDecimal(splitFontMetrics[1], fontCharSize.y);
		imageFixed.resize(imageCache.metrics());
		for(uint32_t y = 0; y < imageFixed.metrics().y; ++y)
		for(uint32_t x = 0; x < imageFixed.metrics().x; ++x) {
			const uint32_t				offsetX					= (fontCharSize.x + fontCharSize.x * (x / fontCharSize.x)) % imageCache.metrics().x;
			const uint32_t				dstX					= offsetX + (x % fontCharSize.x);
			::gpk::bgra					& dstPixel				= imageFixed[y][x];
			dstPixel				= imageCache[y][dstX].r ? ::gpk::BLACK : ::gpk::WHITE;
		}
		::gpk::au0_t					pngBytes;
		::gpk::pngFileWrite(imageFixed, pngBytes);
		::gpk::fileFromMemory(filenameOutput, pngBytes);

		filenameOutput[filenameOutput.size() - 3]		= 'b';
		filenameOutput[filenameOutput.size() - 2]		= '6';
		filenameOutput[filenameOutput.size() - 1]		= '4';
		::pngToFont(imageFixed, verticalAtlas, fontTexture, filenameOutput, fontCharSize, false);
	}
	return 0;
}

