#include "s3d_xcb_texture_manager.h"

#include "gpk_framework.h"

#ifndef S3D_XCB_FRAMEWORK_H_213984398
#define S3D_XCB_FRAMEWORK_H_213984398

namespace s3d
{
	struct SXCBFramework {
		::gpk::SRuntimeValues											& RuntimeValues;
		::gpk::SFrameworkDetail											PlatformDetail			= {};
		::gpk::SWindow													MainDisplay				= {};
		::gpk::STimer													Timer					= {};
		::gpk::pobj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	MainDisplayOffscreen	= {};
		::gpk::pobj<::gpk::SInput>										Input					= {};
		::gpk::pobj<::gpk::SGUI>										GUI						= {};
#if !defined(GPK_WINDOWS)
		::s3d::SXCBTextureManager											TextureManager			= {};
#endif
																			SXCBFramework			(const SXCBFramework&)	noexcept = delete;
		SXCBFramework&														operator =				(const SXCBFramework&)	noexcept = delete;

																			SXCBFramework			(::gpk::SRuntimeValues& runtimeValues)	: RuntimeValues(runtimeValues) {
			MainDisplayOffscreen.create();
			Input.create();
			GUI.create();
		}
	};

	::gpk::error_t				mainWindowCreate			(::s3d::SXCBFramework & framework);
	::gpk::error_t				frameworkUpdate				(::s3d::SXCBFramework & framework);

} // namespace

#endif // S3D_XCB_FRAMEWORK_H_213984398
