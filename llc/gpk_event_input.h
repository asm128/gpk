#include "gpk_system_event.h"
#include "gpk_n2.h"
#include "gpk_encoding.h"

#ifndef GPK_EVENT_INPUT_H_23627
#define GPK_EVENT_INPUT_H_23627

nmsp gpk
{	
	GDEFINE_ENUM_TYPE(EVENT_TEXT, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_TEXT, Char		, 0);
	GDEFINE_ENUM_VALUE(EVENT_TEXT, Position	, 1);
	GDEFINE_ENUM_VALUE(EVENT_TEXT, Cursor	, 2);

	GDEFINE_ENUM_TYPE (INPUT_MODIFIER, u2_t);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, Shift		, 0x0001);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, CapsLock		, 0x0002);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, Control		, 0x0004);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, Alt			, 0x0008);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, NumLock		, 0x0010);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, ScrollLock	, 0x0020);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, Super		, 0x0040);
	GDEFINE_ENUM_VALUE(INPUT_MODIFIER, AltGr		, 0x0080);

#pragma pack(push, 1)
	struct SInputTextEvent {
		u2_t		CodePoint	= {};
		u2_t		KeySym		= {};
		s2_t		DeviceId	= -1;
		u0_t		ScanCode	= {};
		u0_t		UTF8		[4] = {};
		u0_t		UTF8Length	= {};
	};

	struct SInputKeyEvent {
		u0_t		Key			= {};
		u0_t		ScanCode	= {};
		s2_t		DeviceId	= -1;
		u2_t		KeySym		= {};
		u2_t		Modifiers	= {};
		u2_t		Time		= {};
	};
#pragma pack(pop)

	stin	err_t	eventEnqueueTextChar		(apobj<SEventSystem> & queue, const SInputTextEvent & event) {
		if(event.CodePoint > 0x10FFFF || (event.CodePoint >= 0xD800 && event.CodePoint <= 0xDFFF))
			return -1;
		SInputTextEvent	encodedEvent	= event;
		::gpk::string		utf8			= {};
		if_fail_fe(::gpk::utf8FromCodePoint(event.CodePoint, utf8));
		if_true_fe(utf8.size() > ::gpk::size(encodedEvent.UTF8));
		encodedEvent.UTF8Length = (u0_t)utf8.size();
		memcpy(encodedEvent.UTF8, utf8.begin(), encodedEvent.UTF8Length);
		return eventEnqueueChild(queue, SYSTEM_EVENT_Text, EVENT_TEXT_Char, encodedEvent);
	}
	stin	err_t	eventEnqueueTextChar		(apobj<SEventSystem> & queue, u2_t codePoint)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Text, EVENT_TEXT_Char, codePoint); }

	GDEFINE_ENUM_TYPE (EVENT_KEYBOARD, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Up		, 0);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Down		, 1);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, SysUp	, 2);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, SysDown	, 3);
	stin	err_t	eventEnqueueKeyboardUp		(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_Up		, key); }
	stin	err_t	eventEnqueueKeyboardDown	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_Down	, key); }
	stin	err_t	eventEnqueueKeyboardSysUp	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_SysUp	, key); }
	stin	err_t	eventEnqueueKeyboardSysDown	(apobj<SEventSystem> & queue, u0_t key)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_SysDown, key); }
	stin	err_t	eventEnqueueKeyboardUp		(apobj<SEventSystem> & queue, const SInputKeyEvent & event)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_Up		, event); }
	stin	err_t	eventEnqueueKeyboardDown	(apobj<SEventSystem> & queue, const SInputKeyEvent & event)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_Down	, event); }
	stin	err_t	eventEnqueueKeyboardSysUp	(apobj<SEventSystem> & queue, const SInputKeyEvent & event)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_SysUp	, event); }
	stin	err_t	eventEnqueueKeyboardSysDown	(apobj<SEventSystem> & queue, const SInputKeyEvent & event)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Keyboard, EVENT_KEYBOARD_SysDown, event); }

	GDEFINE_ENUM_TYPE (EVENT_MOUSE, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Position	, 0);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Up			, 1);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Down		, 2);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, DoubleClick	, 3);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Wheel		, 4);
	stin	err_t	eventEnqueueMousePosition	(apobj<SEventSystem> & queue, n2s1_t coord)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Mouse, EVENT_MOUSE_Position		, coord ); }
	stin	err_t	eventEnqueueMouseUp			(apobj<SEventSystem> & queue, u0_t button)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Mouse, EVENT_MOUSE_Up			, button); }
	stin	err_t	eventEnqueueMouseDown		(apobj<SEventSystem> & queue, u0_t button)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Mouse, EVENT_MOUSE_Down			, button); }
	stin	err_t	eventEnqueueMouseDoubleClick(apobj<SEventSystem> & queue, u0_t button)		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Mouse, EVENT_MOUSE_DoubleClick	, button); }
	stin	err_t	eventEnqueueMouseWheel		(apobj<SEventSystem> & queue, s1_t delta )		{ return eventEnqueueChild(queue, SYSTEM_EVENT_Mouse, EVENT_MOUSE_Wheel		, delta ); }

	GDEFINE_ENUM_TYPE (EVENT_TOUCH, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Begin		, 0);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, End			, 1);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Update		, 2);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Ownership	, 3);
	stin	err_t	eventEnqueueTouchBegin		(apobj<SEventSystem> & queue, n2u1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Touch, EVENT_TOUCH_Begin	, coord); }
	stin	err_t	eventEnqueueTouchEnd		(apobj<SEventSystem> & queue, n2u1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Touch, EVENT_TOUCH_End		, coord); }
	stin	err_t	eventEnqueueTouchUpdate		(apobj<SEventSystem> & queue, n2u1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Touch, EVENT_TOUCH_Update	, coord); }
	stin	err_t	eventEnqueueTouchOwnership	(apobj<SEventSystem> & queue, n2u1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Touch, EVENT_TOUCH_Ownership, coord); }

	GDEFINE_ENUM_TYPE(EVENT_JOYPAD, u0_t);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Move		, 0);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_up	, 1);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_down, 2);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, ZMove		, 3);
	stin	err_t	eventEnqueueJoyPadMove		(apobj<SEventSystem> & queue, n2s1_t coord)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Joypad, EVENT_JOYPAD_Move		, coord ); }
	stin	err_t	eventEnqueueJoyPadZMove		(apobj<SEventSystem> & queue, u1_t value)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Joypad, EVENT_JOYPAD_ZMove		, value ); }
	stin	err_t	eventEnqueueJoyPadUp		(apobj<SEventSystem> & queue, u0_t button)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Joypad, EVENT_JOYPAD_Button_up	, button); }
	stin	err_t	eventEnqueueJoyPadDown		(apobj<SEventSystem> & queue, u0_t button)	{ return eventEnqueueChild(queue, SYSTEM_EVENT_Joypad, EVENT_JOYPAD_Button_down, button); }
} // nmsp

#endif // GPK_EVENT_INPUT_H_23627
