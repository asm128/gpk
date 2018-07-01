#include "gpk_gui.h"
#include "gpk_runtime.h"
#include "gpk_display.h"
#include "gpk_input.h"
#include "gpk_timer.h"
#include "gpk_frameinfo.h"
#include "gpk_texture.h"
#include "gpk_ptr.h"

#ifndef GPK_FRAMEWORK_H_20987347928
#define GPK_FRAMEWORK_H_20987347928

namespace gpk
{

	struct SFramework {
		typedef				::gpk::SColorBGRA											TTexel;
		typedef				::gpk::STexture<TTexel>										TOffscreen;

							::gpk::SRuntimeValues										& RuntimeValues								;
							::gpk::SDisplay												MainDisplay									= {};
							::gpk::ptr_obj<::gpk::SRenderTarget<TTexel, uint32_t>>		MainDisplayOffscreen						= {};
							::gpk::ptr_obj<::gpk::SInput>								Input										= {};
							::gpk::STimer												Timer										= {};
							::gpk::SFrameInfo											FrameInfo									= {};
							::gpk::SGUI													GUI											= {};

																						SFramework									(::gpk::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues)		{}
	}; // struct

						::gpk::error_t												updateFramework								(::gpk::SFramework& framework);
						::gpk::error_t												mainWindowCreate							(::gpk::SDisplay& mainWindow, SRuntimeValuesDetail& runtimeValues, ::gpk::ptr_obj<SInput>& displayInput);
						::gpk::error_t												mainWindowDestroy							(::gpk::SDisplay& mainWindow);
} // namespace

#endif // GPK_FRAMEWORK_H_20987347928
