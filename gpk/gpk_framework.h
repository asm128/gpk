#include "gpk_runtime.h"
#include "gpk_display.h"
#include "gpk_input.h"
#include "gpk_timer.h"
#include "gpk_frameinfo.h"
#include "gpk_texture.h"

#ifndef GPK_FRAMEWORK_H_20987347928
#define GPK_FRAMEWORK_H_20987347928

namespace gpk
{
	struct SRenderTarget {
							::gpk::STexture<::gpk::SColorBGRA>					Color								= {};
							::gpk::STexture<uint32_t>							DepthStencil						= {};

	};
							::gpk::error_t									clearTarget							(::gpk::SRenderTarget& targetToClear);

	struct SFramework {
		typedef				::gpk::STexture<::gpk::SColorBGRA>					TOffscreen;

							::gpk::SRuntimeValues								& RuntimeValues								;
							::gpk::SDisplay										MainDisplay									= {};
							::gpk::SInput										Input										= {};
							::gpk::STimer										Timer										= {};
							::gpk::SFrameInfo									FrameInfo									= {};
							::gpk::SRenderTarget								MainDisplayOffscreen						= {};

																				SFramework									(::gpk::SRuntimeValues& runtimeValues)			noexcept	: RuntimeValues(runtimeValues) {}
	}; // struct

						::gpk::error_t										updateFramework								(::gpk::SFramework& framework);
} // namespace

#endif // GPK_FRAMEWORK_H_20987347928
