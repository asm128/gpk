#include "gpk_gui.h"
#include "gpk_virtual_keyboard.h"
#include "gpk_event_input.h"

#ifndef GPK_GUI_INPUTBOX_H_23627
#define GPK_GUI_INPUTBOX_H_23627

nmsp gpk
{
	err_t 		virtualKeyboardSetup	(SGUI & gui, SVirtualKeyboard & vk, u0_t rowWidth, cnst vcu0_t & keys);
	err_t 		virtualKeyboardSetup437	(SGUI & gui, SVirtualKeyboard & vk);

	struct SInputBox {
		cid_t				IdRoot				= -1;
		cid_t				IdText				= -1;
		SVirtualKeyboard	VirtualKeyboard		= {};
		string				Text				= {};
		u2_t				Cursor				= 0;
		u2_t				MaxLength			= 16;
		b8_t				Editing				= false;

		inln	err_t		SetText				(SGUI & gui, vcst_t text) {
			Text				= text;
			Cursor				= Text.size();
			return controlTextSet(gui, IdText, vcst_t{Text});
		}
		err_t 				Edit				(SGUI & gui, b8_t editing) {
			Editing				= editing;
			gui.Controls.SetHidden(IdRoot, false == Editing);
			return one_if(Editing);
		}
#ifndef GPK_ATMEL		
		err_t 				Update				(SGUI & gui, cnst SVirtualKeyboard & virtualKeyboard, view<cnst pobj<SEventSystem>> frameEvents, vcid processableControls);
#endif // GPK_ATMEL
	};

	err_t 		inputBoxCreate		(SInputBox & inputBox, SGUI & gui, cid_t iParent = -1);
} // nmsp

#endif // GPK_GUI_INPUTBOX_H_23627
