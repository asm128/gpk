#include "gpk_enum.h"
#include "gpk_gui_control_flag.h"
#include "gpk_gui_keyboard.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_23627
#define GPK_VIRTUAL_KEYBOARD_H_23627

nmsp gpk
{
	tydf SGUIKeyboardEvent SVirtualKeyboardEvent;

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
