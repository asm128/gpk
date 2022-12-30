#include "gpk_array.h"
#include "gpk_enum.h"

#ifndef GPK_VIRTUAL_KEYBOARD_H_293874239874
#define GPK_VIRTUAL_KEYBOARD_H_293874239874

namespace gpk
{
	GDEFINE_ENUM_TYPE (VK_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(VK_EVENT, PRESS		,  1);
	GDEFINE_ENUM_VALUE(VK_EVENT, RELEASE	,  2);
	GDEFINE_ENUM_VALUE(VK_EVENT, HOLD		,  3);
	GDEFINE_ENUM_VALUE(VK_EVENT, EDIT		,  4);
	GDEFINE_ENUM_VALUE(VK_EVENT, ARROW		,  5);

	GDEFINE_ENUM_TYPE (VK_SCANCODE, uint16_t);
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
		int32_t											IdRoot;
		::gpk::apod<uint16_t>						Keys;

		::gpk::apod<::gpk::SVirtualKeyboardEvent>	Events;
	};

	::gpk::error_t				virtualKeyboardHandleEvent		(::gpk::SVirtualKeyboard & vk, int32_t iControl);
} // namespace 

#endif GPK_VIRTUAL_KEYBOARD_H_293874239874
