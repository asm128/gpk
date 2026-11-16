#include "gpk_enum.h"
#include "gpk_gui_control_flag.h"
#include "gpk_gui_keyboard.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_23627
#define GPK_VIRTUAL_KEYBOARD_H_23627

nmsp gpk
{
	tydf SGUIKeyboardEvent SVirtualKeyboardEvent ;
	GDEFINE_ENUM_TYPE (VK_EVENT, u0_t);
	GDEFINE_ENUM_VALUE(VK_EVENT, Press		, GK_EVENT_Press   );
	GDEFINE_ENUM_VALUE(VK_EVENT, Release	, GK_EVENT_Release );
	GDEFINE_ENUM_VALUE(VK_EVENT, Hold		, GK_EVENT_Hold    );
	GDEFINE_ENUM_VALUE(VK_EVENT, Edit		, GK_EVENT_Edit    );
	GDEFINE_ENUM_VALUE(VK_EVENT, Arrow		, GK_EVENT_Arrow   );

	GDEFINE_ENUM_TYPE (VK_SCANCODE, u0_t);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Escape		, GK_SCANCODE_Escape    );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Enter		, GK_SCANCODE_Enter     );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Backspace	, GK_SCANCODE_Backspace );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Clear		, GK_SCANCODE_Clear     );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Left		, GK_SCANCODE_Left      );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Right		, GK_SCANCODE_Right     );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Up			, GK_SCANCODE_Up        );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Down		, GK_SCANCODE_Down      );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Front		, GK_SCANCODE_Front     );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Back		, GK_SCANCODE_Back      );
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Delete		, GK_SCANCODE_Delete    );

	struct SVirtualKeyboard {
		cid_t						IdRoot;
		acid						IdKeys;
		au1_t						Keys;

		apod<SVirtualKeyboardEvent>	Events;
	};
	err_t 		virtualKeyboardHandleEvent	(cnst SVirtualKeyboard & vk, cid_t iControl, apod<SVirtualKeyboardEvent> & events);

	//stin	gpk::err_t virtualKeyboardHandleEvent	(SVirtualKeyboard & vk, cid_t iControl) { return virtualKeyboardHandleEvent(vk, iControl, vk.Events); }
} // nmsp 

#endif // GPK_VIRTUAL_KEYBOARD_H_23627
