#include "gpk_array_ptr.h"
#include "gpk_image.h"
#include "gpk_base64.h"

#ifndef GPK_FONT_H_23627
#define GPK_FONT_H_23627

namespace gpk
{
	struct SRasterFont {
		::gpk::n2u0_t		CharSize;
		::gpk::imgmonou3_t	Texture;
	};

	struct SRasterFontBase64 {
		::gpk::n2u0_t		CharSize;
		::gpk::vcst_t		Base64String;
	};

	struct SRasterFontManager {
		::gpk::apobj<::gpk::SRasterFont>	Fonts						= {};
	};

	::gpk::err_t	rasterFontDefaults	(::gpk::SRasterFontManager & manager);

	tplt<tpnm TCell>
	::gpk::err_t	rasterFontLoad		(::gpk::imgmono<TCell> & image, ::gpk::n2u0_c & charSize, ::gpk::vcu0_c decoded) {
		cnst ::gpk::u2_t	imageHeight	= charSize.y * 256U;
		if_fail_fef(image.resize(charSize.x, imageHeight), "Character size: {%u, %u}.", charSize.x, charSize.y);
		memcpy(image.Texels.begin(), decoded.begin(), ::gpk::min(image.Texels.byte_count(), decoded.byte_count()));
		rtrn 0;
	}
	tplt<tpnm TCell>
	::gpk::err_t	rasterFontLoadB64	(::gpk::imgmono<TCell> & image, ::gpk::n2u0_c & charSize, ::gpk::vcu0_c & base64String) {
		::gpk::au0_t		decoded;
		if_fail_fe(::gpk::base64Decode(base64String, decoded));
		rtrn rasterFontLoad(image, charSize, decoded);
	}
	tplt<tpnm TCell>
	::gpk::err_t	rasterFontLoadB64	(::gpk::imgmono<TCell> & image, ::gpk::n2u0_c & charSize, ::gpk::vcst_t base64String) {
		::gpk::au0_t		decoded;
		if_fail_fe(::gpk::base64Decode(base64String, decoded));
		rtrn rasterFontLoad(image, charSize, decoded);
	}
};

#endif // GPK_FONT_H_2903874982374


