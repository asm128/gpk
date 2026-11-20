#include "gpk_apod_color.h"
#include "gpk_img_color.h"
#include "gpk_grid_color.h"

#ifndef GPK_BITMAP_FILE_H_23627
#define GPK_BITMAP_FILE_H_23627

nmsp gpk
{
#if defined(GPK_WINDOWS)	// These functions exist in order to help with loading some images from a third-party game. Otherwise their use is discouraged in favor of png* functions.
			err_t bmpFileLoad	(vcu0_t source, a8bgra & out_Colors, g8bgra & out_ImageView);
			err_t bmpFileLoad	(vcst_t filename, a8bgra & out_Colors, g8bgra & out_ImageView);

	stin	err_t bmpFileLoad	(vcu0_t source  , img8bgra & out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	stin	err_t bmpFileLoad	(vcst_t filename, img8bgra & out_Texture)	{ return bmpFileLoad(filename	, out_Texture.Texels, out_Texture.View); }
#endif
}

#endif // GPK_BITMAP_FILE_H_23627
