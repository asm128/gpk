#include "gpk_gui.h"
#include "gpk_virtual_keyboard.h"
#include "gpk_system_key.h"

#ifndef GPK_GUI_INPUTBOX_H
#define GPK_GUI_INPUTBOX_H

namespace gpk
{
	::gpk::error_t			virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, const ::gpk::view<const uint16_t> & keys);
	::gpk::error_t			virtualKeyboardSetup437	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk);

	struct SUIInputBox {
		int32_t					IdRoot				= {};
		int32_t					IdText				= {};
		::gpk::SVirtualKeyboard	VirtualKeyboard		= {};
		::gpk::apod<char>		Text				= {};
		uint32_t				MaxLength			= 16;
		bool					Editing				= false;


		inline	::gpk::error_t	SetText				(::gpk::SGUI & gui, ::gpk::vcs text) { return ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text = text}); }
		::gpk::error_t			Edit				(::gpk::SGUI & gui, bool editing) {
			Editing					= editing;
			gui.Controls.States[IdRoot].Hidden	= false == Editing;
			return one_if(Editing);
		}
		::gpk::error_t			Update				(::gpk::SGUI & gui, const ::gpk::SVirtualKeyboard & virtualKeyboard, ::gpk::view<const ::gpk::SSysEvent> frameEvents, ::gpk::vcu32 processableControls) { 
			if(false == Editing)
				return 0;

			int32_t						handledControl		= 0;
			::gpk::apod<::gpk::SVirtualKeyboardEvent>	vkEvents;
			gpk_necs(::gpk::guiProcessControls(gui, processableControls, [&](int32_t iControl) {
				if(::gpk::virtualKeyboardHandleEvent(virtualKeyboard, iControl, vkEvents))
					handledControl = iControl;
				return 0;
			}));

			::gpk::aobj<::gpk::SSysEvent>					sysEvents			= frameEvents;
			for(uint32_t iEvent = 0; iEvent < vkEvents.size(); ++iEvent) {
				switch(vkEvents[iEvent].Type) {
				case ::gpk::VK_EVENT_RELEASE:
					if(Text.size() < MaxLength) {
						Text.push_back((char)vkEvents[iEvent].ScanCode);
						::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
					}
					break;
				case ::gpk::VK_EVENT_EDIT:
						 if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Backspace	) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Backspace)	; sysEvents.push_back(evt); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Enter		) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Enter	)	; sysEvents.push_back(evt); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Escape	) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Escape	)	; sysEvents.push_back(evt); } 
					else if(vkEvents[iEvent].ScanCode == gpk::VK_SCANCODE_Clear		) { ::gpk::SSysEvent evt{::gpk::SYSEVENT_KEY_DOWN,}; evt.Data.push_back(gpk::SYSTEM_KEY_Clear	)	; sysEvents.push_back(evt); } 
				}
			}
			frameEvents				= sysEvents;
			bool						enterKeyPressed		= false;
			for(uint32_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
				switch(frameEvents[iEvent].Type) {
				default						: break;
				case ::gpk::SYSEVENT_CHAR	:
					if(Text.size() < MaxLength)
						if(frameEvents[iEvent].Data[0] >= 0x20 && frameEvents[iEvent].Data[0] <= 0x7F) {
							Text.push_back(frameEvents[iEvent].Data[0]);
							::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text});
						}
					break;
				case ::gpk::SYSEVENT_KEY_DOWN:
					switch(frameEvents[iEvent].Data[0]) {
					case ::gpk::SYSTEM_KEY_Escape		: Edit(gui, false);			break;
					case ::gpk::SYSTEM_KEY_Enter		: enterKeyPressed = true;	break; 
					case ::gpk::SYSTEM_KEY_Backspace	: Text.pop_back()	; ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text}); break;
					case ::gpk::SYSTEM_KEY_Clear		: Text.clear()		; ::gpk::controlTextSet(gui, IdText, ::gpk::vcs{Text}); break;
					}
				}
			}
			return enterKeyPressed ? INT_MAX : handledControl ? handledControl : 0;
		}

	};

	::gpk::error_t			inputBoxCreate		(::gpk::SUIInputBox & inputBox, ::gpk::SGUI & gui, int32_t iParent = -1);
} // namespace

#endif // GPK_GUI_EDITOR_H
