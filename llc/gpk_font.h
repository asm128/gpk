#include "gpk_array_ptr.h"
#include "gpk_image.h"
#include "gpk_base64.h"

#ifndef GPK_FONT_H_23627
#define GPK_FONT_H_23627

namespace gpk
{
	struct SRasterFont {
		n2u0_t		CharSize;
		imgmonou3_t	Texture;
	};

	struct SRasterFontBase64 {
		n2u0_t		CharSize;
		vcst_t		Base64String;
	};

	struct SRasterFontManager {
		apobj<SRasterFont>	Fonts						= {};
	};

	err_t	rasterFontDefaults	(SRasterFontManager & manager);

	tplt<tpnm _sizeChunk>
	err_t	rasterFontLoad		(imgmono<_sizeChunk> & image, const n2u0_t & charSize, vcu0_c decoded) {
		const u2_t		imageHeight			= charSize.y * 256;
		if_fail_fef(image.resize(charSize.x, imageHeight), "charSize: {%i, %i}", charSize.x, charSize.y);
		memcpy(image.Texels.begin(), decoded.begin(), min(image.Texels.byte_count(), decoded.byte_count()));
		return 0;
	}
	tplt<tpnm _sizeChunk>
	err_t	rasterFontLoadB64	(imgmono<_sizeChunk> & image, const n2u0_t & charSize, vcu0_c base64String) {
		au0_t		decoded;
		if_fail_fe(gpk::base64Decode(base64String, decoded));
		return rasterFontLoad(image, charSize, decoded);
	}
	tplt<tpnm _sizeChunk>
	err_t	rasterFontLoadB64	(imgmono<_sizeChunk> & image, const n2u0_t & charSize, vcst_t base64String) {
		return rasterFontLoadB64(image, charSize, *(vcu0_c*)&base64String);
	}
};

#endif // GPK_FONT_H_2903874982374