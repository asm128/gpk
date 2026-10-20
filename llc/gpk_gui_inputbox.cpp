#include "gpk_gui_inputbox.h"

#include "gpk_label.h"
#include "gpk_view_serialize.h"

#include <limits.h>

stacxpr	::gpk::n2u1_t		VIRTUAL_KEYBOARD_KEY_SIZE_437	= {14, 22};
stacxpr	::gpk::n2u1_t		VIRTUAL_KEYBOARD_COMMAND_SIZE	= {88, 20};
stacxpr	::gpk::SYSTEM_KEY	VIRTUAL_KEYBOARD_COMMANDS	[]	=
	{ ::gpk::SYSTEM_KEY_Escape
	, ::gpk::SYSTEM_KEY_Enter
	, ::gpk::SYSTEM_KEY_Backspace
	, ::gpk::SYSTEM_KEY_Clear
	, ::gpk::SYSTEM_KEY_Left
	, ::gpk::SYSTEM_KEY_Right
	, ::gpk::SYSTEM_KEY_Up
	, ::gpk::SYSTEM_KEY_Down
	};

sttc ::gpk::err_t virtualKeyboardSetupFail(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & keyboard, ::gpk::err_t error) {
	if(keyboard.IdRoot != ::gpk::CID_INVALID)
		::gpk::controlDelete(gui, keyboard.IdRoot);
	keyboard.IdRoot = ::gpk::CID_INVALID;
	keyboard.Characters.clear();
	keyboard.Commands.clear();
	rtrn error;
}

::gpk::err_t gpk::virtualKeyboardSetup437(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & keyboard) {
	::gpk::au1_t keys = {};
	for(::gpk::u1_t iCharacter = 1; iCharacter < 255; ++iCharacter) {
		if_fail_fe(keys.push_back((::gpk::u0_t)iCharacter));
	}
	if_fail_fe(keys.push_back('!'));
	if_fail_fe(keys.push_back('?'));
	if_fail_fe(::gpk::virtualKeyboardSetup(gui, keyboard, 32, ::VIRTUAL_KEYBOARD_KEY_SIZE_437, keys));
	gui.Controls.SetHidden(keyboard.IdRoot, true);
	rtrn keyboard.IdRoot;
}

::gpk::err_t gpk::virtualKeyboardSetup(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & keyboard, ::gpk::u0_t rowWidth, ::gpk::n2u1_c & keySize, ::gpk::vcu1_c & keys) {
	if_true_fef(keyboard.IdRoot != ::gpk::CID_INVALID, "Virtual keyboard root already assigned:%i.", keyboard.IdRoot);
	if_zero_fef(rowWidth, "%s", "Virtual keyboard row width cannot be zero.");
	if_true_fef(0 == keySize.x || 0 == keySize.y, "Virtual keyboard key size cannot be zero:{%u, %u}.", keySize.x, keySize.y);

	cnst ::gpk::u2_t rowCount = keys.size() / rowWidth + one_if(keys.size() % rowWidth);
	cnst ::gpk::u3_t width = (::gpk::u3_t)keySize.x * rowWidth + 4U + ::VIRTUAL_KEYBOARD_COMMAND_SIZE.x;
	cnst ::gpk::u3_t heightCharacters = (::gpk::u3_t)keySize.y * rowCount + 4U;
	cnst ::gpk::u3_t heightCommands = (::gpk::u3_t)::gpk::size(::VIRTUAL_KEYBOARD_COMMANDS) * ::VIRTUAL_KEYBOARD_COMMAND_SIZE.y + 4U;
	cnst ::gpk::u3_t height = ::gpk::max(heightCharacters, heightCommands);
	if_true_fef(width > INT16_MAX || height > INT16_MAX, "Virtual keyboard size exceeds signed 16-bit control geometry:{%" GPK_FMT_U3 ", %" GPK_FMT_U3 "}.", width, height);
	if_fail_fe(keyboard.Characters.resize(keys.size()));
	if_fail_fe(keyboard.Commands.resize(::gpk::size(::VIRTUAL_KEYBOARD_COMMANDS)));

	keyboard.IdRoot = ::gpk::controlCreate(gui);
	if_true_fef(keyboard.IdRoot < 0, "Failed to create virtual keyboard root:%i.", keyboard.IdRoot);
	::gpk::SControlPlacement & rootPlacement = gui.Controls.Placement[keyboard.IdRoot];
	rootPlacement.Area.Size = {(::gpk::s1_t)width, (::gpk::s1_t)height};
	rootPlacement.Align = ::gpk::ALIGN_CENTER_BOTTOM;

	for(::gpk::u2_t iCharacter = 0; iCharacter < keys.size(); ++iCharacter) {
		cnst ::gpk::cid_t idControl = ::gpk::controlCreateChild(gui, keyboard.IdRoot);
		if(idControl < 0)
			rtrn ::virtualKeyboardSetupFail(gui, keyboard, idControl);
		::gpk::SControlPlacement & placement = gui.Controls.Placement[idControl];
		placement.Area.Size = keySize.s1();
		placement.Area.Offset.x = (::gpk::s1_t)((iCharacter % rowWidth) * keySize.x);
		placement.Area.Offset.y = (::gpk::s1_t)((iCharacter / rowWidth) * keySize.y);
		placement.Border = {1, 1, 1, 1};
		gui.Controls.Text[idControl].Align = ::gpk::ALIGN_CENTER;
		cnst ::gpk::sc_t characterText[] = {(::gpk::sc_t)keys[iCharacter], 0};
		cnst ::gpk::err_t textResult = ::gpk::controlTextSet(gui, idControl, ::gpk::label(characterText));
		if(::gpk::failed(textResult))
			rtrn ::virtualKeyboardSetupFail(gui, keyboard, textResult);
		keyboard.Characters[iCharacter] = {idControl, keys[iCharacter]};
	}

	for(::gpk::u2_t iCommand = 0; iCommand < ::gpk::size(::VIRTUAL_KEYBOARD_COMMANDS); ++iCommand) {
		cnst ::gpk::cid_t idControl = ::gpk::controlCreateChild(gui, keyboard.IdRoot);
		if(idControl < 0)
			rtrn ::virtualKeyboardSetupFail(gui, keyboard, idControl);
		::gpk::SControlPlacement & placement = gui.Controls.Placement[idControl];
		placement.Area.Size = ::VIRTUAL_KEYBOARD_COMMAND_SIZE.s1();
		placement.Area.Offset.y = (::gpk::s1_t)(iCommand * ::VIRTUAL_KEYBOARD_COMMAND_SIZE.y);
		placement.Align = ::gpk::ALIGN_TOP_RIGHT;
		placement.Border = {1, 1, 1, 1};
		gui.Controls.Text[idControl].Align = ::gpk::ALIGN_CENTER;
		cnst ::gpk::err_t textResult = ::gpk::controlTextSet(gui, idControl, ::gpk::get_value_label(::VIRTUAL_KEYBOARD_COMMANDS[iCommand]));
		if(::gpk::failed(textResult))
			rtrn ::virtualKeyboardSetupFail(gui, keyboard, textResult);
		keyboard.Commands[iCommand] = {idControl, ::VIRTUAL_KEYBOARD_COMMANDS[iCommand]};
	}
	rtrn keyboard.IdRoot;
}

