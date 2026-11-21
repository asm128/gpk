#include "gpk_gui_inputbox.h"
#include "gpk_gui_control_list.h"
#include "gpk_label.h"
#include "gpk_string.h"
#include "gpk_encoding.h"
#include "gpk_gui_keyboard.h"

GPK_USING_TYPEINT();

gpk::err_t 		gpk::virtualKeyboardSetup437(SGUI & gui, SVirtualKeyboard & vk) {
	if_fail_fe(virtualKeyboardSetup(gui, vk, GUI_KEYBOARD_Default_row_width, GUI_KEYBOARD_Default_keys));
	gui.Controls.SetHidden(vk.IdRoot, true);
	return vk.IdRoot;
}

gpk::err_t 		gpk::virtualKeyboardSetup	(SGUI & gui, SVirtualKeyboard & vk, u0_t rowWidth, vcu0_c & keys) {
	vk.Keys					= keys;

	vk.IdRoot				= controlCreate(gui);
	cnst n2u0_t		charSize					= gui.Fonts[gui.SelectedFont]->CharSize;
	cnst n2s1_t		sizeKey						= (charSize + n2u0_t{6, 6}).s1();
	stxp	u1_t			SIZE_BUTTON					= 88;
	cnst n2s1_t		sizeKeypad					= {s1_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), s1_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	{
		SControlPlacement	& controlRoot				= gui.Controls.Placement[vk.IdRoot];
		controlRoot.Area.Size	= sizeKeypad;
		controlRoot.Align		= ALIGN_CENTER_BOTTOM;
	}
	for(u2_t iKey = 0; iKey < vk.Keys.size(); ++iKey) {
		sc_t						text[2]		= {(sc_t)vk.Keys[iKey], 0};

		cid_t				idKey			= controlCreate(gui);
		SControlPlacement	& controlKey	= gui.Controls.Placement[idKey];
		controlKey.Area.Size	= sizeKey;
		controlKey.Area.Offset.x= s1_t((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y= s1_t((iKey / rowWidth) * sizeKey.y);
		controlKey.Border		= {1, 1, 1, 1};
		gui.Controls.Text[idKey].Align	= ALIGN_CENTER;

		if_fail_fe(controlSetParent(gui.Controls, idKey, vk.IdRoot));
		if_fail_fe(controlTextSet(gui.Controls, idKey, label(text)));
	}
	if_fail_fe(guiCreateControlList<VK_SCANCODE>(gui, vk.IdRoot, SIZE_BUTTON, 0, ALIGN_TOP_RIGHT, ALIGN_CENTER, vk.IdKeys));
	return 0;
}

#ifndef GPK_ATMEL
gpk::err_t		gpk::SInputBox::Update	(SGUI & gui, cnst SVirtualKeyboard & virtualKeyboard, view<cnst pobj<SEventSystem>> frameEvents, vcid processableControls) {
	if(false == Editing)
		return 0;

	cid_t						handledControl			= 0;
	apod<SVirtualKeyboardEvent>	vkEvents				= {};
	if_fail_fe(handledControl = (cid_t)controlHandle(gui.Controls, processableControls, [&](cid_t iControl) {
		if(virtualKeyboardHandleEvent(virtualKeyboard, iControl, vkEvents))
			return err_t(handledControl = iControl);
		return 0;
	}));

	b8_t						enterKeyPressed			= false;
	auto handleEdit = [this, &gui, &enterKeyPressed](VK_SCANCODE scanCode) -> err_t {
		switch(scanCode) {
		default: break;
		case VK_SCANCODE_Escape		: Editing = false;									break;
		case VK_SCANCODE_Enter		: enterKeyPressed = true;						break;
		case VK_SCANCODE_Clear		: Text.clear(); Cursor = 0;						break;
		case VK_SCANCODE_Left		: if(Cursor) --Cursor;							break;
		case VK_SCANCODE_Right		: if(Cursor < Text.size()) ++Cursor;				break;
		case VK_SCANCODE_Home		: Cursor = 0;									break;
		case VK_SCANCODE_End		: Cursor = Text.size();						break;
		case VK_SCANCODE_Backspace	:
			if(Cursor) {
				if_fail_fe(Text.remove(Cursor - 1));
				--Cursor;
			}
			break;
		case VK_SCANCODE_Delete:
			if(Cursor < Text.size())
				if_fail_fe(Text.remove(Cursor));
			break;
		}
		return controlTextSet(gui.Controls, IdText, vcst_t{Text});
	};

	for(u2_t iEvent = 0; iEvent < vkEvents.size(); ++iEvent) {
		cnst SVirtualKeyboardEvent & event = vkEvents[iEvent];
		if(VK_EVENT_Release == event.Type) {
			if(Text.size() < MaxLength) {
				if_fail_fe(Text.insert(Cursor, (sc_t)event.ScanCode));
				++Cursor;
				if_fail_fe(controlTextSet(gui.Controls, IdText, vcst_t{Text}));
			}
		}
		else if(VK_EVENT_Edit == event.Type)
			if_fail_fe(handleEdit(event.ScanCode));
	}

	for(u2_t iEvent = 0; iEvent < frameEvents.size(); ++iEvent) {
		cnst pobj<SEventSystem> & systemEvent = frameEvents[iEvent];
		if(0 == systemEvent.get_ref())
			continue;
		if(SYSTEM_EVENT_Text == systemEvent->Type) {
			if_fail_fe(eventExtractAndHandle<EVENT_TEXT>(*systemEvent, [this, &gui](cnst SEventView<EVENT_TEXT> & eventText) {
				if(EVENT_TEXT_Char != eventText.Type || eventText.Data.size() < sizeof(u2_t) || Text.size() >= MaxLength)
					return 0;
				u2_t codePoint = {};
				memcpy(&codePoint, eventText.Data.begin(), sizeof(codePoint));
				u0_t character = {};
				if(0 <= unicodeToCP437(codePoint, character)) {
					if_fail_fe(Text.insert(Cursor, (sc_t)character));
					++Cursor;
					if_fail_fe(controlTextSet(gui.Controls, IdText, vcst_t{Text}));
				}
				return 0;
			}));
		}
		else if(SYSTEM_EVENT_Keyboard == systemEvent->Type) {
			if_fail_fe(eventExtractAndHandle<EVENT_KEYBOARD>(*systemEvent, [&handleEdit](cnst SEventView<EVENT_KEYBOARD> & eventKeyboard) {
				if(EVENT_KEYBOARD_Down != eventKeyboard.Type || 0 == eventKeyboard.Data.size())
	return 0;
				switch(eventKeyboard.Data[0]) {
				default: return 0;
				case SYSTEM_KEY_Escape		: return handleEdit(VK_SCANCODE_Escape);
				case SYSTEM_KEY_Enter		: return handleEdit(VK_SCANCODE_Enter);
				case SYSTEM_KEY_Backspace	: return handleEdit(VK_SCANCODE_Backspace);
				case SYSTEM_KEY_Clear		: return handleEdit(VK_SCANCODE_Clear);
				case SYSTEM_KEY_Delete		: return handleEdit(VK_SCANCODE_Delete);
				case SYSTEM_KEY_Left		: return handleEdit(VK_SCANCODE_Left);
				case SYSTEM_KEY_Right		: return handleEdit(VK_SCANCODE_Right);
				case SYSTEM_KEY_Home		: return handleEdit(VK_SCANCODE_Home);
				case SYSTEM_KEY_End			: return handleEdit(VK_SCANCODE_End);
				}
			}));
		}
	}
	return enterKeyPressed ? INT_MAX : handledControl;
}
#endif // GPK_ATMEL

gpk::err_t 		gpk::inputBoxCreate		(SInputBox & inputBox, SGUI & gui, cid_t iParent) {
	if_fail_fe(inputBox.IdRoot = controlCreate(gui));
	gui.Controls.States		[inputBox.IdRoot].Mask		&= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.Placement	[inputBox.IdRoot].Border	= {};
	gui.Controls.Placement	[inputBox.IdRoot].Margin	= {};
	//gui.Controls.Draw		[inputBox.IdRoot].NoBorder	= true;
	gui.Controls.Draw		[inputBox.IdRoot].NoClient	= true;

	if_fail_fe(inputBox.IdText = controlCreate(gui));
	gui.Controls.Placement	[inputBox.IdText].Border	= {2, 2, 2, 2};
	gui.Controls.Placement	[inputBox.IdText].Margin	= {1, 1, 1, 1};
	gui.Controls.Placement	[inputBox.IdText].Area.Size	= {0, 22};
	gui.Controls.Text		[inputBox.IdText].Align		= ALIGN_CENTER;

	if_fail_fe(virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard));
	gui.Controls.SetHidden(inputBox.VirtualKeyboard.IdRoot, false);
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Align	= gui.Controls.Placement[inputBox.IdText].Align;
	gui.Controls.Placement	[inputBox.VirtualKeyboard.IdRoot].Area.Offset.y
		= gui.Controls.Placement	[inputBox.IdText].Area.Offset.y
		+ gui.Controls.Placement	[inputBox.IdText].Area.Size.y
		;
	gui.Controls.Placement[inputBox.IdText].Area.Size.x	= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x;

	gui.Controls.Placement[inputBox.IdRoot].Area.Size.x	= max(gui.Controls.Placement[inputBox.IdText].Area.Size.x, gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.x);
	gui.Controls.Placement[inputBox.IdRoot].Area.Size.y	+= gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot].Area.Size.y;

	if_fail_fe(controlSetParent(gui.Controls, inputBox.IdText					, inputBox.IdRoot));
	if_fail_fe(controlSetParent(gui.Controls, inputBox.VirtualKeyboard.IdRoot	, inputBox.IdRoot));
	if(iParent >= 0)
		if_fail_fef(controlSetParent(gui.Controls, inputBox.IdRoot, iParent), "iParent: %i", iParent);

	return inputBox.IdRoot;
}
