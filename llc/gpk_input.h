#include "gpk_array_pod.h"
#include "gpk_coord.h"

#ifndef GPK_INPUT_H_2890374982734
#define GPK_INPUT_H_2890374982734

namespace gpk
{
#pragma pack(push, 1)
	struct SInputJoystick {
		::gpk::n2i32				Position				= {};
		::gpk::n3i32				Deltas					= {};
		uint32_t					ButtonState				= 0;
	};
	struct SInputMouse {
		::gpk::n2i32				Position				= {};
		::gpk::n3i32				Deltas					= {};
		uint8_t						ButtonState		[32]	= {};
		double						ButtonTime		[32]	= {};
		int64_t						ButtonStrength	[32]	= {};
	};

	struct SInputKeyboard {
		uint8_t						KeyState		[256]	= {};
		double						KeyTime			[256]	= {};
		int64_t						KeyStrength		[256]	= {};
	};
#pragma pack(pop)
	struct SInput {
		::gpk::apod<SInputKeyboard>	StateKeyboard			= {};
		::gpk::apod<SInputMouse>	StateMouse				= {};
		::gpk::apod<float>			TimeStampKeyboard		= {};
		::gpk::apod<float>			TimeStampMouse			= {};

		::gpk::apod<SInputJoystick>	JoystickPrevious		= {};
		::gpk::apod<SInputJoystick>	JoystickCurrent			= {};

		SInputKeyboard				KeyboardPrevious		= {};
		SInputKeyboard				KeyboardCurrent			= {};
		SInputMouse					MousePrevious			= {};
		SInputMouse					MouseCurrent			= {};

		inlcxpr	bool				ButtonUp				(uint8_t buttonIndex)	const	noexcept	{ return MouseCurrent.ButtonState[buttonIndex	] == 0 && MousePrevious.ButtonState[buttonIndex	] != 0; }
		inlcxpr	bool				ButtonDown				(uint8_t buttonIndex)	const	noexcept	{ return MouseCurrent.ButtonState[buttonIndex	] != 0 && MousePrevious.ButtonState[buttonIndex	] == 0; }
		inlcxpr	bool				KeyUp					(uint8_t keyIndex	)	const	noexcept	{ return KeyboardCurrent.KeyState[keyIndex		] == 0 && KeyboardPrevious.KeyState[keyIndex	] != 0; }
		inlcxpr	bool				KeyDown					(uint8_t keyIndex	)	const	noexcept	{ return KeyboardCurrent.KeyState[keyIndex		] != 0 && KeyboardPrevious.KeyState[keyIndex	] == 0; }
	};
} // namespace

#endif // GPK_INPUT_H_2890374982734
