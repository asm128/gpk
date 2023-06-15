#include "gpk_event.h"
#include "gpk_n2.h"

#ifndef GPK_SYSEVENT_H_2094098237
#define GPK_SYSEVENT_H_2094098237

namespace gpk
{
	GDEFINE_ENUM_TYPE(SYSTEM_EVENT, uint32_t);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Runtime		, 0);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Screen			, 1);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Keyboard		, 2);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Mouse			, 3);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Touch			, 4);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Joypad			, 5);
	GDEFINE_ENUM_VALUE(SYSTEM_EVENT, Text			, 6);
	typedef ::gpk::SEvent            <SYSTEM_EVENT> SSystemEvent;
	typedef ::gpk::FEventHandler     <SYSTEM_EVENT>	FSystemEvent;
	typedef ::gpk::FEventHandlerConst<SYSTEM_EVENT>	FSystemEventConst;

	GDEFINE_ENUM_TYPE (EVENT_SCREEN, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Create			,  0);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Destroy		,  1);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Close			,  2);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Move			,  3);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Activate		,  4);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Deactivate		,  5);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Resize			,  6);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Redraw			,  7);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Show			,  8);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Hide			,  9);
	GDEFINE_ENUM_VALUE(EVENT_SCREEN, Constraints	, 10);
	stainli	::gpk::error_t	eventEnqueueScreenCreate		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Create		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenDestroy		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Destroy	, key); }
	stainli	::gpk::error_t	eventEnqueueScreenClose			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Close		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenMove			(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Move		, coord); }
	stainli	::gpk::error_t	eventEnqueueScreenActivate		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Activate	, key); }
	stainli	::gpk::error_t	eventEnqueueScreenDeactivate	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Deactivate	, key); }
	stainli	::gpk::error_t	eventEnqueueScreenResize		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 size)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Resize		, size); }
	stainli	::gpk::error_t	eventEnqueueScreenRedraw		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Redraw		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenShow			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Show		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenHide			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Hide		, key); }
	stainli	::gpk::error_t	eventEnqueueScreenConstraints	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Screen, ::gpk::EVENT_SCREEN_Constraints, key); }
	
	GDEFINE_ENUM_TYPE (EVENT_KEYBOARD, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Up			, 0);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, Down			, 1);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, System_up	, 2);
	GDEFINE_ENUM_VALUE(EVENT_KEYBOARD, System_down	, 3);
	stainli	::gpk::error_t	eventEnqueueKeyboardUp			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_Up			, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardDown		(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_Down		, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardSystemUp	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_System_up	, key); }
	stainli	::gpk::error_t	eventEnqueueKeyboardSystemDown	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t key)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Keyboard, ::gpk::EVENT_KEYBOARD_System_down, key); }

	GDEFINE_ENUM_TYPE (EVENT_RUNTIME, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Init			, 0);	
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Exit			, 1);	
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_queue	, 2);	
	GDEFINE_ENUM_VALUE(EVENT_RUNTIME, Process_event	, 3);	
	stainli	::gpk::error_t	eventEnqueueRuntimeInit			(::gpk::apobj<::gpk::SSystemEvent> & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Init			, ::gpk::EVENT_RUNTIME_Init			); }
	stainli	::gpk::error_t	eventEnqueueRuntimeExit			(::gpk::apobj<::gpk::SSystemEvent> & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Exit			, ::gpk::EVENT_RUNTIME_Exit			); }
	stainli	::gpk::error_t	eventEnqueueRuntimeProcessQueue	(::gpk::apobj<::gpk::SSystemEvent> & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_queue, ::gpk::EVENT_RUNTIME_Process_queue); }
	stainli	::gpk::error_t	eventEnqueueRuntimeProcessEvent	(::gpk::apobj<::gpk::SSystemEvent> & queue)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Runtime, ::gpk::EVENT_RUNTIME_Process_event, ::gpk::EVENT_RUNTIME_Process_event); }
	
	GDEFINE_ENUM_TYPE (EVENT_MOUSE, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Position		, 0);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Up				, 1);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Down			, 2);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, DoubleClick		, 3);
	GDEFINE_ENUM_VALUE(EVENT_MOUSE, Wheel			, 4);
	stainli	::gpk::error_t	eventEnqueueMousePosition		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Position		, coord ); }
	stainli	::gpk::error_t	eventEnqueueMouseUp				(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Up			, button); }
	stainli	::gpk::error_t	eventEnqueueMouseDown			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Down			, button); }
	stainli	::gpk::error_t	eventEnqueueMouseDoubleClick	(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_DoubleClick	, button); }
	stainli	::gpk::error_t	eventEnqueueMouseWheel			(::gpk::apobj<::gpk::SSystemEvent> & queue, int16_t delta )		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Mouse, ::gpk::EVENT_MOUSE_Wheel		, delta ); }

	GDEFINE_ENUM_TYPE (EVENT_TOUCH, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Begin			, 0);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, End				, 1);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Update			, 2);
	GDEFINE_ENUM_VALUE(EVENT_TOUCH, Ownership		, 3);
	stainli	::gpk::error_t	eventEnqueueTouchBegin			(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Begin	, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchEnd			(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_End		, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchUpdate			(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Update	, coord); }
	stainli	::gpk::error_t	eventEnqueueTouchOwnership		(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Touch, ::gpk::EVENT_TOUCH_Ownership, coord); }

	GDEFINE_ENUM_TYPE(EVENT_JOYPAD, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Move			, 0);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_up		, 1);
	GDEFINE_ENUM_VALUE(EVENT_JOYPAD, Button_down	, 2);
	stainli	::gpk::error_t	eventEnqueueJoyPadMove			(::gpk::apobj<::gpk::SSystemEvent> & queue, ::gpk::n2i16 coord)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Move		, coord ); }
	stainli	::gpk::error_t	eventEnqueueJoyPadUp			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Button_up	, button); }
	stainli	::gpk::error_t	eventEnqueueJoyPadDown			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint8_t button)		{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Joypad, ::gpk::EVENT_JOYPAD_Button_down, button); }

	GDEFINE_ENUM_TYPE(EVENT_TEXT, uint8_t);
	GDEFINE_ENUM_VALUE(EVENT_TEXT, Char, 0);
	stainli	::gpk::error_t	eventEnqueueTextChar			(::gpk::apobj<::gpk::SSystemEvent> & queue, uint16_t mbchar)	{ return ::gpk::eventEnqueueChild(queue, ::gpk::SYSTEM_EVENT_Text, ::gpk::EVENT_TEXT_Char, mbchar); }

	GDEFINE_ENUM_TYPE (SYSEVENT, uint32_t);
	GDEFINE_ENUM_VALUE(SYSEVENT, NOOP				, (SYSTEM_EVENT_Runtime		<< 8) | 0);
	GDEFINE_ENUM_VALUE(SYSEVENT, MINMAXINFO			, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Constraints	);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_DESTROY		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Destroy		);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_CLOSE		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Close			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_MOVE		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Move			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_ACTIVATE	, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Activate		);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_DEACTIVATE	, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Deactivate		);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_RESIZE		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Resize			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_REDRAW		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Redraw			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_SHOW		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Show			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_HIDE		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Hide			);
	GDEFINE_ENUM_VALUE(SYSEVENT, WINDOW_CREATE		, (SYSTEM_EVENT_Screen		<< 8) | EVENT_SCREEN_Create			);
	GDEFINE_ENUM_VALUE(SYSEVENT, KEY_UP				, (SYSTEM_EVENT_Keyboard	<< 8) | EVENT_KEYBOARD_Up			);
	GDEFINE_ENUM_VALUE(SYSEVENT, KEY_DOWN			, (SYSTEM_EVENT_Keyboard	<< 8) | EVENT_KEYBOARD_Down			);
	GDEFINE_ENUM_VALUE(SYSEVENT, SYSKEY_UP			, (SYSTEM_EVENT_Keyboard	<< 8) | EVENT_KEYBOARD_System_up	);
	GDEFINE_ENUM_VALUE(SYSEVENT, SYSKEY_DOWN		, (SYSTEM_EVENT_Keyboard	<< 8) | EVENT_KEYBOARD_System_down	);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_POSITION		, (SYSTEM_EVENT_Mouse		<< 8) | EVENT_MOUSE_Position		);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_UP			, (SYSTEM_EVENT_Mouse		<< 8) | EVENT_MOUSE_Up				);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_DOWN			, (SYSTEM_EVENT_Mouse		<< 8) | EVENT_MOUSE_Down			);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_DBLCLK		, (SYSTEM_EVENT_Mouse		<< 8) | EVENT_MOUSE_DoubleClick		);
	GDEFINE_ENUM_VALUE(SYSEVENT, MOUSE_WHEEL		, (SYSTEM_EVENT_Mouse		<< 8) | EVENT_MOUSE_Wheel			);
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_BEGIN		, (SYSTEM_EVENT_Touch		<< 8) | EVENT_TOUCH_Begin			);
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_END			, (SYSTEM_EVENT_Touch		<< 8) | EVENT_TOUCH_End				);
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_UPDATE		, (SYSTEM_EVENT_Touch		<< 8) | EVENT_TOUCH_Update			);
	GDEFINE_ENUM_VALUE(SYSEVENT, TOUCH_OWNERSHIP	, (SYSTEM_EVENT_Touch		<< 8) | EVENT_TOUCH_Ownership		);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_BUTTON_DOWN	, (SYSTEM_EVENT_Joypad		<< 8) | EVENT_JOYPAD_Button_down	);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_BUTTON_UP		, (SYSTEM_EVENT_Joypad		<< 8) | EVENT_JOYPAD_Button_up		);
	GDEFINE_ENUM_VALUE(SYSEVENT, JOY_MOVE			, (SYSTEM_EVENT_Joypad		<< 8) | EVENT_JOYPAD_Move			);
	GDEFINE_ENUM_VALUE(SYSEVENT, CHAR				, (SYSTEM_EVENT_Text		<< 8) | EVENT_TEXT_Char				);

	typedef ::gpk::SEvent            <SYSEVENT> SSysEvent;
	typedef ::gpk::FEventHandler     <SYSEVENT>	FSysEvent;
	typedef ::gpk::FEventHandlerConst<SYSEVENT>	FSysEventConst;
} // namespace 										  

#endif // GPK_SYSEVENT_H_2094098237
