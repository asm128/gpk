#include "gpk_framework.h"
#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_inputbox.h"
#include "gpk_gui_control_list.h"
#include "blitter.h"

#include <mutex>

#ifndef BLITTER_DASHBOARD_H_23627
#define BLITTER_DASHBOARD_H_23627

namespace blt
{
	GPK_USING_TYPEINT();

	enum DASHBOARD_COMMAND : s2_t {
		DASHBOARD_COMMAND_Load,
		DASHBOARD_COMMAND_Refresh,
		DASHBOARD_COMMAND_Flush,
		DASHBOARD_COMMAND_Exit,
		DASHBOARD_COMMAND_Count,
	};

	struct SBlitterDashboard {
		::gpk::mutex					LockRender			= {};
		::gpk::SFramework			Framework;
		::gpk::pobj<::gpk::rtbgra8d32>	Offscreen			= {};

		::gpk::SDialog				DialogMain			= {};
		::gpk::SInputBox			ConfigInput			= {};
		::gpk::SControlList			Commands				= {};
		::gpk::SControlList			DatabaseRows			= {};
		::gpk::cid_t				IdTitle					= ::gpk::CID_INVALID;
		::gpk::cid_t				IdStatus				= ::gpk::CID_INVALID;
		::gpk::cid_t				Viewport				= ::gpk::CID_INVALID;

		::gpk::string				StatusText			= {};
		::gpk::aobj<::gpk::string>	DatabaseRowText		= {};
		::blt::SBlitter				Blitter				= {};

									SBlitterDashboard		(::gpk::SRuntimeValues & runtimeValues) : Framework(runtimeValues) {}
	};
}

#endif // BLITTER_DASHBOARD_H_23627
