#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog.h"

#include <mutex>

#ifndef APPLICATION_H_2078934982734
#define APPLICATION_H_2078934982734

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
					::gpk::SFramework													Framework;
					::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>	Offscreen						= {};
					::gpk::array_obj<::gpk::SImage<::gpk::SColorBGRA>>					PNGImages						= {};
					
					int32_t																IdExit							= -1;

					::std::mutex														LockGUI;
					::std::mutex														LockRender;

					::gpk::SDialog														DialogMain;
					int32_t																Slider;
					int32_t																NumericTuner;
					int32_t																CheckBox;

																						SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

 
#endif // APPLICATION_H_2078934982734
