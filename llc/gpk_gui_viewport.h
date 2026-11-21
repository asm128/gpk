#include "gpk_gui.h"

#ifndef GPK_GUI_VIEWPORT_H_23627
#define GPK_GUI_VIEWPORT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	enum VIEWPORT_CONTROL : ::gpk::s0_t
		{ VIEWPORT_CONTROL_RESIZE_LEFT				= 0
		, VIEWPORT_CONTROL_RESIZE_TOP
		, VIEWPORT_CONTROL_RESIZE_RIGHT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM
		, VIEWPORT_CONTROL_RESIZE_TOP_LEFT
		, VIEWPORT_CONTROL_RESIZE_TOP_RIGHT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT
		, VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT
		, VIEWPORT_CONTROL_TITLE
		, VIEWPORT_CONTROL_CLOSE
		, VIEWPORT_CONTROL_TARGET
		, VIEWPORT_CONTROL_COUNT
		};

	enum GUI_CONTROL_TYPE : ::gpk::s0_t
		{ GUI_CONTROL_TYPE_Viewport					= 0
		, GUI_CONTROL_TYPE_PaletteGrid
		, GUI_CONTROL_TYPE_ControlList
		, GUI_CONTROL_TYPE_COUNT
		};

	struct SViewportSettings {
		bool Dragging = false;
		bool Unfolded = false;
		bool DisplacementLockX = false;
		bool DisplacementLockY = false;
	};

	struct SViewport {
		cid_t					IdControl			= (::gpk::s2_t)-1;
		::gpk::astcid<::gpk::VIEWPORT_CONTROL_COUNT>	IdControls		= {};
		cid_t					IdDesktopElement	= (::gpk::s2_t)-1;
		GUI_CONTROL_TYPE		ControlType			= (GUI_CONTROL_TYPE)-1;
		SViewportSettings Settings = {false, true, false, false};
		SViewportSettings SettingsOld = {};
	};

	gpk::err_t			viewportInitialize		(::gpk::SGUI & gui, ::gpk::SViewport & viewport);
	::gpk::cid_t		viewportCreate			(::gpk::SGUI & gui, ::gpk::SViewport & viewport);
	gpk::err_t			viewportFold			(::gpk::SGUI & gui, ::gpk::SViewport & viewport, bool fold);
	gpk::err_t			viewportUpdate		(::gpk::SGUI & gui, ::gpk::SInput & input, ::gpk::SViewport & viewport);
#pragma pack(pop)
}

#endif // GPK_GUI_VIEWPORT_H_23627
