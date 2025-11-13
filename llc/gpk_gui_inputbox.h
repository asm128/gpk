#include "gpk_gui.h"
#include "gpk_virtual_keyboard.h"
#include "gpk_system_key.h"
#include "gpk_event_input.h"

#include <limits.h>

#ifndef GPK_GUI_INPUTBOX_H_23627
#define GPK_GUI_INPUTBOX_H_23627

namespace gpk
{
	::gpk::error_t			virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, const ::gpk::vcu1_t & keys);
	::gpk::error_t			virtualKeyboardSetup437	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk);

	struct SInputBox {
		cid_t					IdRoot				= {};
		cid_t					IdText				= {};
		::gpk::SVirtualKeyboard	VirtualKeyboard		= {};
		::gpk::asc_t			Text				= {};
		uint32_t				MaxLength			= 16;
		bool					Editing				= false;


		inline	::gpk::error_t	SetText				(::gpk::SGUI & gui, ::gpk::vcst_t text) { return ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text = text}); }
		::gpk::error_t			Edit				(::gpk::SGUI & gui, bool editing) {
			Editing					= editing;
			gui.Controls.SetHidden(IdRoot, false == Editing);
			return one_if(Editing);
		}
#ifndef GPK_ATMEL		
		// Returns INT_MAX if the (real or virtual) enter key was pressed.
		::gpk::error_t			Update				(::gpk::SGUI & gui, const ::gpk::SVirtualKeyboard & virtualKeyboard, ::gpk::view<const ::gpk::pobj<::gpk::SEventSystem>> frameEvents, ::gpk::vcid processableControls) { 
			if(false == Editing)
				return 0;

			cid_t						handledControl;
			::gpk::apod<::gpk::SVirtualKeyboardEvent>	vkEvents;
			gpk_necs(handledControl = (cid_t)::gpk::guiProcessControls(gui, processableControls, [&](::gpk::cid_t iControl) {
				if(::gpk::virtualKeyboardHandleEvent(virtualKeyboard, iControl, vkEvents))
					return ::gpk::error_t(handledControl = iControl);

				return 0;
			}));

			::gpk::apobj<::gpk::SEventSystem>	sysEvents			= frameEvents;
			for(uint32_t iEvent = 0; iEvent < vkEvents.size(); ++iEvent) {
				switch(vkEvents[iEvent].Type) {
				case ::gpk::VK_EVENT_RELEASE:
					if(Text.size() < MaxLength) {
						Text.push_back((sc_t)vkEvents[iEvent].ScanCode);
						::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
					}
					break;
				case ::gpk::VK_EVENT_EDIT:
						 if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Backspace	) { if_fail_e(::gpk::eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Backspace)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Enter		) { if_fail_e(::gpk::eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Enter	)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Escape	) { if_fail_e(::gpk::eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Escape	)); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Clear		) { if_fail_e(::gpk::eventEnqueueKeyboardDown(sysEvents, gpk::SYSTEM_KEY_Clear	)); } 
				}
			}
			;
			bool						enterKeyPressed		= false;
			sysEvents.for_each([this, &gui, &enterKeyPressed](auto ev) mutable { 
				switch(ev->Type) { 
				case ::gpk::SYSTEM_EVENT_Text: 
					::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT>(*ev, [this, &gui](auto cev) {
						if(Text.size() < MaxLength)
							if(cev.Data[0] >= 0x20 && cev.Data[0] <= 0x7F) {
								Text.push_back(cev.Data[0]);
								::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
							}
						return 0;
					});
					break; 
				case ::gpk::SYSTEM_EVENT_Keyboard: 
					::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD>(*ev, [this, &gui, &enterKeyPressed](auto kev) mutable {
						if(kev.Type == ::gpk::EVENT_KEYBOARD_Down) {
							switch(kev.Data[0]) {
							case ::gpk::SYSTEM_KEY_Escape		: Edit(gui, false);			break;
							case ::gpk::SYSTEM_KEY_Enter		: enterKeyPressed = true;	break; 
							case ::gpk::SYSTEM_KEY_Backspace	: 
								if(Text.size()) {
									gpk_necs(Text.pop_back());
									gpk_necs(gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text})); 
								}
								break;
							case ::gpk::SYSTEM_KEY_Clear		: 
								Text.clear();				
								gpk_necs(gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text})); 
								break;
							}
						}
						return 0; 
					});
					break; 
				}
			});
			return enterKeyPressed ? INT_MAX : handledControl ? handledControl : 0;
		}
#endif // GPK_ATMEL
	};

	::gpk::error_t			inputBoxCreate		(::gpk::SInputBox & inputBox, ::gpk::SGUI & gui, ::gpk::cid_t iParent = -1);
} // namespace

#endif // GPK_GUI_INPUTBOX_H_23627
