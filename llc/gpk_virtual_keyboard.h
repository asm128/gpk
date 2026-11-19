#include "gpk_enum.h"
#include "gpk_gui_control_flag.h"
#include "gpk_gui_keyboard.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_23627
#define GPK_VIRTUAL_KEYBOARD_H_23627

nmsp gpk
{
	tydf SGUIKeyboardEvent SVirtualKeyboardEvent;

	struct SVirtualKeyboard {
		cid_t						IdRoot				= -1;
		acid						IdKeys				= {};
		au0_t						Keys				= {};

		apod<SVirtualKeyboardEvent>	Events				= {};
	};
	err_t 		virtualKeyboardHandleEvent	(cnst SVirtualKeyboard & vk, cid_t iControl, apod<SVirtualKeyboardEvent> & events);
} // nmsp 

#endif // GPK_VIRTUAL_KEYBOARD_H_23627
