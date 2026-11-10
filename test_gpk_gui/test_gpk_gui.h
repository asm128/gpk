#include "gpk_framework.h"
#include "gpk_gui_control_list.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627

namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{
	struct SApplication {
		::gpk::SFramework				Framework							;
		::gpk::img8bgra					VerticalAtlas						;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};

		int16_t							Palettes[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};

		::gpk::cid_t					IdExit								= -1;
		::gpk::cid_t					IdMode								= -1;
		::gpk::cid_t					IdTheme								= -1;
		::gpk::cid_t					IdNewPalette						= -1;
		::gpk::SControlList				ActionList							= {};
		::gpk::SPaletteGrid				ColorPicker							= {};
		::gpk::cid_t					PaletteRow							= 10;
		::gpk::cid_t					PaletteColumn						= 16;
		::gpk::mutex					LockGUI;
		::gpk::mutex					LockRender;

						SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_23627
