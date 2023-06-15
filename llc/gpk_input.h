#include "gpk_array_pod.h"
#include "gpk_n3.h"
#include "gpk_array_static.h"

#ifndef GPK_INPUT_H_2890374982734
#define GPK_INPUT_H_2890374982734

namespace gpk
{
#pragma pack(push, 1)
	struct SInputJoystick {
		::gpk::n2i16				Position			= {};
		::gpk::n3i16				Deltas				= {};
		uint32_t					ButtonState			= 0;
	};
	struct SInputMouse {
		::gpk::n2i16				Position			= {};
		::gpk::n3i16				Deltas				= {};
		::gpk::astu8 <32>			ButtonState			= {};
		::gpk::astf64<32>			ButtonTime			= {};
		::gpk::asti64<32>			ButtonStrength		= {};
	};

	struct SInputKeyboard {
		::gpk::astu8 <256>			KeyState			= {};
		::gpk::astf64<256>			KeyTime				= {};
		::gpk::asti64<256>			KeyStrength			= {};
	};
#pragma pack(pop)
	struct SInput {
		::gpk::apod<SInputKeyboard>	StateKeyboard		= {};
		::gpk::apod<SInputMouse>	StateMouse			= {};
		::gpk::af32					TimeStampKeyboard	= {};
		::gpk::af32					TimeStampMouse		= {};

		::gpk::apod<SInputJoystick>	JoystickPrevious	= {};
		::gpk::apod<SInputJoystick>	JoystickCurrent		= {};

		SInputKeyboard				KeyboardPrevious	= {};
		SInputKeyboard				KeyboardCurrent		= {};
		SInputMouse					MousePrevious		= {};
		SInputMouse					MouseCurrent		= {};

		bool						ButtonUp			(uint8_t buttonIndex)	const	{ return MouseCurrent.ButtonState[buttonIndex	] == 0 && MousePrevious.ButtonState[buttonIndex	] != 0; }
		bool						ButtonDown			(uint8_t buttonIndex)	const	{ return MouseCurrent.ButtonState[buttonIndex	] != 0 && MousePrevious.ButtonState[buttonIndex	] == 0; }
		bool						KeyUp				(uint8_t keyIndex	)	const	{ return KeyboardCurrent.KeyState[keyIndex		] == 0 && KeyboardPrevious.KeyState[keyIndex	] != 0; }
		bool						KeyDown				(uint8_t keyIndex	)	const	{ return KeyboardCurrent.KeyState[keyIndex		] != 0 && KeyboardPrevious.KeyState[keyIndex	] == 0; }
	};
} // namespace

#endif // GPK_INPUT_H_2890374982734
