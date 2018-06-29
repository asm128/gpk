#include "gpk_texture.h"
#include "gpk_color.h"

#ifndef GPK_BITMAP_FILE_H_236544263544352344
#define GPK_BITMAP_FILE_H_236544263544352344

namespace gpk
{

						::gpk::error_t							bmpFileLoad							(const byte_t					* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmpFileLoad							(FILE							* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmpFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);

						::gpk::error_t							bmgFileLoad							(const byte_t					* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmgFileLoad							(FILE							* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);

						::gpk::error_t							bmgFileWrite						(byte_t							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);
						::gpk::error_t							bmgFileWrite						(FILE							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);
						::gpk::error_t							bmgFileWrite						(const ::gpk::view_const_string	& filename		, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);

	static inline		::gpk::error_t							bmpFileLoad							(const byte_t					* source		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmpFileLoad							(FILE							* source		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmpFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmpFileLoad(filename	, out_Texture.Texels, out_Texture.View); }

	static inline		::gpk::error_t							bmgFileLoad							(const byte_t					* source		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmgFileLoad							(FILE							* source		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::STexture	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(filename	, out_Texture.Texels, out_Texture.View); }

						::gpk::error_t							bmpOrBmgLoad						(::gpk::view_string bmpFileName, ::gpk::STexture<::gpk::SColorBGRA>& loaded);

}

#endif // GPK_BITMAP_FILE_H_236544263544352344
