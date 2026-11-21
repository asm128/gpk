#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_component_scene.h"

#include <mutex>

#ifndef APPLICATION_H_23627
#define APPLICATION_H_23627


namespace gme // I'm gonna use a different namespace in order to test a few things about the macros.
{

	struct SApplication {
		::gpk::SFramework				Framework;

		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen					= {};
		::gpk::aobj<::gpk::img8bgra>	PNGImages					= {};
		::gpk::SComponentScene			Scene						= {};

		::gpk::cid_t					IdModel						= -1;

		::gpk::cid_t					IdExit						= -1;

		::std::mutex					LockGUI;
		::std::mutex					LockRender;

										SApplication				(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace

#endif // APPLICATION_H_23627
