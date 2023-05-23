#include "gpk_image.h"
#include "gpk_color.h"

#ifndef GPK_BITMAP_FILE_H_236544263544352344
#define GPK_BITMAP_FILE_H_236544263544352344

namespace gpk
{
#if defined(GPK_WINDOWS)	// These functions exist in order to help with loading some images from a third-party game. Otherwise their use is discouraged in favor of ::gpk::png* functions.
	::gpk::error_t			bmpFileLoad	(const int8_t					* source	, ::gpk::apod<::gpk::bgra>& out_Colors, ::gpk::view2d<::gpk::bgra>& out_ImageView);
	::gpk::error_t			bmpFileLoad	(FILE							* source	, ::gpk::apod<::gpk::bgra>& out_Colors, ::gpk::view2d<::gpk::bgra>& out_ImageView);
	::gpk::error_t			bmpFileLoad	(const ::gpk::vcs	& filename	, ::gpk::apod<::gpk::bgra>& out_Colors, ::gpk::view2d<::gpk::bgra>& out_ImageView);

	stainli	::gpk::error_t	bmpFileLoad	(const int8_t					* source	, ::gpk::img<::gpk::bgra>& out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	stainli	::gpk::error_t	bmpFileLoad	(FILE							* source	, ::gpk::img<::gpk::bgra>& out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	stainli	::gpk::error_t	bmpFileLoad	(const ::gpk::vcs	& filename	, ::gpk::img<::gpk::bgra>& out_Texture)	{ return bmpFileLoad(filename	, out_Texture.Texels, out_Texture.View); }
#endif
}

#endif // GPK_BITMAP_FILE_H_236544263544352344
