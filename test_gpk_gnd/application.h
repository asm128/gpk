#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_scene.h"
#include "gpk_ro_rsw.h"
#include "gpk_ro_rsm.h"
#include "gpk_ro_gnd.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627


namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{

	struct SApplication {
		::gpk::SFramework				Framework						;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen						;

		::gpk::cid_t					IdExit							= -1;

		::std::mutex					LockRender						;

		::gpk::SDialog					DialogMain						;
		::gpk::cid_t					SliderH							;
		::gpk::cid_t					SliderV							;
		::gpk::cid_t					NumericTuner					;
		::gpk::cid_t					CheckBox						;
		::gpk::cid_t					Viewport						;
		::gpk::cid_t					ViewportMinimap					;
		::gpk::cid_t					IdFrameRateUpdate				= -1;
		::gpk::cid_t					IdFrameRateRender				= -1;
		char							StringFrameRateUpdate	[256]	= {};
		char							StringFrameRateRender	[256]	= {};

		::gpk::img8bgra							TextureMinimap					= {};
		::gpk::aobj<::gpk::img8bgra>			TexturesGND						= {};
		::gpk::SModelPivot<float>				GridPivot						= {};
		::gpk::SScene							Scene;
		::gpk::n3f2_t							LightDirection					= {10, 5, 0};
		::gpk::SGNDFileContents					GNDData							= {};
		::gpk::SRSWFileContents					RSWData							= {};
		::gpk::aobj<::gpk::SRSMFileContents>	RSMData							= {};
		::gpk::SModelGND						GNDModel						= {};
		::gpk::SRenderCache						RenderCache						= {};

												SApplication					(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_23627
