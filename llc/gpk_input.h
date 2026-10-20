#include "gpk_array_pod.h"
#include "gpk_n3.h"
#include "gpk_array_static.h"

#ifndef GPK_INPUT_H_23627
#define GPK_INPUT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SInputJoystick {
		::gpk::n2s1_t				Position			= {};
		::gpk::n3s1_t				Deltas				= {};
		::gpk::u2_t				ButtonState			= 0;
	};
	struct SInputMouse {
		::gpk::n2s1_t				Position			= {};
		::gpk::n3s1_t				Deltas				= {};
		::gpk::astu0_t<32>			ButtonState			= {};
		::gpk::astf3_t<32>			ButtonTime			= {};
		::gpk::asts3_t<32>			ButtonStrength		= {};
	};

	struct SInputKeyboard {
		::gpk::astu0_t<256>			KeyState			= {};
		::gpk::astf3_t<256>			KeyTime				= {};
		::gpk::asts3_t<256>			KeyStrength			= {};
	};
#pragma pack(pop)
	struct SInput {
		::gpk::apod<SInputKeyboard>	StateKeyboard		= {};
		::gpk::apod<SInputMouse>	StateMouse			= {};
		::gpk::af2_t				TimeStampKeyboard	= {};
		::gpk::af2_t				TimeStampMouse		= {};

		::gpk::apod<SInputJoystick>	JoystickPrevious	= {};
		::gpk::apod<SInputJoystick>	JoystickCurrent		= {};

		SInputKeyboard				KeyboardPrevious	= {};
		SInputKeyboard				KeyboardCurrent		= {};
		SInputMouse					MousePrevious		= {};
		SInputMouse					MouseCurrent		= {};

		bool						ButtonUp			(::gpk::u0_t buttonIndex)	const	{ return buttonIndex < MouseCurrent.ButtonState.size() && MouseCurrent.ButtonState[buttonIndex] == 0 && MousePrevious.ButtonState[buttonIndex] != 0; }
		bool						ButtonDown			(::gpk::u0_t buttonIndex)	const	{ return buttonIndex < MouseCurrent.ButtonState.size() && MouseCurrent.ButtonState[buttonIndex] != 0 && MousePrevious.ButtonState[buttonIndex] == 0; }
		bool						KeyUp				(::gpk::u0_t keyIndex)		const	{ return KeyboardCurrent.KeyState[keyIndex] == 0 && KeyboardPrevious.KeyState[keyIndex] != 0; }
		bool						KeyDown				(::gpk::u0_t keyIndex)		const	{ return KeyboardCurrent.KeyState[keyIndex] != 0 && KeyboardPrevious.KeyState[keyIndex] == 0; }
	};

	inln void					inputNextFrame		(::gpk::SInput & input) {
		input.KeyboardPrevious		= input.KeyboardCurrent;
		input.MousePrevious			= input.MouseCurrent;
		input.MouseCurrent.Deltas	= {};
		input.JoystickPrevious		= input.JoystickCurrent;
		for(::gpk::u2_t iJoystick = 0; iJoystick < input.JoystickCurrent.size(); ++iJoystick)
			input.JoystickCurrent[iJoystick].Deltas = {};
	}
} // namespace

#endif // GPK_INPUT_H_23627


