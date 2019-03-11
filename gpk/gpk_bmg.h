#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_view_grid.h"
#include "gpk_image.h"

#ifndef GPK_BMG_H_9203742983
#define GPK_BMG_H_9203742983

namespace gpk
{
						::gpk::error_t							bmgFileLoad							(const byte_t					* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmgFileLoad							(FILE							* source		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);
						::gpk::error_t							bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::array_pod	<::gpk::SColorBGRA>& out_Colors, ::gpk::view_grid<::gpk::SColorBGRA>& out_ImageView);

	static inline		::gpk::error_t							bmgFileLoad							(const byte_t					* source		, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmgFileLoad							(FILE							* source		, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::gpk::error_t							bmgFileLoad							(const ::gpk::view_const_string	& filename		, ::gpk::SImage	<::gpk::SColorBGRA>& out_Texture)			{ return bmgFileLoad(filename	, out_Texture.Texels, out_Texture.View); }

						::gpk::error_t							bmgFileWrite						(byte_t							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);
						::gpk::error_t							bmgFileWrite						(FILE							* destination	, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);
						::gpk::error_t							bmgFileWrite						(const ::gpk::view_const_string	& filename		, const ::gpk::view_grid<::gpk::SColorBGRA>& in_ImageView);
} // namespace

#endif // GPK_BMG_H_9203742983
