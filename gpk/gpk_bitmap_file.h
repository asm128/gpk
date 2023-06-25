#include "gpk_apod_color.h"
#include "gpk_img_color.h"
#include "gpk_grid_color.h"

#ifndef GPK_BITMAP_FILE_H_236544263544352344
#define GPK_BITMAP_FILE_H_236544263544352344

namespace gpk
{
#if defined(GPK_WINDOWS)	// These functions exist in order to help with loading some images from a third-party game. Otherwise their use is discouraged in favor of ::gpk::png* functions.
	::gpk::error_t			bmpFileLoad	(const int8_t		* source	, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView);
	::gpk::error_t			bmpFileLoad	(FILE				* source	, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView);
	::gpk::error_t			bmpFileLoad	(const ::gpk::vcs	& filename	, ::gpk::a8bgra & out_Colors, ::gpk::g8bgra & out_ImageView);

	stainli	::gpk::error_t	bmpFileLoad	(const int8_t		* source	, ::gpk::img8bgra & out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	stainli	::gpk::error_t	bmpFileLoad	(FILE				* source	, ::gpk::img8bgra & out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	stainli	::gpk::error_t	bmpFileLoad	(const ::gpk::vcs	& filename	, ::gpk::img8bgra & out_Texture)	{ return bmpFileLoad(filename	, out_Texture.Texels, out_Texture.View); }
#endif
}

#endif // GPK_BITMAP_FILE_H_236544263544352344
