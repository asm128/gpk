#include "gpk_array_ptr.h"
#include "gpk_image.h"
#include "gpk_base64.h"

#ifndef GPK_FONT_H_2903874982374
#define GPK_FONT_H_2903874982374

namespace gpk
{
	struct SRasterFont {
		::gpk::n2u8			CharSize;
		::gpk::imgmonou64	Texture;
	};

	struct SRasterFontBase64 {
		::gpk::n2u8			CharSize;
		::gpk::vcc			Base64String;
	};

	struct SRasterFontManager {
		::gpk::apobj<::gpk::SRasterFont>	Fonts						= {};
	};

	::gpk::error_t	rasterFontDefaults	(::gpk::SRasterFontManager & manager);

	template<typename _sizeChunk>
	::gpk::error_t	rasterFontLoad		(::gpk::SImageMonochrome<_sizeChunk> & image, const ::gpk::n2u8 & charSize, const ::gpk::vcu8 decoded)			{
		const uint32_t		imageHeight			= charSize.y * 256;
		gpk_necall(image.resize(charSize.x, imageHeight), "charSize: {%i, %i}", charSize.x, charSize.y);
		memcpy(image.Texels.begin(), decoded.begin(), ::gpk::min(image.Texels.byte_count(), decoded.byte_count()));
		return 0;
	}

	template<typename _sizeChunk>
	::gpk::error_t	rasterFontLoadB64	(::gpk::SImageMonochrome<_sizeChunk> & image, const ::gpk::n2u8 & charSize, const ::gpk::vcu8 & base64String)			{
		::gpk::au8			decoded;
		gpk_necs(::gpk::base64Decode(base64String, decoded));
		return rasterFontLoad(image, charSize, decoded);
	}

	template<typename _sizeChunk>
	::gpk::error_t	rasterFontLoadB64	(::gpk::SImageMonochrome<_sizeChunk> & image, const ::gpk::n2u8 & charSize, const ::gpk::vcc & base64String)			{
		return ::gpk::rasterFontLoadB64(image, charSize, *(const ::gpk::vcu8*)&base64String);
	}
};

#endif // GPK_FONT_H_2903874982374