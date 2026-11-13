#include "gpk_framework.h"
#include "gpk_gui.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_path.h"

#include <mutex>

#ifndef GPK_FILE_EXPLORER_H_2078934982734
#define GPK_FILE_EXPLORER_H_2078934982734

namespace gfe
{
	struct SApplication {
		::gpk::SFramework				Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen							= {};
		::gpk::SDialog					DialogMain							= {};

		::gpk::cid_t					IdExit								= -1;

		::std::mutex					LockGUI;
		::std::mutex					LockRender;

										SApplication		(::gpk::SRuntimeValues& runtimeValues)	: Framework(runtimeValues)		{}
	};
} // namespace gpk_file_explorer

#endif // GPK_FILE_EXPLORER_H_2078934982734
