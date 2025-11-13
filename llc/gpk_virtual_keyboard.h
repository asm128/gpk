#include "gpk_enum.h"
#include "gpk_gui_control_state.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_23627
#define GPK_VIRTUAL_KEYBOARD_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE (VK_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(VK_EVENT, PRESS		,  1);
	GDEFINE_ENUM_VALUE(VK_EVENT, RELEASE	,  2);
	GDEFINE_ENUM_VALUE(VK_EVENT, HOLD		,  3);
	GDEFINE_ENUM_VALUE(VK_EVENT, EDIT		,  4);
	GDEFINE_ENUM_VALUE(VK_EVENT, ARROW		,  5);

	GDEFINE_ENUM_TYPE (VK_SCANCODE, uint8_t);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Escape		, 0);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Enter		, 1);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Backspace	, 2);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Clear		, 3);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Left		, 4);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Right		, 5);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Up			, 6);
	GDEFINE_ENUM_VALUE(VK_SCANCODE, Down		, 7);
#pragma pack(push, 1)
	struct SVirtualKeyboardEvent {
		VK_EVENT		Type;
		VK_SCANCODE		ScanCode;
	};
#pragma pack(pop)

	struct SVirtualKeyboard {
		::gpk::cid_t	IdRoot;
		::gpk::acid		IdKeys;
		::gpk::au1_t	Keys;

		::gpk::apod<::gpk::SVirtualKeyboardEvent>	Events;
	};

	
	::gpk::error_t			virtualKeyboardHandleEvent	(const ::gpk::SVirtualKeyboard & vk, ::gpk::cid_t iControl, ::gpk::apod<::gpk::SVirtualKeyboardEvent> & events);

	//stainli	::gpk::error_t	virtualKeyboardHandleEvent	(::gpk::SVirtualKeyboard & vk, cid_t iControl) { return ::gpk::virtualKeyboardHandleEvent(vk, iControl, vk.Events); }
} // namespace 

#endif // GPK_VIRTUAL_KEYBOARD_H_23627
