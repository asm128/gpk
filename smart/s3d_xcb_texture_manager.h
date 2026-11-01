#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_ptr_obj.h"

#if !defined(GPK_WINDOWS)
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

#ifndef S3D_XCB_TEXTURE_MANAGER_H_029734982
#define S3D_XCB_TEXTURE_MANAGER_H_029734982

namespace s3d
{
	struct SXCBPixmap {
		//xcb_image_t							* XCBImage		= {};
		xcb_connection_t					* XCBConnection	= {};
		xcb_pixmap_t						XCBPixmap		= {};
		xcb_gcontext_t						XCBGC			= {};
		::gpk::SImage<::gpk::SColorBGRA>	Image			= {};

											~SXCBPixmap		()	{
			info_printf("%s", "Destroying Pixmap GC");
			if(XCBGC) {
				xcb_free_gc(XCBConnection, XCBGC);
				info_printf("%s", "Pixmap GC freed");
			}
			if(XCBPixmap) {
				xcb_free_pixmap(XCBConnection, XCBPixmap);
				info_printf("%s", "Pixmap freed");
			}
		//
		//	//if(XCBImage)
		//	//	xcb_image_destroy(XCBImage);
		}
	};

	struct SXCBTextureManager {
		::gpk::aobj<::gpk::pobj<SXCBPixmap>>	XCBPixmap				= {};
		//::gpk::array_obj<::gpk::view_const_char	>		Filename				= {};

		::gpk::error_t									Create(xcb_connection_t * c, xcb_drawable_t idDrawable, uint32_t depth, ::gpk::grid<const ::gpk::SColorBGRA> pixels);
		::gpk::error_t									CreateFromFile(xcb_connection_t * c, xcb_drawable_t idDrawable, uint32_t depth, ::gpk::vcst filename);
	};
} // namespace

#endif // S3D_XCB_TEXTURE_MANAGER_H_029734982
#endif // !WIN32
