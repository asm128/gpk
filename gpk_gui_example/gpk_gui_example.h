#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog.h"
#include "gpk_camera.h"
#include "gpk_matrix.h"
#include "gpk_scene.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	GDEFINE_ENUM_TYPE (GUI_EXAMPLE_ACTION, ::gpk::u0_t);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_ACTION, Exit         , 0);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_ACTION, Edit_label   , 1);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_ACTION, Fold_viewport, 2);

	GDEFINE_ENUM_TYPE (GUI_EXAMPLE_KLIB_ACTION, ::gpk::u0_t);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_KLIB_ACTION, Production, 0);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_KLIB_ACTION, Research  , 1);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_KLIB_ACTION, Upgrade   , 2);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_KLIB_ACTION, Exit      , 3);

	GDEFINE_ENUM_TYPE (GUI_EXAMPLE_CONTROL, ::gpk::u0_t);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Frame_rate_update, 0);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Frame_rate_render, 1);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Dryer_panel      , 2);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Program_label    , 3);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Program_value    , 4);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Status_label     , 5);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Status_value     , 6);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Klib_panel       , 7);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, Klib_selection   , 8);
	GDEFINE_ENUM_VALUE(GUI_EXAMPLE_CONTROL, COUNT            , 9);

	struct SVertexCache {
		::gpk::apod<::gpk::tri3<float>>	Triangle3dTransformed			= {};
		::gpk::apod<::gpk::bgra>		Triangle3dColorList				= {};
	};

	struct SCamera {
		::gpk::n3f2_t					Position;
		::gpk::n3f2_t					Target;
		::gpk::minmaxf2_t				NearFar							= {0.01f , 1000.0f};
	};

	struct SViewportScene {
		stacxpr	const ::gpk::n3f2_t		CameraUp						= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
		::gme::SCamera					Camera							= {{10, 5, 0}, {}};

		::gpk::n3f2_t					LightPos						= {10, 5, 0};

		::gpk::m4<float>				Projection						= {};
		::gpk::m4<float>				ViewMatrix						= {};
	};

	struct SUIRadialGauge {
		::gpk::apod<::gpk::n3f2_t>		Vertices				= {};
		::gpk::apod<::gpk::trii16>		Indices					= {};
		int16_t							CurrentValue			= 0;
		int16_t							MaxValue				= 64;
		::gpk::img<::gpk::bgra>			Image					= {};

		stacxpr	::gpk::rgbaf			COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		stacxpr	::gpk::rgbaf			COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		stacxpr	::gpk::rgbaf			COLOR_MAX_DEFAULT		= ::gpk::RED;
		stacxpr	::gpk::rgbaf			COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;

		inline	int16_t				SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t				SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

	struct SApplication {
		::std::mutex					LockRender						;

		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen						= {};
		::gpk::aobj<::gpk::img8bgra>	PNGImages						= {};

		::gpk::acid						IdActions						= {};
		::gpk::acid						IdControls						= {};
		::gpk::acid						IdKlibActions					= {};
		::gpk::SInputBox				InputBox						= {};

		::std::mutex					LockViewport					;
		::gpk::SDialog					DialogMain						= {};
		::gpk::cid_t					Slider							= -1;
		::gpk::cid_t					NumericTuner					= -1;
		::gpk::cid_t					CheckBox						= -1;
		::gpk::cid_t					Viewport						= -1;
		::gpk::cid_t					GaugeControl					= -1;
		SUIRadialGauge					Gauge							= {};
		//::gpk::tri3<float>			CubePositions	[12]			= {};
		::gpk::SModelGeometryIndexed<float, uint8_t>	ModelGeometry;

		::gpk::pobj<::gpk::rtbgra8d32>	Buffer3D;
		SVertexCache					VertexCache;

		char							StringFrameRateUpdate	[256]	= {};
		char							StringFrameRateRender	[256]	= {};

		SViewportScene					Scene;

										SApplication					(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_23627
