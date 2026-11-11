#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"
#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_label.h"

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	GPK_USING_TYPEINT();

	GDEFINE_ENUM_TYPE (UI_PROFILE, u0_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name				, 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score			, 1);

	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen	= {};
		::gpk::aobj<::gpk::img8bgra>	PNGImages	= {};

		::gpk::cid_t					IdExit		= -1;

		::gpk::mutex					LockRender;

		::gpk::SDialog					DialogMain;
		::gpk::acid					ControlsDialogHome	= {};
		::gpk::SInputBox				InputBox			= {};
		::gpk::vcst_t					PlayerName			= "Player 1";
		int32_t							Slider;
		int32_t							NumericTuner;
		int32_t							CheckBox;
		int32_t							Viewport;

										SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace


#endif // APPLICATION_H_23627
