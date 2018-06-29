#include "gpk_gui.h"

#ifndef GPK_GUI_CONTROL_H_2903749802374
#define GPK_GUI_CONTROL_H_2903749802374

namespace gpk
{
	enum CONTROL_LIST_DIRECTION : int32_t 
		{	CONTROL_LIST_DIRECTION_HORIZONTAL		= 1
		,	CONTROL_LIST_DIRECTION_VERTICAL			= 2
		};

	struct SControlList {
			int32_t																		IdControl									= -1;
			::gpk::CONTROL_LIST_DIRECTION												Orientation									= ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL;
			int32_t																		IndexParentList								= -1;
			int32_t																		IndexParentItem								= -1;
			::gpk::array_pod<int32_t>													IdControls									= {};
			int32_t																		IdSelected									= -1;
			::gpk::array_pod<int32_t>													IdMultiselect								= {};
			::gpk::array_pod<int64_t>													Events										= {};
	};

#pragma pack(push, 1)
		::gpk::error_t																controlListInitialize						(::gpk::SGUI& gui, ::gpk::SControlList& menu);
		::gpk::error_t																controlListArrange							(::gpk::SGUI& gui, ::gpk::SControlList& menu);
		::gpk::error_t																controlListPush								(::gpk::SGUI& gui, ::gpk::SControlList& menu, const ::gpk::view_const_string& text, int64_t eventCode = -1);

	enum VIEWPORT_CONTROL : int8_t
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

	enum GUI_CONTROL_TYPE : int8_t
		{ GUI_CONTROL_TYPE_Viewport					= 0
		, GUI_CONTROL_TYPE_PaletteGrid
		, GUI_CONTROL_TYPE_ControlList
		, GUI_CONTROL_TYPE_COUNT
		};

	struct SViewport {
				int32_t																	IdControl									= -1;
				::gpk::array_static<int32_t, ::gpk::VIEWPORT_CONTROL_COUNT>				IdControls									= {};
				int32_t																	IdDesktopElement							= -1;
				GUI_CONTROL_TYPE														ControlType									= (GUI_CONTROL_TYPE)-1;
	};

			::gpk::error_t															viewportInitialize							(::gpk::SGUI& gui, ::gpk::SViewport& viewport);

#pragma pack(pop)

	struct SPaletteGrid {
				int32_t																	IdControl									= -1;
				::gpk::array_pod<int32_t>												IdControls									= {};
				::gpk::view_grid<::gpk::SColorBGRA>										Colors										= {};
	};

			::gpk::error_t															paletteGridInitialize						(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette);
			::gpk::error_t															paletteGridColorsSet						(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette, const ::gpk::view_grid<::gpk::SColorBGRA>& colors);

}

#endif