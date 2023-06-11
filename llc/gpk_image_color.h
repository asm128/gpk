#include "gpk_image.h"
#include "gpk_grid_color.h"

#ifndef GPK_IMAGE_COLOR_H_90238749823611
#define GPK_IMAGE_COLOR_H_90238749823611

namespace gpk
{
	typedef	img<::gpk::bgr  >	img8bgr	;
	typedef	img<::gpk::rgb  >	img8rgb	;
	typedef	img<::gpk::bgra >	img8bgra;
	typedef	img<::gpk::rgba >	img8rgba;
	typedef	img<::gpk::rgbaf>	imgfrgba;

	typedef rt<::gpk::bgra, uint32_t>	rtbgra8d32;
	typedef rt<::gpk::rgba, uint32_t>	rtrgba8d32;
	typedef rt<::gpk::bgra, uint8_t>	rtbgra8s8;
	typedef rt<::gpk::rgba, uint8_t>	rtrgba8s8;
} // namespace

#endif // GPK_IMAGE_COLOR_H_90238749823611
