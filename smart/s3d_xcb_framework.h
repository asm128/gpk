#include "s3d_xcb_texture_manager.h"

#include "gpk_framework.h"

#ifndef S3D_XCB_FRAMEWORK_H_213984398
#define S3D_XCB_FRAMEWORK_H_213984398

namespace s3d
{
	usns gpk;
	struct SXCBFramework {
		SRuntimeValues				& RuntimeValues;
		SFrameworkDetail			PlatformDetail			= {};
		SWindow						MainDisplay				= {};
		STimer						Timer					= {};
		pobj<rt<SColorBGRA, u2_t>>	MainDisplayOffscreen	= {};
		pobj<SInput>				Input					= {};
		pobj<SGUI>					GUI						= {};
#ifndef GPK_WINDOWS
		SXCBTextureManager			TextureManager			= {};
#endif
									SXCBFramework			(const SXCBFramework&)	noexcept = delete;
		SXCBFramework&				operator =				(const SXCBFramework&)	noexcept = delete;

									SXCBFramework			(::gpk::SRuntimeValues& runtimeValues)	: RuntimeValues(runtimeValues) {
			MainDisplayOffscreen.create();
			Input.create();
			GUI.create();
		}
	};

	::gpk::err_t				mainWindowCreate			(::s3d::SXCBFramework & framework);
	::gpk::err_t				frameworkUpdate				(::s3d::SXCBFramework & framework);

} // namespace

#endif // S3D_XCB_FRAMEWORK_H_213984398
