#include "s3d_xcb_texture_manager.h"
#include "gpk_png.h"
#include "gpk_stdstring.h"

#if !defined(GPK_WINDOWS)

//#define STBI_NO_HDR
//#define STBI_NO_LINEAR
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#define NANOSVG_IMPLEMENTATION
//#include "nanosvg.h"
//#define NANOSVGRAST_IMPLEMENTATION
//#include "nanosvgrast.h"

::gpk::error_t					s3d::SXCBTextureManager::Create(xcb_connection_t * c, xcb_drawable_t idDrawable, uint32_t depth, ::gpk::view_grid<const ::gpk::SColorBGRA> pixels) {
	::gpk::ptr_obj<::s3d::SXCBPixmap>	newPixmap		= {};
	::gpk::SImage<::gpk::SColorBGRA>	& loadedImage	= newPixmap->Image;
	gpk_necall(loadedImage.Texels.append({pixels.begin(), pixels.size()}), "%s", "");

	loadedImage.View				= {loadedImage.Texels.begin(), pixels.metrics()};
	xcb_pixmap_t						pixmap			= xcb_generate_id(c);
	xcb_create_pixmap(c, depth, pixmap, idDrawable, loadedImage.View.metrics().x, loadedImage.View.metrics().y);

	//
	xcb_gcontext_t						gc				= xcb_generate_id(c);
	xcb_create_gc(c, gc, pixmap, 0, NULL);

	//
	xcb_image_format_t					format			= XCB_IMAGE_FORMAT_Z_PIXMAP;
	xcb_image_t							* image			= xcb_image_create_native(c, loadedImage.View.metrics().x, loadedImage.View.metrics().y, format, depth, (uint8_t*)loadedImage.View.begin(), loadedImage.View.size() * 4, (uint8_t*)loadedImage.View.begin());
	xcb_image_put(c, pixmap, gc, image, 0, 0, 0);
	image->base						= 0;
	xcb_image_destroy(image);

	//
	newPixmap->XCBConnection		= c;
	newPixmap->XCBGC				= gc;
	newPixmap->XCBPixmap			= pixmap;
	//newPixmap->XCBImage				= image;

	return XCBPixmap.push_back(newPixmap);
}

::gpk::error_t					s3d::SXCBTextureManager::CreateFromFile(xcb_connection_t * c, xcb_drawable_t idDrawable, uint32_t depth, ::gpk::view_const_char filename) {
	::gpk::view_const_char				extension		= {};
	::gpk::array_pod<char_t>			ext_lwr			= {};
	gpk_necall(filename.slice(extension, filename.size() - 3, 3), "File extension not supported for file '%s'", filename.begin());
	gpk_necall(ext_lwr.append(extension), "%s", "");
	::gpk::tolower(ext_lwr);
	static const ::gpk::vcs				str_png			= "png";
	//static const ::gpk::vcs				str_svg			= "svg";

	::gpk::ptr_obj<::s3d::SXCBPixmap>	newPixmap		= {};
	::gpk::SImage<::gpk::SColorBGRA>	& loadedImage	= newPixmap->Image;
	if(ext_lwr == str_png) {
		info_printf("Loading png file: '%s'", filename.begin());
		gpk_necall(::gpk::pngFileLoad(filename, loadedImage), "Failed to load png file: '%s'", filename.begin());
	}
	//else if(ext_lwr == str_svg) {
	//	info_printf("Loading svg file: '%s'", filename.begin());
	//	int									w = 0, h = 0, n = 0;
	//	if (::gpk::SColorBGRA *data = (::gpk::SColorBGRA*)stbi_load((char*)::gpk::toString(filename).begin(), &w, &h, &n, 4)) {
	//		info_printf("Loaded with stb_image: '%s'", filename.begin());
	//		gpk_necall(loadedImage.Texels.append({data, uint32_t(w * h)}), "%s", "");
	//		free(data);
	//	}
	//	else if (NSVGimage * shapes = nsvgParseFromFile(::gpk::toString(filename).begin(), "px", 96.0f)) {
	//		info_printf("Loaded with nanosvg: '%s'", filename.begin());
	//		w							= (int)shapes->width;
	//		h							= (int)shapes->height;
	//		NSVGrasterizer					* rast	= nsvgCreateRasterizer();
	//		loadedImage.Texels.resize(w * h);
	//		nsvgRasterize(rast, shapes, 0, 0, 1, (uint8_t*)loadedImage.Texels.begin(), w, h, w * 4);
	//		nsvgDelete(shapes);
	//		nsvgDeleteRasterizer(rast);
	//	}
	//	else
	//		return -1;
	//	loadedImage.View			= {loadedImage.Texels.begin(), (uint32_t)w, (uint32_t)h};
	//	for(uint32_t i = 0, len = loadedImage.Texels.size(); i < len; ++i) {	//rgba to bgra
	//		const ::gpk::SColorBGRA			texel	= loadedImage.Texels[i];
	//		loadedImage.Texels[i]		= {texel.r, texel.g, texel.b, texel.a};
	//	}
	//}
	else {
		error_printf("Invalid extension: '%s'", ext_lwr.begin());
		return -1;
	}
	xcb_pixmap_t						pixmap		= xcb_generate_id(c);
	xcb_create_pixmap(c, depth, pixmap, idDrawable, loadedImage.metrics().x, loadedImage.metrics().y);

	//
	xcb_gcontext_t						gc			= xcb_generate_id(c);
	xcb_create_gc(c, gc, pixmap, 0, NULL);

	//
	xcb_image_format_t					format		= XCB_IMAGE_FORMAT_Z_PIXMAP;
	xcb_image_t							* image		= xcb_image_create_native(c, loadedImage.metrics().x, loadedImage.metrics().y, format, depth, (uint8_t*)loadedImage.Texels.begin(), loadedImage.Texels.size() * 4, (uint8_t*)loadedImage.Texels.begin());
	xcb_image_put(c, pixmap, gc, image, 0, 0, 0);
	image->base						= 0;
	xcb_image_destroy(image);

	//
	newPixmap->XCBConnection		= c;
	newPixmap->XCBGC				= gc;
	newPixmap->XCBPixmap			= pixmap;
	//newPixmap->XCBImage				= image;
	return XCBPixmap.push_back(newPixmap);
	//return Filename.push_back(filename);
}

#endif