#ifndef GPK_ATMEL
::gpk::err_t gpk::SInputBox::Update(::gpk::SGUI & gui, cnst ::gpk::SVirtualKeyboard & virtualKeyboard, ::gpk::view<cnst ::gpk::pobj<::gpk::SEventSystem>> frameEvents, ::gpk::vcid processableControls) {
	if(false == Editing)
		rtrn 0;

	::gpk::TQueueSystemEvent systemEvents = frameEvents;
	::gpk::cid_t handledControl = ::gpk::CID_INVALID;
	cnst ::gpk::err_t processResult = ::gpk::guiProcessControls(gui, processableControls, [&virtualKeyboard, &systemEvents, &handledControl](::gpk::cid_t iControl) {
		cnst ::gpk::err_t keyboardResult = ::gpk::virtualKeyboardHandleEvent(virtualKeyboard, iControl, systemEvents);
		if(::gpk::failed(keyboardResult))
			rtrn keyboardResult;
		if(keyboardResult)
			handledControl = iControl;
		rtrn keyboardResult;
	});
	if_fail_fe(processResult);

	bool enterKeyPressed = false;
	for(::gpk::u2_t iEvent = 0; iEvent < systemEvents.size(); ++iEvent) {
		if_zero_fef(systemEvents[iEvent].get_ref(), "Null system event at index:%u.", iEvent);
		switch(systemEvents[iEvent]->Type) {
		case ::gpk::SYSTEM_EVENT_Text:
			if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT>(*systemEvents[iEvent], [this, &gui](cnst ::gpk::SEView<::gpk::EVENT_TEXT> & eventText) {
				if(eventText.Type != ::gpk::EVENT_TEXT_Char)
					rtrn 0;
				::gpk::vcu0_t input = eventText.Data;
				::gpk::u2_t character = {};
				if_fail_fe(::gpk::loadPOD(input, character));
				if_true_fef(input.size(), "Text character payload has %u trailing bytes.", input.size());
				if(Text.size() < MaxLength && character >= 0x20 && character <= 0xFF) {
					if_fail_fe(Text.push_back((::gpk::sc_t)character));
					if_fail_fe(::gpk::controlTextSet(gui, IdText, {Text.begin(), Text.size()}));
				}
				rtrn 0;
			}));
			break;
		case ::gpk::SYSTEM_EVENT_Keyboard:
			if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD>(*systemEvents[iEvent], [this, &gui, &enterKeyPressed](cnst ::gpk::SEView<::gpk::EVENT_KEYBOARD> & eventKeyboard) {
				if(eventKeyboard.Type != ::gpk::EVENT_KEYBOARD_Down)
					rtrn 0;
				::gpk::vcu0_t input = eventKeyboard.Data;
				::gpk::u0_t key = {};
				if_fail_fe(::gpk::loadPOD(input, key));
				if_true_fef(input.size(), "Keyboard payload has %u trailing bytes.", input.size());
				switch((::gpk::SYSTEM_KEY)key) {
				case ::gpk::SYSTEM_KEY_Escape:		Edit(gui, false);		break;
				case ::gpk::SYSTEM_KEY_Enter:		enterKeyPressed = true;	break;
				case ::gpk::SYSTEM_KEY_Backspace:
					if(Text.size()) {
						if_fail_fe(Text.pop_back());
						if_fail_fe(::gpk::controlTextSet(gui, IdText, {Text.begin(), Text.size()}));
					}
					break;
				case ::gpk::SYSTEM_KEY_Clear:
					Text.clear();
					if_fail_fe(::gpk::controlTextSet(gui, IdText, {Text.begin(), Text.size()}));
					break;
				}
				rtrn 0;
			}));
			break;
		}
	}
	rtrn enterKeyPressed ? INT_MAX : handledControl != ::gpk::CID_INVALID ? handledControl : 0;
}
#endif

