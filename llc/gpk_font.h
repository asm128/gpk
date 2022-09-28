#include "gpk_ptr.h"
#include "gpk_image.h"
#include "gpk_base64.h"

#ifndef GPK_FONT_H_2903874982374
#define GPK_FONT_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
#pragma pack(pop)
	struct SRasterFont {
		::gpk::SCoord2<uint8_t>					CharSize;
		::gpk::SImageMonochrome<uint32_t>		Texture;
	};

	struct SRasterFontBase64 {
		::gpk::SCoord2<uint8_t>					CharSize;
		::gpk::vcs								Base64String;
	};

	struct SRasterFontManager {
		::gpk::array_pobj<::gpk::SRasterFont>	Fonts						= {};
	};

	::gpk::error_t				rasterFontDefaults			(::gpk::SRasterFontManager & manager);

	template<typename _sizeChunk>
	::gpk::error_t				rasterFontLoad				(::gpk::SImageMonochrome<_sizeChunk> & image, const ::gpk::SCoord2<uint8_t> & charSize, const ::gpk::vcc decoded)			{
		const uint32_t					imageHeight					= charSize.y * 256;
		gpk_necall(image.resize(charSize.x, imageHeight), "charSize: {%i, %i}", charSize.x, charSize.y);
		memcpy(image.Texels.begin(), decoded.begin(), ::gpk::min(decoded.size(), charSize.x * imageHeight));
		return 0;
	}

	template<typename _sizeChunk>
	::gpk::error_t				rasterFontLoadB64			(::gpk::SImageMonochrome<_sizeChunk> & image, const ::gpk::SCoord2<uint8_t> & charSize, const ::gpk::vcc & base64String)			{
		::gpk::array_pod<byte_t>		decoded;
		gpk_necs(::gpk::base64Decode(base64String, decoded));
		return rasterFontLoad(image, charSize, decoded);
	}
};

#endif // GPK_FONT_H_2903874982374