#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_scene.h"
#include "gpk_ro_rsw.h"
#include "gpk_ro_rsm.h"
#include "gpk_ro_gnd.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734


namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{

	struct SApplication {
					::gpk::SFramework													Framework;
					::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen;

					int32_t																IdExit							= -1;

					::std::mutex														LockRender;

					::gpk::SDialog														DialogMain;
					int32_t																SliderH;
					int32_t																SliderV;
					int32_t																NumericTuner;
					int32_t																CheckBox;
					int32_t																Viewport;
					int32_t																ViewportMinimap;
					int32_t																IdFrameRateUpdate				= -1;
					int32_t																IdFrameRateRender				= -1;
					char																StringFrameRateUpdate	[256]	= {};
					char																StringFrameRateRender	[256]	= {};

					::gpk::SImage<::gpk::SColorBGRA>									TextureMinimap						= {};
					::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					TexturesGND							= {};
					::gpk::SModelPivot	<float>											GridPivot							= {};
					::gpk::SScene														Scene;
					::gpk::SCoord3<float>												LightDirection						= {10, 5, 0};
					::gpk::SGNDFileContents												GNDData								= {};
					::gpk::SRSWFileContents												RSWData								= {};
					::gpk::array_obj<::gpk::SRSMFileContents>							RSMData								= {};
					::gpk::SModelGND													GNDModel							= {};
					::gpk::SRenderCache													RenderCache							= {};

																						SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_2078934982734