::gpk::err_t gpk::inputBoxCreate(::gpk::SInputBox & inputBox, ::gpk::SGUI & gui, ::gpk::cid_t iParent) {
	if_true_fef(inputBox.IdRoot != ::gpk::CID_INVALID, "Input-box root already assigned:%i.", inputBox.IdRoot);
	if_true_fef(iParent != ::gpk::CID_INVALID && ::gpk::controlInvalid(gui, iParent), "Invalid input-box parent:%i.", iParent);
	inputBox.IdRoot = ::gpk::controlCreate(gui);
	if_true_fef(inputBox.IdRoot < 0, "Failed to create input-box root:%i.", inputBox.IdRoot);
	if(iParent != ::gpk::CID_INVALID && ::gpk::failed(::gpk::controlSetParent(gui, inputBox.IdRoot, iParent))) {
		::gpk::controlDelete(gui, inputBox.IdRoot);
		inputBox.IdRoot = ::gpk::CID_INVALID;
		rtrn -1;
	}

	inputBox.IdText = ::gpk::controlCreateChild(gui, inputBox.IdRoot);
	if(inputBox.IdText < 0) {
		::gpk::controlDelete(gui, inputBox.IdRoot);
		inputBox.IdRoot = ::gpk::CID_INVALID;
		rtrn inputBox.IdText;
	}
	gui.Controls.States[inputBox.IdRoot].Mask &= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	gui.Controls.Placement[inputBox.IdRoot].Border = {};
	gui.Controls.Placement[inputBox.IdRoot].Margin = {};
	gui.Controls.Draw[inputBox.IdRoot].NoClient = true;
	gui.Controls.Placement[inputBox.IdText].Border = {2, 2, 2, 2};
	gui.Controls.Placement[inputBox.IdText].Margin = {1, 1, 1, 1};
	gui.Controls.Placement[inputBox.IdText].Area.Size = {0, 22};
	gui.Controls.Text[inputBox.IdText].Align = ::gpk::ALIGN_CENTER;

	cnst ::gpk::err_t keyboardResult = ::gpk::virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard);
	if(::gpk::failed(keyboardResult)) {
		::gpk::controlDelete(gui, inputBox.IdRoot);
		inputBox.IdRoot = inputBox.IdText = ::gpk::CID_INVALID;
		rtrn keyboardResult;
	}
	if(::gpk::failed(::gpk::controlSetParent(gui, inputBox.VirtualKeyboard.IdRoot, inputBox.IdRoot))) {
		::gpk::controlDelete(gui, inputBox.VirtualKeyboard.IdRoot);
		::gpk::controlDelete(gui, inputBox.IdRoot);
		inputBox.IdRoot = inputBox.IdText = ::gpk::CID_INVALID;
		inputBox.VirtualKeyboard = {};
		rtrn -1;
	}
	gui.Controls.SetHidden(inputBox.VirtualKeyboard.IdRoot, false);
	::gpk::SControlPlacement & textPlacement = gui.Controls.Placement[inputBox.IdText];
	::gpk::SControlPlacement & keyboardPlacement = gui.Controls.Placement[inputBox.VirtualKeyboard.IdRoot];
	keyboardPlacement.Align = textPlacement.Align;
	keyboardPlacement.Area.Offset.y = textPlacement.Area.Offset.y + textPlacement.Area.Size.y;
	textPlacement.Area.Size.x = keyboardPlacement.Area.Size.x;
	gui.Controls.Placement[inputBox.IdRoot].Area.Size = {textPlacement.Area.Size.x, (::gpk::s1_t)(textPlacement.Area.Size.y + keyboardPlacement.Area.Size.y)};
	inputBox.Edit(gui, inputBox.Editing);
	rtrn inputBox.IdRoot;
}


