#include "gpk_gui.h"
#include "gpk_virtual_keyboard.h"
#include "gpk_system_key.h"
#include "gpk_event_input.h"

#include <limits.h>

#ifndef GPK_GUI_INPUTBOX_H_23627
#define GPK_GUI_INPUTBOX_H_23627

nmsp gpk
{
	gpk::err_t 		virtualKeyboardSetup	(SGUI & gui, SVirtualKeyboard & vk, u0_t rowWidth, const vcu1_t & keys);
	gpk::err_t 		virtualKeyboardSetup437	(SGUI & gui, SVirtualKeyboard & vk);

	struct SInputBox {
		cid_t					IdRoot				= {};
		cid_t					IdText				= {};
		SVirtualKeyboard		VirtualKeyboard		= {};
		string					Text				= {};
		u2_t					MaxLength			= 16;
		bool					Editing				= false;


		inln	gpk::err_t SetText				(SGUI & gui, vcst_t text) { return controlTextSet(gui, IdText, vcst_t{Text = text}); }
		gpk::err_t 		Edit				(SGUI & gui, bool editing) {
			Editing					= editing;
			gui.Controls.SetHidden(IdRoot, false == Editing);
			return one_if(Editing);
		}
#ifndef GPK_ATMEL		
		// Returns INT_MAX if the (real or virtual) enter key was pressed.
		gpk::err_t 		Update				(SGUI & gui, const SVirtualKeyboard & virtualKeyboard, view<const pobj<SEventSystem>> frameEvents, vcid processableControls) { 
			if(false == Editing)
				return 0;

			cid_t						handledControl;
			apod<SVirtualKeyboardEvent>	vkEvents;
			if_fail_fe(handledControl = (cid_t)guiProcessControls(gui, processableControls, [&](cid_t iControl) {
				if(virtualKeyboardHandleEvent(virtualKeyboard, iControl, vkEvents))
					return err_t(handledControl = iControl);

				return 0;
			}));

			apobj<SEventSystem>	sysEvents			= frameEvents;
			for(u2_t iEvent = 0; iEvent < vkEvents.size(); ++iEvent) {
				switch(vkEvents[iEvent].Type) {
				case VK_EVENT_Release:
					if(Text.size() < MaxLength) {
						Text.push_back((sc_t)vkEvents[iEvent].ScanCode);
						controlTextSet(gui, IdText, vcst_t{Text});
					}
					break;
				case VK_EVENT_Edit:
						 if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Backspace	) { if_fail_e(eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Backspace)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Enter		) { if_fail_e(eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Enter	)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Escape	) { if_fail_e(eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Escape	)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Clear		) { if_fail_e(eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Clear	)); } 
				}
			}
			;
			bool						enterKeyPressed		= false;
			sysEvents.for_each([this, &gui, &enterKeyPressed](auto ev) mutable { 
				switch(ev->Type) { 
				case SYSTEM_EVENT_Text: 
					eventExtractAndHandle<EVENT_TEXT>(*ev, [this, &gui](auto cev) {
						if(Text.size() < MaxLength)
							if(cev.Data[0] >= 0x20 && cev.Data[0] <= 0x7F) {
								Text.push_back(cev.Data[0]);
								controlTextSet(gui, IdText, vcst_t{Text});
							}
						return 0;
					});
					break; 
				case SYSTEM_EVENT_Keyboard: 
					eventExtractAndHandle<EVENT_KEYBOARD>(*ev, [this, &gui, &enterKeyPressed](auto kev) mutable {
						if(kev.Type == EVENT_KEYBOARD_Down) {
							switch(kev.Data[0]) {
							case SYSTEM_KEY_Escape		: Edit(gui, false);			break;
							case SYSTEM_KEY_Enter		: enterKeyPressed = true;	break; 
							case SYSTEM_KEY_Backspace	: 
								if(Text.size()) {
									if_fail_fe(Text.pop_back());
									if_fail_fe(gpk::controlTextSet(gui, IdText, vcst_t{Text})); 
								}
								break;
							case SYSTEM_KEY_Clear		: 
								Text.clear();				
								if_fail_fe(gpk::controlTextSet(gui, IdText, vcst_t{Text})); 
								break;
							}
						}
						return 0; 
					});
					break; 
				}
				return 0;
			});
			return enterKeyPressed ? INT_MAX : handledControl ? handledControl : 0;
		}
#endif // GPK_ATMEL
	};

	gpk::err_t 		inputBoxCreate		(SInputBox & inputBox, SGUI & gui, cid_t iParent = -1);
} // nmsp

#endif // GPK_GUI_INPUTBOX_H_23627
