#include "gpk_system_event.h"
#include "gpk_n2.h"

#ifndef GPK_EVENT_INPUT_H_23627
#define GPK_EVENT_INPUT_H_23627

namespace gpk
{	
	GDEFINE_ENUM_TYPE(EVENT_TEXT, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_TEXT, Char, 0);
	stainli	::gpk::error_t	eventEnqueueTextChar		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint32_t utf8)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Text, ::gpk::EVENT_TEXT_Char, utf8); }

	GDEFINE_ENUM_TYPE (EVENT_KEYBOARD, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Up		, 0);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Down		, 1);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, SysUp	, 2);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, SysDown	, 3);
	stainli	::gpk::error_t	eventEnqueueKeyboardUp		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_Up		, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardDown	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_Down	, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardSysUp	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_SysUp	, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardSysDown	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_SysDown, key); }

	GDEFINE_ENUM_TYPE (EVENT_MOUSE, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Position	, 0);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Up			, 1);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Down		, 2);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, DoubleClick	, 3);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Wheel		, 4);
	stainli	::gpk::error_t	eventEnqueueMousePosition	(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Position		, coord ); }
	stainli	::gpk::error_t	eventEnqueueMouseUp			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Up			, button); }
	stainli	::gpk::error_t	eventEnqueueMouseDown		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Down			, button); }
	stainli	::gpk::error_t	eventEnqueueMouseDoubleClick(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_DoubleClick	, button); }
	stainli	::gpk::error_t	eventEnqueueMouseWheel		(::gpk::apobj<::gpk::SSystemEvent> & queue, int16_t delta )		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Wheel		, delta ); }

	GDEFINE_ENUM_TYPE (EVENT_TOUCH, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Begin		, 0);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, End			, 1);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Update		, 2);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Ownership	, 3);
	stainli	::gpk::error_t	eventEnqueueTouchBegin		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2u16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Begin	, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchEnd		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2u16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_End		, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchUpdate		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2u16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Update	, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchOwnership	(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2u16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Ownership, coord); }

	GDEFINE_ENUM_TYPE(EVENT_JOYPAD, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Move		, 0);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_up	, 1);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_down, 2);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, ZMove		, 3);
	stainli	::gpk::error_t	eventEnqueueJoyPadMove		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Move		, coord ); }
	stainli	::gpk::error_t	eventEnqueueJoyPadZMove		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint16_t value)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_ZMove		, value ); }
	stainli	::gpk::error_t	eventEnqueueJoyPadUp		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Button_up	, button); }
	stainli	::gpk::error_t	eventEnqueueJoyPadDown		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Button_down, button); }
} // namespace

#endif // GPK_EVENT_INPUT_H_23627
