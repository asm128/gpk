#include "gpk_array_pod.h"
#include "gpk_n3.h"
#include "gpk_array_static.h"
#include "gpk_system_key.h"

#ifndef GPK_INPUT_H_23627
#define GPK_INPUT_H_23627

namespace gpk
{
// TODO: REDO (this is heavily outdated and I think it should be heavily improved)
#pragma pack(push, 1)
	struct SInputJoystick {
		::gpk::n2s1_t				Position			= {};
		::gpk::n3s1_t				Deltas				= {};
		u2_t					ButtonState			= 0;
	};
	struct SInputMouse {
		::gpk::n2s1_t				Position			= {};
		::gpk::n3s1_t				Deltas				= {};
		::gpk::astu0_t <32>			ButtonState			= {};
		::gpk::astf3_t<32>			ButtonTime			= {};
		::gpk::asts3_t<32>			ButtonStrength		= {};
	};

	struct SInputKeyboard {
		::gpk::astu0_t <256>			KeyState			= {};
		::gpk::astf3_t<256>			KeyTime				= {};
		::gpk::asts3_t<256>			KeyStrength			= {};
	};
#pragma pack(pop)
	stin	void	inputKeyboardStateSet	(SInputKeyboard & state, u0_t keyCode, bool pressed) {
		if(keyCode)
			state.KeyState[keyCode] = pressed;
		switch(keyCode) {
		default: break;
		case ::gpk::SYSTEM_KEY_LShift:
		case ::gpk::SYSTEM_KEY_RShift	: state.KeyState[::gpk::SYSTEM_KEY_Shift] = pressed; break;
		case ::gpk::SYSTEM_KEY_LControl:
		case ::gpk::SYSTEM_KEY_RControl	: state.KeyState[::gpk::SYSTEM_KEY_Control] = pressed; break;
		case ::gpk::SYSTEM_KEY_LMenu:
		case ::gpk::SYSTEM_KEY_RMenu		: state.KeyState[::gpk::SYSTEM_KEY_Menu] = pressed; break;
		}
	}

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

		bool						ButtonUp			(u0_t buttonIndex)	const	{ return MouseCurrent.ButtonState[buttonIndex	] == 0 && MousePrevious.ButtonState[buttonIndex	] != 0; }
		bool						ButtonDown			(u0_t buttonIndex)	const	{ return MouseCurrent.ButtonState[buttonIndex	] != 0 && MousePrevious.ButtonState[buttonIndex	] == 0; }
		bool						KeyUp				(u0_t keyIndex	)	const	{ return KeyboardCurrent.KeyState[keyIndex		] == 0 && KeyboardPrevious.KeyState[keyIndex	] != 0; }
		bool						KeyDown				(u0_t keyIndex	)	const	{ return KeyboardCurrent.KeyState[keyIndex		] != 0 && KeyboardPrevious.KeyState[keyIndex	] == 0; }
	};
	stin	err_t	inputKeyboardStateSet	(SInput & input, u0_t keyCode, bool pressed, s2_t deviceId = -1, f2_t timeStamp = {}) {
		::gpk::inputKeyboardStateSet(input.KeyboardCurrent, keyCode, pressed);
		if(0 > deviceId)
			return 0;
		const u2_t	deviceIndex	= (u2_t)deviceId;
		if(input.StateKeyboard.size() <= deviceIndex) {
			if_fail_fe(input.StateKeyboard.resize(deviceIndex + 1));
			if_fail_fe(input.TimeStampKeyboard.resize(deviceIndex + 1));
		}
		::gpk::inputKeyboardStateSet(input.StateKeyboard[deviceIndex], keyCode, pressed);
		input.TimeStampKeyboard[deviceIndex] = timeStamp;
		return 0;
	}
} // namespace

#endif // GPK_INPUT_H_23627
